#include <iostream>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <mutex>


namespace interprocess {
template <typename T>
class SharedMemoryDataType {
public:
    SharedMemoryManager(const char* name) : sharedMemoryName(name), shared_var(nullptr) {}

    bool create() {
        const size_t sharedMemorySize = sizeof(T);

        // Create or open the shared memory object
        shm_fd = shm_open(sharedMemoryName, O_CREAT | O_RDWR, 0666);
        if (shm_fd == -1) {
            std::cerr << "Error creating/opening shared memory" << std::endl;
            return false;
        }

        // Configure the size of the shared memory object
        ftruncate(shm_fd, sharedMemorySize);

        // Map the shared memory object into this process's address space
        shared_var = static_cast<T*>(mmap(NULL, sharedMemorySize, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0));
        if (shared_var == MAP_FAILED) {
            std::cerr << "Error mapping shared memory" << std::endl;
            return false;
        }

        return true;
    }

    T read() {
       std::lock_guard<std::mutex> lock(mutex_);
        if (!shared_var) {
            std::cerr << "Shared memory variable is not initialized." << std::endl;
            // You can throw an exception or handle the error in a different way if needed.
            // Here, we'll return a default-constructed T.
            return T();
        }
        return *shared_var;
    }

    void update(const T& new_value) {
       std::lock_guard<std::mutex> lock(mutex_);
        if (!shared_var) {
            std::cerr << "Shared memory variable is not initialized." << std::endl;
            return;
        }
        *shared_var = new_value;
    }

    void lock() {
        mutex_.lock();
    }

    void unlock() {
        mutex_.unlock();
    }

    bool destroy() {
        if (shared_var) {
            //Unmap the shared memory and close the file descriptor
            munmap(shared_var, sizeof(T));
            close(shm_fd);
            shared_var = nullptr;
        }

        // Remove the shared memory object (optional, comment out if you want to keep it after the program exits)
        shm_unlink(sharedMemoryName);

        return true;
    }

    ~SharedMemoryManager() {
        destroy();
    }

private:
    const char* sharedMemoryName;
    int shm_fd;
    T* shared_var;
    std::mutex mutex_; // Mutex for synchronization
};



}
