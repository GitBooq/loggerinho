#include "formatters/json_formatter.h"
#include <nlohmann/json.hpp>

namespace formatter {
std::string JsonFormatter::operator()(const LogRecord& record) const {
  using json = nlohmann::json;

  json jSon;
  jSon["level"] = logLevelToString(record.level);
  jSon["logger"] = record.loggerName;
  jSon["msg"] = record.message;
  jSon["timestamp"] = formatter::formatTimestamp(record.timestamp);

  for (const auto& [key, value] : record.fields) {
    jSon[key] = value;
  }

  return jSon.dump(2);
}
}  // namespace formatter