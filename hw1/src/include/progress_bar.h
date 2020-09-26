#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

#include <iostream>
#include <iomanip>
#include <cstdlib>
using namespace std;
// https://stackoverflow.com/questions/14539867/how-to-display-a-progress-indicator-in-pure-c-c-cout-printf
void progress_bar(float progress, int barWidth, long current = -1, long tot = -1)
{
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
  if (tot == -1)
    cout << "] " << int(progress * 100.0) << " %\r";
  else
    cout << "] " << current << "/" << tot << " " << setprecision(2) << fixed << (progress * 100.0f) << " % \r";

  cout.flush();
}
#endif // PROGRESS_BAR_H