// g++ -pthread -o thread_tut07 thread_tut07.cpp && ./thread_tut07
// https://youtu.be/SZQ6-pf-5Us?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
1. promise
2. future
*/
#include <iostream>
#include <string>
#include <thread>
#include <condition_variable> // Prevent wasting resource
#include <mutex>              // Solve the race condition
                              // by synchronizing the access of the common resource
#include <future>

using std::cout;
using std::endl;
using std::ofstream;
using std::string;

// Global variables

int factorial(std::future<int> &f)
{
    int res = 1;
    int N = f.get(); // exception: std::future_errc::broken_promise
    for (int i = N; i > 1; --i)
        res *= i;
    cout << "Result is: " << res << endl;
    return res;
}

int main()
{
    std::promise<int> p; // pass argument from parent to child thread
    std::future<int> f = p.get_future();
    // a channel to get the result from child thread
    std::future<int> fu = std::async(std::launch::async, factorial, std::ref(f));
    // First parameters of std::async()
    // std::launch::deferred                        // launch as function
    // std::launch::async                           // launch as thread
    // std::launch::async | std::launch::deferred   // depends (default value)

    // do something else
    std::this_thread::sleep_for(std::chrono::milliseconds(20));
    // (1) Fulfill the promise p
    // p.set_value(4);
    // (2) Break the promise p
    p.set_exception(std::make_exception_ptr(std::runtime_error("Break the promise p")));

    // wait until the child thread finish and return the value from child thread
    // can be called only once!
    int x = fu.get();
    cout << "Get from child: " << x << endl;
    // fu.get(); // crashed!
    return 0;
}