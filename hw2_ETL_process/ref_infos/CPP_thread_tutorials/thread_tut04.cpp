// g++ -pthread -o thread_tut04 thread_tut04.cpp && ./thread_tut04
// https://youtu.be/_N0B5ua7oN8?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
Avoid deadlock:
1. Prefer locking single mutex.
2. Avoid locking a mutex and then calling a user provided function (it may lock another mutex).
3. Use std::lock() to lock more than one mutex.
4. Lock the mutex in same order.

Locking Granularity:
- Fine-grained lock:    protects small amount of data.
- Coarse-grained lock:  protects big amount of data. (Lose the opportunity of parallel computing)
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
    std::mutex _mu;
    std::mutex _mu2;
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
        // (1) (2)
        // std::lock_guard<std::mutex> locker(_mu);
        // std::lock_guard<std::mutex> locker2(_mu2);

        // (3) Solution to dead lock (by "std::adopt_lock")
        std::lock(_mu, _mu2);
        std::lock_guard<std::mutex> locker(_mu, std::adopt_lock);
        std::lock_guard<std::mutex> locker2(_mu2, std::adopt_lock);
        cout << id << value << endl;
    }
    void shared_print2(string id, int value)
    {
        // (1) Dead Lock case study:
        // std::lock_guard<std::mutex> locker2(_mu2);
        // std::lock_guard<std::mutex> locker(_mu);

        // (2) Solution to dead lock
        // std::lock_guard<std::mutex> locker(_mu);
        // std::lock_guard<std::mutex> locker2(_mu2);

        // (3) Solution to dead lock (by "std::adopt_lock")
        std::lock(_mu, _mu2);
        std::lock_guard<std::mutex> locker(_mu, std::adopt_lock);
        std::lock_guard<std::mutex> locker2(_mu2, std::adopt_lock);
        cout << id << value << endl;
    }
};

void function_1(LogFile &log)
{
    for (int i = 0; i > -100; --i)
        log.shared_print("From t1: ", i);
}

int main()
{
    LogFile log;
    std::thread t1(function_1, std::ref(log));
    for (int i = 0; i < 100; ++i)
        log.shared_print2("From main: ", i);
    t1.join();
    return 0;
}