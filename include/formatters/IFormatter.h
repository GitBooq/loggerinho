#pragma once

#include "core/log_record.h"
#include <sstream>
#include <string>

namespace formatter {
class IFormatter {
public:
  virtual ~IFormatter() = default;
  virtual std::string operator()(const LogRecord &record) const = 0;
};

inline std::string logLevelToString(LogLevel level) {
  switch (level) {
  case LogLevel::TRACE:
    return "TRACE";
  case LogLevel::DEBUG:
    return "DEBUG";
  case LogLevel::INFO:
    return "INFO";
  case LogLevel::WARN:
    return "WARN";
  case LogLevel::ERROR:
    return "ERROR";
  case LogLevel::FATAL:
    return "FATAL";
  default:
    return "UNKNW";
  }
}

using TimePoint = std::chrono::system_clock::time_point;
inline std::string formatTimestamp(const TimePoint &timestamp) {
  auto timeT = std::chrono::system_clock::to_time_t(timestamp);
  auto *tm = std::localtime(&timeT);

  std::ostringstream oss;
  oss << std::put_time(tm, "%Y-%m-%d %H:%M:%S"); // 2026-05-10 15:30:01
  return oss.str();
}
} // namespace formatter
