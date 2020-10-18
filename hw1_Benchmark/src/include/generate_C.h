#ifndef GENERATE_C_H
#define GENERATE_C_H

#include <stdio.h>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <random>
#include "progress_bar.h"
#include "Mem_Sys_Set.h"

// MEM -> FILE
// mode:[0,user][1,memTot]
// default_size (MiB)
long generate_C(string filename, int mode = 0,
                long size = 1L)
{
    // Open Output File
    string filePath = getpath() + "/test_files/" + filename;
    FILE *outFile = openFile(filePath.c_str(), "w");

    // random generator, distribution
    srand(time(0));
    default_random_engine generator(time(NULL));
    uniform_int_distribution<int> unif(INT_MIN, INT_MAX);

    /*********** Determine File size by mode ************/
    long memTot = getMemInfo(1, false) << 10;
    const long Filesize = (mode == 1 ? memTot : (size << 20)); // size MiB
    cout.imbue(locale(""));
    cout << left << setw(15) << "MemTotal:"
         << right << setw(16) << memTot << "\tBytes\n";
    cout << left << setw(15) << "Gen File size:"
         << right << setw(16) << Filesize << "\tBytes\n";
    cout << left << setw(15) << "buffer.size():"
         << right << setw(16) << BF_WRT_SZ << "\tBytes\n";
    // printf("MemTotal:\t%ld\tBytes\n", memTot);
    // printf("Gen File size:\t%ld\tBytes\n", Filesize);
    // printf("buffer.size():\t%zu\tBytes\n", BF_WRT_SZ);

    // Dynamic Heap Allocation
    char *buf_out = new char[BF_WRT_SZ]; // Use *char as buffer

    puts("\n================== START GENERATING ==================");
    long bx = 0L, bxx = 0L;
    while (bxx + bx < Filesize)
    {
        if (bx >= 0 && bx < BF_WRT_SZ - 12) // -12 prevent overflow
        {
            bx += snprintf(buf_out + bx, BF_WRT_SZ - bx,
                           "%d\n", unif(generator)); // MEM -> MEM
        }
        else if (bx >= BF_WRT_SZ - 12) // dump to disk
        {
            fprintf(outFile, "%s", buf_out); // MEM -> FILE
            bxx += bx;
            bx = 0;
            progress_bar(bxx, Filesize);
        }
        else
        {
            delete[] buf_out; // Reallocate Heap memory
            perror("buffer overflow");
            exit(EXIT_FAILURE);
        }
    }
    if (bx > 0)
        fprintf(outFile, "%s", buf_out);
    progress_bar(bxx + bx, Filesize);
    cout << "\n================== END GENERATING ==================\n";
    cout.imbue(locale(""));
    cout << "\n\n"
         << left << setw(25) << "bxx + bx: "
         << right << setw(16) << bxx + bx << "\n";
    cout << left << setw(25) << "\"" + filename + "\" size: "
         << right << setw(16) << ftell(outFile) << " bytes\n";
    delete[] buf_out; // Reallocate Heap memory
    return ftell(outFile);
}

#endif // GENERATE_C_H