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
#define RAND_TEST false
#define RAND_TEST_NUM 1000
#define DB_OUT 1
// 0: print all         (For alg.-detailed debug)
// 1: print wrong       (For large input)
// 2: print all at cerr (For seg. fault)
using namespace std;
string DB_OUT_TITLE[] = {"print all steps",
                         "print wrong step",
                         "print all but cerr"};

int main(int argc, char *argv[])
{
    string jojo("\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
12345678900"); // string overflow test
    BPTree test_treeF(10);
    int64_t arr_record[] = {208, 272, 0, 109, 4, 108, 18, 49, 8, 160,
                            329, 190, 12, 242, 254, 123, 139, 52, 35, 146,
                            342, 21, 373, 319, 42, 293, 26, 110, 28, 389,
                            318, 5, 32, 350, 341, 301, 175, 286, 227, 39,
                            348, 41, 383, 43, 44, 241, 223, 47, 288, 296,
                            281, 55, 358, 338, 215, 263, 380, 57, 58, 388,
                            273, 196, 239, 258, 346, 131, 66, 369, 299, 77,
                            156, 137, 72, 67, 362, 116, 256, 45, 86, 294,
                            92, 89, 82, 355, 396, 85, 333, 187, 368, 246,
                            90, 130, 176, 260, 94, 93, 102, 204, 321, 377,
                            100, 105, 174, 395, 151, 129, 312, 326, 155, 149,
                            193, 251, 188, 76, 385, 115, 229, 104, 233, 119,
                            120, 121, 122, 335, 352, 73, 180, 152, 391, 54,
                            189, 46, 132, 330, 112, 118, 237, 235, 327, 332,
                            88, 376, 142, 138, 178, 397, 354, 228, 159, 71,
                            148, 10, 217, 268, 307, 31, 291, 157, 199, 365,
                            399, 96, 162, 163, 255, 206, 379, 23, 168, 169,
                            62, 279, 64, 328, 29, 270, 259, 357, 382, 271,
                            128, 285, 216, 289, 324, 386, 186, 87, 325, 214,
                            280, 183, 192, 51, 61, 79, 305, 244, 65, 114,
                            127, 201, 202, 203, 97, 366, 306, 53, 107, 56,
                            210, 309, 158, 117, 344, 16, 182, 150, 266, 303,
                            292, 218, 133, 164, 136, 372, 226, 315, 6, 262,
                            179, 317, 232, 340, 234, 11, 236, 124, 311, 170,
                            134, 339, 375, 17, 33, 250, 370, 78, 9, 184,
                            359, 74, 275, 231, 278, 230, 316, 27, 295, 3,
                            113, 261, 75, 343, 264, 172, 274, 267, 219, 63,
                            70, 177, 283, 40, 7, 209, 308, 37, 14, 247,
                            225, 393, 282, 334, 284, 367, 249, 36, 161, 398,
                            83, 384, 103, 349, 145, 48, 19, 167, 298, 220,
                            59, 99, 185, 290, 304, 269, 2, 140, 238, 265,
                            310, 364, 323, 22, 13, 101, 154, 95, 30, 245,
                            320, 153, 111, 147, 191, 50, 80, 363, 224, 213,
                            222, 351, 276, 257, 371, 392, 345, 221, 336, 141,
                            374, 135, 287, 144, 356, 390, 302, 347, 337, 198,
                            197, 381, 69, 207, 297, 253, 353, 313, 194, 143,
                            360, 361, 171, 84, 200, 240, 34, 173, 331, 68,
                            81, 243, 60, 277, 125, 15, 38, 24, 394, 387,
                            300, 195, 252, 165, 166, 205, 25, 211, 212, 98,
                            91, 126, 378, 322, 20, 314, 106, 1, 181, 248};

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
    dbss << "arr[]=\n{\n";
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
            hexstr << hex << i;
            test_treeF.insert(key, to_string(key), cso); // print case(?...)
            test_treeF.visualize(6, cso);
            cso << "tree.nodeNum: " << test_treeF.nodeNum << endl;
            // bool err = !test_treeF.check(j == 0 ? i + 1 : n, cso);
            bool err = false;
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
    cerr << "\n\nReady insert overflow char*\n";

    test_treeF.insert(0, jojo, cerr);
    test_treeF.insert(1, jojo, cerr);
    test_treeF.insert(2, jojo, cerr);
    test_treeF.visualize(6, cout, true);
    test_treeF.search(-1);
    test_treeF.search(250);
    cerr << "\n=========== END OF DEBUG ===========\n";
    return 0;
}
// sort(arr_record, arr_record + 50);