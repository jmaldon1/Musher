#include <iostream>
#include <string>

#include "song.h"
#include "functional_test.h"


int main()
{
    std::cout << "================================================================" << std::endl;
    std::cout << "This is the c++ wrapper to test the Song shared library"          << std::endl;
    std::cout << "================================================================" << std::endl;

    Song first_song("My First Song", 4.5);
    Song second_song("My Second Song", 2.3);

    std::cout << first_song.GetName() << " " << first_song.GetLength() << std::endl;
    std::cout << second_song.GetName() << " " << second_song.GetLength() << std::endl;

    first_song.PrintMessage();
    second_song.PrintMessage();

    PrintFunctionalMessage("This is a message for a functional program");

    return 0;
}