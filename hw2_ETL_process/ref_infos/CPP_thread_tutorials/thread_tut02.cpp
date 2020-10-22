// g++ -pthread -o thread_tut02 thread_tut02.cpp && ./thread_tut02
#include <iostream>
#include <string>
#include <thread>

void function_1()
{
    std::cout << "this is function_1!" << std::endl;
}

class Fctor
{
public:
    void operator()()
    {
        for (int i = 0; i > -100; --i)
        {
            std::cout << "from t1 " << i << std::endl;
        }
    }
};

int main()
{
    // Case 1
    // Fctor fctor;
    // std::thread t1(fctor); // t1 starts running
    // Case 2
    std::thread t1((Fctor())); // t1 starts running

    try
    {
        for (int i = 0; i < 100; ++i)
            std::cout << "from main: " << i << std::endl;
    }
    catch (...)
    {
        t1.join();
        throw;
    }
    return 0;
}