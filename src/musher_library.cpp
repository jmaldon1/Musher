#include <iostream>
#include <string>
#include <iterator>
#include <fstream>
#include <filesystem>
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <variant>

#include "musher_library.h"
#include "utils.h"


// std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>> MUSHER_API um;
/*
bitDept: int
numChannels: int
numSamplesPerChannel: int
sampleRate: uint32_t
lengthInSeconds: double
isMono: bool
isStereo: bool
*/

void CPrintFunctionalMessage(const char* message)
{
    std::cout << message << std::endl;
}


std::vector<uint8_t> CLoadAudioFile(const std::string& filePath)
{
	std::error_code e;
	std::filesystem::path audioFileAbsPath = std::filesystem::canonical(filePath, e);
	if( e.value() != 0 ) {
		// ERROR
		std::filesystem::path absolutePath = std::filesystem::absolute(filePath);
		std::string eMessage = "No file found at ";
		eMessage += "'";
		eMessage += absolutePath.string();
		eMessage += "'";
		throw std::runtime_error(eMessage);
	}
	std::ifstream audioFile (audioFileAbsPath, std::ios::binary);

	/* skip white space */
	audioFile.unsetf (std::ios::skipws);
    std::istream_iterator<uint8_t> begin (audioFile), end;

    std::vector<uint8_t> fileData (begin, end);
	return fileData;
}

// std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>>
template <class UnorderedMapIterator>
void CDecodeAudio(UnorderedMapIterator first, const std::string& filePath, const std::vector<uint8_t>& fileData){
	if(!filePath.empty()){
		std::vector<uint8_t> fileData;
		fileData = CLoadAudioFile(filePath);
	}
}


template <class UnorderedMapIterator>
void CDecodeWav(UnorderedMapIterator first, const std::vector<uint8_t>& fileData)
{
	// typename std::iterator_traits<iterator>::value_type temp = *first;
	// auto t = *first;

	// // -----------------------------------------------------------
 //    // HEADER CHUNK
 //    std::string headerChunkID (fileData.begin(), fileData.begin() + 4);
 //    //int32_t fileSizeInBytes = fourBytesToInt (fileData, 4) + 8;
 //    std::string format (fileData.begin() + 8, fileData.begin() + 12);
    
 //    // -----------------------------------------------------------
 //    // try and find the start points of key chunks
 //    // [](const uint8_t& val){ 
 //    // 	if (val % 3 == 0) 
 //    // 		return true; 
 //    // 	return false; 
 //    // }
 //    // std::vector<int>::iterator it2 = std::find_if(vecOfNums.begin(), vecOfNums.end());
 //    const char*keyChunkData= "data";
	// auto it = std::search(fileData.begin(), fileData.end(), keyChunkData, keyChunkData + strlen(keyChunkData));
	// int indexOfDataChunk = std::distance(fileData.begin(), it);
	// std::cout << indexOfDataChunk << std::endl;

    // int indexOfDataChunk = getIndexOfString (fileData, "data");
    // int indexOfFormatChunk = getIndexOfString (fileData, "fmt");
    
    // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    // if (indexOfDataChunk == -1 || indexOfFormatChunk == -1 || headerChunkID != "RIFF" || format != "WAVE")
    // {
    //     // std::cout << "ERROR: this doesn't seem to be a valid .WAV file" << std::endl;
    //     // return false;
    //     throw std::runtime_error("this doesn't seem to be a valid .WAV file");
    // }
}


bool CAcceptDecode(const char* message, bool (*decodef)(const char*))
{
	// *decodef("hello")
	std::cout << "Hello from Accept Decode!" << std::endl;
	return decodef(message);
}
