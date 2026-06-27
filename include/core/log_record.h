#pragma once
#include <chrono>
#include <map>
#include <string>

// Order defines priority!
enum class LogLevel { TRACE, DEBUG, INFO, WARN, ERROR, FATAL };

struct LogRecord {
  LogLevel level;
  std::string message;
  std::string loggerName;
  std::chrono::system_clock::time_point timestamp;
  std::map<std::string, std::string> fields; // optional data to log
};
