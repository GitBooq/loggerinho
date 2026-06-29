#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <vector>
#include <memory>

class BufferedSink : public ILogSink {
public:
  BufferedSink(std::shared_ptr<formatter::IFormatter> formatter,
               std::unique_ptr<ILogSink> downstream, std::size_t batchSize);

  ~BufferedSink() noexcept override = default;
  // disallow copy but move
  BufferedSink(const BufferedSink &) = delete;
  BufferedSink(BufferedSink &&) noexcept = default;
  BufferedSink &operator=(const BufferedSink &) = delete;
  BufferedSink &operator=(BufferedSink &&) noexcept = default;

  [[nodiscard]] bool open() const noexcept override;
  void write(const LogRecord &record) noexcept override;
  void write(const std::string &message) noexcept override;
  void flush() noexcept override;

private:
  std::shared_ptr<formatter::IFormatter> formatter_;
  std::unique_ptr<ILogSink> downstream_;
  std::size_t batchSize_;
  std::vector<std::string> formattedMessages_;
};