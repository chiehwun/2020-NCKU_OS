// g++ -pthread -o thread_tut06 thread_tut06.cpp && ./thread_tut06
// https://youtu.be/13dFggo4t_I?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
*/
#include <iostream>
#include <fstream>
#include <string>
#include <deque>
#include <thread>
#include <condition_variable> // Prevent wasting resource
#include <mutex>              // Solve the race condition
                              // by synchronizing the access of the common resource
using std::cout;
using std::deque;
using std::endl;
using std::ofstream;
using std::string;

// Global variables
deque<int> q;
std::mutex mu;
std::condition_variable cond;

void function_1()
{
    int count = 10;
    while (count > 0)
    {
        std::unique_lock<std::mutex> locker(mu); // lock for q
        q.push_front(count);
        locker.unlock();
        cond.notify_one(); // (3) Notify one waiting thread, if there is one.
        // cond.notify_all(); // (-) Notify all waiting thread.
        std::this_thread::sleep_for(std::chrono::seconds(1));
        --count;
    }
}

void function_2_cond()
{
    int data = 0;
    while (data != 1)
    {
        std::unique_lock<std::mutex> locker(mu); // lock for q
        // cond.wait(locker);   // 1. unlock "locker" before sleeping  2. sleep until notified by thread1, and then lock again

        // (3)
        // If spurious wake (self-waking??) happens,
        // it will check the lambda function,
        // if returns true  => go ahead
        // if returns false => sleep
        cond.wait(locker, []() { return !q.empty(); });
        data = q.back();
        q.pop_back();
        locker.unlock();
        cout << "t2 got a value from t1: " << data << endl;
    }
}

void function_2()
{
    int data = 0;
    while (data != 1)
    {
        std::unique_lock<std::mutex> locker(mu); // lock for q
        if (!q.empty())
        {
            data = q.back();
            q.pop_back();
            locker.unlock();
            cout << "t2 got a value from t1: " << data << endl;
        }
        else
        {
            locker.unlock();
            // busy looping

            // Solution (1)
            // std::this_thread::sleep_for(std::chrono::milliseconds(10));
            // Cons:
            // time too short => busy looping
            // time too long => cannot get the data in time
        }
    }
}

int main()
{
    std::thread t1(function_1);
    std::thread t2(function_2_cond);
    t1.join();
    t2.join();
    return 0;
}