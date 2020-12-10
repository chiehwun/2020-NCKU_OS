#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include <random>
#include <algorithm>
#include "db.h"
#include "BPTree.h"
#include "progress_bar.h"
#define RAND_TEST true
#define RAND_TEST_NUM 100
#define DB_OUT 1 // 0: print all, 1: print wrong
using namespace std;

int main(int argc, char *argv[])
{
    BPTree test_treeF(3);
    int64_t arr_record[] = {1, 2, 3, 4, 5,
                            6, 7, 8, 9, 10,
                            11, 12, 13, 14, 15};

    int64_t n = RAND_TEST ? RAND_TEST_NUM : sizeof(arr_record) / sizeof(*arr_record);
    int64_t arr_rand[n]{}, arr_sort[n]{};
    int64_t *arr_chosen = NULL;
    for (int64_t i = 0; i < n; ++i)
        arr_sort[i] = arr_rand[i] = i;
    cerr << "\n=========== START DEBUG ===========\n";
    if (RAND_TEST)
    {
        cerr << "Debug mode:\t\trandom" << endl;
        arr_chosen = arr_rand;
        // Shuffle and copy a new one
        default_random_engine generator(time(NULL));
        uniform_int_distribution<int64_t> unif_key(0, n - 1);
        for (int64_t i = 0; i < n; ++i)
            swap(arr_rand[unif_key(generator)], arr_rand[unif_key(generator)]);
    }
    else
    {
        cerr << "Debug mode:\t\trecord" << endl;
        arr_chosen = arr_record;
    }
    cerr << "test array size:\t" << n << endl;
    cerr << "Debug output type:\t"
         << (DB_OUT == 0 ? "print all steps" : "print wrong step")
         << endl;
    // Debug start
    ostringstream dbss;
    string log_title, log, log_prev;
    ostream &cso = (DB_OUT == 0 ? cout : dbss);

    // record the insert order to log file
    cso << "arr[]=\n{\n";
    for (int64_t i = 0; i < n; ++i)
        cso << arr_chosen[i] << (i == n - 1 ? "\n}\n" : (i % 10 == 9 ? ",\n" : ", "));
    if (DB_OUT == 1)
    {
        // record and flush sstream
        log_title = dbss.str();
        dbss.str(string());
    }
    // Start insertion and check
    for (int j = 0; j < 2; ++j)
    {
        for (int64_t i = 0; i < n; ++i)
        {
            if (DB_OUT == 1)
            {
                log_prev = log;
            }
            int64_t key = arr_chosen[i];
            cso << "\ninsert arr[" << i << "]: " << key << "\n";
            test_treeF.insert(key, "a", cso); // print case(?...)
            test_treeF.visualize(3, cso);
            cso << "tree.nodeNum: " << test_treeF.nodeNum << endl;
            bool err = !test_treeF.check(j == 0 ? i + 1 : n, cso);
            if (DB_OUT == 1)
            {
                // record and flush sstream
                log = dbss.str();
                dbss.str(string());
            }
            if (err)
            {
                // throw error message
                cout << log_title << log_prev << log;
                exit(EXIT_FAILURE);
            }
            progress_bar(j * n + (i + 1), n * 2);
        }
    }
    test_treeF.visualize(3, cout);
    test_treeF.check_leaf(arr_sort, n, cout);
    return 0;
}
// sort(arr_record, arr_record + 50);