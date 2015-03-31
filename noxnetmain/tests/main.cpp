#include <unittest++/UnitTest++.h>

#include "shared/common.h"

TEST(test_unittest){
  CHECK(test_unittest_add(2,3) == 5);
  CHECK(test_unittest_add(2,5) == 6);
}

int main()
{
  return UnitTest::RunAllTests();
}

