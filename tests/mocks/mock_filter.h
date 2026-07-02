#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "filters/IFilter.h"

class MockFilter : public IFilter {
public:
  MOCK_METHOD(bool, ParanthesisOp, (const LogRecord &), ());
  bool operator()(const LogRecord &record) override {
    return ParanthesisOp(record);
  }
};