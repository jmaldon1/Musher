#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

#include "gtest/gtest.h"
#include "musher/core/test/gtest_extras.h"
#include "musher/core/utils.h"

using namespace musher::core;

TEST(TestUtils, Uint8_tVectorToHexString) {
  std::vector<uint8_t> actual = { 1, 2, 3, 4, 5 };
  std::string actualHex = uint8VectorToHexString(actual);
  std::string expectedHex = "0102030405";
  EXPECT_EQ(actualHex, expectedHex);
}

TEST(TestUtils, GetStringBetweenSingleQuotes) {
  std::string stringInsideSQuotes = "Hello This is a 'Test'";
  std::string actual = strBetweenSQuotes(stringInsideSQuotes);
  std::string expected = "Test";
  EXPECT_EQ(actual, expected);
}

TEST(TestUtils, Magnitude) {
  std::complex<double> complex_pair(3, -4);
  double expected_magnitude = 5;
  double actual_magnitude = Magnitude(complex_pair);

  EXPECT_DOUBLE_EQ(expected_magnitude, actual_magnitude);
}

TEST(TestUtils, FrequencySpectrumTest) {
  size_t inp_size = 100;
  // inp = [1, 1, 1, 1, 1, 1 ...]
  std::vector<double> inp(inp_size, 1.0);

  std::vector<double> actual_out;
  actual_out = convertToFrequencySpectrum(inp);

  std::vector<double> expected_out(inp_size / 2 + 1, 0.0);
  // out = [100, 0, 0, 0, 0 ...]
  expected_out[0] = inp_size;

  EXPECT_VEC_EQ(expected_out, actual_out);
}

TEST(TestUtils, QuadraticInterpolationTest) {
  /*
      This test was based off the image on this page:

      https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html
  */
  double left_point_val = 2;
  double middle_point_val = 4;
  double right_point_val = 3;
  int middle_point_index = 0;

  double expected_peak_location = 0.166667;        // Just over 0
  double expected_peak_height_estimate = 4.04167;  // Just over 4

  double actual_peak_location;
  double actual_peak_height_estimate;
  std::tie(actual_peak_location, actual_peak_height_estimate) =
      quadraticInterpolation(left_point_val, middle_point_val, right_point_val, middle_point_index);

  EXPECT_NEAR(expected_peak_location, actual_peak_location, 0.00001);
  EXPECT_NEAR(expected_peak_height_estimate, actual_peak_height_estimate, 0.00001);
}
