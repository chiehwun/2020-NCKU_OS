// g++ -pthread -o thread_tut07_2 thread_tut07_2.cpp && ./thread_tut07_2
// https://youtu.be/nU18p75u1oQ?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
Using callable object
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

class A
{
public:
    void f(int x, char c)
    {
    }
    long g(double x) { return 0; }
    int operator()(int N) { return 0; }
};

void foo(int x) {}

int main()
{
    A a;
    std::thread t1(a, 6);                           // copy_of_a() in a different thread
    std::thread t2(std::ref(a), 6);                 // a() in a different thread
    std::thread t8(std::move(a), 6);                // a() is no longer usable in main thread
    std::thread t3(A(), 6);                         // temp A
    std::thread t4([](int x) { return x * x; }, 6); // lambda function
    std::thread t5(foo, 7);
    std::thread t6(&A::f, a, 8, 'w');  // copy_of_a.f(8, 'w') in a different thread
    std::thread t6(&A::f, &a, 8, 'w'); // a.f(8, 'w') in a different thread

    // std::bind(a, 6);
    // std::async();
    // std::call_once(once_flag, a, 6);
    // ...
    return 0;
}