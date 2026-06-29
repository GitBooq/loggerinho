#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

class ConsoleSink : public ILogSink {
public:
  ConsoleSink() = default;

  explicit ConsoleSink(std::shared_ptr<formatter::IFormatter> formatter)
      : formatter_(std::move(formatter)) {}

  [[nodiscard]] bool open() const noexcept override { return true; }
  void write(const LogRecord &record) noexcept override;
  void write(const std::string &message) noexcept override;
  void flush() noexcept override;

private:
  std::shared_ptr<formatter::IFormatter> formatter_;
};