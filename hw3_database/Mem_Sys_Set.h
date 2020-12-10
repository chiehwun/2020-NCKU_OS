#ifndef MEM_SYS_SET_H
#define MEM_SYS_SET_H
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <string>
#include <libgen.h> // dirname
#include <unistd.h> // readlink
#include <climits>
#include <clocale> // Print integer with thousands and millions separator

using namespace std;
long getMemInfo(int mode = 0, bool show = false); // mode: [0,free][1,tot]

// Parameters Modify
#define MEM_SF 0.8                     // memory safety factor 0.8
const long BF_WRT_SZ = (1 << 20) * 10; // 10 MiB
size_t FREE_MEM = getMemInfo(0, true);

// Read meminfo from "/proc/meminfo" (Ubuntu)
// mode: [0,free][1,tot]
long getMemInfo(int mode, bool show)
{
    long memTot, memFree;
    string s1, s2;
    ifstream meminfo("/proc/meminfo", ios::in);
    for (int i = 0; i < 2; ++i)
    {
        meminfo >> s1 >> (i == 0 ? memTot : memFree) >> s2;
        if (show)
        {
            cout.imbue(locale(""));
            cout << left << setw(15)
                 << s1 << right << setw(16)
                 << (i == 0 ? memTot : memFree)
                 << "\t" << s2 << endl;
        }
    }
    meminfo.close();
    return mode == 0 ? memFree : memTot;
}

#endif // MEM_SYS_SET_H