#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstdlib>

using namespace std;

void func(ostream &cso, int num)
{
    cso << "func(" << num << ").\n";
}

int main()
{
    string log, log_prev;
    ostringstream ss;
    for (int i = 0; i < 3; ++i)
    {
        log_prev = log;
        func(ss, i);
        // ostringstream debug code
        log = ss.str();
        ss.str(string());
    }
    cout << log;
    return 0;
}