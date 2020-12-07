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
void BPTree::search(int64_t x)
{
    // If tree is empty
    if (root == NULL)
        cout << "Tree is empty\n";

    // Traverse to find the value
    else
    {

        Node *cursor = root;

        // Till we reach leaf node
        while (cursor->IS_LEAF == false)
        {

            for (int i = 0;
                 i < cursor->size; i++)
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
        for (int i = 0;
             i < cursor->size; i++)
        {

            // If found then return
            if (cursor->key[i] == x)
            {
                cout << "Found\n";
                return;
            }
        }

        // Else element is not present
        cout << "Not found\n";
    }
}

// Function to implement the Insert
// Operation in B+ Tree
void BPTree::insert(int64_t x, string y)
{
    // If root is null then return
    // newly created node
    if (root == NULL)
    {
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
        cerr << "call insertInternal(): case (D)" << endl;
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

        int i = 0, j;

        // Traverse to find where the new
        // node is to be inserted
        while (x > virtualKey[i] && i < MAX)
            i++;

        // Traverse the virtualKey node
        // and update the current key
        // to its previous node key
        // (MAX + 1)->(MAX)??
        for (int j = MAX; j > i; j--)
            virtualKey[j] = virtualKey[j - 1];

        virtualKey[i] = x;

        // Traverse the virtualKey node
        // and update the current ptr
        // to its previous node ptr
        // (MAX + 2)->(MAX + 1)??
        for (int j = MAX + 1; j > i + 1; j--)
            virtualPtr[j] = virtualPtr[j - 1];

        virtualPtr[i + 1] = child;
        newInternal->IS_LEAF = false;
        // Update size
        cursor->size = (MAX + 1) / 2;
        newInternal->size = MAX - (MAX + 1) / 2; // drop one off

        // Insert new node as an internal node
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size; i++, j++)
            newInternal->key[i] = virtualKey[j];
        for (i = 0, j = cursor->size + 1;
             i < newInternal->size + 1; i++, j++)
            newInternal->ptr[i] = virtualPtr[j];

        // If cursor is the root node
        if (cursor == root)
        {
            cerr << "call insertInternal(): case (E)" << endl;
            // Create a new root node
            Node *newRoot = new Node;

            // Update key value
            newRoot->key[0] = cursor->key[cursor->size];

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
            cerr << "call insertInternal(): case (F)" << endl;
            // Recursive Call to insert the data
            insertInternal(cursor->key[cursor->size],
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
    {
        return NULL;
    }

    // Traverse the current node with
    // all its child
    for (int i = 0;
         i < cursor->size + 1; i++)
    {

        // Update the parent for the
        // child Node
        if (cursor->ptr[i] == child)
        {
            parent = cursor;
            return parent;
        }

        // Else recursively traverse to
        // find child node
        else
        {
            parent = findParent(cursor->ptr[i],
                                child);

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