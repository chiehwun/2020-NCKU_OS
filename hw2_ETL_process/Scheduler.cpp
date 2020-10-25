#ifndef SCHEDULER_H
#define SCHEDULER_H
#include "Scheduler.h"

// class node
node::node(int init)
{
    file_code = init;
    left = right = parent = NULL;
}

void node::run(Scheduler *sch)
{
    if (left == NULL || right == NULL)
        return;
    thread t_left(&node::run, ref(left), sch);
    thread t_right(&node::run, ref(right), sch);
    t_left.join();
    t_right.join();
    concatenates(left->file_code, right->file_code);

    sch->update_status(right->file_code);
    sch->tr->remove_child(this);
}

// class tree
tree::tree(int file_num) : file_num(file_num),
                           layer(0)
{
    // Constructs leaf nodes
    vector<node *> child_nodes;
    for (int i = 0; i < file_num; ++i)
        child_nodes.push_back(new node(i));

    while (child_nodes.size() > 1)
    {
        // cerr << "\nlayer: " << layer << endl;
        vector<node *> parent_nodes;
        for (int i = 0; i < child_nodes.size() - 1; i += 2)
        {
            // cerr << "i=" << i << "(" << child_nodes.at(i)->file_code << ") ";
            parent_nodes.push_back(new node(child_nodes.at(i)->file_code));
            build(parent_nodes.back(), child_nodes.at(i), child_nodes.at(i + 1));
        }
        // Exists a prepared node
        if (child_nodes.size() % 2 == 1)
            parent_nodes.push_back(child_nodes.back());
        child_nodes.clear();
        child_nodes = move(parent_nodes);
        ++layer;
    }
    root = child_nodes.front();
#ifdef DEBUG
    preview();
#endif
}

void tree::preview()
{
    cout << "\n\nVisualize Parallel Scheduler: " << layer << endl;
    vector<node *> p_nodes;
    p_nodes.push_back(root);
    int lay = layer + 2;
    cout << setw(lay-- * 7) << " ";
    cout << setw(3) << root->file_code << "\n";
    while (p_nodes.size() > 0)
    {
        vector<node *> c_nodes;
        cout << setw(lay-- * 7) << " ";
        for (int i = 0; i < p_nodes.size(); ++i)
        {
            if (p_nodes.at(i)->left != NULL)
            {
                c_nodes.push_back(p_nodes.at(i)->left);
                cout << setw(3) << c_nodes.back()->file_code;
            }
            else
                cout << setw(3) << "x";

            if (p_nodes.at(i)->right != NULL)
            {
                c_nodes.push_back(p_nodes.at(i)->right);
                cout << setw(3) << c_nodes.back()->file_code << "|";
            }
            else
                cout << setw(3) << "x"
                     << "|";
        }
        cout << "\n";
        p_nodes.clear();
        p_nodes = move(c_nodes);
    }
}

node *tree::get_root()
{
    return root;
}
void tree::remove_child(node *p)
{
    if (p == NULL)
        return;
    if (p->left != NULL)
    {
        delete p->left;
        p->left = NULL;
    }
    if (p->right != NULL)
    {
        delete p->right;
        p->right = NULL;
    }
}

void tree::build(node *p, node *l, node *r)
{
    if (p == NULL)
        return;
    p->left = l;
    p->right = r;
    if (r != NULL)
        r->parent = p;
    if (l != NULL)
        l->parent = p;
}
bool tree::is_root(node *n)
{
    return n->parent == NULL;
}
bool tree::is_leaf(node *n)
{
    return n->left == NULL && n->right == NULL;
}

// class Scheduler
Scheduler::Scheduler(int tot) : file_num(tot),
                                tr(new tree(file_num))
{
    cout << "Launching Scheduler...\n";
    status = vector<bool>(file_num, true);
}

void Scheduler::start()
{
    print_status();
    start_thd = thread(&node::run, tr->get_root(), this);
}

void Scheduler::update_status(int file_code)
{
    // protect cout and status
    unique_lock<mutex> locker(mu_cout);
    status.at(file_code) = false;
    print_status();
}

void Scheduler::print_status()
{
    for (int i = 0; i < file_num; ++i)
        if (status.at(i))
            cout << setw(4) << i;
    cout << "\n";
}

void Scheduler::join()
{
    start_thd.join();
}

Scheduler::~Scheduler()
{
    delete tr;
}

void concatenates(int f1, int f2)
{
    string filename1 = to_string(f1) + ".thdout";
    string filename2 = to_string(f2) + ".thdout";
    ofstream fs1(filename1, ios::app);
    ifstream fs2(filename2, ios::in);
    if (!fs1)
    {
        cerr << "\"" << filename1 << "\" failed to open." << endl;
        return;
    }
    if (!fs2)
    {
        cerr << "\"" << filename2 << "\" failed to open." << endl;
        return;
    }
    char buffer[BF_RD_CONC_SZ] = "";

    while (!fs2.eof())
    {
        fs2.read(buffer, BF_RD_CONC_SZ);
        fs1 << buffer;
        memset(buffer, 0, BF_RD_CONC_SZ);
    }
    fs1.close();
    fs2.close();
    // protect remove()
    unique_lock<mutex> locker(mu_remove);
    remove(filename2.c_str());
}
#endif // SCHEDULER_H