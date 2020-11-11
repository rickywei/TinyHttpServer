#include "HttpResponse.h"

#include <cassert>

using namespace hs;

const unordered_map<string, string> HttpResponse::SUFFIX_TYPE = {
    {".html", "text/html"},
    {".xml", "text/xml"},
    {".xhtml", "application/xhtml+xml"},
    {".txt", "text/plain"},
    {".rtf", "application/rtf"},
    {".pdf", "application/pdf"},
    {".word", "application/nsword"},
    {".png", "image/png"},
    {".gif", "image/gif"},
    {".jpg", "image/jpeg"},
    {".jpeg", "image/jpeg"},
    {".au", "audio/basic"},
    {".mpeg", "video/mpeg"},
    {".mpg", "video/mpeg"},
    {".avi", "video/x-msvideo"},
    {".gz", "application/x-gzip"},
    {".tar", "application/x-tar"},
    {".css", "text/css "},
    {".js", "text/javascript "},
};

const unordered_map<int, string> HttpResponse::CODE_STATUS = {
    {200, "OK"},
    {400, "Bad Request"},
    {403, "Forbidden"},
    {404, "Not Found"},
};

const unordered_map<int, string> HttpResponse::CODE_PATH = {
    {400, "/400.html"},
    {403, "/403.html"},
    {404, "/404.html"},
};

HttpResponse::HttpResponse()
    : code_(-1),
      isKeepAlive_(false),
      src_dir_(),
      mmFile_(nullptr),
      mmFile_stat_{} {}

HttpResponse::~HttpResponse() { UnmapFile(); }

void HttpResponse::Init(const string& srcDir, string& path, bool isKeepAlive,
                        int code) {
  assert(src_dir_ != "");
  if (mmFile_) {
    UnmapFile();
  }
  code_ = code;
  isKeepAlive_ = isKeepAlive;
  path_ = path;
  src_dir_ = srcDir;
  mmFile_ = nullptr;
  mmFile_stat_ = {0};
}

void HttpResponse::MakeResponse(string& buff) {
  if (stat((src_dir_ + path_).data(), &mmFile_stat_) < 0 ||
      S_ISDIR(mmFile_stat_.st_mode)) {
    code_ = 400;
  } else if (!(mmFile_stat_.st_mode & S_IROTH)) {
    code_ = 403;
  } else if (code_ == -1) {
    code_ = 200;
  }
  ErrorHtml_();
  AddStateLine_(buff);
  AddHeader_(buff);
  AddContent_(buff);
}

string HttpResponse::GetFile() const { return string(mmFile_); }

size_t HttpResponse::GetFileLen() const { return mmFile_stat_.st_size; }

int HttpResponse::GetCode() const { return code_; }

void HttpResponse::AddStateLine_(string& buff) {
  string status;
  if (CODE_STATUS.count(code_) == 1) {
    status = CODE_STATUS.find(code_)->second;
  } else {
    code_ = 400;
    status = CODE_STATUS.find(400)->second;
  }
  buff.append("HTTP/1.1 " + to_string(code_) + " " + status + "\r\n");
}

void HttpResponse::AddHeader_(string& buff) {
  buff.append("Connection: ");
  if (isKeepAlive_) {
    buff.append("keep-alive\r\n");
    buff.append("keep-alive: max=6, timeout=120\r\n");

  } else {
    buff.append("close\r\n");
  }
  buff.append("Content-type: " + GetFileType_() + "\r\n");
}