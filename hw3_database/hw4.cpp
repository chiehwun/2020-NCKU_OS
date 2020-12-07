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

    int64_t arr_record[] = {
        30, 39, 8, 26, 36, 28, 6, 7, 41, 9,
        35, 4, 38, 15, 14, 17, 5, 32, 46, 42,
        37, 43, 22, 33, 49, 3, 23, 40, 10, 31,
        1, 34, 19, 21, 12, 44, 11, 13, 24, 20,
        27, 0, 16, 45, 29, 47, 18, 2, 48, 25};
    int64_t n = sizeof(arr_record) / sizeof(*arr_record);
    int64_t arr_rand[n]{}, arr_sort[n]{};
    int64_t *arr_chosen = NULL;
    bool random_test = false;
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
    {
        arr_chosen = arr_record;
    }

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
    // test_treeF.check_tot(arr_sort, n);

    return 0;
}
// s:5 | 20 40 60 80 100
// ptr[0]s:5 | 1 2 3 4 5
// ptr[1]s:5 | 20 22 24 25 26
// ptr[2]s:5 | 40 42 44 46 47
// ptr[3]s:5 | 60 61 63 65 66
// ptr[4]s:5 | 80 82 84 86 87
// ptr[5]s:3 | 100 101 103 104 105

// BPTree tree;
// /* cursor is root */
// tree.insert(1, "a");
// tree.insert(2, "a");
// tree.insert(3, "a");
// tree.insert(20, "a");
// tree.insert(40, "a");
// tree.insert(60, "a"); // break
// tree.display(tree.root);
// for (int i = 0; i < 6; ++i)
// {
//     cout << "ptr[" << i << "]";
//     tree.display(tree.root->ptr[i]);
// }
// // fill rest ptr[0]
// tree.insert(4, "a");
// tree.insert(5, "a");

// tree.insert(22, "a");
// tree.insert(24, "a");
// tree.insert(80, "a"); // break
// // fill rest ptr[1]
// tree.insert(25, "a");
// tree.insert(26, "a");

// tree.insert(42, "a");
// tree.insert(44, "a");
// tree.insert(100, "a"); // break
// // fill rest ptr[2]
// tree.insert(46, "a");
// tree.insert(47, "a");

// tree.insert(61, "a");
// tree.insert(63, "a");
// tree.insert(82, "a"); // break
// // fill rest ptr[3]
// tree.insert(65, "a");
// tree.insert(66, "a");

// tree.insert(84, "a");
// tree.insert(101, "a");
// tree.insert(103, "a"); //break
// // fill rest ptr[3]
// tree.insert(86, "a");
// tree.insert(87, "a");
// // fill rest ptr[4]
// tree.insert(104, "a");
// tree.insert(105, "a");

// tree.insert(23, "a");
// /* cursor not root && parent overflow */
// cout << "test2" << endl;

// // 3-layer print
// tree.display(tree.root);
// for (int i = 0; i <= tree.root->size; ++i)
// {
//     cout << "ptr[" << i << "]";
//     tree.display(tree.root->ptr[i]);
// }
// for (int j = 0; j <= tree.root->size; ++j)
// {
//     for (int i = 0; i <= tree.root->ptr[j]->size; ++i)
//     {
//         cout << "root->ptr[" << j << "]->ptr[" << i << "]:";
//         tree.display(tree.root->ptr[j]->ptr[i]);
//     }
// }
// tree.display_leaf();

// int arr[] = {1, 2, 3, 20, 40,
//              60, 4, 5, 22, 24,
//              80, 25, 26, 42, 44,
//              100, 46, 47, 61, 63,
//              82, 65, 66, 84, 101,
//              103, 86, 87, 104, 105};