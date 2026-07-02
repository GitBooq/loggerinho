#pragma once

#include "enrichers/IEnricher.h"
#include "enrichers/base_enricher.h"
#include "enrichers/enricher_flags.h"
#include "enrichers/timestamp_enricher.h"
#include <memory>

class EnricherFactory {
public:
  static std::shared_ptr<IEnricher>
  create(enricher::EnricherFlag flags = enricher::EnricherFlag::NONE) {
    std::shared_ptr<IEnricher> result = std::make_shared<BaseEnricher>();

    if (hasFlag(flags, enricher::EnricherFlag::TIMESTAMP)) {
      result = std::make_shared<TimestampEnricher>(result);
    }

    return result;
  }
};