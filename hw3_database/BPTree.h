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
#include <sstream>
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
    Node(int m);
};

// BP tree
class BPTree
{
private:
    void insertInternal(int64_t x,
                        stack<Node *> &parent_stack,
                        Node *child,
                        ostream &cso);
    Node *findParent(Node *cursor,
                     Node *child);
    void visualize(Node *root,
                   int width,
                   int lay,
                   ostream &cso);

    bool locate(const int64_t x,
                const int64_t *arr,
                const int n,
                int &pos);

public:
    Node *root;
    const int MAX;
    BPTree(int m);
    bool search(int64_t x);
    void insert(int64_t x, string y,
                ostream &cso);
    Node *getRoot();
    void visualize(int width,
                   ostream &cso);
    string str_repeat(int n, const string str);
    bool check(int64_t key_num,
               ostream &cso);
    bool check_tot(int64_t arr_sort[],
                   int64_t n,
                   ostream &cso);
};
// cout << "├── \n│├──\n└─\n\n";
#endif // BPTREE