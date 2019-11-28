#include <iostream>
#include <string>
#include <vector>
#include "gtest/gtest.h"

#include "utils.h"


TEST(TestUtils, Uint8_tVectorToHexString) {
	std::vector<uint8_t> actual = {1, 2, 3, 4, 5};
	std::string actualHex = uint8_vector_to_hex_string(actual);
	std::string expectedHex = "0102030405";
	EXPECT_EQ(actualHex, expectedHex);
}

TEST(TestUtils, GetStringBetweenSingleQuotes) {
	std::string stringInsideSQuotes = "Hello This is a 'Test'";
	std::string actual = get_str_between_two_squotes(stringInsideSQuotes);
	std::string expected = "Test";
	EXPECT_EQ(actual, expected);
}