#pragma once

#include "IEnricher.h"

class BaseEnricher final : public IEnricher {
public:
  LogRecord enrich(LogRecord &record) override {
    return record;
  }
};