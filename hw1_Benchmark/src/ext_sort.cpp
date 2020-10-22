#include <iostream>
#include <locale> // Print integer with thousands and millions separator
#include <fstream>
#include <cstring>
#include <vector>
#include <string>
#include "include/Mem_Sys_Set.h"
#include "include/Clock.h"        // execution time measurement tool
#include "include/progress_bar.h" // progress visualization tool
#include "include/generate_C.h"
#include "include/Merge_sort.h"
#include "include/List_sort.h"
#include "include/MinHeap.h"

void check(string filename);         // check large file line by line
int ext_merge_sort(string filename); // main methods
bool is_sorted(int *list, size_t start, size_t end);
long check_run(FILE *runFile);
long check_run2(FILE *runFile);
int CreateInitRuns(string filename);
void MergeRuns(int run_num, const int Kway);
vector<long> RUN_LINE;

int main(int argc, char *argv[])
{
    // Set input arguments then read
    const string argsets = "sgit";
    bool modes[argsets.length()] = {};
    read_arg(argc, argv, argsets, modes);

    cout.imbue(locale("")); // Print integer with thousands and millions separator
    Clock clock;
    if (modes[2]) // -i toggled
    {
        return 0;
    }
    else if (modes[1]) // -g toggled
    {
        clock.start();
        long Filesize = generate_C("input.txt", 1, (1L << 10)); // MiB
        clock.stop();
        cout << "generate(): execution time = \t"
             << clock.getElapsedTime() << "\tms\n";
        cout << "Writing avg. speed = \t\t" << fixed << setprecision(2)
             << (double)Filesize / clock.getElapsedTime()
             << "\tMB/sec\n"; // Byte/ms = MB/s
        return 0;
    }
    else if (modes[0]) // -s toggled
    {
        clock.start();
        ext_merge_sort("input.txt");
        clock.stop();
        cout << "\next_merge_sort(): execution time = \t"
             << clock.getElapsedTime() << "\tms\n";
        return 0;
    }
    else if (modes[3]) // -t toggled
    {
        string filename = "output.txt";
        string filePath = LOC_PATH + "/test_files/" + filename;
        FILE *inFile = openFile(filePath.c_str(), "r");
        long x = check_run(inFile);
        cout << "\ncheck_run("
             << "\"" << filename << "\""
             << "): " << x << " lines" << endl;
        // MergeRuns(0, 3);
        return 0;
    }
    // string filepath = LOC_PATH + "/test_files/in.txt";
    // FILE *run = openFile(filepath.c_str(), "r");
    // printf("\n\nline: %ld\n", check_run(run));
    return 0;
}

