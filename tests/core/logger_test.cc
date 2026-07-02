#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/logger.h"
#include "mocks/mock_enricher.h"
#include "mocks/mock_filter.h"
#include "mocks/mock_log_sink.h"

using ::testing::_;
using ::testing::An;
using ::testing::Exactly;
using ::testing::InSequence;
using ::testing::NiceMock;
using ::testing::Return;

class LoggerTest : public ::testing::Test {
protected:
  void SetUp() override {
    mockFilter1_ = std::make_shared<NiceMock<MockFilter>>();
    mockFilter2_ = std::make_shared<NiceMock<MockFilter>>();
    mockEnricher_ = std::make_shared<NiceMock<MockEnricher>>();
    mockSink1_ = std::make_shared<NiceMock<MockLogSink>>();
    mockSink2_ = std::make_shared<NiceMock<MockLogSink>>();
  }

  static std::string captureCout(const std::function<void()> &func) {
    std::streambuf *original = std::cout.rdbuf();
    std::stringstream buffer;
    std::cout.rdbuf(buffer.rdbuf());

    func();

    std::cout.rdbuf(original);
    return buffer.str();
  }

  std::shared_ptr<MockFilter> mockFilter1_;
  std::shared_ptr<MockFilter> mockFilter2_;
  std::shared_ptr<MockEnricher> mockEnricher_;
  std::shared_ptr<MockLogSink> mockSink1_;
  std::shared_ptr<MockLogSink> mockSink2_;
};

TEST_F(LoggerTest, CtorWithName) {
  std::string name{"TestLogger"};
  Logger logger(name);
  EXPECT_EQ(logger.name(), name);
}

TEST_F(LoggerTest, DefaultCtorCreatesConsoleSink) {
  std::string output = captureCout([]() {
    Logger logger("TestLogger");

    LogRecord record;
    record.loggerName = "TestLogger";
    record.message = "Test message";
    record.level = LogLevel::INFO;

    logger.log(record);
    logger.flush();
  });

  EXPECT_TRUE(output.find("Test message") != std::string::npos);
}

TEST_F(LoggerTest, FullConstructor) {
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter1_, mockFilter2_};
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_, mockSink2_};

  Logger logger("TestLogger", filters, mockEnricher_, sinks);

  EXPECT_EQ(logger.name(), "TestLogger");
}

TEST_F(LoggerTest, LogCallsFilters) {
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter1_, mockFilter2_};
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_};

  Logger logger("TestLogger", filters, mockEnricher_, sinks);

  LogRecord record;

  EXPECT_CALL(*mockFilter1_, ParanthesisOp(_)).WillOnce(Return(true));
  EXPECT_CALL(*mockFilter2_, ParanthesisOp(_)).WillOnce(Return(true));
  EXPECT_CALL(*mockEnricher_, enrich(_)).WillOnce(Return(record));
  EXPECT_CALL(*mockSink1_, write(An<const LogRecord &>())).Times(Exactly(1));

  logger.log(record);
}

TEST_F(LoggerTest, FilterCanRejectRecord) {
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter1_, mockFilter2_};
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_};

  Logger logger("TestLogger", filters, mockEnricher_, sinks);

  LogRecord record;

  EXPECT_CALL(*mockFilter1_, ParanthesisOp(_)).WillOnce(Return(true));
  EXPECT_CALL(*mockFilter2_, ParanthesisOp(_)).WillOnce(Return(false));
  EXPECT_CALL(*mockEnricher_, enrich(_)).Times(0);
  EXPECT_CALL(*mockSink1_, write(An<const LogRecord &>())).Times(0);

  logger.log(record);
}

TEST_F(LoggerTest, LogSendsToAllSinks) {
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter1_};
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_, mockSink2_};

  Logger logger("TestLogger", filters, mockEnricher_, sinks);

  LogRecord record;

  EXPECT_CALL(*mockFilter1_, ParanthesisOp(_)).WillOnce(Return(true));
  EXPECT_CALL(*mockEnricher_, enrich(_)).WillOnce(Return(record));
  EXPECT_CALL(*mockSink1_, write(An<const LogRecord &>())).Times(Exactly(1));
  EXPECT_CALL(*mockSink2_, write(An<const LogRecord &>())).Times(Exactly(1));

  logger.log(record);
}

TEST_F(LoggerTest, FlushCallsFlushOnAllSinks) {
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_, mockSink2_};

  Logger logger("TestLogger", {}, nullptr, sinks);

  EXPECT_CALL(*mockSink1_, flush()).Times(Exactly(1));
  EXPECT_CALL(*mockSink2_, flush()).Times(Exactly(1));

  logger.flush();
}

TEST_F(LoggerTest, EmptyFilters) {
  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink1_};

  Logger logger("TestLogger", {}, mockEnricher_, sinks);

  LogRecord record;

  EXPECT_CALL(*mockEnricher_, enrich(_)).WillOnce(Return(record));
  EXPECT_CALL(*mockSink1_, write(An<const LogRecord &>())).Times(Exactly(1));

  logger.log(record);
}

TEST_F(LoggerTest, EmptySinks) {
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter1_};

  Logger logger("TestLogger", filters, mockEnricher_, {});

  LogRecord record;

  EXPECT_CALL(*mockFilter1_, ParanthesisOp(_)).WillOnce(Return(true));
  EXPECT_CALL(*mockEnricher_, enrich(_)).WillOnce(Return(record));
  EXPECT_CALL(*mockSink1_, write(An<const LogRecord &>())).Times(0);

  logger.log(record);
}