#pragma once

#include "IFormatter.h"

namespace formatter {
class JsonFormatter : public IFormatter {
 public:
  std::string operator()(const LogRecord& record) const override;
};
}  // namespace formatter