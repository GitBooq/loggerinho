#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "formatters/IFormatter.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class MockFormatter : public formatter::IFormatter {
public:
  MOCK_METHOD(std::string, ParanthesisOp, (const LogRecord &), (const));
  std::string operator()(const LogRecord &record) const override {
    return ParanthesisOp(record);
  }
};