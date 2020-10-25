#include <thread>
#include <future>
#include <functional> // std::bind()
#include <chrono>
#include <vector>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <algorithm>
using namespace std;
extern mutex mu_cout;
extern mutex mu_remove;
const long BF_RD_CONC_SZ = 5 << 20; // 5 MiB

void concatenates(int f1, int f2);
class Scheduler;

class node
{
public:
    node(int init);
    void run(Scheduler *sch);
    int file_code;
    class node *left;
    class node *right;
    class node *parent;
};

class tree
{
public:
    tree(int file_num);
    void preview(); // Visualize the tree
    node *get_root();
    void remove_child(node *p);
    const int file_num;

private:
    void build(node *p, node *l, node *r);
    bool is_root(node *n);
    bool is_leaf(node *n);
    node *root;
    int layer;
};

class Scheduler
{
public:
    Scheduler(int tot);
    ~Scheduler();
    void start();
    void update_status(int file_code); // protect cout and status
    void join();
    const int file_num; // beware of initialization order!
    tree *tr;

private:
    void print_status();
    thread start_thd;
    vector<bool> status;
};