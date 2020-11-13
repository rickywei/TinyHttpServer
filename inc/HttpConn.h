#ifndef _HTTPCONN_H
#define _HTTPCONN_H

#include <arpa/inet.h>
#include <sys/types.h>

#include <atomic>
#include <string>

#include "HttpRequest.h"
#include "HttpResponse.h"

namespace tiny {

using namespace std;

class HttpConn {
 public:
  HttpConn() = default;
  HttpConn(int fd, const sockaddr_in& addr);
  ~HttpConn();
  void Init(int sockFd, const sockaddr_in& addr);
  ssize_t Read(int& saveError);
  ssize_t Write(int& saveError);
  void Close();
  bool Process();
  int GetFd() const;
  struct sockaddr_in GetAddr() const;
  string GetIP() const;
  int GetPort() const;
  bool GetIsKeepAlive() const;

  static const string src_dir;
  static atomic<int> conns;

 private:
  int fd_;
  struct sockaddr_in addr_;
  bool isClose_;
  string read_buff_;
  string write_buff_;
  HttpRequest request_;
  HttpResponse response_;
};

}  // namespace tiny

#endif