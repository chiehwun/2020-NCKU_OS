// g++ -pthread -o thread_tut05 thread_tut05.cpp && ./thread_tut05
// https://youtu.be/IBu5ka1MQ7w?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
*/
#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <mutex> // Solve the race condition
                 // by synchronizing the access of the common resource
using std::cout;
using std::endl;
using std::ofstream;
using std::string;

class LogFile
{
    std::mutex _mu; // for printing files
    ofstream _f;

public:
    LogFile()
    {
        _f.open("log.txt");
    }
    ~LogFile()
    {
        _f.close();
    }
    void shared_print(string id, int value)
    {
        // lock more flexible
        std::unique_lock<std::mutex> locker(_mu, std::defer_lock);
        // do something else without using ofstream

        locker.lock();
        cout << id << value << endl;
        locker.unlock();

        locker.lock(); // lock again
        // ... do _f
        locker.unlock();
        // std::unique_lock<std::mutex> can be move
        std::unique_lock<std::mutex> locker2 = std::move(locker);
    }
};

class LogFile_lazy
{
    std::mutex _mu; // for printing file
    // std::mutex _mu_open;  // for opening file (1)(2)
    std::once_flag _flag; // for opening file once (3)
    ofstream _f;

public:
    ~LogFile_lazy()
    {
        _f.close();
    }
    void shared_print(string id, int value)
    {
        // // std::unique_lock<std::mutex> locker2(_mu_open); // (2) Waste resource
        // if (!_f.is_open())
        // {
        //     // std::unique_lock<std::mutex> locker2(_mu_open); // (1) Not thread save
        //     _f.open("log.txt");
        // }

        // (3) File will be opened inly once by one thread
        std::call_once(_flag, [&]() { _f.open("log.txt"); });
        std::unique_lock<std::mutex> locker(_mu, std::defer_lock);
        _f << id << value << endl;
    }
};

int main()
{
    return 0;
}