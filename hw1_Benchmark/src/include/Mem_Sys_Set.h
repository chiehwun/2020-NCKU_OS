#ifndef MEM_SYS_SET_H
#define MEM_SYS_SET_H
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <string>
#include <libgen.h> // dirname
#include <unistd.h> // readlink
#include <climits>
#include <cctype>  // for read_arg()
#include <clocale> // Print integer with thousands and millions separator

using namespace std;
extern char *optarg;
extern int optind, opterr, optopt;
int getopt(int argc, char const *argv[], const char *optstring);
long getMemInfo(int mode = 0, bool show = false); // mode: [0,free][1,tot]
string getpath();

// Parameters Modify
#define MEM_SF 0.8                     // memory safety factor 0.8
const long BF_WRT_SZ = (1 << 20) * 10; // 10 MiB
size_t FREE_MEM = getMemInfo(0, true);
string LOC_PATH = getpath();

template <typename P>
bool isAllocated(P pt, string var = "")
{
    if (pt == NULL)
    {
        fprintf(stderr, "%s allocated fail!\n", var.c_str());
        exit(EXIT_FAILURE);
        return false;
    }
    return true;
}

string getpath() // get user's execution path (Ubuntu)
{
    char buf[PATH_MAX + 1];
    if (readlink("/proc/self/exe", buf, sizeof(buf) - 1) == -1)
        throw string("readlink() failed");
    string str(buf);
    return str.substr(0, str.rfind('/'));
}

FILE *openFile(const char *fileName, const char *mode)
{
    FILE *fp = fopen(fileName, mode);
    if (fp == NULL)
    {
        fprintf(stderr, "Error while opening \"%s\".\n", fileName);
        exit(EXIT_FAILURE);
    }
    return fp;
}
// Read meminfo from "/proc/meminfo" (Ubuntu)
long getMemInfo(int mode, bool show) // mode: [0,free][1,tot]
{
    long memTot, memFree;
    // C++ I/O
    string s1, s2;
    ifstream meminfo("/proc/meminfo", ios::in);
    for (int i = 0; i < 2; ++i)
    {
        meminfo >> s1 >> (i == 0 ? memTot : memFree) >> s2;
        if (show)
        {
            cout.imbue(locale("")); // Print integer with thousands and millions separator
            cout << left << setw(15)
                 << s1 << right << setw(16)
                 << (i == 0 ? memTot : memFree)
                 << "\t" << s2 << endl;
        }
    }
    meminfo.close();
    // // C I/O
    // char s1[2][15], s2[2][5];
    // FILE *meminfo = openFile("/proc/meminfo", "r");
    // fscanf(meminfo, "%s\t%ld\t%s\n", s1[0], &memTot, s2[0]);
    // fscanf(meminfo, "%s\t%ld\t%s\n", s1[1], &memFree, s2[1]);
    // if (show)
    // {
    //     printf("%s\t%ld\t\t%s\n", s1[0], memTot, s2[0]);
    //     printf("%s\t%ld\t\t%s\n", s1[1], memFree, s2[1]);
    // }
    // fclose(meminfo);
    return mode == 0 ? memFree : memTot;
}

void read_arg(int argc, char *argv[], const string argsets, bool modes[])
{
    int cmd_opt = 0;
    while ((cmd_opt = getopt(argc, argv, argsets.c_str())) != -1)
    {
        int narg = 0;
        // Lets parse
        switch (cmd_opt)
        {
        // No args
        case 's':
        case 'g':
        case 'i':
        case 't':
            narg = argsets.find((char)cmd_opt);
            cerr << '-' << argsets[narg] << ' ';
            modes[narg] = true;
            break;

        // Single arg ":"
        // case 'c':
        // case 'd':
        //     cerr << "option arg \'" << (char)cmd_opt << "\': " << optarg << endl;
        //     break;

        // Optional args "::"
        // case 'e':
        //     if (optarg)
        //         cerr << "option arg:" << optarg << endl;
        //     break;

        // Error handle: Mainly missing arg (Single arg ":") or illegal option
        case '?':
            cerr << "Illegal option:-" << (isprint(optopt) ? optopt : '#') << endl;
            break;
        default:
            cerr << "Not supported option\n";
            break;
        }
        // cerr << "process index:" << optind << endl; // next pointer
    }

    cerr << endl;
    // // Do we have args?
    // if (argc > optind)
    //     for (int i = optind; i < argc; ++i)
    //         cerr << "argv[" << i << "] = " << argv[i] << endl;
}
#endif // MEM_SYS_SET_H