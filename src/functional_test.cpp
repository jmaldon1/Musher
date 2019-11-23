#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>

#include "functional_test.h"

void CPrintFunctionalMessage(const char* message)
{
    std::cout << message << std::endl;
}

bool LoadAudioFile(const char* filePath)
{
	std::cout << std::filesystem::exists(filePath) << std::endl;
	std::ifstream audioFile (filePath, std::ios::binary);



	std::cout << filePath << std::endl;
	return true;
}

char* getFileAbsPath(const char* filePath)
{
	std::filesystem::exists(filePath);
	std::filesystem::path p = std::filesystem::canonical(filePath);

	std::cout << p << std::endl;
	return "hello";
}