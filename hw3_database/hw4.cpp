#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#include <climits>
#include "db.h"
#include "BPTree.h"
using namespace std;

int main(int argc, char *argv[])
{
    BPTree tree;
    /* cursor is root */
    tree.insert(1, "a");
    tree.insert(2, "a");
    tree.insert(3, "a");
    tree.insert(20, "a");
    tree.insert(40, "a");
    tree.insert(60, "a"); // break
    tree.display(tree.root);
    for (int i = 0; i < 6; ++i)
    {
        cout << "ptr[" << i << "]";
        tree.display(tree.root->ptr[i]);
    }
    // fill rest ptr[0]
    tree.insert(4, "a");
    tree.insert(5, "a");

    tree.insert(22, "a");
    tree.insert(24, "a");
    tree.insert(80, "a"); // break
    // fill rest ptr[1]
    tree.insert(25, "a");
    tree.insert(26, "a");

    tree.insert(42, "a");
    tree.insert(44, "a");
    tree.insert(100, "a"); // break
    // fill rest ptr[2]
    tree.insert(46, "a");
    tree.insert(47, "a");

    tree.insert(61, "a");
    tree.insert(63, "a");
    tree.insert(82, "a"); // break
    // fill rest ptr[3]
    tree.insert(65, "a");
    tree.insert(66, "a");

    tree.insert(84, "a");
    tree.insert(101, "a");
    tree.insert(103, "a"); //break
    // fill rest ptr[3]
    tree.insert(86, "a");
    tree.insert(87, "a");
    tree.display(tree.root);
    for (int i = 0; i < 6; ++i)
    {
        cout << "ptr[" << i << "]";
        tree.display(tree.root->ptr[i]);
    }
    tree.display_leaf();

    /* cursor not root && parent overflow */
    cerr << "test2" << endl;
    tree.insert(23, "a");
    // 3-layer print
    tree.display(tree.root);
    for (int i = 0; i <= tree.root->size; ++i)
    {
        cout << "ptr[" << i << "]";
        tree.display(tree.root->ptr[i]);
    }
    for (int j = 0; j <= tree.root->size; ++j)
    {
        for (int i = 0; i <= tree.root->ptr[j]->size; ++i)
        {
            cout << "root->ptr[" << j << "]->ptr[" << i << "]:";
            tree.display(tree.root->ptr[j]->ptr[i]);
        }
    }
    return 0;
}
// s:5 | 20 40 60 80 100
// ptr[0]s:5 | 1 2 3 4 5
// ptr[1]s:5 | 20 22 24 25 26
// ptr[2]s:5 | 40 42 44 46 47
// ptr[3]s:5 | 60 61 63 65 66
// ptr[4]s:5 | 80 82 84 86 87
// ptr[5]s:3 | 100 101 103