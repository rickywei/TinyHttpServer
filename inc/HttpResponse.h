#ifndef _HTTPRESPONSE_H
#define _HTTPRESPONSE_H

#include <sys/stat.h>

#include <string>
#include <unordered_map>

namespace hs {

using namespace std;

class HttpResponse {
 public:
  HttpResponse();
  ~HttpResponse();
  void Init(const std::string& srcDir, std::string&& path,
            bool isKeepAlive = false, int code = -1);
  void MakeResponse(string& buff);
  void UnmapFile();
  string GetFile() const;
  size_t GetFileLen() const;
  int GetCode() const;

 private:
  void AddStateLine_(string& buff);
  void AddHeader_(string& buff);
  void AddContent_(string& buff);
  void ErrorContent(string& buff, std::string message);
  void ErrorHtml_();
  string GetFileType_();

  int code_;
  bool isKeepAlive_;
  string path_;
  string src_dir_;
  char* mmFile_;
  struct stat mmFile_stat_;
  static const unordered_map<string, string> SUFFIX_TYPE;
  static const unordered_map<int, string> CODE_STATUS;
  static const unordered_map<int, string> CODE_PATH;
};

}  // namespace hs

#endif