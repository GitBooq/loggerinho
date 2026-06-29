#pragma once

#include "enrichers/IEnricher.h"
#include "filters/IFilter.h"
#include "logger.h"
#include "sinks/ILog_sink.h"

#include <map>
#include <memory>
#include <mutex>
#include <optional>
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

private:
  LogManager() = default;
  ~LogManager() = default;

  mutable std::mutex mutex_;
  std::map<std::string, std::shared_ptr<Logger>> loggers_;
};