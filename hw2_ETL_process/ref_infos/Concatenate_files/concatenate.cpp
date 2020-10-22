#include <iostream>
#include <fstream>
using namespace std;
// https://stackoverflow.com/questions/19564450/concatenate-two-huge-files-in-c
int main()
{
    ifstream if_a("a.txt", ios_base::binary);
    ifstream if_b("b.txt", ios_base::binary);
    ofstream of_c("c.txt", ios_base::binary);

    cout << "Concatenate Files...";
    of_c << if_a.rdbuf() << if_b.rdbuf();
    cout << "\nComplete!\n";
    return 0;
}