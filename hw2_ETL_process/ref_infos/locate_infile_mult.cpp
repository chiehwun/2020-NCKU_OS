#include <iostream>
#include <iomanip>
#include <locale>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
ifstream infs[FOPEN_MAX];
long getFileline(ifstream *inFile);
void locate_infs(int thread_num, long line_num);
typedef struct
{
    long line;
    long pos;
} Line_Pos;
vector<Line_Pos> LP_tab;

int main()
{
    for (int i = 0; i < FOPEN_MAX; ++i)
    {
        infs[i].open("../input_demo0.csv", ios::out);
        if (!infs[i])
        {
            cerr << "open file failure!\n";
            exit(EXIT_FAILURE);
        }
    }
    locate_infs(getFileline(&infs[0]), 3);
    // Close files
    for (int i = 0; i < FOPEN_MAX; ++i)
        infs[i].close();
    return 0;
}

void locate_infs(int thread_num, long line_num)
{
    long line, line_per_thd = line_num / thread_num + (line_num % thread_num == 0 ? 0 : 1);
    cout.imbue(locale(""));
    cout << left << setw(35) << "line_num:"
         << right << setw(16) << line_num << endl;
    cout << left << setw(35) << "thread_num:"
         << right << setw(16) << thread_num << endl;
    cout << left << setw(35) << "line_per_thd"
         << right << setw(16) << line_per_thd << endl;
    string str;
    infs[0].seekg(0, infs[0].beg);
    int thread = 0;
    for (line = 0; line < line_num; ++line)
    {
        if (line % line_per_thd == 0)
        {
            LP_tab.push_back(Line_Pos{.line = line, .pos = infs[0].tellg()});
            if (thread != 0) // infs[0] do not have to change
                infs[thread].seekg(LP_tab.back().pos);
            cout << LP_tab.back().line << ":" << LP_tab.back().pos << ":" << infs[thread].tellg() << endl;
            ++thread;
        }
        getline(infs[0], str);
    }
    // Reset pos of infs[0]
    infs[0].clear();
    infs[0].seekg(0, ios::beg);
    if (thread_num != thread)
    {
        cout << "locate_infs() failed!" << endl;
        return;
    }
    // Check
    for (int i = 0; i < thread_num; ++i)
    {
        cout << "thread" << i << ":\n";
        int line_end = line_per_thd;
        if (i == thread_num - 1 && line_num % line_per_thd != 0) // last thread end condition
            line_end = line_num % line_per_thd;
        for (line = 0; line < line_end; ++line)
        {
            getline(infs[i], str);
            cout << str << "\n";
        }
        cout << "\n";
    }

    infs[0].clear();
    infs[0].seekg(0, infs[0].beg);
}

long getFileline(ifstream *inFile)
{
    long line;
    string str;
    inFile->seekg(0, inFile->beg);
    for (line = 0; getline(*inFile, str); ++line)
        ;
    inFile->clear();
    inFile->seekg(0, inFile->beg);
    return line;
}