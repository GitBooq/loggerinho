#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "enrichers/base_enricher.h"
#include "enrichers/timestamp_enricher.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class MockTimeProvider : public ITimeProvider {
public:
  MOCK_METHOD(TimePoint, getCurrentTimepoint, (), (const, override));
};

class TimestampEnricherTest : public ::testing::Test {
protected:
  void SetUp() override {
    mockTimeProvider_ = std::make_shared<NiceMock<MockTimeProvider>>();
    baseEnricher_ = std::make_shared<BaseEnricher>();
    enricher_ =
        std::make_shared<TimestampEnricher>(baseEnricher_, mockTimeProvider_);
  }

  std::shared_ptr<MockTimeProvider> mockTimeProvider_;
  std::shared_ptr<BaseEnricher> baseEnricher_;
  std::shared_ptr<TimestampEnricher> enricher_;
};

TEST_F(TimestampEnricherTest, AddsSpecificTimestamp) {
  auto expectedTime = std::chrono::system_clock::now();

  EXPECT_CALL(*mockTimeProvider_, getCurrentTimepoint())
      .WillOnce(Return(expectedTime));

  LogRecord record;
  auto result = enricher_->enrich(record);

  EXPECT_EQ(result.timestamp, expectedTime);
}

TEST_F(TimestampEnricherTest, DifferentTimestamps) {
  auto time1 = std::chrono::system_clock::now();
  auto time2 = time1 + std::chrono::milliseconds(100);

  EXPECT_CALL(*mockTimeProvider_, getCurrentTimepoint())
      .WillOnce(Return(time1))
      .WillOnce(Return(time2));

  LogRecord record;
  auto result1 = enricher_->enrich(record);
  auto result2 = enricher_->enrich(record);

  EXPECT_EQ(result1.timestamp, time1);
  EXPECT_EQ(result2.timestamp, time2);
}