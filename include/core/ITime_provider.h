#pragma once

#include <chrono>

class ITimeProvider {
protected:
  using TimePoint = std::chrono::time_point<std::chrono::system_clock>;

public:
  virtual ~ITimeProvider() = default;
  [[nodiscard]] virtual TimePoint getCurrentTimepoint() const = 0;
};