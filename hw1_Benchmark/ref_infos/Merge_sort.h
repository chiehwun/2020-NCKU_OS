#ifndef MERGE_SORT_H
#define MERGE_SORT_H

using namespace std;

template <typename T>
void Merge(T *initList, T *mergedList, const int l, const int m, const int n)
{
  // initList[l:m] and initList[m + 1:n] are sorted arrays
  // We will merge the above arrays into mergedList[l:n] which will be sorted
  int i1 = l, iResult = l, i2 = m + 1; // i1, i2, and iResult are positions
  for (;
       i1 <= m && i2 <= n; // Neither the input lists run out
       iResult++)
  {
    if (initList[i1] <= initList[i2])
    {
      mergedList[iResult] = initList[i1];
      ++i1;
    }
    else
    {
      mergedList[iResult] = initList[i2];
      ++i2;
    }
  }
  // FACT about std::copy(): copy(a, a+n, b) only copy b[0:n-1] = a[0:n-1];
  // if the first list has remained records, then copy them
  copy(initList + i1, initList + m + 1, mergedList + iResult);
  // if the second list has remained records, then copy them
  copy(initList + i2, initList + n + 1, mergedList + iResult);
}

template <typename T>
void MergePass(T *initList, T *resultList, const int n, const int s)
{
  // Adjacent pairs of sublists of size s are merged from
  // initList to resultList.   n: # of records in initList.
  int i = 1; // i is first position in first of the sublists being merged
  for (;
       i <= n - 2 * s + 1; // enough elements for two sublists of length s?
       i += 2 * s)
  {
    Merge(initList, resultList, i, i + s - 1, i + 2 * s - 1);
    printArr(resultList, n + 1);
  }
  // merge [i:i+s-1] and [i+s:i+2*s-1]

  // merge remaining list of size < 2 * s
  if ((i + s - 1) < n)
  {
    cout << "i(" << i << ") + s(" << s << ") - 1 < "
         << "n(" << n << ") ==> Merge(" << i << ", " << i + s - 1 << ", " << n << ")\n";
    Merge(initList, resultList, i, i + s - 1, n);
  }
  else
  {
    cout << "i(" << i << ") + s(" << s << ") - 1 >= "
         << "n(" << n << ") ==> copy("
         << "initList + " << i
         << ",  initList + " << n + 1
         << ", resultList + " << i << ")\n";
    copy(initList + i, initList + n + 1, resultList + i);
  }
}

template <typename T>
void iMergeSort(T *a, const int n)
// Sort a[1:n] into nondecreasing order.
{
  T *tempList = new T[n + 1]{0}; // tempList[0:n] has n+1 elements
  // l: length of the sublist currently being merged.
  for (int l = 1; l < n; l *= 2)
  {
    cout << "a->t, s=" << l << ":" << endl;
    MergePass(a, tempList, n, l);
    printArr(tempList, n + 1);
    cout << endl;

    l *= 2;

    cout << "t->a, s=" << l << ":" << endl;
    MergePass(tempList, a, n, l);
    printArr(a, n + 1);
    cout << endl;
    //interchange role of a and tempList
  }
  delete[] tempList; // free an array
}

/************************ Recursive way ************************/
template <typename T>
int ListMerge(T *a, int *link, const int start1, const int start2)
{                  // The Sorted chains beginning at start1 and start2, respectively, are merged.
                   // link[0] is used as a temporary header. Return start of merged chain.
  int iResult = 0; // last record of result chain
  int i1 = start1, i2 = start2;
  for (; i1 && i2;)
    if (a[i1] <= a[i2])
    {
      link[iResult] = i1;
      iResult = i1;
      i1 = link[i1];
    }
    else
    {
      link[iResult] = i2;
      iResult = i2;
      i2 = link[i2];
    }
  // attach remaining records to result chain
  if (i1 == 0)
    link[iResult] = i2;
  else
    link[iResult] = i1;
  return link[0];
}

template <typename T>
int rMergeSort(T *a, int *link, const int left, const int right)
{ // a[left:right] is to be sorted. link[i] is initially 0 for all i.
  // rMergSort returns the index of the first element in the sorted chain.
  if (left >= right)
    return left;
  int mid = (left + right) / 2;
  return ListMerge(a, link,
                   rMergeSort(a, link, left, mid),
                   // sort left half
                   rMergeSort(a, link, mid + 1, right)); // sort right half
}
#endif // MERGE_SORT_H