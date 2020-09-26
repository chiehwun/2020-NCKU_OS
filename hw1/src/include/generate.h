#ifndef GENERATE_H
#define GENERATE

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <cstdlib>
#include <random>
#include <climits>
#include <libgen.h> // dirname
#include <unistd.h> // readlink
#include "progress_bar.h"

using namespace std;

string getpath() // get user's execution path (Ubuntu)
{
    char buf[PATH_MAX + 1];
    if (readlink("/proc/self/exe", buf, sizeof(buf) - 1) == -1)
        throw std::string("readlink() failed");
    std::string str(buf);
    return str.substr(0, str.rfind('/'));
}

long generate(string filename, int mode = 0, int default_size = 500) // default_size (MB)
{
    // Read meminfo from "/proc/meminfo" (Ubuntu)
    long int memTot, memFree;
    string s1, s2;
    ifstream meminfo("/proc/meminfo", ios::in);
    for (int i = 0; i < 2; ++i)
    {
        meminfo >> s1 >> (i == 0 ? memTot : memFree) >> s2;
        cout << s1 << "\t" << (i == 0 ? memTot : memFree) << "\t" << s2 << endl;
    }
    meminfo.close();

    // File Opening
    string filePath = getpath() + "/test_files/" + filename;
    ofstream outFile(filePath, ios::out);
    if (!outFile)
    {
        cerr << "Fail opening \"" << filePath << "\"\n";
        exit(EXIT_FAILURE);
    }

    // random generator
    srand(time(0));
    default_random_engine generator(time(NULL));
    // random distribution
    uniform_int_distribution<int> unif(INT_MIN, INT_MAX);

    // Generate Test Data
    memTot <<= 10; // change KiB to Bytes

    /*********** Toggle File size by mode ************/
    const long Filesize = (mode == 1 ? memTot : default_size << 20); // byte

    cout << "MemTotal:\t" << memTot << "\tbyte" << endl;
    cout << "File size:\t" << Filesize << "\tbyte" << endl;
    string buffer = ""; // Use string type as buffer

    /********* Choose Proper size of buffer *********/
    const unsigned int buffer_size = 1 << 20;

    cout << "buffer.max_size():\t" << buffer.max_size() << endl;
    cout << "buffer.size():\t\t" << buffer_size << endl;
    cout << "\n================== START GENERATING ==================\n";
    while (outFile.tellp() < Filesize)
    {
        buffer.append(to_string(unif(generator)) + "\n");
        if (buffer.size() > buffer_size)
        {
            outFile << buffer;
            progress_bar((float)outFile.tellp() / Filesize, 100,
                         outFile.tellp(), Filesize);
            buffer.clear();
        }
    }
    if (!buffer.empty())
    {
        outFile << buffer;
        buffer.clear();
    }
    cout << "\n================== END GENERATING ==================\n";

    cout << "\n\n\"" << filename << "\" size: " << outFile.tellp() << " bytes\n";
    return outFile.tellp();
}

#endif // GENERATE_H