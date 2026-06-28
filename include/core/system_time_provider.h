#pragma once

#include "ITime_provider.h"
#include <chrono>

class SystemTimeProvider final : public ITimeProvider {
public:
  [[nodiscard]] TimePoint getCurrentTimepoint() const override {
    return std::chrono::system_clock::now();
  }
};