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
  std::vector<uint8_t> actual = {1, 2, 3, 4, 5};
  std::string actualHex = uint8_vector_to_hex_string(actual);
  std::string expectedHex = "0102030405";
  EXPECT_EQ(actualHex, expectedHex);
}

TEST(TestUtils, GetStringBetweenSingleQuotes) {
  std::string stringInsideSQuotes = "Hello This is a 'Test'";
  std::string actual = strBetweenSQuotes(stringInsideSQuotes);
  std::string expected = "Test";
  EXPECT_EQ(actual, expected);
}

// TEST(TestUtils, getItemFromVariant) {
// 	double d = 0.01;
// 	std::variant<int, uint32_t, double, bool, std::string> varD = d;

// 	double DFromVariant = variantToType<double>(varD);

// 	EXPECT_EQ(d, DFromVariant);

// 	std::string s = "musher";
// 	std::variant<int, double, std::string> varS = s;
// 	std::string sFromVariant = variantToType<std::string>(varS);

// 	EXPECT_EQ(s, sFromVariant);
// }

// TEST(TestUtils, failedGetItemFromVariant) {
// 	/* Cannot put commas in an EXPECT_THROW, so we just initialize variant out here */
// 	std::variant<int, double, std::string> varF = 1;

// 	EXPECT_THROW({
//         try
//         {
//         	/* Try getting a type (float) that does not exist in the variant */
// 			float test = variantToType<float>(varF);
//         }
//         catch( const std::runtime_error& e )
//         {
//             /* This tests if the error message is equal */
//             EXPECT_STREQ( "The template type was not found within the variant." , e.what() );
//             throw;
//         }
//     }, std::runtime_error );
// }

TEST(TestUtils, Magnitude) {
  std::complex<double> complex_pair(3, -4);
  double expected_magnitude = 5;
  double actual_magnitude = magnitude(complex_pair);

  EXPECT_FLOAT_EQ(expected_magnitude, actual_magnitude);
}

TEST(TestUtils, FrequencySpectrumTest) {
  size_t inp_size = 100;
  std::vector<double> expected_out(inp_size / 2 + 1, 0.0);
  expected_out[0] = inp_size;
  // out = [100, 0, 0, 0, 0 ...]

  std::vector<double> inp(inp_size, 1.0);
  // inp = [1, 1, 1, 1, 1, 1 ...]
  std::vector<double> actual_out;
  actual_out = convertToFrequencySpectrum(inp);

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
  auto [actual_peak_location, actual_peak_height_estimate] =
      quadraticInterpolation(left_point_val, middle_point_val, right_point_val, middle_point_index);

  EXPECT_NEAR(expected_peak_location, actual_peak_location, 0.00001);
  EXPECT_NEAR(expected_peak_height_estimate, actual_peak_height_estimate, 0.00001);
}
