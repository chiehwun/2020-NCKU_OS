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
#include <chrono>
#include "progress_bar.h"
#include "Clock.h"
#include <experimental/filesystem>

#define THREAD_MAX 100
#define COL_NUM 20
const long BF_RD_SZ = (1 << 10) * 5 * COL_NUM; // 5 KiB lines
// const long BF_WRT_SZ = (1 << 20) * 5;          // 5 MiB
long INFILE_SZ, INFILE_LINE, LINE_PER_THD;
ifstream inFile[THREAD_MAX];
ofstream thdFile[THREAD_MAX];
ofstream outFile;
ofstream logFile("log.log", std::ios::out);
const string INDENT = "    ";
const string INDENT2 = "        ";
typedef struct
{
    long line;
    long pos;
} Line_Pos;
vector<Line_Pos> LP_tab;
typedef struct
{
    long cur;
    long tot;
    string filename;
} PROG;
vector<PROG> THD_LOG;
void show_mult_prog(int thread_n)
{
    long cur, tot;
    do
    {
        cur = 0, tot = 0;
        for (int i = 0; i < thread_n; ++i)
        {
            cout << "[t" << i << ": " << setw(7) << fixed << setprecision(2)
                 << 100.0 * THD_LOG.at(i).cur / THD_LOG.at(i).tot << "%] ";
            cur += THD_LOG.at(i).cur;
            tot += THD_LOG.at(i).tot;
        }
        cout << "\r";
        cout.flush();
    } while (cur < tot);
    cout << endl;
}

long getFileline(ifstream *inFile)
{
    long i;
    string line;
    inFile->seekg(0, inFile->beg);
    for (i = 0; getline(*inFile, line); ++i)
        ;
    inFile->clear();
    inFile->seekg(0, inFile->beg);
    return i;
}
void single_threaded();
void multi_thread_shots(const int thread, const int thread_n, ifstream *infs, ofstream *outfs);
void multi_threaded(string inFilename, int thread_n);
bool concatenate(); // filename[] merge to "output.txt"

using namespace std;
namespace fs = experimental::filesystem;

int main(int argc, char *argv[])
{
    // Open input and output files
    const string inFilename = "input.csv";
    const string outFilename = "output.txt";
    try
    {
        fs::path inFilepath = fs::current_path() / inFilename;
        INFILE_SZ = fs::file_size(inFilepath);
        inFile[0].open(inFilename, ios::in);
        outFile.open(outFilename, ios::out);
        INFILE_LINE = getFileline(&inFile[0]);
        inFile[0].close();
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
    }

    // Read argument from terminal
    const int threads_num = atoi(argv[1]);
    if (threads_num <= 0 || threads_num > THREAD_MAX)
    {
        cerr << "illegal thread_num! (0~" << THREAD_MAX << ")" << endl;
        exit(EXIT_FAILURE);
    }
    cout << "\n============ Analyzing \"" << inFilename << "\" ============\n";
    cout.imbue(locale(""));
    cout << left << setw(35) << "input file size:"
         << right << setw(16) << INFILE_SZ << " Bytes\n";
    cout << left << setw(35) << "input file #line:"
         << right << setw(16) << INFILE_LINE << "\n";
    cout << "\n=============== Threads Settings ==============\n";
    cout << left << setw(35) << "threads_num:"
         << right << setw(16) << threads_num << "\n";
    cout << left << setw(35) << "FOPEN_MAX:"
         << right << setw(16) << FOPEN_MAX << "\n";
    cout << left << setw(35) << "BF_RD_SZ:"
         << right << setw(16) << BF_RD_SZ << "\n";
    Clock clk;
    clk.start();
    multi_threaded(inFilename, threads_num);
    // single_threaded();
    clk.stop();
    fs::path outFilepath = fs::current_path() / outFilename;
    cout << left << setw(35) << "EX. time of [converter.cpp]:"
         << right << setw(16) << clk.getElapsedTime() << " ms\n";
    cout << left << setw(35) << "\"" + outFilename + "\" size:"
         << right << setw(16) << fs::file_size(outFilepath) << " Bytes\n";
    return 0;
}

