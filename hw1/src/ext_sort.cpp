#include <iostream>
#include "include/progress_bar.h"
#include "include/generate.h"

#include "include/Clock.h" // execution time measurement tool
using namespace std;

void check(string filename);         // check large file line by line
int ext_merge_sort(string filename); // main methods

int main(int argc, char *arv[])
{
    // check("in.txt");
    Clock clock;
    clock.start();
    long Filesize = generate("inputLight.txt", 0, 50); // testing
    // ext_merge_sort("inut.txt");
    clock.stop();
    cout << "generate(): execution time = \t" << clock.getElapsedTime() << "\tms" << endl;
    cout << "Writing avg. speed = \t\t" << (double)Filesize / clock.getElapsedTime() << "\tMB/sec" << endl; // Byte/ms = MB/s
    return 0;
}

int ext_merge_sort(string filename)
{
    cout << "\n================== START SORTING ==================\n";
    // Read input file
    string filePath = getpath() + "/test_files/" + filename;
    ifstream inFile(filePath, ios::in);
    if (!inFile)
    {
        cerr << "Fail opening \"" << filePath << "\"\n";
        exit(EXIT_FAILURE);
    }

    // External Sorting Algorithm

        return 0;
}

void check(string filename)
{
    string filePath = getpath() + "/test_files/" + filename;
    ifstream inFile(filePath, ios::in);
    if (!inFile)
    {
        cerr << "Fail opening \"" << filePath << "\"\n";
        exit(EXIT_FAILURE);
    }
    int lineCode = 0;
    string line = "";
    while (true)
    {
        // getchar(); // press Enter to continue
        getline(inFile, line);
        cout << ++lineCode << ": " << line;
        if (inFile.eof())
            break;
    }
    inFile.close();
    cout << "\n------------- File END -------------\n";
}
