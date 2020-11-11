#include "Epoller.h"

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

#include <cassert>

using namespace hs;

Epoller::Epoller(int max_event)
    : epoll_fd_(epoll_create(max_event)), events_() {}

Epoller::~Epoller() { close(epoll_fd_); }

bool Epoller::AddFd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }
  epoll_event ev;
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &ev);
}

bool Epoller::ModFd(int fd, uint32_t events) {
  if (fd < 0) {
    return false;
  }
  epoll_event ev;
  ev.data.fd = fd;
  ev.events = events;
  return 0 == epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &ev);
}

int Epoller::Wait(int timeoutMS) {
  return epoll_wait(epoll_fd_, &events_[0], static_cast<int>(events_.size()),
                    timeoutMS);
}

int Epoller::GetEventFd(size_t i) const {
  assert(i < events_.size() && i >= 0);
  return events_[i].data.fd;
}

uint32_t Epoller::GetEvents(size_t i) const {
  assert(i < events_.size() && i >= 0);
  return events_[i].events;
}