void locate_infs(int thread_n)
{
    LINE_PER_THD = INFILE_LINE / thread_n + (INFILE_LINE % thread_n == 0 ? 0 : 1);
    cout << left << setw(35) << "LINE_PER_THD:"
         << right << setw(16) << LINE_PER_THD << endl;
    string str;
    int thread = 0;
    long line;
    for (line = 0; line < INFILE_LINE; ++line)
    {
        if (line % LINE_PER_THD == 0)
        {
            LP_tab.push_back(Line_Pos{.line = line, .pos = inFile[0].tellg()});
            if (thread != 0) // inFile[0] do not have to change
                inFile[thread].seekg(LP_tab.back().pos);
            cout << LP_tab.back().line << ":" << LP_tab.back().pos << ":" << inFile[thread].tellg() << endl;
            ++thread;
        }
        getline(inFile[0], str);
    }
    // Reset pos of inFile[0]
    inFile[0].clear();
    inFile[0].seekg(0, ios::beg);

    // Check
    // for (int i = 0; i < thread_n; ++i)
    // {
    //     cout << "thread" << i << ":\n";
    //     int line_end = LINE_PER_THD;
    //     if (i == thread_n - 1 && INFILE_LINE % LINE_PER_THD != 0) // last thread end condition
    //         line_end = INFILE_LINE % LINE_PER_THD;
    //     for (line = 0; line < line_end; ++line)
    //     {
    //         getline(inFile[i], str);
    //         cout << str << "\n";
    //     }
    //     cout << "\n";
    // }
    // inFile[0].clear();
    // inFile[0].seekg(0, inFile[0].beg);
}

void multi_threaded(string inFilename, int thread_n)
{
    THD_LOG = vector<PROG>(thread_n, PROG{0L, 0L, ""});
    // Open threads input file
    for (int i = 0; i < thread_n; ++i)
    {
        inFile[i].open(inFilename, ios::out);
        if (!inFile[i])
        {
            cerr << "open file failure!\n";
            exit(EXIT_FAILURE);
        }
    }
    // Multi-threaded conversion
    locate_infs(thread_n);
    cout << "\n=============== Multi-threaded conversion ===============\n";
    // // Test by sequence
    // for (int i = 0; i < thread_n; ++i)
    // {
    //     multi_thread_shots(i, thread_n, &inFile[i], &thdFile[i]);
    // }

    thread shot[thread_n];
    for (int i = 0; i < thread_n; ++i)
    {
        shot[i] = thread(multi_thread_shots, i, thread_n, &inFile[i], &thdFile[i]);
    }
    this_thread::sleep_for(chrono::milliseconds(1000)); // wait for 1 sec to launch monitor
    thread monitor(show_mult_prog, thread_n);

    for (int i = 0; i < thread_n; ++i)
    {
        shot[i].join();
    }
    monitor.join();

    concatenate();
}

void multi_thread_shots(const int thread, const int thread_n, ifstream *infs, ofstream *outfs)
{
    // Open thread output files
    string thdFilename = to_string(thread) + ".thdout";
    THD_LOG.at(thread).filename = thdFilename;
    outfs->open(thdFilename, ios::out);
    if (!(*outfs))
    {
        cerr << "\"" << thdFilename << "\" failed to open!\n";
        abort();
    }
    int buffer[BF_RD_SZ] = {};
    long line = 0, size = 0, line_end = LINE_PER_THD;
    // last line end condition
    if (thread == thread_n - 1 && INFILE_LINE % LINE_PER_THD != 0)
        line_end = INFILE_LINE % LINE_PER_THD;
    THD_LOG.at(thread).tot = line_end;
    while (line < line_end)
    {
        // DISK -> MEM
        size = 0;
        while (size < BF_RD_SZ && size / COL_NUM < line_end && (*infs) >> buffer[size])
        {
            infs->ignore();
            ++size;
        }
        line += size / COL_NUM;
        // MEM -> DISK
        for (long ix = 0; ix < size;)
        {
            (*outfs) << INDENT << "{\n";
            // [1:19]
            for (int j = 1; j < COL_NUM; ++j)
                (*outfs) << INDENT2 << "\"col_" << j
                         << "\":" << buffer[ix++] << ",\n";
            // [20]
            (*outfs) << INDENT2 << "\"col_" << COL_NUM
                     << "\":" << buffer[ix++] << "\n"
                     << INDENT << "}"
                     << (thread == thread_n - 1 && ix == size && line == line_end ? "\n" : ",\n");
            // last thread & last element in buffer & last line (last group of dump)
        }
        THD_LOG.at(thread).cur = line; // Pass progress to glob. var.
    }
    // cout << "\n=============== End of conversion t" << thread << " ===============\n";
    // Close all files
    infs->close();
    outfs->close();
}

