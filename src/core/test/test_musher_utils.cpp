#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "gtest/gtest.h"
#include "src/core/test/gtest_extras.h"
#include "src/core/test/utils.h"
#include "src/core/utils.h"

using namespace musher::core;
using namespace musher::core::test;

TEST(TestUtils, Uint8_tVectorToHexString) {
  std::vector<uint8_t> actual = { 1, 2, 3, 4, 5 };
  std::string actualHex = Uint8VectorToHexString(actual);
  std::string expectedHex = "0102030405";
  EXPECT_EQ(actualHex, expectedHex);
}

TEST(TestUtils, GetStringBetweenSingleQuotes) {
  std::string stringInsideSQuotes = "Hello This is a 'Test'";
  std::string actual = StrBetweenSQuotes(stringInsideSQuotes);
  std::string expected = "Test";
  EXPECT_EQ(actual, expected);
}

TEST(TestUtils, Deinterweave) {
  std::vector<double> interweaved_vec({ 1., 9., 2., 8., 3., 7., 4., 6. });
  std::vector<std::vector<double>> actual_deinterweaved_vectors = Deinterweave(interweaved_vec);

  std::vector<std::vector<double>> expected_deinterweaved_vectors({
      { 1., 2., 3., 4. },
      { 9., 8., 7., 6. },
  });

  EXPECT_MATRIX_EQ(expected_deinterweaved_vectors, actual_deinterweaved_vectors)
}
