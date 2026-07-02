#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/log_manager.h"
#include "mocks/mock_enricher.h"
#include "mocks/mock_filter.h"
#include "mocks/mock_log_sink.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class LogManagerTest : public ::testing::Test {
protected:
  void SetUp() override {
    LogManager::instance().clear();

    mockSink_ = std::make_shared<NiceMock<MockLogSink>>();
    mockEnricher_ = std::make_shared<NiceMock<MockEnricher>>();
    mockFilter_ = std::make_shared<NiceMock<MockFilter>>();
  }

  void TearDown() override { LogManager::instance().clear(); }

  std::shared_ptr<MockLogSink> mockSink_;
  std::shared_ptr<MockEnricher> mockEnricher_;
  std::shared_ptr<MockFilter> mockFilter_;
};

TEST_F(LogManagerTest, SingletonReturnsSameInstance) {
  auto &instance1 = LogManager::instance();
  auto &instance2 = LogManager::instance();

  EXPECT_EQ(&instance1, &instance2);
}

TEST_F(LogManagerTest, GetOrCreateCreatesNewLogger) {
  auto &manager = LogManager::instance();
  std::string name{"TestLogger"};
  auto logger = manager.getOrCreate(name);

  EXPECT_NE(logger, nullptr);
  EXPECT_EQ(logger->name(), name);
}

TEST_F(LogManagerTest, GetOrCreateReturnsExistingLogger) {
  auto &manager = LogManager::instance();

  std::string name{"TestLogger"};
  auto logger1 = manager.getOrCreate(name);
  auto logger2 = manager.getOrCreate(name);

  EXPECT_EQ(logger1, logger2);
}

TEST_F(LogManagerTest, GetOrCreateWithParameters) {
  auto &manager = LogManager::instance();

  std::vector<std::shared_ptr<ILogSink>> sinks = {mockSink_};
  std::vector<std::shared_ptr<IFilter>> filters = {mockFilter_};

  std::string name{"TestLogger"};
  auto logger = manager.getOrCreate(name, sinks, mockEnricher_, filters);

  EXPECT_NE(logger, nullptr);
  EXPECT_EQ(logger->name(), name);
}

TEST_F(LogManagerTest, GetReturnsExistingLogger) {
  auto &manager = LogManager::instance();

  std::string name{"TestLogger"};
  manager.getOrCreate(name);
  auto logger = manager.get(name);

  EXPECT_NE(logger, nullptr);
  EXPECT_EQ(logger->name(), name);
}

TEST_F(LogManagerTest, GetThrowsWhenLoggerNotFound) {
  auto &manager = LogManager::instance();

  EXPECT_THROW(manager.get("x"), std::runtime_error);
}

TEST_F(LogManagerTest, TryGetReturnsNulloptWhenLoggerNotFound) {
  auto &manager = LogManager::instance();

  auto result = manager.tryGet("x");

  EXPECT_FALSE(result.has_value());
}

TEST_F(LogManagerTest, TryGetReturnsLoggerWhenExists) {
  auto &manager = LogManager::instance();

  std::string name{"TestLogger"};
  manager.getOrCreate(name);
  auto result = manager.tryGet(name);

  EXPECT_TRUE(result.has_value());
  EXPECT_NE(*result, nullptr);
  EXPECT_EQ((*result)->name(), name);
}

TEST_F(LogManagerTest, ContainsReturnsTrueWhenLoggerExists) {
  auto &manager = LogManager::instance();

  std::string name{"TestLogger"};
  manager.getOrCreate(name);

  EXPECT_TRUE(manager.contains(name));
}

TEST_F(LogManagerTest, ContainsReturnsFalseWhenLoggerDoesNotExist) {
  auto &manager = LogManager::instance();

  EXPECT_FALSE(manager.contains("TestLogger"));
}

TEST_F(LogManagerTest, RemoveDeletesLogger) {
  auto &manager = LogManager::instance();

  std::string name{"TestLogger"};
  manager.getOrCreate(name);

  EXPECT_TRUE(manager.remove(name));
  EXPECT_FALSE(manager.contains(name));
}

TEST_F(LogManagerTest, ClearRemovesAllLoggers) {
  auto &manager = LogManager::instance();

  std::string name1{"TestLogger1"};
  std::string name2{"TestLogger2"};
  std::string name3{"TestLogger3"};
  manager.getOrCreate(name1);
  manager.getOrCreate(name2);
  manager.getOrCreate(name3);

  manager.clear();

  EXPECT_FALSE(manager.contains(name1));
  EXPECT_FALSE(manager.contains(name2));
  EXPECT_FALSE(manager.contains(name3));
}