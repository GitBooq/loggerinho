#pragma once
#include "core/log_record.h"

class IFilter {
  /**
   * @brief Determines whether a log record should be processed further
   *
   * @param record The log record to check
   * @return true if the record passes the filter, false otherwise
   */
  virtual bool operator()(const LogRecord &record) = 0;
};