// g++ -pthread -o thread_tut03 thread_tut03.cpp && ./thread_tut03
// https://youtu.be/3ZxZPeXPaM4?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
Avoid Data Race:
1. Use mutex to synchronize data access.
2. Never leak a handle of data to outside.
3. Design interdace apprpriately.
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

std::mutex mu; // Create object
void shared_print(string msg, int id)
{
    mu.lock(); // lock the mutex => only allows one thread to access "cout"
    cout << msg << id << endl;
    mu.unlock(); // unlock the mutex
    // Comment:
    // If line16 throw exception,
    // "cout" would be locked permantly.
    // Solution below:
}

void shared_print_save(string msg, int id)
{
    std::lock_guard<std::mutex> guard(mu); //RAII
    cout << msg << id << endl;
    // Comment:
    // Pros. It always be unlocked with or without exception
    // Cons. "cout" (global var.) is not completely under the protection of mu
}

class LogFile
{
    std::mutex m_mutex;
    ofstream f;

public:
    LogFile()
    {
        f.open("log.txt");
    }
    ~LogFile()
    {
        f.close();
    }
    void shared_print_real(string id, int value)
    {
        std::lock_guard<std::mutex> guard(mu); //RAII
        f << id << value << endl;
    }
    // Never return f to the outside world
    ofstream &getStream()
    {
        // user can access f without mutex protection
        return f;
    }
    // Never pass f as an argument to user provided function
    void processf(void fun(ofstream &))
    {
        // user can access f without mutex protection
        fun(f);
    }
};

void shared_print(string msg, int id)
{
    mu.lock(); // lock the mutex => only allows one thread to access "cout"
    cout << msg << id << endl;
    mu.unlock(); // unlock the mutex
}

void function_1()
{
    for (int i = 0; i > -100; --i)
        shared_print("From thread: ", i);
    // cout << "From thread: " << i << endl;
}

void function_2(LogFile &log)
{
    for (int i = 0; i > -100; --i)
        log.shared_print_real("From t2: ", i);
}

int main()
{
    // (1)
    std::thread t1(function_1);
    for (int i = 0; i < 100; ++i)
    {
        shared_print("From main: ", i);
        // cout << "From main: " << i << endl;
    }

    // (2)
    LogFile log;
    std::thread t2(function_2, std::ref(log));
    for (int i = 0; i < 100; ++i)
        log.shared_print_real("From main: ", i);

    // Both threads are racing for the common resource "std::cout"

    t1.join();
    return 0;
}