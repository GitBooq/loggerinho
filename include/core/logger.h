#pragma once

#include <algorithm>
#include <string>
#include <utility>

#include "enrichers/IEnricher.h"
#include "enrichers/base_enricher.h"
#include "formatters/plain_text_formatter.h"
#include "filters/IFilter.h"
#include "sinks/ILog_sink.h"
#include "sinks/console_sink.h"

class Logger {
public:
  explicit Logger(std::string name)
      : name_(std::move(name)), enricher_(std::make_shared<BaseEnricher>()) {
    sinks_.push_back(std::make_shared<ConsoleSink>(std::make_shared<formatter::PlainTextFormatter>()));
  }

  Logger(std::string name, std::vector<std::shared_ptr<IFilter>> filters,
         std::shared_ptr<IEnricher> enricher = std::make_shared<BaseEnricher>(),
         std::vector<std::shared_ptr<ILogSink>> sinks = {})
      : name_(std::move(name)), filters_(std::move(filters)),
        enricher_(std::move(enricher)), sinks_(std::move(sinks)) {}

  void log(const LogRecord &record) {
    LogRecord processed = record;

    if (!shouldBeLogged(processed)) {
      return;
    }

    processed = enricher_->enrich(processed);

    passToSinks(processed);
  }

  void flush() {
    for (const auto &sink : sinks_) {
      sink->flush();
    }
  }

  [[nodiscard]] const std::string &name() const noexcept { return name_; }

  void addFilter(std::shared_ptr<IFilter> filter) {
    filters_.push_back(std::move(filter));
  }

  void addSink(std::shared_ptr<ILogSink> sink) {
    sinks_.push_back(std::move(sink));
  }

  void setEnricher(std::shared_ptr<IEnricher> enricher) noexcept {
    enricher_ = std::move(enricher);
  }

private:
  // Whether record pass this logger filters
  [[nodiscard]] bool shouldBeLogged(const LogRecord &record) const {
    return std::ranges::all_of(
        filters_, [&record](const auto &filter) { return (*filter)(record); });
  }

  void passToSinks(const LogRecord &record) const {
    for (const auto &sink : sinks_) {
      sink->write(record);
    }
  }

  std::string name_;
  std::vector<std::shared_ptr<IFilter>> filters_;
  std::shared_ptr<IEnricher> enricher_;
  std::vector<std::shared_ptr<ILogSink>> sinks_;
};