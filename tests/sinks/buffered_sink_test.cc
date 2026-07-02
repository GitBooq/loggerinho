#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "mocks/mock_formatter.h"
#include "mocks/mock_log_sink.h"
#include "sinks/buffered_sink.h"

using ::testing::_;
using ::testing::An;
using ::testing::AtLeast;
using ::testing::Exactly;
using ::testing::InSequence;
using ::testing::Return;

class BufferedSinkTest : public ::testing::Test {
 protected:
  void SetUp() override {
    mockDownstream_ = std::make_unique<MockLogSink>();
    mockFormatter_ = std::make_shared<MockFormatter>();
  }

  std::unique_ptr<MockLogSink> mockDownstream_;
  std::shared_ptr<MockFormatter> mockFormatter_;
};

TEST_F(BufferedSinkTest, WriteAndFlushOnlyOnDestruction) {
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .Times(Exactly(2))
      .WillRepeatedly(Return("Formatted message"));

  // LogRecord doesn't have operator== which required by EXPECT_CALL
  // -> using ::testing::An<const LogRecord&>() matcher
  EXPECT_CALL(*mockDownstream_, write(An<const LogRecord&>())).Times(0);
  EXPECT_CALL(*mockDownstream_, write(An<const std::string&>())).Times(0);
  EXPECT_CALL(*mockDownstream_, flush()).Times(0);

  constexpr std::size_t kBatchSize = 3;
  auto rawDownstreamPtr = mockDownstream_.get();
  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), kBatchSize);

  LogRecord record;

  sink.write(record);  // to sink buff
  sink.write(record);  // same

  testing::Mock::VerifyAndClearExpectations(rawDownstreamPtr);

  EXPECT_CALL(*rawDownstreamPtr, write(An<const std::string&>()))
      .Times(Exactly(2))
      .WillRepeatedly(Return());
  EXPECT_CALL(*rawDownstreamPtr, flush()).Times(AtLeast(1));
}

TEST_F(BufferedSinkTest, ReachingBatchSizeTriggersFlush) {
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .Times(Exactly(3))
      .WillRepeatedly(Return("Message"));

  EXPECT_CALL(*mockDownstream_, write("Message")).Times(Exactly(3));
  EXPECT_CALL(*mockDownstream_, flush()).Times(Exactly(1));

  constexpr std::size_t kBatchSize = 3;
  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), kBatchSize);

  LogRecord record;

  sink.write(record);  // to sink buff
  sink.write(record);  // same
  sink.write(record);  // buff is full -> flush
}

TEST_F(BufferedSinkTest, WriteStringOverload) {
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).Times(0);

  EXPECT_CALL(*mockDownstream_, write("msg1")).Times(Exactly(1));
  EXPECT_CALL(*mockDownstream_, write("msg2")).Times(Exactly(1));
  EXPECT_CALL(*mockDownstream_, flush()).Times(Exactly(1));

  constexpr std::size_t kBatchSize = 2;
  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), kBatchSize);

  sink.write("msg1");  // to buff
  sink.write("msg2");  // flush
}

TEST_F(BufferedSinkTest, FlushFlushesBuffer) {
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .Times(Exactly(3))
      .WillRepeatedly(Return("Message"));

  EXPECT_CALL(*mockDownstream_, write("Message")).Times(Exactly(3));
  EXPECT_CALL(*mockDownstream_, flush()).Times(Exactly(1));

  constexpr std::size_t kBatchSize = 10;
  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), kBatchSize);

  LogRecord record;

  sink.write(record);  // to buff
  sink.write(record);  // same
  sink.write(record);  // same

  sink.flush();
}

TEST_F(BufferedSinkTest, FlushEmptyBuffer) {
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).Times(0);
  EXPECT_CALL(*mockDownstream_, write(An<const LogRecord&>())).Times(0);
  EXPECT_CALL(*mockDownstream_, write(std::string{})).Times(0);
  EXPECT_CALL(*mockDownstream_, flush()).Times(0);

  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), 1);

  sink.flush();
}

TEST_F(BufferedSinkTest, OpenCallsDownstreamOpen) {
  MockLogSink* rawDownstream = mockDownstream_.get();

  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), 1);

  EXPECT_CALL(*rawDownstream, open()).WillOnce(Return(true));

  EXPECT_TRUE(sink.open());

  EXPECT_CALL(*rawDownstream, open()).WillOnce(Return(false));

  EXPECT_FALSE(sink.open());
}

TEST_F(BufferedSinkTest, MultipleBatches) {
  constexpr std::size_t kBatchSize = 3;
  constexpr std::size_t kCallsAmt = 6;
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .Times(Exactly(kCallsAmt))
      .WillRepeatedly(Return("Message"));

  EXPECT_CALL(*mockDownstream_, write("Message")).Times(Exactly(kCallsAmt));

  EXPECT_CALL(*mockDownstream_, flush()).Times(Exactly(kCallsAmt / kBatchSize));

  BufferedSink sink(mockFormatter_, std::move(mockDownstream_), kBatchSize);

  LogRecord record;

  for (auto i = 0UL; i < (kCallsAmt); ++i) {
    sink.write(record);
  }
}