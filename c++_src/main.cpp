#include "IPCSharedMemory.hpp"

int main() {
    interprocess::SharedMemoryManager<double> sharedDouble("MySharedMemory");
    sharedDouble.create();
   // sharedDouble.lock();
    sharedDouble.update(42.42);
   // sharedDouble.unlock();
      sleep(10);

    if (sharedDouble.create()) {
        // Use the shared memory variable
     for(size_t i = 0 ; i < 10000000 ; i++) {
      //  sharedDouble.lock();
        sharedDouble.update(42.42+i);
      //  sharedDouble.unlock();

        // Wait for a while (you can perform other tasks here)
        std::cout << "Waiting for Python program to read the shared memory..."  << sharedDouble.read()  << std::endl;
        sleep(0.000000000000001);
     }
        // Read the shared variable with a lock
      //  sharedDouble.lock();
        double value = sharedDouble.read();
      //  sharedDouble.unlock();

        // Print the value of the shared variable (it might have been updated by Python)
        std::cout << "Shared variable value: " << value << std::endl;

        // Destroy the shared memory variable (optional if you want to keep it after the program exits)
       
    }

    sharedDouble.destroy();
    return 0;
}
