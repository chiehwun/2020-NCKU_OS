#include "BPTree.h"
const string BPTree::jojo = "\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
JOJOJOJOJOJOJOJOJOJOJOJOJOJOJO\
12345678900";
// Constructor of Node(MAX, ISLEAF = true)
Node::Node(int m, bool isleaf) : MAX_SIZE(m)
{
    size = 0;
    IS_LEAF = isleaf;
    // Dynamic allocation
    datum = new Data[MAX_SIZE]();
    ptr = new Node *[MAX_SIZE + 1]();
}

Node::~Node()
{
    // Dynamic allocation
    delete[] datum;
    delete[] ptr;
}

// Remind ptrR > size
int64_t Node::leastRSub(int ptrR)
{
    if (!this || !this->ptr[ptrR] || ptrR > size)
    {
        cerr << "Node::leastRSub(): err!\n";
        exit(EXIT_FAILURE);
    }
    Node *cursor = this->ptr[ptrR];
    // Reach leaf node
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    return cursor->datum[0].key;
}

void Node::set_sizeMAX()
{
    size = MAX_SIZE;
}

void Node::display()
{
    cout.imbue(std::locale("C"));
    cout << "========== Node::display() ==========\n";
    cout << left << setw(15) << "Node address:" << hex << this << dec << endl;
    cout << left << setw(15) << "MAX_SIZE: " << MAX_SIZE << endl;
    cout << left << setw(15) << "size: " << size << endl;
    cout << left << setw(15) << "IS_LEAF: " << boolalpha << IS_LEAF << endl;
    for (int i = 0; i < size; ++i)
    {
        cout << "datum[" << right << setw(3) << i << "]"
             << "(" << setw(3) << NNTC_SIZE(datum[i].str) << "):"
             << right << setw(25) << datum[i].key << ",";
        cout.write(datum[i].str,
                   NNTC_SIZE(datum[i].str));
        cout << endl;
    }
}

// ENTRY_PER_BLK, NODE_MAX_NUM
BPTree::BPTree(int m, int n) : MAX(m),
                               HALF((m + 1) / 2),
                               MIN(m / 2),
                               NODE_MAX_NUM(n)
{
    root = NULL;
    leafNodeNum = NodeNum = 0;
}

BPTree::~BPTree()
{
    // cout.imbue(std::locale("C"));
    // cout << hex << "****** ~BPTree(): " << this << dec << " ******\n";
    del_tree(root, true);
    if (NodeNum != 0)
    // cout << "~BPTree(): delete OK.\n";
    // else
    {
        cerr << "~BPTree(): NodeNum != 0 !!\n";
        exit(EXIT_FAILURE);
    }
}

// delete all nodes and set root to NULL
void BPTree::reset_tree()
{
    this->~BPTree();
    NodeNum = leafNodeNum = 0;
    root = NULL;
}

// Bulk load part 2: construct parent nodes
void BPTree::bulk_load_parents(queue<Node *> &qu)
{
    queue<Node *> qu_parent;
    // Update node number
    leafNodeNum = NodeNum = qu.size();
    while (qu.size() > 1)
    {
        while (!qu.empty())
        {
            Node *nonleaf = new Node(MAX, false);
            ++NodeNum;
            for (int i = 0; i < HALF + 1 && !qu.empty(); ++i)
            {
                nonleaf->ptr[i] = qu.front();
                if (i > 0)
                {
                    ++nonleaf->size;
                    nonleaf->datum[i - 1].key = nonleaf->leastRSub(i);
                }
                qu.pop(); // after linking
            }
            if (qu.size() == 1)
            {
                // cout << "qu.size = 1" << endl;
                ++nonleaf->size;
                nonleaf->ptr[nonleaf->size] = qu.front();
                nonleaf->datum[nonleaf->size - 1].key =
                    nonleaf->leastRSub(nonleaf->size);
                qu.pop(); // after linking
            }
            qu_parent.push(nonleaf);
        }
        qu.swap(qu_parent);
    }
    root = qu.front();
    update_mkey();
}

