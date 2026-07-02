#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>

#include "mocks/mock_formatter.h"
#include "sinks/file_sink.h"

namespace fs = std::filesystem;

class FileSinkTest : public ::testing::Test {
protected:
  void SetUp() override {
    testDir_ = fs::temp_directory_path() / "file_sink_test";
    fs::create_directories(testDir_);

    testFile_ = testDir_ / "test.log";

    mockFormatter_ = std::make_shared<NiceMock<MockFormatter>>();

    ON_CALL(*mockFormatter_, ParanthesisOp(_))
        .WillByDefault(Return("Test message"));
  }

  void TearDown() override { fs::remove_all(testDir_); }

  static std::string readFile(const fs::path &path) {
    std::ifstream file(path);
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    return content;
  }

  static bool fileExists(const fs::path &path) {
    return fs::exists(path) && fs::is_regular_file(path);
  }

  static size_t getFileSize(const fs::path &path) {
    return fs::exists(path) ? fs::file_size(path) : 0;
  }

  fs::path testDir_;
  fs::path testFile_;
  std::shared_ptr<MockFormatter> mockFormatter_;
};

TEST_F(FileSinkTest, WritesToFile) {
  FileSink sink(mockFormatter_, testFile_, 1024);

  std::string msg{"TEST!"};
  std::string expectedMsg = msg + "\n";
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).WillOnce(Return(msg));

  sink.write(LogRecord{});
  sink.flush();

  EXPECT_TRUE(fileExists(testFile_));
  EXPECT_EQ(getFileSize(testFile_), expectedMsg.size());
  EXPECT_EQ(readFile(testFile_), expectedMsg);
}

TEST_F(FileSinkTest, OpenReturnsTrueWhenFileIsOpen) {
  FileSink sink(mockFormatter_, testFile_, 1024);

  sink.write(LogRecord{});

  EXPECT_TRUE(sink.open());
}

TEST_F(FileSinkTest, FlushNoThrow) {
  FileSink sink(mockFormatter_, testFile_, 1024);

  sink.write(LogRecord{});

  EXPECT_NO_THROW(sink.flush());
}

TEST_F(FileSinkTest, RotatesWhenFileExceedsLimit) {
  constexpr std::size_t kMaxFileSize = 20;
  FileSink sink(mockFormatter_, testFile_, kMaxFileSize);

  std::string firstMsg{"test"};
  firstMsg.resize(kMaxFileSize);
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).WillOnce(Return(firstMsg));

  sink.write(LogRecord{});
  sink.flush();

  EXPECT_EQ(getFileSize(testFile_), firstMsg.size() + 1);
  EXPECT_FALSE(fileExists(testFile_.generic_string() + ".1"));
  EXPECT_FALSE(fileExists(testFile_.generic_string() + ".2"));
  EXPECT_FALSE(fileExists(testFile_.generic_string() + ".3"));

  std::string secondMsg{"test"};
  secondMsg.resize(kMaxFileSize);

  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).WillOnce(Return(secondMsg));

  sink.write(LogRecord{});
  sink.flush();

  EXPECT_TRUE(fileExists(testFile_));
  EXPECT_TRUE(fileExists(testFile_.generic_string() + ".1"));
  EXPECT_FALSE(fileExists(testFile_.generic_string() + ".2"));
  EXPECT_FALSE(fileExists(testFile_.generic_string() + ".3"));
}

TEST_F(FileSinkTest, KeepsOnlyMaxArchives) {
  constexpr std::size_t kMaxFileSize = 1;
  FileSink sink(mockFormatter_, testFile_, kMaxFileSize);

  constexpr std::size_t maxArchives = 3;
  for (auto i = 0UL; i < (maxArchives * 2); ++i) {
    EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).WillOnce(Return("x"));
    sink.write(LogRecord{});
  }
  sink.flush();

  int archiveCount = 0;
  for (auto i = 1UL; i <= (maxArchives * 2); ++i) {
    fs::path archive = testFile_.generic_string() + "." + std::to_string(i);
    if (fileExists(archive)) {
      ++archiveCount;
    }
  }

  EXPECT_EQ(archiveCount, 3);
}

TEST_F(FileSinkTest, LargeMessage) {
  constexpr std::size_t kMaxFileSize = 150;
  FileSink sink(mockFormatter_, testFile_, kMaxFileSize);

  std::string largeMessage(kMaxFileSize * 2, 'a');
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_)).WillOnce(Return(largeMessage));

  sink.write(LogRecord{});
  sink.flush();

  EXPECT_TRUE(fileExists(testFile_));
  EXPECT_GE(getFileSize(testFile_), kMaxFileSize);
}

TEST_F(FileSinkTest, MultipleWritesToSameFile) {
  FileSink sink(mockFormatter_, testFile_, 1024);

  const std::string first{"first"};
  const std::string second{"second"};
  const std::string third{"third"};
  EXPECT_CALL(*mockFormatter_, ParanthesisOp(_))
      .WillOnce(Return(first))
      .WillOnce(Return(second))
      .WillOnce(Return(third));

  sink.write(LogRecord{});
  sink.write(LogRecord{});
  sink.write(LogRecord{});
  sink.flush();

  std::string content = readFile(testFile_);
  EXPECT_TRUE(content.find(first + "\n") != std::string::npos);
  EXPECT_TRUE(content.find(second + "\n") != std::string::npos);
  EXPECT_TRUE(content.find(third + "\n") != std::string::npos);
}