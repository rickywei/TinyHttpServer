#include "Timer.h"

using namespace tiny;

TimerManager::TimerManager() {}

TimerManager::~TimerManager() {}

void TimerManager::AddTimerNode(int fd, int timeout,
                                const TimeoutCallBack& cb) {
  SPTN tn =
      make_shared<TimerNode>(fd, Clock::now() + static_cast<MS>(timeout), cb);
  timers_.emplace(tn);
  fd2node_[fd] = tn;
}

void TimerManager::ModEpireTime(int fd, int timeout) {
  if (fd2node_.count(fd) > 0) {
    fd2node_.find(fd)->second->expire_time =
        Clock::now() + static_cast<MS>(timeout);
  }
}

void TimerManager::ClearExpiredTimerNode() {
  while (!timers_.empty()) {
    if (timers_.top()->expire_time >= Clock::now()) {
      timers_.top()->cb();
      fd2node_.erase(timers_.top()->fd);
      timers_.pop();
    } else {
      break;
    }
  }
}