// http://wen00072.github.io/blog/2013/12/29/using-getopt-parse-command-line-parameter/#%E5%8F%83%E8%80%83%E8%B3%87%E6%96%99
// .\getopt_demo -a -emy_args -c dddd  123 456 7890 -b -f -d
// g++ -o getopt_demo getopt_demo.cpp && ./getopt_demo -a -b -c-e123 -f gsgr
// abc:d:e:: >> -a, -b, -c<carg> or -c <carg>, -d<darg> or -d <darg>, -e<earg> or -e
#include <iostream>
#include <unistd.h> // readlink
// #include <cctype>
// #include <stdarg.h>
// getopt() global (cross-filed) variables
extern char *optarg;
extern int optind, opterr, optopt;
int getopt(int argc, char const *argv[], char const *optstring);
void read_arg(int argc, char *argv[], const char *argsets);

using namespace std;

int main(int argc, char *argv[])
{
    const char *argsets = "abc:d:e::f::";
    read_arg(argc, argv, argsets);
    return 0;
}
void read_arg(int argc, char *argv[], const char *argsets)
{
    int cmd_opt = 0;
    while ((cmd_opt = getopt(argc, argv, argsets)) != -1)
    {
        // Lets parse
        switch (cmd_opt)
        {
        // No args
        case 'a':
        case 'b':
            cerr << "No arg \'" << (char)cmd_opt << "\'" << endl;
            break;

        // Single arg ":"
        case 'c':
        case 'd':
            cerr << "Single arg \'" << (char)cmd_opt << "\': " << optarg << endl;
            break;

        // Optional args "::"
        case 'e':
        case 'f':
            if (optarg)
                cerr << "Option arg \'" << (char)cmd_opt << "\':: " << optarg << endl;
            else
                cerr << "Option arg \'" << (char)cmd_opt << "\'" << endl;
            break;

        // Error handle: Mainly missing arg (Single arg ":") or illegal option
        case '?':
            cerr << "Illegal option:-" << (isprint(optopt) ? optopt : '#') << endl;
            break;
        default:
            cerr << "Not supported option\n";
            break;
        }
        // cerr << "process index:" << optind << endl; // next pointer
    }

    // Do we have args?
    if (argc > optind)
        for (int i = optind; i < argc; ++i)
            cerr << "argv[" << i << "] = " << argv[i] << endl;
}