bool concatenate() // filename[] merge to "output.txt"
{
    for (int i = 0; i < THD_LOG.size(); ++i)
        cout << setw(10) << THD_LOG.at(i).filename << ": " << THD_LOG.at(i).cur << "/" << THD_LOG.at(i).tot << endl;
    if (THD_LOG.size() > THREAD_MAX)
    {
        cerr << "file number exceeds! " << THD_LOG.size() << " > " << THREAD_MAX << endl;
        return false;
    }
    ifstream in[THREAD_MAX];
    ofstream out;
    for (int i = 0; i < THD_LOG.size(); ++i)
    {
        // cout << THD_LOG.at(i).filename << endl;
        in[i].open(THD_LOG.at(i).filename, ios_base::binary);
        if (!in[i])
        {
            cerr << "\"" << THD_LOG.at(i).filename << "\""
                 << " failed to open123123!\n";
            return false;
        }
    }
    out.open("output.txt", ios_base::binary);
    if (!out)
    {
        cerr << "\"output.txt\" failed to open!\n";
        return false;
    }
    cout << "Concatenate Files...";
    out << "[\n";
    for (int i = 0; i < THD_LOG.size(); ++i)
    {
        out << in[i].rdbuf();
        in[i].close();
    }
    out << "]";
    out.close();
    cout << "\nComplete!\n";
    return true;
}

void single_threaded()
{
    // Single-threaded conversion
    cout << "\n=============== Single-threaded conversion ===============\n";
    int buffer[BF_RD_SZ] = {};
    string buf_out = "";
    outFile << "[\n";
    long line = 0, size = 0;
    // while (inFile[0].peek() != EOF)
    while (line < INFILE_LINE)
    {
        // DISK -> MEM
        size = 0;
        while (size < BF_RD_SZ && inFile[0] >> buffer[size])
        {
            inFile[0].ignore();
            ++size;
        }

        line += size / COL_NUM /* + (size % COL_NUM > 0 ? 1 : 0)*/;
        // MEM -> DISK
        for (long ix = 0; ix < size;)
        {
            // buf_out.append("{\n");
            outFile << INDENT << "{\n";
            // [1:19]
            for (int j = 1; j < COL_NUM; ++j)
                // buf_out.append(INDENT2 + "\"col_" + to_string(j) + "\":" + to_string(buffer[ix++]) + ",\n");
                outFile << INDENT2 << "\"col_" << j
                        << "\":" << buffer[ix++] << ",\n";
            // [20]
            // buf_out.append(INDENT2 + "\"col_" + to_string(COL_NUM) + "\":" + to_string(buffer[ix++]) + "\n");
            // buf_out.append(INDENT + "}" + (ix == size && line == INFILE_LINE ? "\n" : ",\n"));
            outFile << INDENT2 << "\"col_" << COL_NUM
                    << "\":" << buffer[ix++] << "\n"
                    << INDENT << "}" << (ix == size && line == INFILE_LINE ? "\n" : ",\n");
        }
        // outFile << buf_out;
        // buf_out.clear();
        logFile << size << ":" << line << endl;
        progress_bar(line, INFILE_LINE);
    }
    outFile << "]";
    cout << "\n=============== End of conversion ===============\n";

    // Close all files
    logFile.close();
    inFile[0].close();
    outFile.close();
}