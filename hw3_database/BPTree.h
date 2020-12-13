#ifndef BPTREE_H
#define BPTREE_H

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
extern ofstream OFS;
extern bool FIRST_PRINT;

// POD struct
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
    bool locate(const int64_t x,
                const Data *datum,
                const int n,
                int &pos);
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

public:
    static const string jojo;
    Node *root;
    const int MAX;
    const int HALF;
    const int MIN;
    const int64_t NODE_MAX_NUM;
    // tree status
    int64_t NodeNum;
    int64_t leafNodeNum;
    int64_t min_key;
    int64_t max_key;
    BPTree(int m, int n);
    ~BPTree();
    void reset_tree();
    void del_tree(Node *cursor, bool del_leaf = true);
    void bulk_load_parents(queue<Node *> &qu);
    void bulk_load_test(int64_t num);
    bool search(int64_t x,
                bool show = true);
    bool insert(int64_t x, string y,
                ostream &cso); // return overflow?
    void update_mkey();
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
#endif // BPTREE_H