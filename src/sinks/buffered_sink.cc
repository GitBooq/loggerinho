#include "sinks/buffered_sink.h"

using formatter::IFormatter;

BufferedSink::BufferedSink(std::shared_ptr<IFormatter> formatter,
                           std::unique_ptr<ILogSink> downstream,
                           std::size_t batchSize)
    : formatter_(std::move(formatter)), downstream_(std::move(downstream)),
      batchSize_(batchSize) {
  formattedMessages_.reserve(batchSize_);
}

[[nodiscard]] bool BufferedSink::open() const noexcept {
  return downstream_->open();
}

void BufferedSink::write(const LogRecord &record) noexcept {
  std::string formatted = (*formatter_)(record);
  formattedMessages_.push_back(formatted);

  if (formattedMessages_.size() >= batchSize_) {
    flush();
  }
}

void BufferedSink::write(const std::string &message) noexcept {
  formattedMessages_.push_back(message);

  if (formattedMessages_.size() >= batchSize_) {
    flush();
  }
}

void BufferedSink::flush() noexcept {
  if (!formattedMessages_.empty()) {
    for (const auto &msg : formattedMessages_) {
      downstream_->write(msg);
    }
    downstream_->flush();
    formattedMessages_.clear();
  }
}
