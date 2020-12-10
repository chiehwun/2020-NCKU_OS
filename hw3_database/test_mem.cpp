#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include "BPTree.h"
#include "Clock.h"
#include "Mem_Sys_Set.h"

using namespace std;

int main()
{
    int n = 100000;
    int64_t arr_sort[n]{};
    for (int64_t i = 0; i < n; ++i)
        arr_sort[i] = i + 1;
    BPTree tree(10);
    tree.bulk_load(n);
    tree.check_leaf(arr_sort, n, cout);
    return 0;
    int MAX = (1 << 12) / sizeof(Data); // assume 4KB/block
    Node *node = new Node(MAX);
    Node *node_cp = new Node(MAX);
    node->set_size_full();
    node_cp->set_size_full();
    cout << "sizeof(Data::key): " << sizeof(Data::key) << endl;
    cout << "sizeof(Data::str): " << sizeof(Data::str) << endl;
    cout << "sizeof(Data): " << sizeof(Data) << endl;
    cout << "4 KB / sizeof(Data): " << float(1 << 12) / sizeof(Data) << endl;
    char strnetwork[128] = "strnetworkkkkkk";
    for (int i = 0; i < node->MAX_SIZE; ++i)
    {
        cout << "copy char[128]: i = " << i << endl;
        node->datum[i].key = i;
        strnetwork[15] = '0' + i % 10;
        memcpy(node->datum[i].str, strnetwork, 128);
    }
    node->display();
    memcpy(node_cp->datum, node->datum, sizeof(Data) * MAX);
    node_cp->display();
    // Write to file
    string outFilename = "debug_logfile/test0.file";
    ofstream ofs(outFilename, ios::out);
    if (!ofs)
    {
        cout << "Cannot open file\"" << outFilename << "\"!";
        exit(EXIT_FAILURE);
    }
    return 0;
}
// Node deletion test
// int64_t *ipt = &node_cp->datum[0].key;
// char *cpt = node_cp->datum[0].str;
// cout << "Before deletion: node_cp->datum[0].key = " << *ipt << endl;
// cout << "Before deletion: node_cp->datum[0].str = " << node_cp->datum[0].str << endl;
// delete node_cp;
// cout << "After  deletion: node_cp->datum[0].key = " << *ipt << endl;
// cout << "Before deletion: node_cp->datum[0].str = " << node_cp->datum[0].str << endl;
