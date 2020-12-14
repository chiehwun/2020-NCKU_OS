#include "db.h"
#include "Clock.h"
// #define HW3_DB
using namespace std;
ofstream OFS;
bool FIRST_PRINT = true;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Wrong input format:\n"
             << "./hw3.out [./*.input_file]\n";
        exit(EXIT_FAILURE);
    }
    string infilename(argv[1]);
    string outfilename(infilename);
    // erase ".input" at last
    size_t found = outfilename.find_last_of("/");
    if (found != string::npos)
        outfilename.erase(outfilename.begin(), outfilename.begin() + found + 1);
    outfilename.erase(outfilename.end() - 6, outfilename.end());
    outfilename += ".output";
    cerr << "infilename : " << infilename << endl;
    cerr << "outfilename: " << outfilename << endl;
    ifstream infs(infilename, ios::in);
    if (!infs)
    {
        cerr << "main(): Cannot find \"" << infilename << "\"!\n";
        exit(EXIT_FAILURE);
    }
    // Data base init. and parameters settings
    int entry = (1 << 12) / sizeof(Data);             // assume 4KB/block
    int64_t block = (1 << 30) / sizeof(Data) / entry; // 1GB per file
    cerr << "entry: " << entry << endl;
    cerr << "block: " << block << endl;
    DB data_base(block, entry);
    data_base.DBinit();

    // /********************************************************************************/
    // Clock clk;
    // clk.start();
    // int64_t N = 10000000;
    // for (int64_t i = 1; i <= N; ++i)
    // {
    //     data_base.get(i);
    //     FIRST_PRINT = false;
    //     data_base.put(i, to_string(i));
    // }
    // data_base.DBsave();
    // clk.stop();
    // cout << left << setw(35) << "PUT instr. num:"
    //      << right << setw(16) << N << "\n";
    // cout << left << setw(35) << "EX. time of [hw3.cpp]:"
    //      << right << setw(16) << clk.getElapsedTime() << " ms\n";

    // return 0;
    // /********************************************************************************/

    string instr, str;
    int64_t key0, key1;
    long instr_cnt = 1;
    while (infs >> instr)
    {
        if (instr.compare("PUT") == 0)
        {

            infs >> key0 >> str;
            data_base.put(key0, str);
#ifdef HW3_DB
            cerr << "[" << instr_cnt << "] PUT" << endl;
#endif
        }
        else if (instr.compare("GET") == 0)
        {
            if (FIRST_PRINT)
            {
                OFS.open(outfilename, ios::out);
                if (!OFS)
                {
                    cerr << "main(): Cannot open \"" << outfilename << "\"!\n";
                    exit(EXIT_FAILURE);
                }
            }
            infs >> key0;
            data_base.get(key0);
            FIRST_PRINT = false;
#ifdef HW3_DB
            cerr << "[" << instr_cnt << "] GET" << endl;
#endif
        }
        else if (instr.compare("SCAN") == 0)
        {
            if (FIRST_PRINT)
            {
                OFS.open(outfilename, ios::out);
                if (!OFS)
                {
                    cerr << "main(): Cannot open \"" << outfilename << "\"!\n";
                    exit(EXIT_FAILURE);
                }
            }
            infs >> key0 >> key1;
            data_base.scan(key0, key1);
            FIRST_PRINT = false;
#ifdef HW3_DB
            cerr << "[" << instr_cnt << "] SCAN" << endl;
#endif
        }
        else
        {
            cerr << "instruction: \"" << instr << "\" error!\n";
            exit(EXIT_FAILURE);
        }
        ++instr_cnt;
    }
    data_base.DBsave();
    infs.close();
    OFS.close();

    return 0;
}