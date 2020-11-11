#ifndef _HTTPREQUEST_H
#define _HTTPREQUEST_H

#include <string>
#include <unordered_map>

namespace hs {

using namespace std;

class HttpRequest {
 public:
  enum ParseState {
    REQUEST_LINE,
    HEADERS,
    BODY,
    FINISH,
  };
  enum HttpCode {
    NO_REQUEST,
    GET_REQUEST,
    BAD_REQUEST,
    NO_RESOURSE,
    FORBIDDEND_REQUEST,
    FILE_REQUEST,
    INTERNET_ERROR,
    CLOSED_CONNECTION,
  };

  HttpRequest();
  ~HttpRequest();
  void Init();
  bool Parse(string &buff);

  string GetPath() const;
  string GetMethod() const;
  string GetVersion() const;
  string GetPost(const string &key) const;
  bool GetIsKeepAlive() const;

 private:
  bool ParseRequestLine_(const string &line);
  void ParsePath_();
  void ParseHeader_(const string &line);
  void ParseBody_(const string &line);
  void ParsePost_();
  void ParseFromUrlencoded_();
  int ConverHex_(char ch);

  ParseState state_;
  string method_;
  string path_;
  string version_;
  string body_;
  unordered_map<string, string> header_;
  unordered_map<string, string> post_;
};

}  // namespace hs

#endif