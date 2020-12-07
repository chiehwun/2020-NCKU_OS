#include <iostream>
#include <fstream>
#include <locale> // Print integer with thousands and millions separator
#include <iomanip>
#include <cstdlib>
#include <climits>
#include <ctime>
#include <random>
#include <string>
#define STR_SZ 128
using namespace std;
const string STR_SET = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
default_random_engine generator(time(NULL));
uniform_int_distribution<int64_t> unif_key(0, INT64_MAX);
uniform_int_distribution<int> unif_str(0, STR_SET.size() - 1);
/*
 * key: 0 - 9,223,372,036,854,775,807 (0-2^63-1) (64-bit)
 * value: char(128): a-z|A-Z|0-9
 */
void gen_put(fstream &ofs, long line);
int main(int argc, char *argv[])
{
    // cout.imbue(locale(""));
    // cout << "sizeof(int64_t) = " << sizeof(int64_t) << "\nrange: " << INT64_MIN << " - " << INT64_MAX << ".\n";
    string outfilename = "gen0.output";
    fstream ofs(outfilename, ios::out);
    if (!ofs)
    {
        cerr << "\"" << outfilename << "\" failed to open.\n";
        exit(EXIT_FAILURE);
    }
    gen_put(ofs, 1 << 24); // 1 Mi-line
    ofs.close();
    return 0;
}

// PUT [key] [value]
void gen_put(fstream &ofs, long line)
{
    string str = "";
    for (long l = 0; l < line; ++l)
    {
        for (int i = 0; i < STR_SZ; ++i)
            str += STR_SET[unif_str(generator)];
        ofs << "PUT " << unif_key(generator) << " " << str << (l == line - 1L ? "" : "\n");
        str = "";
    }
}

// GET [key]
void gen_get()
{
}

// SCAN [key1] [key2]
void gen_scan()
{
}