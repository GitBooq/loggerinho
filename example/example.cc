#include "core/log_manager.h"

int main() {
  std::string seviceName = "ipv4filter";

  auto logger_console_plain = LogManager::builder()
                                  .named("logger_console_plain")
                                  .withLevelFilter(LogLevel::TRACE)
                                  .withNameFilter({seviceName})
                                  .withTimestamp()
                                  .withConsoleSink()
                                  .build();

  auto logger_console_plain_buf =
      LogManager::builder()
          .named("logger_console_plain_buf")
          .withLevelFilter(LogLevel::TRACE)
          .withNameFilter({seviceName})
          .withTimestamp()
          .withBufferedSink(std::make_unique<ConsoleSink>(), 1024,
                            std::make_shared<formatter::PlainTextFormatter>())
          .build();

  auto logger_console_json =
      LogManager::builder()
          .withConsoleSink(std::make_shared<formatter::JsonFormatter>())
          .build();

  auto logger_console_json_buf =
      LogManager::builder()
          .withBufferedSink(std::make_unique<ConsoleSink>(), 1024,
                            std::make_shared<formatter::JsonFormatter>())
          .build();

  LogRecord record;
  record.loggerName = seviceName;
  record.level = LogLevel::TRACE;
  record.message = "Rejected";
  record.fields = {{"IPAddress", "192.168.88.10"},
                   {"chain", "input"},
                   {"port", "53"},
                   {"protocol", "17"}};

  std::cout << "logger_console_plain:\n";
  logger_console_plain->log(record);
  std::cout << '\n';

  std::cout << "logger_console_json:\n";
  logger_console_json->log(record);
  std::cout << '\n';

  std::cout << "logger_console_plain_buf:\n";
  logger_console_plain_buf->log(record);
  logger_console_plain_buf->flush();
  std::cout << '\n';

  std::cout << "logger_console_json_buf:\n";
  logger_console_json_buf->log(record);
  logger_console_json_buf->flush();
  std::cout << '\n';
}