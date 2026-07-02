#pragma once

#include "core/log_record.h"

#include <iostream>
#include <source_location>

class ILogSink {
public:
  virtual ~ILogSink() = default;
  [[nodiscard]] virtual bool open() const noexcept = 0;
  virtual void write(const LogRecord &record) noexcept = 0;
  virtual void write(const std::string &message) noexcept = 0;
  virtual void flush() noexcept = 0;
  virtual void onError(const std::string &errorMessage,
                       const std::source_location &location =
                           std::source_location::current()) noexcept {
    try {
      std::cerr << "[" << location.function_name() << "] " << errorMessage
                << '\n';
    } catch (...) {
      // supress
    }
  };
};