#include "BPTree.h"
// Constructor of Node
Node::Node()
{
    key = new int64_t[MAX]();
    str = new string[MAX]();
    size = 0;
    ptr = new Node *[MAX + 1]();
    IS_LEAF = false;
}

// Initialize the BPTree Node
BPTree::BPTree()
{
    root = NULL;
}

// Function to find any element
// in B+ Tree
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
void BPTree::insert(int64_t x, string y)
{
    // If root is null then return
    // newly created node
    if (root == NULL)
    {
        cout << "case (A)" << endl;
        root = new Node;
        root->key[0] = x;
        root->str[0] = y;
        root->IS_LEAF = true;
        root->size = 1;
    }
    // Traverse the B+ Tree
    else
    {
        Node *cursor = root;
        Node *parent;

        // Till cursor reaches the leaf node
        while (cursor->IS_LEAF == false)
        {
            parent = cursor;
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
            cout << "case (B)" << endl;
            int i = 0;
            // TODO: CHANGE TO BINARY SEARCH
            while (x > cursor->key[i] && i < cursor->size)
                i++;
            // cout << "x: " << x << " cursor->key[" << i << "]=" << cursor->key[i] << endl;
            // // key not exists (new key)
            // if (x < cursor->key[i] || i >= cursor->size)
            // {
            // right shift after i
            for (int j = cursor->size; j > i; j--)
                cursor->key[j] = cursor->key[j - 1];

            // Update last ptr (link to next leaf node)
            cursor->size++;
            cursor->ptr[cursor->size] = cursor->ptr[cursor->size - 1];
            cursor->ptr[cursor->size - 1] = NULL;
            // }
            cursor->key[i] = x;
            cursor->str[i] = y;
        }
        // leaf node Overflow
        else
        {
            // Create a newLeaf node
            Node *newLeaf = new Node;
            int virtualNode[MAX + 1] = {0};

            // Update cursor to virtual node created
            for (int i = 0; i < MAX; i++)
                virtualNode[i] = cursor->key[i];

            // Traverse to find where the
            // new node is to be inserted
            int i = 0, j;
            while (x > virtualNode[i] && i < MAX)
                i++;

            // Update the current virtual
            // Node to its previous
            // (MAX + 1)->(MAX)??
            for (int j = MAX; j > i; j--)
                virtualNode[j] = virtualNode[j - 1];

            virtualNode[i] = x;
            newLeaf->IS_LEAF = true;
            // Update size
            cursor->size = (MAX + 1) / 2; // left bias
            newLeaf->size = MAX + 1 - (MAX + 1) / 2;
            // Update next link
            cursor->ptr[cursor->size] = newLeaf;
            newLeaf->ptr[newLeaf->size] = cursor->ptr[MAX];

            cursor->ptr[MAX] = NULL;

            // Update the current virtual
            // Node's key to its previous
            for (i = 0;
                 i < cursor->size; i++)
                cursor->key[i] = virtualNode[i];

            // Update the newLeaf key
            // to virtual Node
            for (i = 0, j = cursor->size;
                 i < newLeaf->size; i++, j++)
                newLeaf->key[i] = virtualNode[j];

            // If cursor is the root node
            if (cursor == root)
            {
                cout << "case (C)" << endl;
                // Create a new Node
                Node *newRoot = new Node;

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
                               parent,
                               newLeaf);
            }
        }
    }
}

// Function to implement the Insert
// Internal Operation in B+ Tree
void BPTree::insertInternal(int64_t x,
                            Node *cursor, // parent
                            Node *child)
{
    // If parent not overflow
    if (cursor->size < MAX)
    {
        cout << "case (D): call insertInternal()" << endl;
        int i = 0;
        // Traverse the child node
        // for current cursor node
        while (x > cursor->key[i] && i < cursor->size)
            i++;

        // Traverse the cursor node
        // and update the current key
        // to its previous node key
        for (int j = cursor->size; j > i; j--)
            cursor->key[j] = cursor->key[j - 1];

        // Traverse the cursor node
        // and update the current ptr
        // to its previous node ptr
        for (int j = cursor->size + 1; j > i + 1; j--)
            cursor->ptr[j] = cursor->ptr[j - 1];

        cursor->key[i] = x;
        cursor->size++;
        cursor->ptr[i + 1] = child;
    }
    // For parent overflow, break the node
    else
    {
        // For new Interval
        Node *newInternal = new Node;
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

        int i = 0, j = 0;
        // Update cursor node key
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
            cout << "case (E): call insertInternal()" << endl;
            // Create a new root node
            Node *newRoot = new Node;

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
            cout << "case (F): call insertInternal()" << endl;
            // Recursive Call to insert the data
            insertInternal(virtualKey[cursor->size],
                           findParent(root, cursor),
                           newInternal);
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

void BPTree::display(Node *cursor)
{
    if (!cursor)
    {
        cout << "empty node.\n";
        return;
    }
    cout << "s:" << cursor->size << " | ";
    for (int i = 0; i < cursor->size; ++i)
    {
        cout << cursor->key[i] << " ";
    }
    cout << endl;
}

void BPTree::display_leaf()
{
    if (root == NULL)
    {
        cout << "empty tree." << endl;
    }
    // Traverse the B+ Tree
    else
    {
        Node *cursor = root;
        // Till cursor reaches the leaf node
        while (!cursor->IS_LEAF)
        {
            cursor = cursor->ptr[0];
        }
        while (cursor)
        {
            for (int i = 0; i < cursor->size; ++i)
                cout << cursor->key[i] << " ";
            cursor = cursor->ptr[cursor->size];
        }
        cout << endl;
    }
}

void BPTree::visualize(int width)
{
    visualize(root, width, 0);
}

void BPTree::visualize(Node *root, int width, int lay)
{
    if (!root)
        return;
    for (int i = 0; i <= root->size; ++i)
    {
        if (!root->IS_LEAF || i < root->size)
        {
            cout << str_repeat(lay - 1, "│" + string(width, ' '));
            if (lay > 0)
            {
                bool islast = root->IS_LEAF ? (i == root->size - 1) : (i == root->size);
                cout << (islast ? "└" : "├"); // last key
                cout << str_repeat(width, "─");
            }
            cout << setprecision(width)
                 << (i == root->size ? "X" : to_string(root->key[i]))
                 << endl;
            BPTree::visualize(root->ptr[i], width, lay + 1);
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

void BPTree::check(int64_t key_num)
{
    queue<int64_t> all_keys;
    if (root == NULL)
    {
        cout << "empty tree." << endl;
        return;
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
        cout << "check(): key_num err!! " << all_keys.size() << " != " << key_num << ".\n";
        exit(EXIT_FAILURE);
    }
    while (!all_keys.empty())
    {
        if (!search(all_keys.front()))
        {
            cout << "check(): key \"" << all_keys.front() << "\" not found!!\n";
            exit(EXIT_FAILURE);
        }
        all_keys.pop();
    }
}

void BPTree::check_tot(int64_t arr_sort[], int64_t n)
{
    if (root == NULL)
        cout << "check_tot(): empty tree.\n";
    // Traverse the B+ Tree
    else
    {
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
                    cout << "check_tot(): " << cursor->key[i] << " != " << arr_sort[j] << "!\n";
                    return;
                }
            }
            cursor = cursor->ptr[cursor->size];
        }
        cout << "check_tot(): OK.\n";
    }
}
