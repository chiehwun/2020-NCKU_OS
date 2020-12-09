#include "BPTree.h"
// Constructor of Node
Node::Node(int m)
{
    key = new int64_t[m]();
    str = new string[m]();
    size = 0;
    ptr = new Node *[m + 1]();
    IS_LEAF = false;
}

// Initialize the BPTree Node
BPTree::BPTree(int m) : MAX(m)
{
    root = NULL;
}

// return true if repeat key exists.
// Traverse and overwrite &pos where
// arr[n] < key, n < pos; key <= arr[n], n >= pos
bool BPTree::locate(const int64_t x,
                    const int64_t *arr,
                    const int n,
                    int &pos)
{
    pos = 0;
    // TODO: CHANGE TO BINARY SEARCH
    while (x > arr[pos] && pos < n)
        pos++;
    return x == arr[pos];
}

// Function to find any element in B+ Tree
bool BPTree::search(int64_t x)
{
    // If tree is empty
    if (root == NULL)
        cout << "Tree is empty\n";
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
                if (x < cursor->key[i])
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

        // Traverse the cursor and find
        // the node with value x
        for (int i = 0; i < cursor->size; i++)
        {
            // If found then return
            if (cursor->key[i] == x)
            {
                // cout << "Found\n";
                return true;
            }
        }

        // Else element is not present
        // cout << "Not found\n";
    }
    return false;
}

