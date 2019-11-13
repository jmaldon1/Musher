#include <iostream>
#include <string>

#include "functional_test.h"

int main()
{
    std::cout << "================================================================" << std::endl;
    std::cout << "This is the c++ wrapper to test the Musher library"               << std::endl;
    std::cout << "================================================================" << std::endl;

    PrintFunctionalMessage("This is a message for a functional program");

    return 0;
}