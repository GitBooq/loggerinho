#pragma once

#include "enricher_factory.h"
#include "filters/IFilter.h"
#include "filters/LevelFilter.h"
#include "filters/NameFilter.h"
#include "formatters/json_formatter.h"
#include "logger.h"
#include "sinks/ILog_sink.h"
#include "sinks/buffered_sink.h"
#include "sinks/file_sink.h"

#include <map>
#include <memory>
#include <mutex>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

class LogManager {
public:
  static LogManager &instance() {
    static LogManager manager; // Thread-­safe static local variable.
    return manager;
  }

  LogManager(const LogManager &) = delete;
  LogManager(LogManager &&) = delete;
  LogManager &operator=(const LogManager &) = delete;
  LogManager &operator=(LogManager &&) = delete;

  std::shared_ptr<Logger> getOrCreate(
      const std::string &name,
      std::vector<std::shared_ptr<ILogSink>> sinks = {},
      std::shared_ptr<IEnricher> enricher = std::make_shared<BaseEnricher>(),
      std::vector<std::shared_ptr<IFilter>> filters = {}) {
    std::scoped_lock<std::mutex> lock(mutex_);

    auto iter = loggers_.find(name);
    if (iter != loggers_.end()) {
      return iter->second;
    }

    auto logger = std::make_shared<Logger>(
        name, std::move(filters), std::move(enricher), std::move(sinks));

    loggers_[name] = logger;
    return logger;
  }

  std::shared_ptr<Logger> get(const std::string &name) {
    std::scoped_lock<std::mutex> lock(mutex_);

    auto iter = loggers_.find(name);
    if (iter == loggers_.end()) {
      throw std::runtime_error("Logger not found: " + name);
    }
    return iter->second;
  }

  std::optional<std::shared_ptr<Logger>> tryGet(const std::string &name) {
    std::scoped_lock<std::mutex> lock(mutex_);

    auto iter = loggers_.find(name);
    if (iter == loggers_.end()) {
      return std::nullopt;
    }
    return iter->second;
  }

  bool contains(const std::string &name) const {
    std::scoped_lock<std::mutex> lock(mutex_);
    return loggers_.contains(name);
  }

  bool remove(const std::string &name) {
    std::scoped_lock<std::mutex> lock(mutex_);
    return loggers_.erase(name) > 0;
  }

  // removes all managed loggers
  void clear() {
    std::scoped_lock<std::mutex> lock(mutex_);
    loggers_.clear();
  }

  std::vector<std::string> getNames() const {
    std::scoped_lock<std::mutex> lock(mutex_);

    std::vector<std::string> names;
    names.reserve(loggers_.size());
    for (const auto &[name, _] : loggers_) {
      names.push_back(name);
    }
    return names;
  }

  void flushAll() {
    std::scoped_lock<std::mutex> lock(mutex_);
    for (auto &[_, logger] : loggers_) {
      logger->flush();
    }
  }

  class Builder {
  public:
    Builder() = default;

    Builder &named(const std::string &name) {
      name_ = name;
      return *this;
    }

    Builder &
    withConsoleSink(const std::shared_ptr<formatter::IFormatter> &formatter =
                        std::make_shared<formatter::PlainTextFormatter>()) {
      sinks_.push_back(std::make_shared<ConsoleSink>(formatter));
      return *this;
    }

    Builder &
    withFileSink(const std::string &path,
                 const std::shared_ptr<formatter::IFormatter> &formatter =
                     std::make_shared<formatter::PlainTextFormatter>(),
                 std::size_t maxSize = 1024 * 1024) {
      sinks_.push_back(std::make_shared<FileSink>(formatter, path, maxSize));
      return *this;
    }

    Builder &withBufferedSink(const std::shared_ptr<ILogSink> &downstream,
                              std::size_t batchSize = 100) {
      sinks_.push_back(std::make_shared<BufferedSink>(
          std::make_shared<formatter::PlainTextFormatter>(),
          std::unique_ptr<ILogSink>(downstream.get()), batchSize));
      return *this;
    }

    Builder &withTimestamp() {
      enricher_ = EnricherFactory::create(enricher::EnricherFlag::TIMESTAMP);
      return *this;
    }

    Builder &withLevelFilter(LogLevel minLevel) {
      filters_.push_back(std::make_shared<LevelFilter>(minLevel));
      return *this;
    }

    Builder &withNameFilter(const std::set<std::string> &allowed) {
      filters_.push_back(std::make_shared<NameFilter>(allowed));
      return *this;
    }

    Builder &withJsonFormatter() {
      formatter_ = std::make_shared<formatter::JsonFormatter>();
      return *this;
    }

    Builder &withPlainTextFormatter() {
      formatter_ = std::make_shared<formatter::PlainTextFormatter>();
      return *this;
    }

    /**
     * @brief Build logger using defaults(if not specified) as
     * PlainTextFormatter, ConsoleSink, TIMESTAMP Enricher
     *
     */
    std::shared_ptr<Logger> build() {
      if (!formatter_) {
        formatter_ = std::make_shared<formatter::PlainTextFormatter>();
      }

      if (sinks_.empty()) {
        sinks_.push_back(std::make_shared<ConsoleSink>(formatter_));
      }

      if (!enricher_) {
        enricher_ = EnricherFactory::create(enricher::EnricherFlag::TIMESTAMP);
      }

      auto logger =
          std::make_shared<Logger>(name_, filters_, enricher_, sinks_);

      LogManager::instance().loggers_[name_] = logger;

      return logger;
    }

  private:
    std::string name_ = "logger";
    std::vector<std::shared_ptr<ILogSink>> sinks_;
    std::shared_ptr<IEnricher> enricher_;
    std::vector<std::shared_ptr<IFilter>> filters_;
    std::shared_ptr<IFormatter> formatter_;
  };

  static Builder builder() { return {}; }

private:
  LogManager() = default;
  ~LogManager() = default;

  mutable std::mutex mutex_;
  std::map<std::string, std::shared_ptr<Logger>> loggers_;
};