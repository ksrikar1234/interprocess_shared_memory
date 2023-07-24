import mmap
import struct
import time
import sys
import select
import fcntl

class SharedMemoryVariable:
    def __init__(self, shared_memory_name, data_type):
        self.shared_memory_name = shared_memory_name
        self.data_type = data_type
        self.data_format, self.data_size = self._get_data_format_size()

    def _get_data_format_size(self):
        if self.data_type == "int":
            return "i", 4
        elif self.data_type == "unsigned int":
            return "I", 4
        elif self.data_type == "float":
            return "f", 4
        elif self.data_type == "double":
            return "d", 8
        else:
            print("Unsupported data type:", self.data_type)
            return None, None

    def acquire_lock(self):
        with open("/dev/shm/" + self.shared_memory_name, "r+b") as file:
            if self.data_type == "write":
                fcntl.flock(file.fileno(), fcntl.LOCK_EX)
            else:
                fcntl.flock(file.fileno(), fcntl.LOCK_SH)

    def release_lock(self):
        with open("/dev/shm/" + self.shared_memory_name, "r+b") as file:
            fcntl.flock(file.fileno(), fcntl.LOCK_UN)

    def read_shared_memory(self):
        try:
            with open("/dev/shm/" + self.shared_memory_name, "r+b") as file:
                mmapped_data = mmap.mmap(file.fileno(), 0)
                shared_var = struct.unpack(self.data_format, mmapped_data[:self.data_size])[0]
                print("Python program read the shared memory variable:", shared_var)

                return time.time()  # Return the timestamp of the current read

        except FileNotFoundError:
            print("Shared memory file not found. Run the C++ program first.")
            return -1

    def update_shared_memory(self, new_value):
        try:
            with open("/dev/shm/" + self.shared_memory_name, "r+b") as file:
                mmapped_data = mmap.mmap(file.fileno(), 0)
                packed_value = struct.pack(self.data_format, new_value)
                mmapped_data[:self.data_size] = packed_value
                print("Python program updated the shared memory variable with:", new_value)

        except FileNotFoundError:
            print("Shared memory file not found. Run the C++ program first.")
