#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stdexcept>

#include "musher_library.h"
#include "utils.h"


void CPrintFunctionalMessage(const char* message)
{
    std::cout << message << std::endl;
}

std::vector<uint8_t> CLoadAudioFile(const std::string filePath)
{
	std::error_code e;
	std::filesystem::path audioFileAbsPath = std::filesystem::canonical(filePath, e);
	if( e.value() != 0 ) {
		// ERROR
		std::filesystem::path absolutePath = std::filesystem::absolute(filePath);
		std::string eMessage = "No file found at ";
		eMessage += absolutePath.string();
		throw std::runtime_error(eMessage);
	}
	std::ifstream audioFile (audioFileAbsPath, std::ios::binary);

	/* skip white space */
	audioFile.unsetf (std::ios::skipws);
    std::istream_iterator<uint8_t> begin (audioFile), end;

    std::vector<uint8_t> fileData (begin, end);
	return fileData;
}


bool CDecodeWav(const char* message)
{
	std::cout << message << std::endl;
	return true;
}


bool CAcceptDecode(const char* message, bool (*decodef)(const char*))
{
	// *decodef("hello")
	std::cout << "Hello from Accept Decode!" << std::endl;
	return decodef(message);
}
