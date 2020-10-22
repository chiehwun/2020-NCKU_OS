// g++ -pthread -o thread_tut09 thread_tut09.cpp && ./thread_tut09
// https://youtu.be/FfbZfBk-3rI?list=PL5jc9xFGsL8E12so1wlMS0r0hTQoJL74M
/* OUTLINE
1.  packaged_task:
    It can link a callable object to a future
*/
#include <iostream>
#include <string>
#include <future>
#include <functional> // std::bind()

using std::cout;
using std::endl;
using std::ofstream;
using std::string;

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
    std::packaged_task<int(int)> t(factorial);
    // std::bind(factorial, 6): is a function object
    std::packaged_task<int()> t2_binded(std::bind(factorial, 6));

    // Executed these in a different context
    t(6);
    t2_binded();
    int x = t.get_future().get();
    cout << "t's future returns to main: " << x << endl;
    return 0;
}