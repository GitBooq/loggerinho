#include <gtest/gtest.h>

#include "filters/NameFilter.h"

TEST(NameFilterTest, InAllowedList) {
  NameFilter filter({"allowed"});

  LogRecord recordAllowed;
  recordAllowed.loggerName = "allowed";

  EXPECT_TRUE(filter(recordAllowed)); // in allowed list -> true
}

TEST(NameFilterTest, NotInAllowedList) {
  NameFilter filter({"allowed"});

  LogRecord notAllowed;
  notAllowed.loggerName = "notAllowed";

  EXPECT_FALSE(filter(notAllowed)); // not in allowed list -> false
}

TEST(NameFilterTest, EmptyAllowedList) {
  NameFilter filter{/*empty*/};

  LogRecord record;
  record.loggerName = "someName";

  EXPECT_TRUE(filter(record)); // empty allowed list -> true
}