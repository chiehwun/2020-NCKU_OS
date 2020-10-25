// g++ -pthread -o write_mult write_mult.cpp && ./write_mult
#include <iostream> // std::cout
#include <thread>   // std::thread
#include <fstream>
#include <string>

using namespace std;

int main()
{
    string outFilename = "output_mult.txt";
    ofstream ofs1(outFilename, ios::out);
    if (!ofs1)
    {
        cerr << "ofs1 failed\n";
        exit(EXIT_FAILURE);
    }
    ofstream ofs2(outFilename, ios::out);
    if (!ofs2)
    {
        cerr << "ofs2 failed\n";
        exit(EXIT_FAILURE);
    }
    ofs1 << "HAHA";
    // ofs2.seekp(ofs1.tellp());
    ofs2.seekp(1);
    ofs2 << "WHATWHAT?";

    ofs1.close();
    ofs2.close();
}