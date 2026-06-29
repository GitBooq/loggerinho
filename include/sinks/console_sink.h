#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <iostream>

using formatter::IFormatter;

class ConsoleSink : public ILogSink {
public:
  ConsoleSink() = default;

  explicit ConsoleSink(std::shared_ptr<IFormatter> formatter)
      : formatter_(std::move(formatter)) {}

  [[nodiscard]] bool open() const noexcept override { return true; }

  void write(const LogRecord &record) noexcept override try {
    if (formatter_ == nullptr) {
      onError("No formatter specified for sink.");
      return;
    }
    std::cout << (*formatter_)(record) << '\n';
  } catch (const std::exception &e) {
    onError("Unexpected error in write: " + std::string(e.what()));
  } catch (...) {
    onError("Unknown error in write");
  }

  void write(const std::string &message) noexcept override try {
    std::cout << message << '\n';
  } catch (const std::exception &e) {
    onError("Unexpected error in write: " + std::string(e.what()));
  } catch (...) {
    onError("Unknown error in write");
  }

  void flush() noexcept override try {
    std::cout.flush();
  } catch (const std::exception &e) {
    onError("Flush error: " + std::string(e.what()));
  } catch (...) {
    onError("Unknown error in flush");
  }

private:
  std::shared_ptr<IFormatter> formatter_;
};