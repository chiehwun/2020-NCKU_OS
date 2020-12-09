#include <iostream>
#include <cstdlib>
#include "BPTree.h"
#include "Clock.h"
#include "Mem_Sys_Set.h"

using namespace std;

int main()
{
    Node n(3);
    cout << "sizeof(n.key):\t\t" << sizeof(n.key) << " bytes" << endl;
    cout << "sizeof(n.str):\t\t" << sizeof(n.str) << " bytes" << endl;
    cout << "sizeof(n.size):\t\t" << sizeof(n.size) << " bytes" << endl;
    cout << "sizeof(n.ptr):\t\t" << sizeof(n.ptr) << " bytes" << endl;
    cout << "sizeof(n.IS_LEAF):\t" << sizeof(n.IS_LEAF) << " bytes" << endl;
    cout << "sizeof(n):\t\t" << sizeof(n) << " bytes" << endl;
    char a[3] = "ab";
    a[2] = 'c';
    cout << a << endl;
    string str(a);
    cout << str << endl;
    return 0;
}