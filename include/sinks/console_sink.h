#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <iostream>

using formatter::IFormatter;

class ConsoleSink : public ILogSink {
public:
  explicit ConsoleSink(std::shared_ptr<IFormatter> formatter)
      : formatter_(std::move(formatter)) {}

  [[nodiscard]] bool open() const override { return true; }

  void write(const LogRecord &record) override {
    std::cout << (*formatter_)(record) << '\n';
  }

  void write(const std::string &message) override {
    std::cout << message << '\n';
  }

  void flush() override { std::cout.flush(); }

private:
  std::shared_ptr<IFormatter> formatter_;
};