int CreateInitRuns(string filename)
{
    Clock CIRClk;
    CIRClk.start();
    string infilePath = LOC_PATH + "/test_files/" + filename;
    // Read input file (FILE -> MEM)
    FILE *inFile = openFile(infilePath.c_str(), "r");

    // Get File Size
    fseek(inFile, 0, SEEK_END);
    cout << left << setw(25) << "\"" + filename + "\" size:"
         << right << setw(16) << ftell(inFile) << " Bytes\n";
    // printf("\"%s\" size: %ld Bytes\n", filename.c_str(), ftell(inFile));
    rewind(inFile);

    puts("\n================== Create Initial Runs ==================");
    const long BF_CNT = long((FREE_MEM << 10) / sizeof(int) * MEM_SF / 2.0); // *sort_list & *linka & *linkb
    cout << left << setw(15) << "BF_CNT:"
         << right << setw(16) << BF_CNT << "\n";
    cout << left << setw(15) << "FREE_MEM:"
         << right << setw(16) << FREE_MEM << "\tKB\n";
    cout << left << setw(15) << "*sort_list MEM:"
         << right << setw(16) << BF_CNT * sizeof(int) << "\tBytes\n";
    // printf("BF_CNT: \t%zu\n", BF_CNT);
    // printf("FREE_MEM:\t%ld\tBytes\n", FREE_MEM << 10);
    // printf("*sort_list MEM:\t%ld\tBytes\n", BF_CNT * sizeof(int));

    Clock runWClk;
    int run_code = 0;

    // Dynamic Heap Allocation
    try
    {
        int *sort_list = new int[BF_CNT]{};
        char *buf_out = new char[BF_WRT_SZ]{};
        while (!feof(inFile))
        {
            // Read single run then do internal sort (FILE -> MEM)
            long runLine = 1; // valid runLine [1:BF_CNT-1]
            fprintf(stderr, "Input to sort_list: run%d:\n", run_code);
            for (; runLine < BF_CNT && !feof(inFile); ++runLine)
            {
                fscanf(inFile, "%d\n", &sort_list[runLine]);
                if (runLine % 10000000 == 0)
                    progress_bar(runLine, BF_CNT);
            }
            RUN_LINE.push_back(runLine - 1);
            progress_bar(runLine, BF_CNT);
            fprintf(stderr, "\n%2d-#sort_list: \t%ld\tsorting...", run_code, runLine);

            // Dynamic Heap Allocation
            runWClk.start();
            int *link = new int[runLine]{}; // zero elements
            List1(sort_list, link, runLine,
                  rMergeSort(sort_list, link, 0, runLine - 1));
            delete[] link;
            runWClk.stop();
            fprintf(stderr, "ok!\n");
            cerr << "\nrMergeSort(): execution time = \t"
                 << runWClk.getElapsedTime() << "\tms\n";

            // // Check sort_list is sorted
            // if (is_sorted(sort_list, 1, runLine - 1))
            //     fprintf(stderr, "ok!\n");
            // else
            // {
            //     fprintf(stderr, "rMergeSort() err!\n");
            //     exit(EXIT_FAILURE);
            // }

            // Bufferredly Writes to runs file
            string runfileName = to_string(run_code) + ".run";
            string runfilePath = LOC_PATH + "/test_files/run/" + runfileName;
            FILE *outFile = openFile(runfilePath.c_str(), "w"); // "w+": write & read
            fprintf(stderr, "Write \"%s\"\n", runfileName.c_str());
            runWClk.start();
            long bx = 0L;
            for (long i = 1L; i < runLine; ++i)
            {
                if (bx >= 0L && bx < BF_WRT_SZ - 12L) // -12 prevent overflow
                {
                    bx += snprintf(buf_out + bx, BF_WRT_SZ - bx,
                                   "%d\n", sort_list[i]); // MEM -> MEM
                    // fprintf(stderr, "sort_list[%ld]: %d, bx: %ld ", i, sort_list[i], bx);
                }
                else if (bx >= BF_WRT_SZ - 12L) // dump to disk
                {
                    fprintf(outFile, "%s", buf_out); // MEM -> FILE
                    bx = 0L;
                    progress_bar(i, runLine);
                }
                else
                {
                    delete[] sort_list;
                    delete[] buf_out;
                    fprintf(stderr, "buf_out overflow");
                    exit(EXIT_FAILURE);
                }
            }
            if (bx > 0) // dump remained to disk
            {
                fprintf(outFile, "%s", buf_out);
                // fprintf(stderr, "\nbuf_out: %s\n", buf_out);
                progress_bar(runLine, runLine);
            }
            runWClk.stop();
            long runSize = ftell(outFile);
            cerr << "\nWrites to run: execution time = \t"
                 << runWClk.getElapsedTime() << "\tms";
            fprintf(stderr,
                    "\nWriting avg. speed = \t\t%.2f\tMB/sec",
                    (double)runSize / runWClk.getElapsedTime()); // Byte/ms = MB/s
            fprintf(stderr,
                    "\nSuccessfully write! \"%s\" runLine: %ld\n\n",
                    runfileName.c_str(), runLine);

            // Check run is sorted
            // printf("\n#line: %ld", check_run2(outFile)); // Lack of memory

            fclose(outFile);
            ++run_code;
        }
        delete[] buf_out;
        fprintf(stderr, "Successfully delete[] buf_out\n");
        delete[] sort_list;
        fprintf(stderr, "Successfully delete[] sort_list\n");
    }
    catch (bad_alloc &memoryAlloc)
    {
        cerr << "Exception: " << memoryAlloc.what() << endl;
        exit(EXIT_FAILURE);
    }
    // Write to "run.info" the record of each run's line number
    string filePath = LOC_PATH + "/test_files/run/run.info";
    FILE *runInfo = openFile(filePath.c_str(), "w");
    fprintf(runInfo, "%d\n", run_code);
    for (int i = 0; i < run_code; ++i)
    {
        fprintf(runInfo, "%ld\n", RUN_LINE.at(i));
    }
    fclose(runInfo);
    CIRClk.stop();
    cout << "\nCreateInitRuns(): execution time = \t"
         << CIRClk.getElapsedTime() << "\tms\n";
    return run_code;
}

