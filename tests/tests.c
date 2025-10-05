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
  RUN_TEST(test_enpassent);
  RUN_TEST(test_check);
  RUN_TEST(test_checkmate);
  RUN_TEST(test_alpha_beta);
  RUN_TEST(test_nn);
  return UNITY_END();
}
