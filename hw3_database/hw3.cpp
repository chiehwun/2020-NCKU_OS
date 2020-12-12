#include "db.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Wrong input format:\n"
             << "./hw3.out [.input_file]\n";
        exit(EXIT_FAILURE);
    }
    string filename(argv[1]);
    fstream infs(filename, ios::in);
    if (!infs)
    {
        cerr << "\"" << filename << "\n not exits!\n";
        exit(EXIT_FAILURE);
    }
    cout << filename << endl;
    string instr, str;
    int64_t key0, key1;
    long instr_cnt = 1;

    while (infs >> instr)
    {
        if (instr.compare("PUT") == 0)
        {
            infs >> key0 >> str;
            cout << "[" << instr_cnt << "] PUT " << key0 << ", " << str;
        }
        else if (instr.compare("GET") == 0)
        {
            infs >> key0;
            cout << "[" << instr_cnt << "] GET " << key0;
        }
        else if (instr.compare("SCAN") == 0)
        {
            infs >> key0 >> key1;
            cout << "[" << instr_cnt << "] SCAN " << key0 << ", " << key1;
        }
        else
        {
            cerr << "instruction: \"" << instr << "\" error!\n";
            exit(EXIT_FAILURE);
        }
        ++instr_cnt;
        cout << endl;
    }

    return 0;
}