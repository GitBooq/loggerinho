#include <gtest/gtest.h>

#include "core/enricher_factory.h"
#include "enrichers/base_enricher.h"
#include "enrichers/timestamp_enricher.h"

TEST(EnricherFactoryTest, NoneFlagCreatesBaseEnricher) {
  auto noneFlagEnricher = EnricherFactory::create(enricher::EnricherFlag::NONE);

  auto *base = dynamic_cast<BaseEnricher *>(noneFlagEnricher.get());
  EXPECT_NE(base, nullptr);

  auto *timestamp = dynamic_cast<TimestampEnricher *>(noneFlagEnricher.get());
  EXPECT_EQ(timestamp, nullptr);
}

TEST(EnricherFactoryTest, NoFlagsCreatesBaseEnricher) {
  auto emptyFlagsEnricher = EnricherFactory::create(/*EnricherFlag::NONE*/);

  auto *base = dynamic_cast<BaseEnricher *>(emptyFlagsEnricher.get());
  EXPECT_NE(base, nullptr);

  auto *timestamp = dynamic_cast<TimestampEnricher *>(emptyFlagsEnricher.get());
  EXPECT_EQ(timestamp, nullptr);
}

TEST(EnricherFactoryTest, TimestampFlagCreatesTimestampEnricher) {
  auto enricher = EnricherFactory::create(enricher::EnricherFlag::TIMESTAMP);

  auto *timestamp = dynamic_cast<TimestampEnricher *>(enricher.get());
  EXPECT_NE(timestamp, nullptr);
}

TEST(EnricherFactoryTest, TimestampFlagAddsTimestamp) {
  auto before = std::chrono::system_clock::now();

  auto enricher = EnricherFactory::create(enricher::EnricherFlag::TIMESTAMP);
  LogRecord record;
  auto result = enricher->enrich(record);

  auto after = std::chrono::system_clock::now();

  EXPECT_GE(result.timestamp, before);
  EXPECT_LE(result.timestamp, after);
}
