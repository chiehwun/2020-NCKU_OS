#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
#define barWidth 50
using namespace std;
// https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf
void progress_bar(size_t current, size_t tot)
{
  double progress = (double)current / tot;
  cout << "[";
  int pos = barWidth * progress;
  for (int i = 0; i < barWidth; ++i)
  {
    if (i < pos)
      cout << "=";
    else if (i == pos)
      cout << ">";
    else
      cout << " ";
  }
  cout << "] " << current << "/" << tot << " " << setprecision(2) << fixed << progress * 100.0 << "%\r";
  cout.flush();
  // cout << "] " << current << "/" << tot << " " << setprecision(2) << fixed << (progress * 100.0) << " % \r";
}
#endif // PROGRESS_BAR_H