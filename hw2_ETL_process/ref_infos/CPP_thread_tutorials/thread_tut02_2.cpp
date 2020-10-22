// g++ -pthread -o thread_tut02_2 thread_tut02_2.cpp && ./thread_tut02_2
// https://youtu.be/f2nMqNj7vxE?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
#include <iostream>
#include <string>
#include <thread>
using std::cout;
using std::endl;
using std::string;

class Fctor
{
public:
    void operator()(string msg)
    // void operator()(string &msg)
    {
        cout << "thread says: " << msg << endl;
        msg = "Message has been changed.";
    }
};

int main()
{
    string s = "A Message.";
    // (1) Parameters to thread is always PASS BY VALUE!!!!
    // Inefficient and there are somethings that can not be passed by value
    // 18: void operator()(string msg)
    // std::thread t1((Fctor()), s); // t1 starts running

    // (2) To pass by reference, use "std::ref()" -> reference wrapper
    // 18: void operator()(string &msg)
    // std::thread t2((Fctor()), std::ref(s)); // t2 starts running

    // (3) Pass by "std::move()"
    // 18: void operator()(string &msg)
    // this will move s from main thread to the t3 => safe and efficient
    std::thread t3((Fctor()), std::move(s)); // t3 starts running

    // (4) To move a thread
    std::thread t3_moved = std::move(t3); // t3 will be deleted

    // t1.join();
    // t2.join();
    t3_moved.join();

    cout << "from main(): " << s << endl;

    // (5) thread id
    cout << "main thread id: " << std::this_thread::get_id() << endl;
    // (6) Oversubscription
    // Too many context switching will degrade performance
    cout << "hardware_concurrency(): " << std::thread::hardware_concurrency() << endl; // Indication
    return 0;
}