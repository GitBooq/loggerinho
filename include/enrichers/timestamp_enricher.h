#pragma once

#include "IEnricher.h"
#include "core/ITime_provider.h"
#include "core/system_time_provider.h"
#include <chrono>
#include <memory>
#include <utility>

class TimestampEnricher final : public IEnricher {
public:
  explicit TimestampEnricher(std::shared_ptr<IEnricher> next,
                             std::shared_ptr<ITimeProvider> timeProvider =
                                 std::make_shared<SystemTimeProvider>())
      : next_(std::move(next)), timeProvider_(std::move(timeProvider)) {}

  LogRecord enrich(LogRecord &record) override {
    auto result = next_->enrich(record);

    result.timestamp = timeProvider_->getCurrentTimepoint();

    return result;
  }

private:
  std::shared_ptr<IEnricher> next_;
  std::shared_ptr<ITimeProvider> timeProvider_;
};