#include "HttpResponse.h"

#include <assert.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "Logger.h"

using namespace hs;
using namespace log;

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

void HttpResponse::Init(const string& srcDir, string&& path, bool isKeepAlive,
                        int code) {
  assert(srcDir != "");
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

void HttpResponse::AddContent_(string& buff) {
  int srcFd = open((src_dir_ + path_).data(), O_RDONLY);
  if (srcFd < 0) {
    DEBUG() << src_dir_ << path_;
    ErrorContent(buff, "File NotFound!");
    return;
  }
  int* mmRet =
      (int*)mmap(0, mmFile_stat_.st_size, PROT_READ, MAP_PRIVATE, srcFd, 0);
  if (*mmRet == -1) {
    DEBUG() << "mmap failed";
    ErrorContent(buff, "File NotFound!");
    return;
  }
  mmFile_ = (char*)mmRet;
  close(srcFd);
  buff.append("Content-length: " + to_string(mmFile_stat_.st_size) +
              "\r\n\r\n");
  buff.append(mmFile_, mmFile_stat_.st_size);
}

void HttpResponse::ErrorContent(string& buff, std::string message) {
  string body;
  string status;
  body += "<html><title>Error</title>";
  body += "<body bgcolor=\"ffffff\">";
  if (CODE_STATUS.count(code_) == 1) {
    status = CODE_STATUS.find(code_)->second;
  } else {
    status = "Bad Request";
  }
  body += to_string(code_) + " : " + status + "\n";
  body += "<p>" + message + "</p>";
  body += "<hr><em>TinyHttpServer by RickyWei</em></body></html>";

  buff.append("Content-length: " + to_string(body.size()) + "\r\n\r\n");
  buff.append(body);
}

void HttpResponse::ErrorHtml_() {
  if (CODE_PATH.count(code_) == 1) {
    path_ = CODE_PATH.find(code_)->second;
    stat((src_dir_ + path_).data(), &mmFile_stat_);
  }
}

void HttpResponse::UnmapFile() {
  if (mmFile_) {
    munmap(mmFile_, mmFile_stat_.st_size);
    mmFile_ = nullptr;
  }
}

string HttpResponse::GetFileType_() {
  string::size_type idx = path_.find_last_of('.');
  if (idx == string::npos) {
    return "text/plain";
  }
  string suffix = path_.substr(idx);
  if (SUFFIX_TYPE.count(suffix) == 1) {
    return SUFFIX_TYPE.find(suffix)->second;
  }
  return "text/plain";
}