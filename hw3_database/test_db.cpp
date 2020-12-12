#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include <random>
#include "db.h"
#include "progress_bar.h"
#define RAND_TEST true
#define RAND_TEST_NUM 10
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
    cerr << "\n=========== START DEBUG ===========\n";
    DB db_test(20000L, 5);
    db_test.DBinit();

    // DFile file(0, 20000L, 5);
    // file.tree->insert(1, BPTree::jojo, cerr);
    // file.tree->insert(2, "D4DJ", cerr);
    // file.tree->insert(2, "Happy Around", cerr);
    // file.tree->insert(3, "Peaky P-key", cerr);
    // file.tree->insert(4, "Photon Maiden", cerr);
    // file.tree->insert(5, "Merm4id", cerr);
    // file.tree->insert(6, "RONDO", cerr);
    // file.tree->insert(7, "Lyrical Lily", cerr);
    // file.tree->insert(8, "D4DJ", cerr);
    // file.tree->insert(9, "D4DJ", cerr);
    // file.tree->insert(10, "D4DJ", cerr);
    // for (int i = 11; i <= 100; ++i)
    //     file.tree->insert(i, to_string(i), cerr);
    // // file.tree->visualize(6, cerr, true);
    // file.dump(cerr);
    // cerr << "========== cache_read ==========\n";
    // file.cache_read();
    // cerr << "========== cache_search ==========\n";
    // file.cache_search(6);
    // file.cache_search(100);
    // file.cache_search(456);
    // file.bulk_load();

    cerr << "\n=========== " << argv[0] << ": END OF DEBUG ===========\n";
    return 0;
}