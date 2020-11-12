#include "HttpRequest.h"

#include <cassert>
#include <regex>

#include "Logger.h"

using namespace hs;
using namespace log;

HttpRequest::HttpRequest()
    : state_(REQUEST_LINE),
      method_(),
      path_(),
      version_(),
      body_(),
      header_() {}

HttpRequest::~HttpRequest() {}

void HttpRequest::Init() {
  method_ = path_ = version_ = body_ = "";
  state_ = REQUEST_LINE;
  header_.clear();
  post_.clear();
}

bool HttpRequest::Parse(string &buff) {
  const string CRLF("\r\n");
  if (buff.size() <= 0) {
    return false;
  }
  int start_pos = 0;
  while (start_pos < buff.size() && state_ != FINISH) {
    int end_pos = buff.find(CRLF, start_pos);
    string line = buff.substr(start_pos, end_pos - start_pos);
    switch (state_) {
      case REQUEST_LINE:
        if (ParseRequestLine_(line)) {
          ParsePath_();
        } else {
          return false;
        }
        break;
      case HEADERS:
        ParseHeader_(line);
        if (end_pos + 2 >= buff.size()) {
          state_ = FINISH;
        }
        break;
      case BODY:
        ParseBody_(line);
        break;
      default:
        break;
    }
    start_pos = end_pos + 2;
  }
  buff.clear();
  return true;
}

string HttpRequest::GetPath() const { return path_; }

string HttpRequest::GetMethod() const { return method_; }

string HttpRequest::GetVersion() const { return version_; }

string HttpRequest::GetPost(const string &key) const {
  if (post_.count(key) == 1) {
    return post_.find(key)->second;
  }
  return "";
}

bool HttpRequest::GetIsKeepAlive() const {
  if (header_.count("Connection") == 1) {
    return header_.find("Connection")->second == "keep-alive" &&
           version_ == "1.1";
  }
  return false;
}

bool HttpRequest::ParseRequestLine_(const string &line) {
  regex pattern("^([^ ]*) ([^ ]*) HTTP/([^ ]*)$");
  smatch match;
  if (regex_match(line, match, pattern)) {
    method_ = match[1];
    path_ = match[2];
    version_ = match[3];
    state_ = HEADERS;
    return true;
  }
  return false;
}

void HttpRequest::ParsePath_() {
  if (path_ == "/") {
    path_ = "/index.html";
  }
}

void HttpRequest::ParseHeader_(const string &line) {
  regex pattern("^([^:]*): ?(.*)$");
  smatch match;
  if (regex_match(line, match, pattern)) {
    header_[match[1]] = match[2];
  } else {
    state_ = BODY;
  }
}

void HttpRequest::ParseBody_(const string &line) {
  body_ = line;
  ParsePost_();
  state_ = FINISH;
}

void HttpRequest::ParsePost_() {
  if (method_ == "POST" &&
      header_["Content-Type"] == "application/x-www-form-urlencoded") {
    ParseFromUrlencoded_();
    path_ = "/post.html";
  }
}

void HttpRequest::ParseFromUrlencoded_() {
  if (body_.size() == 0) {
    return;
  }
  string key, value;
  int num = 0;
  int n = body_.size();
  int i = 0, j = 0;

  for (; i < n; i++) {
    char ch = body_[i];
    switch (ch) {
      case '=':
        key = body_.substr(j, i - j);
        j = i + 1;
        break;
      case '+':
        body_[i] = ' ';
        break;
      case '%':
        num = ConverHex_(body_[i + 1]) * 16 + ConverHex_(body_[i + 2]);
        body_[i + 2] = num % 10 + '0';
        body_[i + 1] = num / 10 + '0';
        i += 2;
        break;
      case '&':
        value = body_.substr(j, i - j);
        j = i + 1;
        post_[key] = value;
        break;
      default:
        break;
    }
  }
  assert(j <= i);
  if (post_.count(key) == 0 && j < i) {
    value = body_.substr(j, i - j);
    post_[key] = value;
  }
}

int HttpRequest::ConverHex_(char ch) {
  if (ch >= 'A' && ch <= 'F') return ch - 'A' + 10;
  if (ch >= 'a' && ch <= 'f') return ch - 'a' + 10;
  return ch;
}