#ifndef BPTREE
#define BPTREE
#include <climits>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <queue>
#include <stack>
#define MAX (3) // key number
using namespace std;

// BP node
class Node
{
public:
    int64_t *key;
    string *str;
    int size; // key number
    Node **ptr;
    bool IS_LEAF;
    friend class BPTree;

public:
    Node();
};

// BP tree
class BPTree
{
private:
    void insertInternal(int64_t x,
                        Node *cursor,
                        Node *child);
    Node *findParent(Node *cursor,
                     Node *child);
    void visualize(Node *root, int width, int lay);

public:
    Node *root;
    BPTree();
    bool search(int64_t x);
    void insert(int64_t x, string y);
    void display(Node *cursor);
    Node *getRoot();
    void display_leaf();
    void visualize(int width);
    string str_repeat(int n, const string str);
    void check(int64_t key_num);
    void check_tot(int64_t arr_sort[], int64_t n);
};
// cout << "├── \n│├──\n└─\n\n";
#endif // BPTREE