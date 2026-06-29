#include "log_manager.h"

int main() {
  std::string seviceName = "ipv4filter";
  auto logger = LogManager::builder()
                    .named("logger")
                    .withLevelFilter(LogLevel::TRACE)
                    .withNameFilter({seviceName})
                    .withTimestamp()
                    .withConsoleSink()
                    .build();

  LogRecord record;
  record.loggerName = seviceName;
  record.level = LogLevel::TRACE;
  record.message = "Rejected";
  record.fields = {{"IPAddress", "192.168.88.10"},
                   {"chain", "input"},
                   {"port", "53"},
                   {"protocol", "17"}};

  logger->log(record);
}