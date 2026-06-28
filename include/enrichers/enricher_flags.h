#pragma once

#include <cstdint>

namespace enricher {
using EnricherFlagType = std::uint8_t;

enum class EnricherFlag : EnricherFlagType { NONE = 0, TIMESTAMP = 1 << 0 };

inline EnricherFlag operator|(EnricherFlag first, EnricherFlag second) {
  return static_cast<EnricherFlag>(static_cast<EnricherFlagType>(first) |
                                   static_cast<EnricherFlagType>(second));
}

inline EnricherFlag operator&(EnricherFlag first, EnricherFlag second) {
  return static_cast<EnricherFlag>(static_cast<EnricherFlagType>(first) &
                                   static_cast<EnricherFlagType>(second));
}

inline bool hasFlag(EnricherFlag flags, EnricherFlag flag) {
  return (static_cast<EnricherFlagType>(flags) &
          static_cast<EnricherFlagType>(flag)) != 0;
}
} // namespace enricher