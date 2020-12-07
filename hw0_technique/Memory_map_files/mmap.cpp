#include <iostream>
#include <cstdlib>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h> // mmap()
#include <sys/stat.h>

using namespace std;

int main()
{
    string filename = "put.input";
    int fd = open(filename.c_str(), O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat sb;
    if (fstat(fd, &sb) == -1)
        cerr << "could not get file size.\n";

    cout << "\"" << filename << "\" size: " << sb.st_size << endl;

    char *file_in_mem = (char *)mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    getc(stdin);
    // for (int i = 0; i < sb.st_size; ++i)
    // {
    //     printf("%c", file_in_mem[i]);
    // }
    return 0;
}