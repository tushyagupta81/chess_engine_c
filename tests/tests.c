#include "tests.h"
#include "unity.h"

void setUp() {}
void tearDown() {}

int main() {
  UNITY_BEGIN();
  RUN_TEST(test_array_1d);
  RUN_TEST(test_array_2d);
  RUN_TEST(test_board);
  RUN_TEST(test_moves);
  RUN_TEST(test_moves_notation);
  return UNITY_END();
}
