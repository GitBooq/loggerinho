#pragma once

#include "IFormatter.h"
#include <format>

namespace formatter {
class PlainTextFormatter final : public IFormatter {

public:
  std::string operator()(const LogRecord &record) const override {
    // the level takes exactly 5 characters (space aligned to the right)
    std::string record_str =
        std::format("[{}] [{:5}] [{}] {}{}", formatTimestamp(record.timestamp),
                    logLevelToString(record.level), record.loggerName,
                    record.message, recordFieldsToString(record.fields));

    return record_str;
  }

private:
  static std::string
  recordFieldsToString(const std::map<std::string, std::string> &fields) {
    std::string result;
    size_t estimatedSize{0};
    for (const auto &[key, value] : fields) {
      estimatedSize += key.size() + value.size() + 2; // space and '='
    }
    result.reserve(estimatedSize);

    for (const auto &[key, value] : fields) {
      result.append(std::format(" {}={}", key, value));
    }

    result.shrink_to_fit();

    return result;
  }
};
} // namespace formatter