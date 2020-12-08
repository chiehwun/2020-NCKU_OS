#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include <random>
#include "db.h"
#include "BPTree.h"
using namespace std;

int main(int argc, char *argv[])
{
    BPTree test_treeF;
    bool random_test = true;

    int64_t arr_record[] = {0};
    int64_t n = random_test ? 1000 : sizeof(arr_record) / sizeof(*arr_record);
    int64_t arr_rand[n]{}, arr_sort[n]{};
    int64_t *arr_chosen = NULL;
    if (random_test)
    {
        arr_chosen = arr_rand;
        for (int64_t i = 0; i < n; ++i)
            arr_sort[i] = arr_rand[i] = i;
        // Shuffle and copy a new one
        default_random_engine generator(time(NULL));
        uniform_int_distribution<int64_t> unif_key(0, n - 1);
        for (int64_t i = 0; i < n; ++i)
            swap(arr_rand[unif_key(generator)], arr_rand[unif_key(generator)]);
    }
    else
        arr_chosen = arr_record;

    // record the insert order to log file
    cout << "arr[]=\n{\n";
    for (int64_t i = 0; i < n; ++i)
        cout << arr_chosen[i] << (i == n - 1 ? "\n}\n" : (i % 10 == 9 ? ",\n" : ", "));
    // Start insertion and check
    for (int64_t i = 0; i < n; ++i)
    {
        int64_t key = arr_chosen[i];
        cout << "insert arr[" << i << "]: " << key << "\n";
        test_treeF.insert(key, "a"); // print case(?...)
        test_treeF.visualize(3);
        test_treeF.check(i + 1);
        cout << "\n";
    }
    test_treeF.check_tot(arr_sort, n);

    return 0;
}