#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "enrichers/IEnricher.h"

class MockEnricher : public IEnricher {
public:
  MOCK_METHOD(LogRecord, enrich, (LogRecord &), (override));
};
