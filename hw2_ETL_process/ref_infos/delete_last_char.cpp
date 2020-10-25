// g++ -o delete_last_char.out delete_last_char.cpp -lstdc++fs && ./delete_last_char.out && cat test.txt
// Failed
#include <fstream>
#include <iostream>
#include <experimental/filesystem>

using namespace std;
namespace fs = experimental::filesystem;

long getFilesize(const string filename)
{
    try
    {
        fs::path inFilepath = fs::current_path() / filename;
        return fs::file_size(inFilepath);
    }
    catch (const exception &e)
    {
        cerr << e.what() << '\n';
        return -1L;
    }
}

int main()
{
    string filename = "test.txt";
    ofstream ofs("test.txt", ios::out);
    ofs << "SSSSS,";
    cout << "now pos.: " << ofs.tellp() << endl;
    ofs.seekp(ofs.tellp() - 1L);
    ofs << '\0' << '\0' << '\0';
    ofs.close();
    cout << "\"" << filename << "\"" << getFilesize(filename) << endl;
    return 0;
}