#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include <random>
#include "db.h"
#include "progress_bar.h"
#include "Mem_Sys_Set.h"
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
    // int test[] = {0, 1, 3, 4, 4, 3, 0};
    // int n1 = sizeof(test) / sizeof(*test);
    // DB ddbb(10, 10);
    // ddbb.create_new_file(0);
    // DFile *new_file = ddbb.create_new_file(0);
    // ddbb.lru_mgmt->refer(new_file);
    // ddbb.lru_mgmt->display();
    // ddbb.lru_mgmt->refer(ddbb.DFile_admin.at(0));
    // ddbb.lru_mgmt->display();
    // // for (int i = 0; i < n1; ++i)
    // // {
    // //     lru.refer(ddbb.DFile_admin.at(test[i]));
    // //     lru.display();
    // // }
    // return 0;

    int entry = (1 << 12) / sizeof(Data);             // assume 4KB/block
    int64_t block = (1 << 30) / sizeof(Data) / entry; // 1GB per file
    cout << "entry: " << entry << endl;
    cout << "block: " << block << endl;
    cerr << "\n=========== START DEBUG ===========\n";
    int n = 100000000;
    DB db_test(block, entry);
    db_test.DBinit();

    // // PUT TEST
    // for (int i = 0; i < n; ++i)
    // {
    //     db_test.put(i, to_string(i));
    //     if (i % 1000000 == 0)
    //     {
    //         cerr << "PUT " << i << "\n";
    //         db_test.DBstatus();
    //         db_test.lru_mgmt->display();
    //     }
    // }
    // // GET TEST
    // for (int i = 0; i < n; ++i)
    //     db_test.get(i);

    // MIX TEST
    // for (int i = 0; i < n; ++i)
    // {
    //     db_test.put(i, to_string(i));
    //     db_test.get(i);
    // }

    db_test.DBsave();
    cerr << "\n=========== " << argv[0] << ": END OF DEBUG ===========\n";
    return 0;
}

// db_test.put(2, "D4DJ");
// db_test.put(2, "Happy Around");
// db_test.put(3, "Peaky P-key");
// db_test.put(4, "Photon Maiden");
// db_test.put(5, "Merm4id");
// db_test.put(6, "RONDO");
// db_test.put(7, "Lyrical Lily");