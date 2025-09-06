#include "memory.h"
#include "pieces.h"
#include "tests.h"
#include "types.h"
#include "unity.h"

void test_array_1d(void) {
  Array *array = new_array(ValuePiece);
  for (int i = 0; i < 64; i++) {
    append(array, (Pieces[]){WhitePawn});
  }
  TEST_ASSERT_EQUAL(64, array->curr_length);
  TEST_ASSERT_EQUAL(64, array->max_length);

  append(array, (Pieces[]){WhitePawn});

  TEST_ASSERT_EQUAL(65, array->curr_length);
  TEST_ASSERT_EQUAL(128, array->max_length);

  pop(array);

  TEST_ASSERT_EQUAL(64, array->curr_length);
  TEST_ASSERT_EQUAL(128, array->max_length);

  set_at(array, (Pieces[]){BlackPawn}, 0);
  TEST_ASSERT_EQUAL(BlackPawn, ((Pieces *)array->values)[0]);
  TEST_ASSERT_EQUAL(WhitePawn, ((Pieces *)array->values)[1]);

  deinit_array(array);
}

void test_array_2d(void) {
  Array *array = new_array(ValueArray);
  for (int i = 0; i < 8; i++) {
    Array *row = new_array(ValuePiece);
    for (int j = 0; j < 8; j++) {
      append(row, (Pieces[]){WhitePawn});
    }
    append(array, &row);
  }
  TEST_ASSERT_EQUAL(array->curr_length, 8);
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      set_at_2d(array, (Pieces[]){WhitePawn}, i, j);
    }
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      TEST_ASSERT_EQUAL(WhitePawn, *(Pieces *)get_at_2d(array, i, j));
    }
  }
  set_at_2d(array, (Pieces[]){BlackPawn}, 0, 0);
  TEST_ASSERT_EQUAL(*(Pieces *)get_at_2d(array, 0, 0), BlackPawn);
  TEST_ASSERT_EQUAL(*(Pieces *)get_at_2d(array, 1, 0), WhitePawn);

  for (int i = 0; i < array->curr_length; i++) {
    deinit_array(*(Array **)get_at(array, i));
  }
  deinit_array(array);
}
