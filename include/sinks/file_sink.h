#pragma once

#include "ILog_sink.h"
#include "formatters/IFormatter.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <utility>

using formatter::IFormatter;

class FileSink : public ILogSink {
public:
  FileSink(std::shared_ptr<IFormatter> formatter,
           std::filesystem::path filePath, std::size_t maxFileSize)
      : formatter_(std::move(formatter)), filePath_(std::move(filePath)),
        maxFileSize_(maxFileSize) {}

  ~FileSink() noexcept override {
    try {
      if (file_.is_open()) {
        file_.close();
      }
    } catch (...) {
    }
  }

  bool open() const noexcept override { return file_.is_open(); }

  void write(const LogRecord &record) noexcept override {
    try {
      if (!file_.is_open()) {
        file_.open(filePath_, std::ios::app);
        if (!file_.is_open()) {
          onError("Cannot open file: " + filePath_.generic_string());
          return;
        }
        writtenBytes_ = std::filesystem::file_size(filePath_);
      }

      if (writtenBytes_ >= maxFileSize_) {
        try {
          rotate();
        } catch (const std::exception &e) {
          onError("Rotation failed: " + std::string(e.what()));
          return;
        }
      }

      std::stringstream buffer;
      try {
        buffer << (*formatter_)(record) << '\n';
      } catch (const std::exception &e) {
        onError("Formatting failed: " + std::string(e.what()));
        return;
      }

      std::size_t messageSize = getBufferSize(buffer);

      try {
        file_ << buffer.rdbuf();
        writtenBytes_ += messageSize;
      } catch (const std::exception &e) {
        onError("Write failed: " + std::string(e.what()));
        file_.close();
      }
    } catch (const std::exception &e) {
      onError("Unexpected error in write: " + std::string(e.what()));
    } catch (...) {
      onError("Unknown error in write");
    }
  }

  void write(const std::string &message) noexcept override {
    try {
      if (!file_.is_open()) {
        file_.open(filePath_, std::ios::app);
        if (!file_.is_open()) {
          onError("Cannot open file.");
          return;
        }
        writtenBytes_ = std::filesystem::file_size(filePath_);
      }

      if (writtenBytes_ >= maxFileSize_) {
        try {
          rotate();
        } catch (const std::exception &e) {
          onError("Rotation failed: " + std::string(e.what()));
          return;
        }
      }

      std::stringstream buffer;
      buffer << message << '\n';

      std::size_t messageSize = getBufferSize(buffer);
      try {
        file_ << buffer.rdbuf();
        writtenBytes_ += messageSize;
      } catch (const std::exception &e) {
        onError("Write failed: " + std::string(e.what()));
        file_.close();
      }

    } catch (const std::exception &e) {
      onError("Unexpected error in write: " + std::string(e.what()));
    } catch (...) {
      onError("Unknown error in write");
    }
  }

  void flush() noexcept override {
    try {
      if (file_.is_open()) {
        file_.flush();
      }
    } catch (const std::exception &e) {
      onError("Flush failed: " + std::string(e.what()));
    } catch (...) {
      onError("Unknown error during flush");
    }
  }

private:
  void rotate() {
    file_.close();

    deleteOldestArchiveIfExist();
    renameArchivesIfAny();
    renameCurrentFileToArchive();

    file_.open(filePath_);
    writtenBytes_ = 0;
  }

  void deleteOldestArchiveIfExist() {
    std::string oldest =
        filePath_.generic_string() + "." + std::to_string(kMaxArchives_);
    if (std::filesystem::exists(oldest)) {
      std::filesystem::remove(oldest);
    }
  }

  void renameArchivesIfAny() {
    for (int i = kMaxArchives_ - 1; i >= 1; --i) {
      std::string from = filePath_.generic_string() + "." + std::to_string(i);
      std::string to = filePath_.generic_string() + "." + std::to_string(i + 1);
      if (std::filesystem::exists(from)) {
        std::filesystem::rename(from, to);
      }
    }
  }

  void renameCurrentFileToArchive() {
    if (std::filesystem::exists(filePath_)) {
      std::filesystem::rename(filePath_, filePath_.generic_string() + ".1");
    }
  }

  static std::size_t getBufferSize(std::stringstream &buffer) {
    auto originalPos = buffer.tellg();
    buffer.seekp(0, std::ios::end);
    size_t bufSize = buffer.tellp();
    buffer.seekp(originalPos);

    return bufSize;
  }

  static constexpr size_t kMaxArchives_ = 3;

  std::shared_ptr<IFormatter> formatter_;
  std::filesystem::path filePath_;
  std::size_t maxFileSize_;
  std::size_t writtenBytes_ = 0;
  std::ofstream file_;
};