#pragma once

#include "core/log_record.h"

class IEnricher {
public:
  virtual ~IEnricher() = default;
  virtual LogRecord enrich(LogRecord &record) = 0;
};
