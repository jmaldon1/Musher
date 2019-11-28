#ifndef __MUSHER_LIBRARY_H__
#define __MUSHER_LIBRARY_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "musher_core.h"


void MUSHER_API CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> MUSHER_API CLoadAudioFile(const std::string& filePath);
bool MUSHER_API CAcceptDecode(const char* message, bool (*decodef)(const char*));
// std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>> 
// template <class UnorderedMapIterator>
// void MUSHER_API CDecodeAudio(UnorderedMapIterator first, const std::string& filePath=std::string(), const std::vector<uint8_t>& fileData=std::vector<uint8_t>());

template MUSHER_API <class UnorderedMapIterator>
void MUSHER_API CDecodeWav(UnorderedMapIterator first, const std::vector<uint8_t>& fileData)
{
	// -----------------------------------------------------------
    // HEADER CHUNK
    std::string headerChunkID (fileData.begin(), fileData.begin() + 4);
    //int32_t fileSizeInBytes = fourBytesToInt (fileData, 4) + 8;
    std::string format (fileData.begin() + 8, fileData.begin() + 12);
    
    // -----------------------------------------------------------

    /* find data chunk in fileData */
    const std::string dataChunkKey = "data";
    int dataChunkIndex = -1;
	auto dataChunkIt = std::search(fileData.begin(), fileData.end(), dataChunkKey.begin(), dataChunkKey.end());
	if(dataChunkIt != fileData.end())
		dataChunkIndex = std::distance(fileData.begin(), dataChunkIt);

	/* find format chunk in fileData */
	std::string formatChunkKey = "fmt";
	int formatChunkIndex = -1;
	auto formatChunkIt = std::search(fileData.begin(), fileData.end(), formatChunkKey.begin(), formatChunkKey.end());
	if(formatChunkIt != fileData.end())
		formatChunkIndex = std::distance(fileData.begin(), formatChunkIt);

	// if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
    // then it is unlikely we'll able to read this file, so abort
    if (dataChunkIndex == -1 || formatChunkIndex == -1 || headerChunkID != "RIFF" || format != "WAVE")
    {
        std::string err_message = "This doesn't seem to be a valid .WAV file";
        throw std::runtime_error(err_message);
    }

    // -----------------------------------------------------------
    // FORMAT CHUNK
    int f = formatChunkIndex;
    std::string formatChunkID (fileData.begin() + f, fileData.begin() + f + 4);
    //int32_t formatChunkSize = fourBytesToInt (fileData, f + 4);
    int16_t audioFormat = twoBytesToInt (fileData, f + 8);
    int16_t numChannels = twoBytesToInt (fileData, f + 10);
    uint32_t sampleRate = (uint32_t) fourBytesToInt (fileData, f + 12);
    int32_t numBytesPerSecond = fourBytesToInt (fileData, f + 16);
    int16_t numBytesPerBlock = twoBytesToInt (fileData, f + 20);
    int bitDepth = (int) twoBytesToInt (fileData, f + 22);

}


#endif /* __MUSHER_LIBRARY_H__ */