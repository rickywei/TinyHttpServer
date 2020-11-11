#include "Logger.h"

#include <string.h>

#include <chrono>
#include <ctime>
#include <iostream>
#include <thread>

using namespace log;

constexpr int MAX_LOG_LINE_LEN = 512;
constexpr int MAX_LOG_BUF_SIZE = 102400;
constexpr int WAIT_INTERVAL = 2;

const map<LogLevel, string> loglevel_str = {
    {LogLevel::FATAL, "FATAL"}, {LogLevel::ERROR, "ERROR"},
    {LogLevel::WARN, "WARN "},  {LogLevel::INFO, "INFO "},
    {LogLevel::DEBUG, "DEBUG"},
};

Logger::Logger(string FILE, string FUNCTION, int LINE, LogLevel loglevel)
    : loglevel_(loglevel),
      log_line_(),
      FILE_(FILE),
      FUNCTION_(FUNCTION),
      LINE_(LINE) {
  log_line_.reserve(MAX_LOG_LINE_LEN);
  CheckDate();
  Format();
}

Logger::~Logger() {
  CheckDate();
  if (Logger::busy_buf_.size() >= MAX_LOG_BUF_SIZE) {
    // local scope
    {
      lock_guard<mutex> lk(Logger::mtx_);
      swap(Logger::busy_buf_, Logger::free_buf_);
    }
    Logger::cond_.notify_one();
  }
  log_line_ += "\n";
  busy_buf_ += log_line_;

  if (Logger::write_terminal_) {
    AddColor();
    cerr << log_line_ << flush;
  }
}

Logger& Logger::operator<<(string s) {
  log_line_ += " " + s;
  return *this;
}

void Logger::Init(bool isterminal) {
  Logger::write_terminal_ = isterminal;

  Logger::busy_buf_.reserve(MAX_LOG_BUF_SIZE);
  Logger::free_buf_.reserve(MAX_LOG_BUF_SIZE);

  thread th(Logger::ThreadFunc);
  if (th.joinable()) {
    th.detach();
  }
}

void Logger::Stop() { Logger::looping_ = false; }

bool Logger::HasLog() { return Logger::free_buf_.size(); }

void Logger::SetColor(bool color) { Logger::color_ = color; }

bool Logger::write_terminal_ = false;

string Logger::file_name_ = string();
fstream Logger::file_ = fstream();
string Logger::date_ = string();

string Logger::busy_buf_ = string();
string Logger::free_buf_ = string();

condition_variable Logger::cond_;
mutex Logger::mtx_;
mutex Logger::mtx_f_;
bool Logger::looping_ = true;
int Logger::interval_ = WAIT_INTERVAL;

bool Logger::color_ = false;

void Logger::ThreadFunc() {
  while (looping_) {
    if (!file_.is_open()) {
      file_ = fstream(Logger::file_name_, ios::app);
    }
    unique_lock<mutex> ulk(mtx_);
    cond_.wait_for(ulk, Logger::interval_ * 1s,
                   [] { return Logger::HasLog(); });

    if (Logger::busy_buf_.size()) {
      swap(Logger::busy_buf_, Logger::free_buf_);
      // local scope
      {
        lock_guard<mutex> lk(Logger::mtx_f_);
        Logger::file_ << Logger::free_buf_ << flush;
      }
      Logger::free_buf_.clear();
      // cout << HasLog() << "busy " << Logger::free_buf_.str() << endl;
    }
  }

  Logger::FlushALL();

  if (file_.is_open()) {
    file_.close();
  }
}

void Logger::CheckDate() {
  if (Logger::date_ < Logger::GetNowDate()) {
    lock_guard<mutex> lk(Logger::mtx_f_);
    if (Logger::date_ < Logger::GetNowDate()) {
      FlushALL();
      Logger::date_ = Logger::GetNowDate();
      Logger::file_name_ = Logger::date_ + ".log";
      fstream new_file = fstream(Logger::file_name_, ios::app);
      Logger::file_.swap(new_file);
      if (new_file.is_open()) {
        new_file.close();
      }
    }
  }
}

void Logger::FlushALL() {
  if (Logger::free_buf_.size() || busy_buf_.size()) {
    if (!Logger::file_.is_open()) {
      file_ = fstream(Logger::file_name_, ios::app);
    }
  }
  if (Logger::free_buf_.size()) {
    {
      lock_guard<mutex> lk(Logger::mtx_f_);
      Logger::file_ << Logger::free_buf_ << flush;
    }
  }
  if (busy_buf_.size()) {
    {
      lock_guard<mutex> lk(Logger::mtx_f_);
      Logger::file_ << Logger::free_buf_ << flush;
    }
  }
}

string Logger::GetNowDate() {
  char tmp[16];
  time_t rawtime;
  tm timeinfo;
  time(&rawtime);
  localtime_r(&rawtime, &timeinfo);
  strftime(tmp, 16, "%Y%m%d", &timeinfo);
  return move(string(string(tmp)));
}

string Logger::GetNowTime() {
  char tmp[16];
  time_t rawtime;
  tm timeinfo;
  time(&rawtime);
  localtime_r(&rawtime, &timeinfo);
  strftime(tmp, 16, "%X", &timeinfo);
  return move(string(string(tmp)));
}

void Logger::Format() {
  log_line_ += Logger::date_ + " " + Logger::GetNowTime() + " " +
               loglevel_str.at(loglevel_) + " " + FILE_ + " " + FUNCTION_ +
               " " + to_string(LINE_);
}

void Logger::AddColor() {
  if (Logger::color_) {
    switch (Logger::loglevel_) {
      case LogLevel::FATAL:
        log_line_ = "\033[31m" + log_line_ + "\033[0m";
        break;
      case LogLevel::ERROR:
        log_line_ = "\033[31m" + log_line_ + "\033[0m";
        break;
      case LogLevel::WARN:
        log_line_ = "\033[33m" + log_line_ + "\033[0m";
        break;
      case LogLevel::INFO:
        break;
      case LogLevel::DEBUG:
        log_line_ = "\033[32m" + log_line_ + "\033[0m";
        break;
      default:
        break;
    }
  }
}