#pragma once

#include "IFormatter.h"
#include <nlohmann/json.hpp>

namespace formatter {
class JsonFormatter : public IFormatter {
public:
  std::string operator()(const LogRecord &record) const override {
    using json = nlohmann::json;

    json jSon;
    jSon["level"] = logLevelToString(record.level);
    jSon["logger"] = record.loggerName;
    jSon["msg"] = record.message;
    jSon["timestamp"] = formatter::formatTimestamp(record.timestamp);

    for (const auto &[key, value] : record.fields) {
      jSon[key] = value;
    }

    return jSon.dump(2);
  }
};
} // namespace formatter