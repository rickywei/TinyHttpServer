#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>
#include <functional>
#include <memory>
#include <queue>
#include <unordered_map>

namespace hs {

using namespace std;

using TimeoutCallBack = function<void()>;
using Clock = chrono::high_resolution_clock;
using MS = chrono::milliseconds;
using TimeStap = Clock::time_point;

struct TimerNode {
  int fd;
  TimeStap expire_time;
  TimeoutCallBack cb;
  TimerNode(int f, TimeStap exp, TimeoutCallBack func)
      : fd(f), expire_time(exp), cb(func) {}
};

struct cmp {
  bool operator()(const TimerNode& n1, const TimerNode& n2) {
    return n1.expire_time > n2.expire_time;
  }
};

class TimerManager {
 public:
  TimerManager();
  ~TimerManager();
  void AddTimerNode(int fd, int timeout, const TimeoutCallBack& cb);
  void ModEpireTime(int fd, int timout);
  void ClearExpiredTimerNode();

 private:
  using SPTN = shared_ptr<TimerNode>;
  priority_queue<SPTN> timers_;
  unordered_map<int, SPTN> fd2node_;
};

}  // namespace hs

#endif