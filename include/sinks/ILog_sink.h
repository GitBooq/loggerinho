#pragma once

#include "core/log_record.h"

#include <iostream>
#include <source_location>

class ILogSink {
public:
  virtual ~ILogSink() = default;
  [[nodiscard]] virtual bool open() const = 0;
  virtual void write(const LogRecord &record) = 0;
  virtual void write(const std::string &message) = 0;
  virtual void flush() = 0;
  virtual void onError(
      const std::string &errorMessage,
      const std::source_location &location = std::source_location::current()) {
    std::cerr << "[" << location.function_name() << "] " << errorMessage
              << '\n';
  };
};