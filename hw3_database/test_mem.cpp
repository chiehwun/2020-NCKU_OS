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
    int MAX = (1 << 12) / sizeof(Data); // assume 4KB/block
    Node *node = new Node(MAX);
    Node *node_cp = new Node(MAX);
    node->set_sizeMAX();
    node_cp->set_sizeMAX();
    cout << "sizeof(Data::key): " << sizeof(Data::key) << endl;
    cout << "sizeof(Data::str): " << sizeof(Data::str) << endl;
    cout << "sizeof(Data): " << sizeof(Data) << endl;
    cout << "4 KB / sizeof(Data): " << float(1 << 12) / sizeof(Data) << endl;
    string test_str("strnetworkkkkkk");
    for (int i = 0; i < node->MAX_SIZE; ++i)
    {
        cout << "copy char[128]: i = " << i << endl;
        node->datum[i].key = i;
        strncpy(node->datum[i].str, test_str.c_str(), sizeof(Data::str));
    }
    node->display();
    memcpy(node_cp->datum, node->datum, sizeof(Data) * MAX);
    node_cp->display();

    // Write to file test
    string outFilename = "debug_logfile/test0.file";
    ofstream ofs(outFilename, ios::out);
    if (!ofs)
    {
        cout << "Cannot open file\"" << outFilename << "\"!";
        exit(EXIT_FAILURE);
    }
    return 0;
}

/******************** Handling Non-null-terminated char ********************/
// Handling Non-null-terminated char (NNTC)
// string test0_str("123456789");
// char test0_chr[5];
// // pass string into function and interact with char
// strncpy(test0_chr, test0_str.c_str(), sizeof(test0_chr));
// cout << "str :" << test0_str << endl;
// cout << "char.length:" << strlen(test0_chr) << endl;
// cout << "char:";
// // print out NNTC
// cout.write(test0_chr, strlen(test0_chr));

// In Data::str <aka char[128]>
// Node *node1 = new Node(5);
// node1->set_sizeMAX();
// string test1_str("12345");
// // copy from string type to NNTC
// strncpy(node1->datum[0].str, test1_str.c_str(), sizeof(Data::str));
// cout << "     input string:" << test1_str << endl;
// cout << "strlen(Data::str):" << strlen(node1->datum[0].str) << endl;
// cout << "sizeof(Data::str):" << sizeof(Data::str) << endl;
// cout << "        Data::str:";
// // print by NNTC
// cout.write(node1->datum[0].str,
//            NNTC_SIZE(node1->datum[0].str));
// // NNTC convert to string
// string test1_1_str(node1->datum[0].str,
//                    NNTC_SIZE(node1->datum[0].str));
// cout << "\n\n====== NNTC convert to string2 ======\n";
// cout << "   string2.length:" << test1_1_str.length() << endl;
// cout << "          string2:" << test1_1_str << "\n\n";
// node1->display();

/******************** Bulk loading test ********************/
// int n = 100000;
// int64_t arr_sort[n]{};
// for (int64_t i = 0; i < n; ++i)
//     arr_sort[i] = i + 1;
// BPTree tree(10);
// tree.bulk_load(n);
// tree.check_leaf(arr_sort, n, cout);

/******************** Node deletion test ********************/
// int64_t *ipt = &node_cp->datum[0].key;
// char *cpt = node_cp->datum[0].str;
// cout << "Before deletion: node_cp->datum[0].key = " << *ipt << endl;
// cout << "Before deletion: node_cp->datum[0].str = " << node_cp->datum[0].str << endl;
// delete node_cp;
// cout << "After  deletion: node_cp->datum[0].key = " << *ipt << endl;
// cout << "Before deletion: node_cp->datum[0].str = " << node_cp->datum[0].str << endl;
