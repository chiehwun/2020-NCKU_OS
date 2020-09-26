#include <iostream>
#include "include/Clock.h"
using namespace std;
int Clock::numClock = 0;       // 定義static變數(.寫在cpp)
int64_t Clock::totalClock = 0; // 定義static變數(.寫在cpp)
Clock::Clock()
{
    ++numClock;
}
Clock::Clock(const Clock &)
{
    ++numClock;
}
Clock::~Clock()
{
    --numClock;
}
void Clock::start()
{
    start_ts = high_resolution_clock::now();
}
void Clock::stop()
{
    elapsed_time = duration_cast<microseconds>(high_resolution_clock::now() - start_ts).count();
    totalClock += elapsed_time;
}
int64_t Clock::getElapsedTime() const
{
    return elapsed_time;
}
int Clock::getNum()
{
    return numClock;
}
int64_t Clock::getTotal()
{
    return totalClock;
}
