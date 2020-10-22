#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char *argv[])
{
    int i = 0;
    ofstream *f;
    do
    {
        f = new ofstream(to_string(i++));
    } while (*f << "hello" << flush);
    --i; // Don't count last iteration, which failed to open anything.
    cout << i << '\n';
}