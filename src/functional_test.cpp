#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stdexcept>

#include "functional_test.h"

void CPrintFunctionalMessage(const char* message)
{
    std::cout << message << std::endl;
}

std::vector<uint8_t> LoadAudioFile(const char* filePath, bool (*decodeFunc)(const char*))
{
	std::error_code e;
	std::filesystem::path audioFileAbsPath = std::filesystem::canonical(filePath, e);
	if( e.value() != 0 ) {
		// ERROR
		std::filesystem::path absolutePath = std::filesystem::absolute(filePath);
		std::string eMessage = "No file found at " + std::string(absolutePath);
		throw std::runtime_error(eMessage);
	}
	std::ifstream audioFile (audioFileAbsPath, std::ios::binary);

	/* skip white space */
	audioFile.unsetf (std::ios::skipws);
    std::istream_iterator<uint8_t> begin (audioFile), end;

    std::vector<uint8_t> fileData (begin, end);

    if(decodeFunc) {
    	/* TODO run decode function and return decoded file info */
    	return fileData; /* this is just here to prevent warnings, remove later */
    }else{
    	return fileData;
    }
}

bool DecodeWav(const char* message)
{
	std::cout << message << std::endl;
	return true;
}

bool AcceptDecode(const char* message, bool (*decodef)(const char*))
{
	// *decodef("hello")
	std::cout << "Hello from Accept Decode!" << std::endl;
	return decodef(message);
}
