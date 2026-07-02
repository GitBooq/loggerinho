#include "sinks/buffered_sink.h"

using formatter::IFormatter;

BufferedSink::BufferedSink(std::shared_ptr<IFormatter> formatter,
                           std::unique_ptr<ILogSink> downstream,
                           std::size_t batchSize)
    : formatter_(std::move(formatter)),
      downstream_(std::move(downstream)),
      batchSize_(batchSize) {
  formattedMessages_.reserve(batchSize_);
}

BufferedSink::~BufferedSink() noexcept { flush(); }

[[nodiscard]] bool BufferedSink::open() const noexcept {
  return downstream_->open();
}

void BufferedSink::write(const LogRecord& record) noexcept try {
  std::string formatted = (*formatter_)(record);
  formattedMessages_.push_back(std::move(formatted));

  if (formattedMessages_.size() >= batchSize_) {
    flush();
  }
} catch (const std::exception& e) {
  std::cerr << "Exception in BufferedSink::write(const LogRecord& "
               "record): "
            << e.what() << '\n';
  // suppress
} catch (...) {
  std::cerr << "Unknown exception in BufferedSink::write(const "
               "LogRecord& record)\n";
  // suppress
}

void BufferedSink::write(const std::string& message) noexcept try {
  formattedMessages_.push_back(message);

  if (formattedMessages_.size() >= batchSize_) {
    flush();
  }
} catch (const std::exception& e) {
  std::cerr << "Exception in BufferedSink::write(const "
               "std::string& message): "
            << e.what() << '\n';
} catch (...) {
  std::cerr << "Unknown exception in BufferedSink::write(const "
               "std::string& message)\n";
  // suppress
}

void BufferedSink::flush() noexcept try {
  if (!formattedMessages_.empty()) {
    for (const auto& msg : formattedMessages_) {
      downstream_->write(msg);
    }
    downstream_->flush();
    formattedMessages_.clear();
  }
} catch (const std::exception& e) {
  std::cerr << "Exception in BufferedSink::flush: " << e.what() << '\n';
  formattedMessages_.clear();
  // suppress
} catch (...) {
  std::cerr << "Unknown exception in BufferedSink::flush\n";
  formattedMessages_.clear();
  // suppress
}