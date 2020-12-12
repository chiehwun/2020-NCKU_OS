#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include <random>
#include <algorithm>
#include "db.h"
#include "progress_bar.h"

#define RAND_TEST true
#define RAND_TEST_NUM 100
#define DB_OUT 0
// 0: print all         (For alg.-detailed debug)
// 1: print wrong       (For large input)
// 2: print all at cerr (For seg. fault)
using namespace std;
string DB_OUT_TITLE[] = {"print all steps",
                         "print wrong step",
                         "print all but cerr"};

int main(int argc, char *argv[])
{
    BPTree test_treeF(3, 50);
    int64_t arr_record[] = {0};
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
         << DB_OUT_TITLE[DB_OUT]
         << endl;
    // Debug start
    ostringstream dbss;
    string log_title, log, log_prev;
    ostream &cso = (DB_OUT == 0 ? cout : (DB_OUT == 1 ? dbss : cerr));

    // record the insert order to log file
    dbss << "arr[" << n << "]=\n{\n";
    for (int64_t i = 0; i < n; ++i)
        dbss << arr_chosen[i] << (i == n - 1 ? "\n}\n" : (i % 10 == 9 ? ",\n" : ", "));
    // record and flush sstream
    log_title = dbss.str();
    dbss.str(string());

    // Force to print at terminal
    cerr << log_title;
    if (DB_OUT != 2)
        cout << log_title;
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
            ostringstream hexstr;
            hexstr << hex << showbase << key;
            test_treeF.insert(key, hexstr.str(), cso); // print case(?...)
            cso << "min_key: " << test_treeF.min_key << ", max_key: " << test_treeF.max_key << endl;
            test_treeF.visualize(6, cso);
            cso << "tree.NodeNum: " << test_treeF.NodeNum << endl;
            cso << "tree.leafNodeNum: " << test_treeF.leafNodeNum << endl;
            bool err = !test_treeF.check(j == 0 ? i + 1 : n, cso);
            // bool err = false;
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
    test_treeF.check_leaf(arr_sort, n, cerr);
    // cerr << "\n\nReady insert overflow char*\n";
    // test_treeF.insert(0, jojo, cerr);
    // test_treeF.insert(1, jojo, cerr);
    // test_treeF.insert(2, jojo, cerr);
    test_treeF.visualize(6, cout, true);
    test_treeF.search(-1L);
    test_treeF.search(250L);
    cerr << "\n=========== END OF DEBUG ===========\n";
    return 0;
}
// sort(arr_record, arr_record + 50);