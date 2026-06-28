#include "formatters/json_formatter.h"

#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>

using formatter::JsonFormatter;
using json = nlohmann::json;

class JsonFormatterTest : public ::testing::Test {
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
  JsonFormatter formatter;
};

TEST_F(JsonFormatterTest, ContainsAllFields) {
  std::string result = formatter(record);
  auto jSon = json::parse(result);

  EXPECT_EQ(jSon["level"], "INFO");
  EXPECT_EQ(jSon["logger"], "http");
  EXPECT_EQ(jSon["msg"], "Request received");
  EXPECT_EQ(jSon["timestamp"], "1970-01-01 03:00:00");
  EXPECT_EQ(jSon["method"], "GET");
  EXPECT_EQ(jSon["path"], "/api");
  EXPECT_EQ(jSon["status"], "200");
}
