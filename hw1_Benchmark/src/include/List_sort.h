#ifndef LIST_SORT_H
#define LIST_SORT_H
using namespace std;
template <typename T>
void List1(T *a, int *linka, const int n, int first)
{
    int *linkb = new int[n]{}; // backward links
    int prev = 0;
    for (int current = first; current; current = linka[current])
    {
        // convert chain into a doubly linked list
        linkb[current] = prev;
        prev = current;
    }
    fprintf(stderr, "linkb created...");
    for (int i = 1; i < n; ++i)
    {
        // move a[first] to position i
        if (first != i)
        {
            if (linka[i])
                linkb[linka[i]] = first;
            linka[linkb[i]] = first;
            swap(a[first], a[i]);
            swap(linka[first], linka[i]);
            swap(linkb[first], linkb[i]);
        }
        first = linka[i];
    }
    delete[] linkb; // Deallocate Heap memory
}

template <typename T>
void List2(T *a, int *link, const int n, int first)
{
    for (int i = 0; i < n; ++i)
    {
        while (first < i)
            first = link[first];
        int q = link[first];
        if (first != i)
        {
            swap(a[i], a[first]);
            link[first] = link[i];
            link[i] = first;
        }
        first = q;
    }
}
#endif // LIST_SORT_H