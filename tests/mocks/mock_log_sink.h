#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "sinks/ILog_sink.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class MockLogSink : public ILogSink {
public:
  MOCK_METHOD(bool, open, (), (const, override));
  MOCK_METHOD(void, write, (const LogRecord &record), (override));
  MOCK_METHOD(void, write, (const std::string &message), (override));
  MOCK_METHOD(void, flush, (), (override));
};