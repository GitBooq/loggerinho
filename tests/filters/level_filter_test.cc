#include <gtest/gtest.h>
#include <vector>

#include "filters/LevelFilter.h"

TEST(LevelFilterTest, FiltersByLevelSimple) {
  LevelFilter filter(LogLevel::WARN);
  LogRecord info_record;
  info_record.level = LogLevel::INFO;

  LogRecord warning_record;
  warning_record.level = LogLevel::WARN;

  LogRecord error_record;
  error_record.level = LogLevel::ERROR;

  EXPECT_FALSE(filter(info_record));   // INFO < WARNING -> false
  EXPECT_TRUE(filter(warning_record)); // WARNING == WARNING -> true
  EXPECT_TRUE(filter(error_record));   // ERROR > WARNING -> true
}

const std::vector<LogLevel> kLogLevels = {LogLevel::TRACE, LogLevel::DEBUG,
                                          LogLevel::INFO,  LogLevel::WARN,
                                          LogLevel::ERROR, LogLevel::FATAL};

class LevelFilterTest
    : public ::testing::TestWithParam<std::tuple<LogLevel, LogLevel>> {};

TEST_P(LevelFilterTest, FiltersCorrectly) {
  auto [filter_level, record_level] = GetParam();

  LevelFilter filter(filter_level);
  LogRecord record;
  record.level = record_level;

  EXPECT_EQ(filter(record), record_level >= filter_level);
}

INSTANTIATE_TEST_SUITE_P(AllLevelCombinations, LevelFilterTest,
                         ::testing::Combine(::testing::ValuesIn(kLogLevels),
                                            ::testing::ValuesIn(kLogLevels)));