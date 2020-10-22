// g++ -pthread -o thread_tut07_2 thread_tut07_2.cpp && ./thread_tut07_2
// https://youtu.be/SZQ6-pf-5Us?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
1. shared_future -> Broadcast kind of communication model
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

int factorial(std::shared_future<int> sf)
{
    int res = 1;
    int N = sf.get(); // exception: std::future_errc::broken_promise
    for (int i = N; i > 1; --i)
        res *= i;
    cout << "Result is: " << res << endl;
    return res;
}

int main()
{
    std::promise<int> p; // pass argument from parent to child thread
    std::future<int> f = p.get_future();
    std::shared_future<int> sf = f.share();
    // Broadcast kind of communication model
    std::future<int> fu1 = std::async(std::launch::async, factorial, sf);
    std::future<int> fu2 = std::async(std::launch::async, factorial, sf);
    std::future<int> fu3 = std::async(std::launch::async, factorial, sf);

    p.set_value(4);

    // fu.get(); // crashed!
    return 0;
}