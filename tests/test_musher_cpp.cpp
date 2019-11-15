#include <iostream>
#include <string>
#include "gtest/gtest.h"

#include "functional_test.h"


// int main()
// {
//     std::cout << "================================================================" << std::endl;
//     std::cout << "This is the c++ wrapper to test the Musher library"               << std::endl;
//     std::cout << "================================================================" << std::endl;

//     CPrintFunctionalMessage("This is a message for a functional program");

//     return 0;
// }

// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(PrintFunctionalMessage, PrintsMessage) {
	CPrintFunctionalMessage("This is a message for a functional program");

	EXPECT_EQ(0, 0);
}