void MergeRuns(int run_num, const int Kway)
{
    Clock MergeClk;
    MergeClk.start();
    if (RUN_LINE.empty()) // CreateInitRuns() did not run
    {
        string filePath = LOC_PATH + "/test_files/run/run.info";
        FILE *runInfo = openFile(filePath.c_str(), "r");
        fscanf(runInfo, "%d\n", &run_num);
        long temp = 0;
        for (int i = 0; i < run_num; ++i)
        {
            fscanf(runInfo, "%ld\n", &temp);
            RUN_LINE.push_back(temp);
        }
        fclose(runInfo);
    }
    puts("\n================== Merge Run Files ==================");
    vector<int> runList = vector<int>(run_num, 0);
    for (int i = 0; i < run_num; ++i)
    {
        runList.at(i) = i;
        printf("%2d ", runList.at(i));
    }
    puts("");

    int run_lastCode = run_num;
    string outfilePath = "", outfileName = "";
    int k = Kway > (FOPEN_MAX - 1) ? (FOPEN_MAX - 1) : Kway;
    printf("k: %u, Kway: %u, FOPEN_MAX-1: %d\n", k, Kway, FOPEN_MAX - 1);
    while (true)
    {
        // Calculate remain runs
        k = k > (int)runList.size() ? runList.size() : k;

        // Set output file
        FILE *outFile;
        if (k == (int)runList.size())
        {
            outfileName = "output.txt";
            outfilePath = LOC_PATH + "/test_files/" + outfileName;
        }
        else
        {
            outfileName = to_string(run_lastCode) + ".run";
            outfilePath = LOC_PATH + "/test_files/run/" + outfileName;
        }
        fprintf(stderr, "Write \"%s\": k=%u\n\n", outfileName.c_str(), k);
        outFile = openFile(outfilePath.c_str(), "w");
        // Set input run file
        long runLine_tot = 0L;
        FILE *inFile[k];
        for (int j = 0; j < k; ++j)
        {
            string runfileName = to_string(runList.at(j)) + ".run";
            string runfilePath = LOC_PATH + "/test_files/run/" + runfileName;
            inFile[j] = openFile(runfilePath.c_str(), "r");
            runLine_tot += RUN_LINE.at(runList.at(j));
        }

        // Merge Files
        MinHeapNode *harr = new MinHeapNode[k];
        int i;
        for (i = 0; i < k; i++)
        {
            if (fscanf(inFile[i], "%d\n", &harr[i].element) != 1)
                break;
            harr[i].i = i;
        }
        // Create the heap
        MinHeap hp(harr, i);
        int count = 0;
        long line = 0L;
        while (count != i)
        {
            MinHeapNode root = hp.getMin();
            fprintf(outFile, "%d\n", root.element);
            if (fscanf(inFile[root.i], "%d\n",
                       &root.element) != 1) // When a file finish scanning
            {
                root.element = INT_MAX;
                ++count;
            }
            ++line;
            hp.replaceMin(root);
            if (line % 100000 == 0)
                progress_bar(line, runLine_tot);
        }
        progress_bar(line, runLine_tot);
        delete[] harr; // Deallocate Heap Memory

        // Close Files
        for (int j = 0; j < k; ++j)
            fclose(inFile[j]);
        fclose(outFile);

        // Terminated Condition
        if (k == (int)runList.size())
            break;
        else
        { // Update Record
            runList.push_back(run_lastCode);
            RUN_LINE.push_back(runLine_tot);
            runList.erase(runList.begin(), runList.begin() + k);
            for (auto it = runList.begin(); it != runList.end(); ++it)
                fprintf(stderr, "%2d ", *it);
            fprintf(stderr, "\n");
            ++run_lastCode;
        }
    }
    MergeClk.stop();
    cout << "\nMergeRuns(Kway=" << Kway << "): execution time = \t"
         << MergeClk.getElapsedTime() << "\tms\n";
}

int ext_merge_sort(string filename)
{
    puts("\n===================== START SORTING =====================");
    printf("FOPEN_MAX = %d\n", FOPEN_MAX);
    int run_num = CreateInitRuns(filename);
    MergeRuns(run_num, run_num); // Largest K_way
    puts("\n====================== END SORTING ======================");
    return 0;
}

