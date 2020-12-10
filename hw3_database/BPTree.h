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
#include <cstring> // memcpy()
#define _MIN(a, b) (((a) < (b)) ? (a) : (b))
#define NNTC_SIZE(a) _MIN(sizeof(Data::str), std::strlen(a))
#define SHOW_NNTC(a, b) (a).write(b, NNTC_SIZE(b))
#define NNTC_to_string(a) string(a, NNTC_SIZE(a))

using namespace std;
// POD structlog_prev
typedef struct Data
{
    int64_t key;
    char str[128];
} Data;

// BP node
class Node
{
public:
    Data *datum;
    // int64_t *key;
    // string *str;
    int size; // key number
    Node **ptr;
    bool IS_LEAF;
    const int MAX_SIZE;
    friend class BPTree;

public:
    Node(int m, bool isleaf = true);
    ~Node();
    int64_t leastRSub(int ptr);
    void set_sizeMAX();
    void display();
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
                   ostream &cso,
                   bool show_str);

    bool locate(const int64_t x,
                const Data *datum,
                const int n,
                int &pos);

public:
    Node *root;
    const int MAX;
    const int HALF;
    const int MIN;
    int64_t nodeNum;
    BPTree(int m);
    ~BPTree();
    void bulk_load(int64_t num);
    void del_tree(Node *cursor);
    bool search(int64_t x);
    void insert(int64_t x, string y,
                ostream &cso);
    void del(int64_t x, ostream &cso);
    Node *getRoot();
    void visualize(int width,
                   ostream &cso,
                   bool show_str = false);
    string str_repeat(int n, const string str);
    bool check(int64_t key_num,
               ostream &cso);
    bool check_leaf(int64_t arr_sort[],
                    int64_t n,
                    ostream &cso);
};
// cout << "├── \n│├──\n└─\n\n";
#endif // BPTREE