#include "gtest/gtest.h"
#include "src/core/spectrum.h"
#include "src/core/test/gtest_extras.h"

using namespace musher::core;

/**
 * @brief Basic conversion to frequency spectrum.
 * 
 */
TEST(Spectrum, FrequencySpectrum) {
  size_t inp_size = 100;
  // inp = [1, 1, 1, 1, 1, 1 ...]
  std::vector<double> inp(inp_size, 1.0);

  std::vector<double> actual_out;
  actual_out = ConvertToFrequencySpectrum(inp);

  std::vector<double> expected_out(inp_size / 2 + 1, 0.0);
  // out = [100, 0, 0, 0, 0 ...]
  expected_out[0] = inp_size;

  EXPECT_VEC_EQ(expected_out, actual_out);
}


/**
 * @brief Calculate magnitude of complex pair.
 * 
 */
TEST(Spectrum, Magnitude) {
  std::complex<double> complex_pair(3, -4);
  double expected_magnitude = 5;
  double actual_magnitude = Magnitude(complex_pair);

  EXPECT_DOUBLE_EQ(expected_magnitude, actual_magnitude);
}