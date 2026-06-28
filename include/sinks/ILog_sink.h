#pragma once

#include "core/log_record.h"

class ILogSink {
public:
  virtual ~ILogSink() = default;
  [[nodiscard]] virtual bool open() const = 0;
  virtual void write(const LogRecord& record) = 0;
  virtual void flush() = 0;
  virtual void onError(const LogRecord& record) = 0;
};