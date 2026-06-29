#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <vector>

using formatter::IFormatter;

class BufferedSink : public ILogSink {
public:
  BufferedSink(std::shared_ptr<IFormatter> formatter,
               std::unique_ptr<ILogSink> downstream, std::size_t batchSize)
      : formatter_(std::move(formatter)), downstream_(std::move(downstream)),
        batchSize_(batchSize) {
    formattedMessages_.reserve(batchSize_);
  }

  ~BufferedSink() noexcept override = default;
  // disallow copy but move
  BufferedSink(const BufferedSink &) = delete;
  BufferedSink(BufferedSink &&) noexcept = default;
  BufferedSink &operator=(const BufferedSink &) = delete;
  BufferedSink &operator=(BufferedSink &&) noexcept = default;

  [[nodiscard]] bool open() const noexcept override {
    return downstream_->open();
  }

  void write(const LogRecord &record) noexcept override {
    std::string formatted = (*formatter_)(record);
    formattedMessages_.push_back(formatted);

    if (formattedMessages_.size() >= batchSize_) {
      flush();
    }
  }

  void write(const std::string &message) noexcept override {
    formattedMessages_.push_back(message);

    if (formattedMessages_.size() >= batchSize_) {
      flush();
    }
  }

  void flush() noexcept override {
    if (!formattedMessages_.empty()) {
      for (const auto &msg : formattedMessages_) {
        downstream_->write(msg);
      }
      downstream_->flush();
      formattedMessages_.clear();
    }
  }

private:
  std::shared_ptr<IFormatter> formatter_;
  std::unique_ptr<ILogSink> downstream_;
  std::size_t batchSize_;
  std::vector<std::string> formattedMessages_;
};