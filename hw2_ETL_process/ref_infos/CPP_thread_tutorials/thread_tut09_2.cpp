// g++ -pthread -o thread_tut09_2 thread_tut09_2.cpp && ./thread_tut09_2
// https://youtu.be/FfbZfBk-3rI?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
3 ways to get a future
 - promise: get_future
 - packaged_task::get_future()
 - async() returns a future
*/
#include <iostream>
#include <string>
#include <deque>
#include <functional> // std::bind()
#include <thread>
#include <condition_variable> // Prevent wasting resource
#include <mutex>              // Solve the race condition
// by synchronizing the access of the common resource
#include <future>

using std::condition_variable;
using std::cout;
using std::deque;
using std::endl;
using std::future;
using std::mutex;
using std::ofstream;
using std::packaged_task;
using std::string;
using std::thread;
using std::unique_lock;

int factorial(int N)
{
    int res = 1;
    for (int i = N; i > 1; --i)
        res *= i;
    cout << "Result is: " << res << endl;
    return res;
}

// Global variable
deque<packaged_task<int()>> task_q;
mutex mu;
condition_variable cond;

void thread_1()
{
    packaged_task<int()> t;
    { // protect "task_q"
        unique_lock<mutex> locker(mu);
        cond.wait(locker, []() { return !task_q.empty(); });
        t = std::move(task_q.front());
        task_q.pop_front();
    }
    t();
}

int main()
{
    thread t1(thread_1);
    packaged_task<int()> t(std::bind(factorial, 6));
    future<int> fu = t.get_future();
    { // protect "task_q"
        unique_lock<mutex> locker(mu);
        task_q.push_back(std::move(t));
    }
    cond.notify_one();

    cout << "at main: " << fu.get() << endl;
    t1.join();
    return 0;
}