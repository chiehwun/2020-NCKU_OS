#ifndef CLOCK_H
#define CLOCK_H
#include <chrono>
using namespace std;
using namespace std::chrono;

class Clock
{
public:
  Clock();
  Clock(const Clock &);
  ~Clock();
  void start();
  void stop();
  int64_t getElapsedTime() const;
  // <為了存取L17的static member> 計算幾個clock object
  static int getNum();
  // <為了存取L18的static member> 計算所有clock objects 的clock_t總和
  static int64_t getTotal();

private:
  high_resolution_clock::time_point start_ts;
  int64_t elapsed_time;
  static int numClock;
  static int64_t totalClock;
};
#endif