bool is_sorted(int *list, size_t start, size_t end)
{
    if (start >= end)
    {
        fprintf(stderr, "is_sorted(): start <= end");
        return false;
    }
    for (size_t i = start; i < end; ++i)
    {
        if (list[i] > list[i + 1])
        {
            fprintf(stderr, "is_sorted(): not sorted!");
            return false;
        }
    }
    return true;
}

long check_run(FILE *runFile)
{
    // obtain file size
    rewind(runFile);
    fseek(runFile, 0, SEEK_END);
    const long runSize = ftell(runFile), freeMEM = getMemInfo() << 10;
    rewind(runFile);
    // check RAM is enough for runFile
    if (runSize > freeMEM)
    {
        fprintf(stderr, "Insufficient Memory: %ld > %ld\n",
                runSize, freeMEM);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Memory allocate: %ld/%ld\n",
            runSize, freeMEM);
    char *buffer = new char[runSize + 1]{};
    char *buffer_ptr = buffer, *buffer_new = buffer;
    // fseek(runFile, 0, SEEK_SET);
    fread(buffer, 1, runSize, runFile);
    int prev = INT_MIN, curr = 0;
    size_t line = 0;
    // Read int from buffer
    while (sscanf(buffer_ptr, "%d\n", &curr) == 1)
    {
        ++line;
        // move buffer ptr to next '\n' occurence
        if ((buffer_new = strchr(buffer_ptr, '\n')) != NULL)
            buffer_ptr = buffer_new + 1; // find '\n'
        else
            break;

        // debug
        progress_bar(line, runSize / 10);
        // fprintf(stderr, "%d %d\n", prev, curr);

        if (prev > curr)
        {
            delete[] buffer; // Reallocate Heap memory
            fprintf(stderr, "check_run(): file not sorted! line: %ld", line);
            return -1L;
        }
        prev = curr;
    }
    delete[] buffer; // Reallocate heap memory
    return line;
}

long check_run2(FILE *runFile)
{
    // obtain file size
    rewind(runFile);
    fseek(runFile, 0, SEEK_END);
    const long runSize = ftell(runFile), freeMEM = getMemInfo() << 10;
    rewind(runFile);
    // check RAM is enough for runFile
    if (runSize > freeMEM)
    {
        fprintf(stderr, "Insufficient Memory: %ld > %ld\n",
                runSize, freeMEM);
        exit(EXIT_FAILURE);
    }
    fprintf(stderr, "Memory allocate: %ld/%ld\n",
            runSize, freeMEM);
    int *buffer = new int[runSize / 11]{};
    int prev = INT_MIN, curr = 0;
    size_t line = 0;
    // Read int from buffer
    while (fscanf(runFile, "%d\n", &curr) == 1)
    {
        buffer[line] = curr;
        ++line;
        // debug
        progress_bar(line, runSize / 11);
        // fprintf(stderr, "%d %d\n", prev, curr);
        if (prev > curr)
        {
            delete[] buffer; // Reallocate heap memory
            fprintf(stderr, "check_run2(): file not sorted! line: %ld", line);
            return -1L;
        }
        prev = curr;
    }
    delete[] buffer; // Reallocate heap memory
    return line;
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
        getchar(); // press Enter to continue
        getline(inFile, line);
        cout << ++lineCode << ": " << line;
        if (inFile.eof())
            break;
    }
    inFile.close();
    cout << "\n------------- File END -------------\n";
}

// puts("\n================== K-way Merge ==================");
// const size_t BF_IN_SZ = 5000 << 10;  // KiB
// const size_t BF_OUT_SZ = 5000 << 10; // KiB
// char **bf_in = new char *[K];
// for (int i = 0; i < K; ++i)
//     bf_in[i] = new char[BF_OUT_SZ];
// char *bf_out = new char[BF_OUT_SZ];
// size_t read_size = 100;
// size_t result = fread(bf_in[0], 1, read_size, inFile);
// if (result != read_size)
// {
//     fputs("fread() error", stderr);
//     exit(EXIT_FAILURE);
// }
// if (fgets(bf_in[0] + result, BF_IN_SZ, inFile) == NULL)
// {
//     fputs("fgets() error", stderr);
//     exit(EXIT_FAILURE);
// }
// printf("%s", bf_in[0]);
// // printf("%d", fgetpos)
// // delete buffer
// delete[] bf_out;
// for (int i = 0; i < K; ++i)
//     delete[] bf_in[i];
// delete[] bf_in;