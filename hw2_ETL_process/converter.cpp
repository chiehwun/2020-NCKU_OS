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
#include <mutex>
#include <condition_variable>
#include <chrono>
#include "progress_bar.h"
#include "Clock.h"
#include <experimental/filesystem>

#define THREAD_MAX 100
#define COL_NUM 20

using namespace std;
namespace fs = experimental::filesystem;

const long BF_RD_SZ = (1 << 10) * 5 * COL_NUM; // 5 KiB lines
// const long BF_WRT_SZ = (1 << 20) * 5;          // 5 MiB
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
    long line_cur;
    long line_tot;
    string filename;
} Prog;

long getFilesize(const string filename)
{
    try
    {
        fs::path filepath = fs::current_path() / filename;
        return fs::file_size(filepath);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        exit(EXIT_FAILURE);
        return -1L;
    }
}

long getFileline(const string filename)
{
    ifstream inFile(filename, ios::in);
    if (!inFile)
    {
        cerr << "getFileline(): \"" << filename << "\" failed to open.\n";
        return -1L;
    }
    long line;
    string str;
    for (line = 0; getline(inFile, str); ++line)
        ;
    inFile.close();
    return line;
}
void single_threaded();
void multi_threaded(string inFilename, string outFilename, const int thread_n, long infile_line_tot);
bool concatenate(vector<Prog> &thd_log, string outFilename);
bool concatenate_mult(vector<Prog> &thd_log, string outFilename);

int main(int argc, char *argv[])
{
    vector<Prog> thd_log;
    concatenate_mult(thd_log, "123");
    return 0;
    // Define input and output files
    const string inFilename = "input.csv";
    const string outFilename = "output.txt";
    const long infile_size = getFilesize(inFilename);
    const long infile_line_tot = getFileline(inFilename);

    // Read argument from terminal
    if (argc < 2)
    {
        cerr << "Please enter a thread number! (1 ~ " << THREAD_MAX << ")\n";
        exit(EXIT_FAILURE);
    }
    const int threads_num = atoi(argv[1]);
    if (threads_num <= 0 || threads_num > THREAD_MAX)
    {
        cerr << "illegal thread number! (1 ~ " << THREAD_MAX << ")\n";
        exit(EXIT_FAILURE);
    }
    cout << "\n============ Analyzing \"" << inFilename << "\" ============\n";
    cout.imbue(locale(""));
    cout << left << setw(35) << "input file size:"
         << right << setw(16) << infile_size << " Bytes\n";
    cout << left << setw(35) << "input file #line:"
         << right << setw(16) << infile_line_tot << "\n";
    cout << "\n=============== Threads Settings ==============\n";
    cout << left << setw(35) << "threads_num:"
         << right << setw(16) << threads_num << "\n";
    cout << left << setw(35) << "BF_RD_SZ:"
         << right << setw(16) << BF_RD_SZ << "\n";

    Clock clk;
    clk.start();
    multi_threaded(inFilename, outFilename, threads_num, infile_line_tot);
    // single_threaded(inFilename, outFilename, infile_line_tot);
    clk.stop();

    cout << left << setw(35) << "EX. time of [converter.cpp]:"
         << right << setw(16) << clk.getElapsedTime() << " ms\n";
    cout << left << setw(35) << "\"" + outFilename + "\" size:"
         << right << setw(16) << getFilesize(outFilename) << " Bytes\n";
    logFile.close();
    return 0;
}

void show_mult_prog(int thread_n, vector<Prog> &thd_log)
{
    bool flag;
    do
    {
        flag = false;
        for (int i = 0; i < thread_n; ++i)
        {
            cout << "[t" << i << ": " << setw(4) << fixed << setprecision(1)
                 << 100.0 * thd_log.at(i).line_cur / thd_log.at(i).line_tot << "%] ";
            flag = flag || (thd_log.at(i).line_cur < thd_log.at(i).line_tot);
        }
        cout << "\r";
        cout.flush();
        this_thread::sleep_for(std::chrono::milliseconds(1));
    } while (flag);

    cout << endl;
}

