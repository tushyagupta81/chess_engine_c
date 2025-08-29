#include "memory.h"
#include "pieces.h"
#include "tests.h"
#include "types.h"
#include "unity.h"

void test_array(void) {
  Array *array = new_array(ValuePiece);
  for (int i = 0; i < 64; i++) {
    append(array, (Pieces[]){WhitePawn});
  }
  TEST_ASSERT_EQUAL(array->curr_length, 64);
  TEST_ASSERT_EQUAL(array->max_length, 64);

  append(array, (Pieces[]){WhitePawn});

  TEST_ASSERT_EQUAL(array->curr_length, 65);
  TEST_ASSERT_EQUAL(array->max_length, 128);

  pop(array);

  TEST_ASSERT_EQUAL(array->curr_length, 64);
  TEST_ASSERT_EQUAL(array->max_length, 128);

  set_at(array, (Pieces[]){BlackPawn}, 0);
  TEST_ASSERT_EQUAL(((Pieces *)array->values)[0], BlackPawn);
  TEST_ASSERT_EQUAL(((Pieces *)array->values)[1], WhitePawn);
}
