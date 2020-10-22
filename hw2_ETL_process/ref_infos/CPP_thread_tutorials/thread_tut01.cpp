#include <iostream>
#include <thread>

void function_1()
{
    std::cout << "this is function_1!" << std::endl;
}

int main()
{
    std::thread t1(function_1);
    // t1.join();   // main thread waits for t1 to finish
    t1.detach(); // t1 will be free on its own (daemon process)

    if (t1.joinable()) // prevent crash if t1 is not joinable
        t1.join();
    return 0;
}