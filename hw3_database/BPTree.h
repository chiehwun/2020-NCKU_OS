#ifndef BPTREE
#define BPTREE
#include <climits>
#include <fstream>
#include <iostream>
#include <sstream>
#define MAX (5) // key number
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

public:
    Node *root;
    BPTree();
    void search(int64_t x);
    void insert(int64_t x, string y);
    void display(Node *cursor);
    Node *getRoot();
    void display_leaf();
};
#endif // BPTREE