void locate_infs_thd(vector<ifstream> &infs_thd, long infile_line_tot, long line_per_thd)
{
    string str;
    int thread = 0;
    long line;
    for (line = 0; line < infile_line_tot; ++line)
    {
        if (line % line_per_thd == 0)
        {
            LP_tab.push_back(Line_Pos{.line = line,
                                      .pos = infs_thd[0].tellg()});
            if (thread != 0) // infs_thd[0] do not have to change
                infs_thd[thread].seekg(LP_tab.back().pos);
            cout << LP_tab.back().line << " : "
                 << LP_tab.back().pos << " : "
                 << infs_thd[thread].tellg() << endl;
            ++thread;
        }
        getline(infs_thd[0], str);
    }
    // Reset pos of infs_thd[0]
    infs_thd[0].clear();
    infs_thd[0].seekg(0, ios::beg);
}

void multi_thread_shots(const int thread, vector<Prog> &thd_log, ifstream &infs, ofstream &outfs)
{
    const int thread_n = thd_log.size();
    int buffer[BF_RD_SZ] = {};
    long line = 0, size = 0, line_end = thd_log[thread].line_tot;
    while (line < line_end)
    {
        // DISK -> MEM
        size = 0;
        while (size < BF_RD_SZ && line + size / COL_NUM < line_end)
        {
            infs >> buffer[size];
            infs.ignore();
            ++size;
        }
        line += size / COL_NUM;
        // MEM -> DISK
        for (long ix = 0; ix < size;)
        {
            outfs << INDENT << "{\n";
            // [1:19]
            for (int j = 1; j < COL_NUM; ++j)
                outfs << INDENT2 << "\"col_" << j
                      << "\":" << buffer[ix++] << ",\n";
            // [20]
            outfs << INDENT2 << "\"col_" << COL_NUM
                  << "\":" << buffer[ix++] << "\n"
                  << INDENT << "}"
                  << (thread == thread_n - 1 && ix == size && line == line_end ? "\n" : ",\n");
            // last thread & last element in buffer & last line (last group of dump)
        }
        thd_log.at(thread).line_cur = line; // update progress
    }
    // Close all files
    infs.close();
    outfs.close();
}

void multi_threaded(string inFilename, string outFilename, const int thread_n, long infile_line_tot)
{
    vector<Prog> thd_log;
    vector<ifstream> infs_thd(thread_n);
    vector<ofstream> ofs_thd(thread_n);
    const long line_per_thd = infile_line_tot / thread_n + (infile_line_tot % thread_n == 0 ? 0 : 1);
    cout << left << setw(35) << "line_per_thd:"
         << right << setw(16) << line_per_thd << endl;
    long _line_end = line_per_thd;
    // Open threads input file and define thread output filename
    for (int i = 0; i < thread_n; ++i)
    {
        // Last line end condition
        if (i == thread_n - 1 && infile_line_tot % line_per_thd != 0)
            _line_end = infile_line_tot % line_per_thd;
        // Sets *.thdout infos
        thd_log.push_back(Prog{.line_cur = 0L,
                               .line_tot = _line_end,
                               .filename = to_string(i) + ".thdout"});
        // Open thread output files
        ofs_thd[i].open(thd_log[i].filename, ios::out);
        if (!ofs_thd[i])
        {
            cerr << "open ofs_thd \"" << thd_log[i].filename << "\" failure!\n";
            exit(EXIT_FAILURE);
        }
        // Open thread input files
        infs_thd[i].open(inFilename, ios::out);
        if (!infs_thd[i])
        {
            cerr << "open infs_thd[" << i << "] failure!\n";
            exit(EXIT_FAILURE);
        }
    }

    // Locates each file position pointer in its thread
    locate_infs_thd(infs_thd, infile_line_tot, line_per_thd);
    // Multi-threaded conversion
    cout << "\n=============== Multi-threaded conversion ===============\n";
    // Test by sequence
    // for (int i = 0; i < thread_n; ++i)
    //     multi_thread_shots(i, ref(thd_log), ref(infs_thd[i]), ref(ofs_thd[i]));

    thread shot[thread_n];
    for (int i = 0; i < thread_n; ++i)
        shot[i] = thread(multi_thread_shots, i, ref(thd_log), ref(infs_thd[i]), ref(ofs_thd[i]));
    this_thread::sleep_for(chrono::milliseconds(1000)); // wait for 1 sec to launch monitor
    thread monitor(show_mult_prog, thread_n, ref(thd_log));

    for (int i = 0; i < thread_n; ++i)
        shot[i].join();
    monitor.join();
    for (int i = 0; i < thd_log.size(); ++i)
    {
        logFile << thd_log[i].line_cur << '\t'
                << thd_log[i].line_tot << '\t'
                << thd_log[i].filename << '\n';
    }

    // Concatenate *.thdout files parellel
    concatenate(thd_log, outFilename);
}

