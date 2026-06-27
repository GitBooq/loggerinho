#pragma once

#include "IFilter.h"
#include <set>
#include <string>

/**
 * @brief Passes records w/ names in allowed list only.
 *        If list is empty passes all records.
 *
 */
class NameFilter : public IFilter {
public:
  // one extra move but who cares
  explicit NameFilter(std::set<std::string> allowed)
      : allowed_(std::move(allowed)) {}

  bool operator()(const LogRecord &record) override {
    return allowed_.empty() || allowed_.contains(record.loggerName);
  }

private:
  std::set<std::string> allowed_;
};