void BPTree::bulk_load_test(int64_t num)
{
    if (root)
        return;
    queue<Node *> qu, qu_parent;
    int64_t i = 0;
    while (i < num)
    {
        Node *leaf = new Node(MAX, true);
        // Link leaf node
        if (!qu.empty())
            qu.back()->ptr[qu.back()->size] = leaf;
        // Fill leaf.key with HALF size
        for (int j = 0; j < HALF && i < num; ++j)
        {
            leaf->datum[j].key = i + 1;
            leaf->size++;
            ++i;
        }
        qu.push(leaf);
    }
    NodeNum = qu.size();
    cerr << "HALF: " << HALF << endl;
    cerr << "NodeNum: " << NodeNum << endl;
    while (qu.size() > 1)
    {
        while (!qu.empty())
        {
            Node *nonleaf = new Node(MAX, false);
            ++NodeNum;
            for (int i = 0; i < HALF + 1 && !qu.empty(); ++i)
            {
                nonleaf->ptr[i] = qu.front();
                if (i > 0)
                {
                    ++nonleaf->size;
                    nonleaf->datum[i - 1].key = nonleaf->leastRSub(i);
                }
                qu.pop(); // after linking
            }
            if (qu.size() == 1)
            {
                cerr << "qu.size = 1" << endl;
                ++nonleaf->size;
                nonleaf->ptr[nonleaf->size] = qu.front();
                nonleaf->datum[nonleaf->size - 1].key =
                    nonleaf->leastRSub(nonleaf->size);
                qu.pop(); // after linking
            }
            qu_parent.push(nonleaf);
        }
        qu.swap(qu_parent);
    }
    root = qu.front();
    visualize(3, cerr);
    check(num, cerr);
}

void BPTree::del_tree(Node *cursor, bool del_leaf)
{
    // LRV traverse
    if (!cursor || (cursor->IS_LEAF && !del_leaf))
        return;
    // leaf node needless to delete last link
    int n = cursor->IS_LEAF ? cursor->size : cursor->size + 1;
    for (int i = 0; i < n; ++i)
        del_tree(cursor->ptr[i], del_leaf);
    --NodeNum;
    delete cursor;
    // cerr << "BPtree::NodeNum = " << NodeNum << "\n\n";
}

// return true if repeat key exists.
// Traverse and overwrite &pos where
// arr[n] < key, n < pos; key <= arr[n], n >= pos
bool BPTree::locate(const int64_t x,
                    const Data *datum,
                    const int n, // size
                    int &pos)
{
    // 1. pos <= n
    // 2. input datum[n] may overflow,
    //    means datum's size > n
    pos = 0;
    // TODO: CHANGE TO BINARY SEARCH
    while (x > datum[pos].key && pos < n)
        pos++;
    if (pos == n)
        // datum[n].key is "garbage value"
        return false;
    else
        return x == datum[pos].key;
}

