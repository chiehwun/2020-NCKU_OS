// g++ -pthread -o thread_tut10 thread_tut10.cpp && ./thread_tut10
// https://youtu.be/3wpBk5Y3gfk?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
*/
#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <condition_variable> // Prevent wasting resource
#include <mutex>              // Solve the race condition
#include <future>
#include <functional> // std::bind()

using std::async;
using std::condition_variable;
using std::cout;
using std::endl;
using std::future;
using std::lock_guard;
using std::mutex;
using std::ofstream;
using std::packaged_task;
using std::promise;
using std::string;
using std::thread;
using std::unique_lock;
// Global variables

int factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; --i)
        res *= i;
    cout << "Result is: " << res << endl;
    return res;
}

int main()
{
    /* thread */
    thread t1(factorial, 6);
    std::this_thread::sleep_for(std::chrono::milliseconds(3));

    std::chrono::steady_clock::time_point tp =
        std::chrono::steady_clock::now() + std::chrono::microseconds(4);
    std::this_thread::sleep_until(tp);

    /* Mutex */
    mutex mu;
    lock_guard<mutex> locker(mu);
    unique_lock<mutex> ulocker(mu);

    ulocker.try_lock();
    ulocker.try_lock_for(std::chrono::nanoseconds(500));
    ulocker.try_lock_until(tp);

    /* Condition Variable */
    // Synchronize the execution order of threads
    condition_variable cond;

    cond.wait(ulocker, std::chrono::microseconds(2));
    cond.wait(ulocker, tp);

    /* Future and Promise */
    promise<int> p;
    future<int> f = p.get_future();

    f.get();
    f.wait();
    f.wait_for(std::chrono::milliseconds(2));
    f.wait_until(tp);

    /* async() */
    future<int> fu = async(factorial, 6);

    /* Packaged Task */
    packaged_task<int(int)> t(factorial);
    future<int> fu2 = t.get_future();
    t(6);
    return 0;
}