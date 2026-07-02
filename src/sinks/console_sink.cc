#include "sinks/console_sink.h"

#include <iostream>

using formatter::IFormatter;

void ConsoleSink::write(const LogRecord &record) noexcept try {
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

void ConsoleSink::write(const std::string &message) noexcept try {
  std::cout << message << '\n';
} catch (const std::exception &e) {
  onError("Unexpected error in write: " + std::string(e.what()));
} catch (...) {
  onError("Unknown error in write");
}

void ConsoleSink::flush() noexcept try {
  std::cout.flush();
} catch (const std::exception &e) {
  onError("Flush error: " + std::string(e.what()));
} catch (...) {
  onError("Unknown error in flush");
}