// Function to find any element in B+ Tree
bool BPTree::search(int64_t x, bool show)
{
    // If tree is empty
    if (root == NULL)
        cerr << "Tree is empty\n";
    // Traverse to find the value
    else
    {
        Node *cursor = root;
        // Till we reach leaf node
        while (!cursor->IS_LEAF)
        {
            for (int i = 0; i < cursor->size; i++)
            {
                // If the element to be
                // found is not present
                if (x < cursor->datum[i].key)
                {
                    cursor = cursor->ptr[i];
                    break;
                }

                // If reaches end of the
                // cursor node
                if (i == cursor->size - 1)
                {
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        if (show)
            cerr << "search(" << x << "):";
        for (int i = 0; i < cursor->size; i++)
        {
            if (cursor->datum[i].key == x)
            {
                OFS << (FIRST_PRINT ? "" : "\n");
                SHOW_NNTC(OFS, cursor->datum[i].str);
                if (show)
                {
                    SHOW_NNTC(cerr, cursor->datum[i].str);
                    cerr << "\n";
                }
                return true;
            }
        }
        OFS << (FIRST_PRINT ? "" : "\n") << "EMPTY";
        if (show)
            cerr << " Not found.\n";
    }
    return false;
}

// Function to implement the Insert
// Operation in B+ Tree
bool BPTree::insert(int64_t x, string y, ostream &cso)
{
    // If root is null then return
    // newly created node
    if (root == NULL)
    {
#ifdef BPTree_DBUG
        cso << "case (A)" << endl;
#endif
        root = new Node(MAX, true);
        ++NodeNum;
        ++leafNodeNum;
        root->datum[0].key = x;
        strncpy(root->datum[0].str, y.c_str(), sizeof(Data::str));
        root->size = 1;
    }
    // Traverse the B+ Tree
    else
    {
        Node *cursor = root;
        stack<Node *> parent_stack;
        // Till cursor reaches the leaf node
        while (cursor->IS_LEAF == false)
        {
            parent_stack.push(cursor);
            for (int i = 0; i < cursor->size; i++)
            {
                // If found the position
                // where we have to insert node
                if (x < cursor->datum[i].key)
                {
                    cursor = cursor->ptr[i];
                    break;
                }

                // If reaches the end
                if (i == cursor->size - 1)
                {
                    cursor = cursor->ptr[i + 1];
                    break;
                }
            }
        }
        if (cursor->size < MAX)
        {
            int pos_leaf = 0; // pass by reference
            if (locate(x, cursor->datum, cursor->size, pos_leaf))
            {
#ifdef BPTree_DBUG
                cso << "case (R0): repeat key = " << x << endl;
#endif
                strncpy(cursor->datum[pos_leaf].str, y.c_str(), sizeof(Data::str));
                return leafNodeNum > NODE_MAX_NUM;
            }
#ifdef BPTree_DBUG
            cso << "case (B): leaf node non-ovf." << endl;
#endif
            // right shift after pos_leaf
            for (int j = cursor->size; j > pos_leaf; j--)
                cursor->datum[j] = cursor->datum[j - 1];
            // Update last ptr (link to next leaf node)
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
            cursor->datum[pos_leaf].key = x;
            strncpy(cursor->datum[pos_leaf].str, y.c_str(), sizeof(Data::str));
        }
        // leaf node Overflow
        else
        {
            Node virtualNode(MAX + 1);
            // int virtualNode[MAX + 1] = {0};
            // Copy overflowed key to virtualNode
            for (int i = 0; i < MAX; i++)
                virtualNode.datum[i] = cursor->datum[i];

            int pos_leaf = 0; // pass by reference
            if (locate(x, virtualNode.datum, MAX, pos_leaf))
            {
#ifdef BPTree_DBUG
                cso << "case (R1): repeat key = " << x << endl;
#endif
                strncpy(cursor->datum[pos_leaf].str, y.c_str(), sizeof(Data::str));
                return leafNodeNum > NODE_MAX_NUM;
            }

            // Right shift
            // (MAX + 1)->(MAX)??
            for (int j = MAX; j > pos_leaf; j--)
                virtualNode.datum[j] = virtualNode.datum[j - 1];
            // Insert new data
            strncpy(virtualNode.datum[pos_leaf].str, y.c_str(), sizeof(Data::str));
            virtualNode.datum[pos_leaf].key = x;
            // Create a newLeaf node
            Node *newLeaf = new Node(MAX, true);
            ++NodeNum;
            ++leafNodeNum;
            // Update size
            cursor->size = HALF;
            newLeaf->size = MAX + 1 - HALF;
            // Update last link
            cursor->ptr[cursor->size] = newLeaf;
            newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
            cursor->ptr[MAX] = NULL;

            // Overwrite the key of cursor node
            int i = 0, j = 0;
            for (i = 0; i < cursor->size; i++)
                cursor->datum[i] = virtualNode.datum[i];
            // Write the newLeaf key
            for (i = 0, j = cursor->size;
                 i < newLeaf->size; i++, j++)
                newLeaf->datum[i] = virtualNode.datum[j];

            // If cursor is the root node
            if (cursor == root)
            {
#ifdef BPTree_DBUG
                cso << "case (C)" << endl;
#endif
                // Create a new Node
                Node *newRoot = new Node(MAX, false);
                ++NodeNum;
                // Update rest field of
                // B+ Tree Node
                newRoot->datum[0].key = newLeaf->datum[0].key;
                strncpy(newRoot->datum[0].str, newLeaf->datum[0].str, sizeof(Data::str));
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newLeaf;
                newRoot->size = 1;
                root = newRoot;
            }
            // If cursor is not the root node
            else
            {
                // Recursive Call for
                // insert in internal
                insertInternal(newLeaf->datum[0].key,
                               parent_stack,
                               newLeaf,
                               cso);
            }
        }
    }
    update_mkey();
    return leafNodeNum > NODE_MAX_NUM;
}

// Function to implement the Insert
// Internal Operation in B+ Tree
void BPTree::insertInternal(int64_t x,
                            stack<Node *> &parent_stack, // parent
                            Node *child,
                            ostream &cso)
{
    Node *cursor = parent_stack.top();
    parent_stack.pop();
    // If parent not overflow
    if (cursor->size < MAX)
    {
#ifdef BPTree_DBUG
        cso << "case (D): call insertInternal()" << endl;
#endif
        // Traverse the child node
        // for current cursor node
        int pos = 0; // pass by reference
        locate(x, cursor->datum, cursor->size, pos);

        // Right shift parent data
        for (int j = cursor->size; j > pos; j--)
            cursor->datum[j] = cursor->datum[j - 1];
        // Right shift parent ptr
        for (int j = cursor->size + 1; j > pos + 1; j--)
            cursor->ptr[j] = cursor->ptr[j - 1];
        // Only key need to be copied
        cursor->datum[pos].key = x;
        cursor->size++;
        cursor->ptr[pos + 1] = child;
    }
    // For parent overflow, break the node
    else
    {
        // For new Internal
        Node *newInternal = new Node(MAX, false);
        ++NodeNum;
        int virtualKey[MAX + 1];
        Node *virtualPtr[MAX + 2];

        // Copy key
        for (int i = 0; i < MAX; i++)
            virtualKey[i] = cursor->datum[i].key;
        // Copy ptr
        for (int i = 0; i < MAX + 1; i++)
            virtualPtr[i] = cursor->ptr[i];
        // Traverse to find where the new
        // node is to be inserted
        int pos = 0;
        while (x > virtualKey[pos] && pos < MAX)
            pos++;
        // Traverse the virtualKey node
        // and update the current key
        // to its previous node key
        // (MAX + 1)->(MAX)??
        for (int j = MAX; j > pos; j--)
            virtualKey[j] = virtualKey[j - 1];
        virtualKey[pos] = x;

        // Traverse the virtualKey node
        // and update the current ptr
        // to its previous node ptr
        // (MAX + 2)->(MAX + 1)??
        for (int j = MAX + 1; j > pos + 1; j--)
            virtualPtr[j] = virtualPtr[j - 1];
        virtualPtr[pos + 1] = child;

        // Split the size
        cursor->size = (MAX + 1) / 2;
        newInternal->size = MAX - (MAX + 1) / 2; // drop one off
        newInternal->IS_LEAF = false;

        // Update cursor node key
        int i = 0, j = 0;
        for (i = pos, j = pos; i < cursor->size; ++i, ++j)
            cursor->datum[i].key = virtualKey[j];
        for (i = pos + 1, j = pos + 1; i < cursor->size + 1; ++i, ++j)
            cursor->ptr[i] = virtualPtr[j];

        // Insert newInternal as an internal node
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size; i++, j++)
            newInternal->datum[i].key = virtualKey[j];
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size + 1; i++, j++)
            newInternal->ptr[i] = virtualPtr[j];

        // If cursor is the root node
        if (cursor == root)
        {
#ifdef BPTree_DBUG
            cso << "case (E): call insertInternal()" << endl;
#endif
            // Create a new root node
            Node *newRoot = new Node(MAX, false);
            ++NodeNum;
            // Update key value
            newRoot->datum[0].key = virtualKey[cursor->size];

            // Update rest field of
            // B+ Tree Node
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newInternal;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        {
#ifdef BPTree_DBUG
            cso << "case (F): call insertInternal()" << endl;
#endif
            // Recursive Call to insert the data
            insertInternal(virtualKey[cursor->size],
                           parent_stack,
                           newInternal,
                           cso);
        }
    }
}

// Function to find the parent node
Node *BPTree::findParent(Node *cursor,
                         Node *child)
{
    Node *parent;
    // If cursor reaches the end of Tree
    if (cursor->IS_LEAF || (cursor->ptr[0])->IS_LEAF)
        return NULL;

    // Traverse the current node with all its child
    for (int i = 0; i < cursor->size + 1; i++)
    {
        // Update the parent for the child Node
        if (cursor->ptr[i] == child)
        {
            parent = cursor;
            return parent;
        }
        // Else recursively traverse to find child node
        else
        {
            parent = findParent(cursor->ptr[i], child);
            // If parent is found, then
            // return that parent node
            if (parent != NULL)
                return parent;
        }
    }
    // Return parent node
    return parent;
}

void BPTree::update_mkey()
{
    if (!root)
    {
        cerr << "BPTree::update_mkey(): empty tree.\n";
        return;
    }
    Node *cursor = root;
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    min_key = cursor->datum[0].key;
    cursor = root;
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[cursor->size];
    max_key = cursor->datum[cursor->size - 1].key;
}

void BPTree::del(int64_t x, ostream &cso)
{
}

// Function to get the root Node
Node *BPTree::getRoot()
{
    return root;
}

void BPTree::visualize(int width, ostream &cso, bool show_str)
{
    visualize(root, width, 0, cso, show_str);
}

void BPTree::visualize(Node *root, int width, int lay, ostream &cso, bool show_str)
{
    if (!root)
        return;
    for (int i = 0; i <= root->size; ++i)
    {
        if (!root->IS_LEAF || i < root->size)
        {
            cso << str_repeat(lay - 1, "│" + string(width, ' '));
            if (lay > 0)
            {
                bool islast = root->IS_LEAF ? (i == root->size - 1) : (i == root->size);
                cso << (islast ? "└" : "├"); // last key
                cso << str_repeat(width, "─");
            }
            if (i == root->size)
                cso << setprecision(width) << "X" << endl;
            else
            {
                if (show_str)
                    cso << setprecision(width)
                        << to_string(root->datum[i].key) + ":" + NNTC_to_string(root->datum[i].str) << endl;
                else
                    cso << setprecision(width) << root->datum[i].key << endl;
            }
            visualize(root->ptr[i], width, lay + 1, cso, show_str);
        }
    }
}

string BPTree::str_repeat(int n, const string str)
{
    string copy = "";
    for (int i = 0; i < n; ++i)
        copy += str;
    return copy;
}

bool BPTree::check(int64_t key_num, ostream &cso)
{
    queue<int64_t> all_keys;
    if (root == NULL)
    {
        cso << "empty tree." << endl;
        return false;
    }
    // Traverse the B+ Tree
    Node *cursor = root;
    // Till cursor reaches the leaf node
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    int64_t leaf_count = 0;
    while (cursor)
    {
        leaf_count++;
        for (int i = 0; i < cursor->size; ++i)
        {
            all_keys.push(cursor->datum[i].key);
        }
        cursor = cursor->ptr[cursor->size];
    }
    if (leaf_count != leafNodeNum)
    {
        cso << "check(): leafNodeNum err!! " << leaf_count << "(real) != " << leafNodeNum << "(record).\n";
        return false;
    }

    if (all_keys.size() != key_num)
    {
        cso << "check(): key_num err!! " << all_keys.size() << "(real) != " << key_num << "(given).\n";
        return false;
    }
    while (!all_keys.empty())
    {
        if (!search(all_keys.front(), false))
        {
            cso << "check(): key \"" << all_keys.front() << "\" not found!!\n";
            return false;
        }
        all_keys.pop();
    }
    return true;
}

bool BPTree::check_leaf(int64_t arr_sort[], int64_t n, ostream &cso)
{
    if (root == NULL)
    {
        cso << "check_leaf(): empty tree.\n";
        return false;
    }
    // Traverse the B+ Tree
    Node *cursor = root;
    // Till cursor reaches the leaf node
    while (!cursor->IS_LEAF)
        cursor = cursor->ptr[0];
    // Visit leaf node
    int64_t j = 0;
    while (cursor)
    {
        for (int i = 0; i < cursor->size; ++i, ++j)
        {
            if (cursor->datum[i].key != arr_sort[j])
            {
                cso << "check_leaf(): "
                    << cursor->datum[i].key
                    << " != " << arr_sort[j] << "!\n";
                return false;
            }
            // Check string data correction @@
            if (atol(cursor->datum[i].str) != arr_sort[j])
            {
                cso << "check_leaf(): "
                    << cursor->datum[i].str
                    << " != " << arr_sort[j] << "!\n";
                return false;
            }
        }
        cursor = cursor->ptr[cursor->size];
    }
    cso << "check_leaf(): OK.\n";
    return true;
}
