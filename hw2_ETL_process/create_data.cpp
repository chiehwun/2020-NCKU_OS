#include <iostream>
#include <locale> // Print integer with thousands and millions separator
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <random>
#include <string>
#include <climits>
#include <thread>
#include "progress_bar.h"
#include "Clock.h"

using namespace std;
void create_data_thread();
void create_data_backup();

// const long DATA_SZ = 1000;  // 500 B
// const long BF_WRT_SZ = 250; // 5 MiB
const long DATA_SZ = 1L << 30;        // 1 GiB
const long BF_WRT_SZ = (1 << 20) * 5; // 5 MiB
long dx = 0L;                         // Current file size
bool f1_runs = true;
string filename = "input.csv";
string buffer = "";
fstream outFile(filename, ios::out);
// random objects
default_random_engine generator(time(NULL));
uniform_int_distribution<int> unif(INT_MIN, INT_MAX);

int main(int argc, char *argv[])
{
    Clock clk;
    clk.start();

    create_data_backup();

    clk.stop();
    cout.imbue(locale(""));
    cout << left << setw(35) << "\"" + filename + "\" size: "
         << right << setw(16) << outFile.tellp() << " Bytes\n";
    cout << left << setw(35) << "EX. time of [create_data.cpp]:"
         << right << setw(16) << clk.getElapsedTime() << " ms\n";
    cout << left << setw(35) << "Writing avg. speed:"
         << fixed << setprecision(2) << right << setw(16)
         << (double)DATA_SZ / clk.getElapsedTime()
         << " MB/sec\n"; // Byte/ms = MB/s
    outFile.close();
    return 0;
}
void f1() // create one line of data
{
    while (outFile.tellp() < DATA_SZ)
    {
        for (int i = 0; i < 19; ++i)
            buffer.append(to_string(unif(generator)) + "|");
        buffer.append(to_string(unif(generator)) + "\n");
    }
}

void f2() // dump to outFile
{
    while (outFile.tellp() < DATA_SZ)
    {
        if (buffer.size() > BF_WRT_SZ)
        {
            // dx += buffer.size();
            outFile << buffer;
            buffer.clear();
            // progress_bar(dx, DATA_SZ);
            progress_bar(outFile.tellp(), DATA_SZ);
        }
    }
}

void create_data_thread()
{
    cout << "\n================== START GENERATING ==================\n";
    thread t1(f1);
    thread t2(f2);
    t1.join();
    t2.join();
    cout << "\n================== END GENERATING ==================\n";
}

void create_data_backup()
{
    cout << "\n================== START GENERATING ==================\n";
    // while (dx < DATA_SZ)
    while (outFile.tellp() < DATA_SZ)
    {
        // create one line of data
        for (int i = 0; i < 19; ++i)
            buffer.append(to_string(unif(generator)) + "|");
        buffer.append(to_string(unif(generator)) + "\n");
        // dump to outFile
        if (buffer.size() > BF_WRT_SZ)
        {
            // dx += buffer.size();
            outFile << buffer;
            buffer.clear();
            // progress_bar(dx, DATA_SZ);
            progress_bar(outFile.tellp(), DATA_SZ);
        }
    }
    cout << "\n================== END GENERATING ==================\n";
}