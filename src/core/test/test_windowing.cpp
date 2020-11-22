#include "gtest/gtest.h"
#include "src/core/test/gtest_extras.h"
#include "src/core/windowing.h"


using namespace musher::core;

/**
 * @brief Square windowing.
 * 
 */
TEST(Windowing, SquareWindowing) {
  std::vector<double> input(1024, 1.);
  unsigned int zero_padding_size = 0;
  bool zero_phase = false;

  std::vector<double> actual_window = Windowing(input, Square, zero_padding_size, zero_phase);

  std::vector<double> expected_window(Normalize(input));

  EXPECT_VEC_EQ(actual_window, expected_window)
}
