#include "HttpServer.h"

#include <arpa/inet.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>

#include "Epoller.h"
#include "ThreadPool.h"
#include "Timer.h"

using namespace hs;

HttpServer::HttpServer(int port)
    : port_(port),
      isLinger_(true),
      timeoutMS_(60000),
      isClose_(false),
      listen_event_(EPOLLRDHUP),
      conn_event_(EPOLLONESHOT | EPOLLRDHUP),
      timer_(make_unique<TimerManager>()),
      pool_(make_unique<ThreadPool>()),
      epoller_(make_unique<Epoller>()) {
  if (!InitSocket_()) {
    isClose_ = true;
  }
}

HttpServer::~HttpServer() {
  close(listen_fd_);
  isClose_ = true;
}

void HttpServer::Start() {
  while (!isClose_) {
    int event_cnt = epoller_->Wait();
    for (int i = 0; i < event_cnt; ++i) {
      int fd = epoller_->GetEventFd(i);
      uint32_t events = epoller_->GetEvents(i);
      if (fd == listen_fd_) {
        HandleListen_();
      } else if (events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)) {
        assert(conns_.count(fd) > 0);
        CloseConn_(&conns_[fd]);
      } else if (events & EPOLLIN) {
        assert(conns_.count(fd) > 0);
        HandleRead_(&conns_[fd]);
      } else if (events & EPOLLOUT) {
        assert(conns_.count(fd) > 0);
        HandleWrite_(&conns_[fd]);
      } else {
      }
    }
  }
}

bool HttpServer::InitSocket_() {
  if (port_ > 65535 || port_ < 0) {
    return false;
  }
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(port_);
  struct linger optLinger = {0};
  if (isLinger_) {
    optLinger.l_onoff = 1;
    optLinger.l_linger = 1;
  }
  listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_fd_ < 0) {
    return false;
  }
  int ret = setsockopt(listen_fd_, SOL_SOCKET, SO_LINGER, &optLinger,
                       sizeof(optLinger));
  if (ret < 0) {
    close(listen_fd_);
    return false;
  }
  ret = bind(listen_fd_, (struct sockaddr*)&addr, sizeof(addr));
  if (ret < 0) {
    return false;
  }
  ret = epoller_->AddFd(listen_fd_, listen_event_ | EPOLLIN);
  SetFdNoblock(listen_fd_);
  return true;
}

void HttpServer::AddHttpConn_(int fd, sockaddr_in addr) {
  assert(fd > 0);
  conns_[fd].Init(fd, addr);
  if (timeoutMS_ > 0) {
    timer_->AddTimerNode(fd, timeoutMS_,
                         bind(&HttpServer::CloseConn_, this, &conns_[fd]));
  }
  epoller_->AddFd(fd, conn_event_ | EPOLLIN);
  SetFdNoblock(fd);
}

void HttpServer::HandleListen_() {
  struct sockaddr_in addr;
  socklen_t len = sizeof(addr);
  int fd = accept(listen_fd_, (struct sockaddr*)&addr, &len);
  if (fd <= 0) {
    return;
  } else if (HttpConn::conns > MAX_FD) {
    SendError_(fd, "Server Busy...");
    return;
  }
}

void HttpServer::HandleRead_(HttpConn* hc) {
  assert(hc);
  ExtentTime_(hc);
  pool_->AddTask(bind(&HttpServer::OnRead_, this, hc));
}

void HttpServer::HandleWrite_(HttpConn* hc) {
  assert(hc);
  ExtentTime_(hc);
  pool_->AddTask(bind(&HttpServer::OnWrite_, this, hc));
}

void HttpServer::OnRead_(HttpConn* hc) {
  assert(hc);
  int ret = -1;
  int readError = 0;
  ret = hc->Read(readError);
  if (ret <= 0 && readError != EAGAIN) {
    CloseConn_(hc);
    return;
  }
  OnProcess_(hc);
}

void HttpServer::OnWrite_(HttpConn* hc) {
  assert(hc);
  int ret = -1;
  int writeError = 0;
  ret = hc->Write(writeError);
  if (ret >= 0) {
    if (hc->GetIsKeepAlive()) {
      OnProcess_(hc);
      return;
    }
  } else {
    if (writeError == EAGAIN) {
      epoller_->ModFd(hc->GetFd(), conn_event_ | EPOLLOUT);
      return;
    }
  }
  CloseConn_(hc);
}

void HttpServer::OnProcess_(HttpConn* hc) {
  if (hc->Process()) {
    epoller_->ModFd(hc->GetFd(), conn_event_ | EPOLLOUT);
  } else {
    epoller_->ModFd(hc->GetFd(), conn_event_ | EPOLLIN);
  }
}

void ExtentTime_(HttpConn* hc) {}

void HttpServer::CloseConn_(HttpConn* hc) {
  assert(hc);
  epoller_->DelFd(hc->GetFd());
  hc->Close();
}

int HttpServer::SetFdNoblock(int fd) {
  assert(fd > 0);
  return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}
