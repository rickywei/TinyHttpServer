#ifndef _EPOLLER_H
#define _EPOLLER_H

#include <sys/epoll.h>

#include <vector>

namespace hs {

using namespace std;

class Epoller {
 public:
  explicit Epoller(int max_event = 1024);
  ~Epoller();
  bool AddFd(int fd, uint32_t events);
  bool ModFd(int fd, uint32_t events);
  bool DelFd(int fd);
  int Wait(int timeoutMS = -1);
  int GetEventFd(size_t i) const;
  uint32_t GetEvents(size_t i) const;

 private:
  int epoll_fd_;
  vector<struct epoll_event> events_;
};

}  // namespace hs

#endif