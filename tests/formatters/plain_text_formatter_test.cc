#include "formatters/plain_text_formatter.h"
#include <gtest/gtest.h>

using formatter::PlainTextFormatter;

class PlainTextFormatterTest : public ::testing::Test {
protected:
  void SetUp() override {
    record.loggerName = "http";
    record.message = "Request received";
    record.level = LogLevel::INFO;
    record.fields = {{"method", "GET"}, {"path", "/api"}, {"status", "200"}};
    record.timestamp =
        std::chrono::system_clock::from_time_t(0); // 1970-01-01 03:00:00
  }

  LogRecord record;
  PlainTextFormatter formatter;
};

TEST_F(PlainTextFormatterTest, FormatsFullRecord) {
  std::string result = formatter(record);
  std::string etalon = "[1970-01-01 03:00:00] [INFO ] [http] Request received "
                       "method=GET path=/api status=200";

  EXPECT_EQ(etalon, result);
}

TEST_F(PlainTextFormatterTest, FormatsEmptyFields) {
  LogRecord record_wo_fields = record;
  record_wo_fields.fields.clear();

  std::string result = formatter(record_wo_fields);
  std::string etalon = "[1970-01-01 03:00:00] [INFO ] [http] Request received";

  EXPECT_EQ(etalon, result);
}
