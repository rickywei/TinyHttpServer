#ifndef _HTTPSERVER_H
#define _HTTPSERVER_H

#include <memory>
#include <unordered_map>

#include "HttpConn.h"

namespace hs {

using namespace std;

class TimerManager;
class Epoller;
class ThreadPool;

class HttpServer {
 public:
  HttpServer(int port = 8080);
  ~HttpServer();
  void HttpServer::Start();

 private:
  bool InitSocket_();
  void AddHttpConn_(int fd, sockaddr_in addr);
  void HandleListen_();
  void HandleRead_(HttpConn* hc);
  void HandleWrite_(HttpConn* hc);
  void OnRead_(HttpConn* hc);
  void OnWrite_(HttpConn* hc);
  void OnProcess_(HttpConn* hc);
  void SendError_(int fd, const char* info);
  void ExtentTime_(HttpConn* hc);
  void CloseConn_(HttpConn* hc);
  static int SetFdNoblock(int fd);

  int port_;
  bool isLinger_;
  int timeoutMS_;
  bool isClose_;
  int listen_fd_;
  uint32_t listen_event_;
  uint32_t conn_event_;
  unique_ptr<TimerManager> timer_;
  unique_ptr<ThreadPool> pool_;
  unique_ptr<Epoller> epoller_;
  unordered_map<int, HttpConn> conns_;
  static constexpr int MAX_FD = 65536;
};

}  // namespace hs

#endif