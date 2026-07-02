#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <sstream>

#include "mocks/mock_formatter.h"
#include "sinks/console_sink.h"

class ConsoleSinkTest : public ::testing::Test {
protected:
  void SetUp() override {
    originalCout_ = std::cout.rdbuf();
    originalCerr_ = std::cerr.rdbuf();

    std::cout.rdbuf(coutBuffer_.rdbuf());
    std::cerr.rdbuf(cerrBuffer_.rdbuf());

    mockFormatter_ = std::make_shared<NiceMock<MockFormatter>>();
  }

  void TearDown() override {
    std::cout.rdbuf(originalCout_);
    std::cerr.rdbuf(originalCerr_);
  }

  std::string getCoutOutput() const { return coutBuffer_.str(); }

  std::string getCerrOutput() const { return cerrBuffer_.str(); }

  void clearBuffers() {
    coutBuffer_.str("");
    coutBuffer_.clear();
    cerrBuffer_.str("");
    cerrBuffer_.clear();
  }

  std::streambuf *originalCout_;
  std::streambuf *originalCerr_;
  std::stringstream coutBuffer_;
  std::stringstream cerrBuffer_;
  std::shared_ptr<MockFormatter> mockFormatter_;
};

TEST_F(ConsoleSinkTest, WriteCallsFormatter) {
  ConsoleSink sink(mockFormatter_);

  LogRecord record;

  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .WillOnce(Return("Formatted message"));

  sink.write(record);
}

TEST_F(ConsoleSinkTest, WriteOutputsFormattedString) {
  ConsoleSink sink(mockFormatter_);

  LogRecord record;

  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .WillOnce(Return("[INFO] Test message"));

  sink.write(record);

  std::string output = getCoutOutput();
  EXPECT_EQ(output, "[INFO] Test message\n");
}