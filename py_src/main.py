from IPCSharedMemory import SharedMemoryVariable

if __name__ == "__main__":
    data_type = "double"  # Change the data type as needed (int, unsigned int, float, double)
    shared_memory_name = "MySharedMemory"  # Specify the shared memory name here

    manager = SharedMemoryVariable(shared_memory_name, data_type)

    print("Press any key to stop reading shared memory.")
    last_timestamp = -1

    while True:
        if sys.stdin in select.select([sys.stdin], [], [], 0)[0]:
            _ = sys.stdin.read(1)
            break

        current_timestamp = manager.read_shared_memory()
       # manager.acquire_lock()
      #  time.sleep(5)
      #  manager.release_lock()
        if last_timestamp != -1:
            latency = current_timestamp - last_timestamp
            print(f"Latency: {latency:.6f} seconds")
        
        last_timestamp = current_timestamp
        if last_timestamp == -1:
            print("loop completed")
            break