// Function to implement the Insert
// Operation in B+ Tree
void BPTree::insert(int64_t x, string y, ostream &cso)
{
    // If root is null then return
    // newly created node
    if (root == NULL)
    {
        cso << "case (A)" << endl;
        root = new Node(MAX);
        root->key[0] = x;
        root->str[0] = y;
        root->IS_LEAF = true;
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
                if (x < cursor->key[i])
                {
                    cursor = cursor->ptr[i];
                    break;
                }

                // If reaches the end
                // && x >= cursor->key[i]
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
            if (locate(x, cursor->key, cursor->size, pos_leaf))
            {
                cso << "case (R0): repeat key = " << x << endl;
                cursor->str[pos_leaf] = y;
                return;
            }

            cso << "case (B): leaf node non-ovf." << endl;
            // right shift after pos_leaf
            for (int j = cursor->size; j > pos_leaf; j--)
                cursor->key[j] = cursor->key[j - 1];

            // Update last ptr (link to next leaf node)
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
            cursor->key[pos_leaf] = x;
            cursor->str[pos_leaf] = y;
        }
        // leaf node Overflow
        else
        {

            // Create a newLeaf node
            Node *newLeaf = new Node(MAX);
            int virtualNode[MAX + 1] = {0};
            // Copy overflowed key to virtualNode
            for (int i = 0; i < MAX; i++)
                virtualNode[i] = cursor->key[i];

            int pos_leaf = 0; // pass by reference
            if (locate(x, cursor->key, MAX, pos_leaf))
            {
                cso << "case (R1): repeat key = " << x << endl;
                cursor->str[pos_leaf] = y;
                return;
            }

            // Right shift + insert virtualNode
            // (MAX + 1)->(MAX)??
            for (int j = MAX; j > pos_leaf; j--)
                virtualNode[j] = virtualNode[j - 1];
            virtualNode[pos_leaf] = x;
            newLeaf->IS_LEAF = true;
            // Update size
            cursor->size = (MAX + 1) / 2;
            newLeaf->size = MAX + 1 - cursor->size;
            // Update last link
            cursor->ptr[cursor->size] = newLeaf;
            newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];
            cursor->ptr[MAX] = NULL;

            // Overwrite the key of cursor node
            int i = 0, j = 0;
            for (i = 0; i < cursor->size; i++)
                cursor->key[i] = virtualNode[i];
            // Write the newLeaf key
            for (i = 0, j = cursor->size;
                 i < newLeaf->size; i++, j++)
                newLeaf->key[i] = virtualNode[j];

            // If cursor is the root node
            if (cursor == root)
            {
                cso << "case (C)" << endl;
                // Create a new Node
                Node *newRoot = new Node(MAX);

                // Update rest field of
                // B+ Tree Node
                newRoot->key[0] = newLeaf->key[0];
                newRoot->ptr[0] = cursor;
                newRoot->ptr[1] = newLeaf;
                newRoot->IS_LEAF = false;
                newRoot->size = 1;
                root = newRoot;
            }
            // If cursor is not the root node
            else
            {
                // Recursive Call for
                // insert in internal
                insertInternal(newLeaf->key[0],
                               parent_stack,
                               newLeaf,
                               cso);
            }
        }
    }
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
        cso << "case (D): call insertInternal()" << endl;
        // Traverse the child node
        // for current cursor node
        int pos = 0; // pass by reference
        locate(x, cursor->key, cursor->size, pos);

        // Traverse the cursor node
        // and update the current key
        // to its previous node key
        for (int j = cursor->size; j > pos; j--)
            cursor->key[j] = cursor->key[j - 1];

        // Traverse the cursor node
        // and update the current ptr
        // to its previous node ptr
        for (int j = cursor->size + 1; j > pos + 1; j--)
            cursor->ptr[j] = cursor->ptr[j - 1];

        cursor->key[pos] = x;
        cursor->size++;
        cursor->ptr[pos + 1] = child;
    }
    // For parent overflow, break the node
    else
    {
        // For new Interval
        Node *newInternal = new Node(MAX);
        int virtualKey[MAX + 1];
        Node *virtualPtr[MAX + 2];

        // Insert the current list key
        // of cursor node to virtualKey
        for (int i = 0; i < MAX; i++)
            virtualKey[i] = cursor->key[i];
        // Insert the current list ptr
        // of cursor node to virtualPtr
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
            cursor->key[i] = virtualKey[j];
        for (i = pos + 1, j = pos + 1; i < cursor->size + 1; ++i, ++j)
            cursor->ptr[i] = virtualPtr[j];

        // Insert newInternal as an internal node
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size; i++, j++)
            newInternal->key[i] = virtualKey[j];
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size + 1; i++, j++)
            newInternal->ptr[i] = virtualPtr[j];

        // If cursor is the root node
        if (cursor == root)
        {
            cso << "case (E): call insertInternal()" << endl;
            // Create a new root node
            Node *newRoot = new Node(MAX);

            // Update key value
            newRoot->key[0] = virtualKey[cursor->size];

            // Update rest field of
            // B+ Tree Node
            newRoot->ptr[0] = cursor;
            newRoot->ptr[1] = newInternal;
            newRoot->IS_LEAF = false;
            newRoot->size = 1;
            root = newRoot;
        }
        else
        {
            cso << "case (F): call insertInternal()" << endl;
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

// Function to get the root Node
Node *BPTree::getRoot()
{
    return root;
}

void BPTree::visualize(int width, ostream &cso)
{
    visualize(root, width, 0, cso);
}

void BPTree::visualize(Node *root, int width, int lay, ostream &cso)
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
            cso << setprecision(width)
                << (i == root->size ? "X" : to_string(root->key[i]))
                << endl;
            visualize(root->ptr[i], width, lay + 1, cso);
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
    while (cursor)
    {
        for (int i = 0; i < cursor->size; ++i)
            all_keys.push(cursor->key[i]);
        cursor = cursor->ptr[cursor->size];
    }
    if (all_keys.size() != key_num)
    {
        cso << "check(): key_num err!! " << all_keys.size() << " != " << key_num << ".\n";
        return false;
    }
    while (!all_keys.empty())
    {
        if (!search(all_keys.front()))
        {
            cso << "check(): key \"" << all_keys.front() << "\" not found!!\n";
            return false;
        }
        all_keys.pop();
    }
    return true;
}

bool BPTree::check_tot(int64_t arr_sort[], int64_t n, ostream &cso)
{
    if (root == NULL)
    {
        cso << "check_tot(): empty tree.\n";
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
            if (cursor->key[i] != arr_sort[j])
            {
                cso << "check_tot(): " << cursor->key[i] << " != " << arr_sort[j] << "!\n";
                return false;
            }
        }
        cursor = cursor->ptr[cursor->size];
    }
    cso << "check_tot(): OK.\n";
    return true;
}
