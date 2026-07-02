#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "core/ITime_provider.h"

using ::testing::_;
using ::testing::NiceMock;
using ::testing::Return;

class MockTimeProvider : public ITimeProvider {
public:
  MOCK_METHOD(TimePoint, getCurrentTimepoint, (), (const, override));
};