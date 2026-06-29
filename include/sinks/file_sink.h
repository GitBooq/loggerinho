#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <filesystem>
#include <fstream>

using formatter::IFormatter;

class FileSink : public ILogSink {
public:
  FileSink(std::shared_ptr<IFormatter> formatter,
           std::filesystem::path filePath, std::size_t maxFileSize);

  ~FileSink() noexcept override;

  bool open() const noexcept override;
  void write(const LogRecord &record) noexcept override;
  void write(const std::string &message) noexcept override;
  void flush() noexcept override;

private:
  void rotate();
  void deleteOldestArchiveIfExist();
  void renameArchivesIfAny();
  void renameCurrentFileToArchive();
  static std::size_t getBufferSize(std::stringstream &buffer);

  static constexpr size_t kMaxArchives_ = 3;

  std::shared_ptr<IFormatter> formatter_;
  std::filesystem::path filePath_;
  std::size_t maxFileSize_;
  std::size_t writtenBytes_ = 0;
  std::ofstream file_;
};