void recover_log(vector<Prog> &thd_log, int thread_n)
{
    thd_log.clear();
    for (int i = 0; i < thread_n; ++i)
        thd_log.push_back(Prog{.line_cur = 0L,
                               .line_tot = 0L,
                               .filename = to_string(i) + ".thdout"});
}

bool concatenate_mult(vector<Prog> &thd_log, string outFilename)
{
    recover_log(thd_log, 5);
    int thread_n = thd_log.size();
    // Scheduler
    while (thd_log.size() > 1)
    {
        for (int i = 0; i < thd_log.size() / 2; ++i)
            cout << "thread(" << thd_log[i * 2].filename << ", " << thd_log[i * 2 + 1].filename << ")\n";
        // thread shot(i);
        for (int i = 0; i < thd_log.size() / 2; ++i)
            cout << "join(" << thd_log[i * 2].filename << ", " << thd_log[i * 2 + 1].filename << ")\n";
        // shot(i).join();
        for (int i = (thd_log.size() % 2 == 0 ? thd_log.size() - 1 : thd_log.size() - 2);
             i > 0 / 2; i -= 2)
            thd_log.erase(thd_log.begin() + i);
    }

    return true;
}

bool concatenate(vector<Prog> &thd_log, string outFilename)
{
    for (int i = 0; i < thd_log.size(); ++i)
        cout << setw(10) << thd_log.at(i).filename << ": "
             << thd_log.at(i).line_cur << "/"
             << thd_log.at(i).line_tot << "\n";
    ifstream in[thd_log.size()];
    ofstream out;
    for (int i = 0; i < thd_log.size(); ++i)
    {
        in[i].open(thd_log.at(i).filename, ios_base::binary);
        if (!in[i])
        {
            cerr << "\"" << thd_log.at(i).filename << "\""
                 << " failed to open!\n";
            return false;
        }
    }
    out.open(outFilename, ios_base::binary);
    if (!out)
    {
        cerr << "\"" << outFilename << "\" failed to open!\n";
        return false;
    }
    cout << "\n=============== Concatenate Files ===============\n";
    out << "[\n";
    for (int i = 0; i < thd_log.size(); ++i)
    {
        out << in[i].rdbuf();
        in[i].close();
    }
    out << "]";
    out.close();
    cout << "\nComplete!\n";
    return true;
}

void single_threaded(string inFilename, string outFilename, long infile_line_tot)
{
    // Open files
    ifstream inFile(inFilename, ios::in);
    if (!inFile)
    {
        cerr << "\"" << inFilename << "\" failed to open!\n";
        exit(EXIT_FAILURE);
    }
    ofstream outFile(outFilename, ios::out);
    if (!outFile)
    {
        cerr << "\"" << outFilename << "\" failed to open!\n";
        exit(EXIT_FAILURE);
    }
    // Single-threaded conversion
    cout << "\n=============== Single-threaded conversion ===============\n";
    int buffer[BF_RD_SZ] = {};
    string buf_out = "";
    outFile << "[\n";
    long line = 0, size = 0;
    // while (inFile.peek() != EOF)
    while (line < infile_line_tot)
    {
        // DISK -> MEM
        size = 0;
        while (size < BF_RD_SZ && inFile >> buffer[size])
        {
            inFile.ignore();
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
            // buf_out.append(INDENT + "}" + (ix == size && line == infile_line_tot ? "\n" : ",\n"));
            outFile << INDENT2 << "\"col_" << COL_NUM
                    << "\":" << buffer[ix++] << "\n"
                    << INDENT << "}" << (ix == size && line == infile_line_tot ? "\n" : ",\n");
        }
        // outFile << buf_out;
        // buf_out.clear();
        logFile << size << ":" << line << endl;
        progress_bar(line, infile_line_tot);
    }
    outFile << "]";
    cout << "\n=============== End of conversion ===============\n";

    // Close all files
    logFile.close();
    inFile.close();
    outFile.close();
}