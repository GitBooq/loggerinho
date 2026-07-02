#pragma once
#include "IFilter.h"
#include "core/log_record.h"

/**
 * @brief Passes only logs with level >= minLevel_
 *
 */
class LevelFilter final : public IFilter {
public:
  explicit LevelFilter(LogLevel minLevel) : minLevel_(minLevel) {};

  bool operator()(const LogRecord &record) override {
    return record.level >= minLevel_;
  }

private:
  LogLevel minLevel_;
};