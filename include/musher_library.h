#ifndef __MUSHER_LIBRARY_H__
#define __MUSHER_LIBRARY_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "musher_core.h"
#include "utils.h"


void MUSHER_API CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> MUSHER_API CLoadAudioFile(const std::string& filePath);
bool MUSHER_API CAcceptDecode(const char* message, bool (*decodef)(const char*));
// std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>> 
// template <class UnorderedMapIterator>
// void MUSHER_API CDecodeAudio(UnorderedMapIterator first, const std::string& filePath=std::string(), const std::vector<uint8_t>& fileData=std::vector<uint8_t>());

template MUSHER_API <class UnorderedMap, class AudioBufferType>
void MUSHER_API CDecodeWav(UnorderedMap& wavDecodedData, const std::vector<uint8_t>& fileData, std::vector< std::vector<AudioBufferType> > samples)
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

    int numBytesPerSample = bitDepth / 8;
    
    // check that the audio format is PCM
    if (audioFormat != 1)
    {
        std::string err_message = "This is a compressed .WAV file and this library does not support decoding them at present";
        throw std::runtime_error(err_message);
    }
    
    // check the number of channels is mono or stereo
    if (numChannels < 1 ||numChannels > 2)
    {
        std::string err_message = "This WAV file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?";
        throw std::runtime_error(err_message);
    }
    
    // check header data is consistent
    if ((numBytesPerSecond != (numChannels * sampleRate * bitDepth) / 8) || (numBytesPerBlock != (numChannels * numBytesPerSample)))
    {
        std::string err_message = "The header data in this WAV file seems to be inconsistent";
        throw std::runtime_error(err_message);
    }
    
    // check bit depth is either 8, 16 or 24 bit
    if (bitDepth != 8 && bitDepth != 16 && bitDepth != 24)
    {
        std::string err_message = "This file has a bit depth that is not 8, 16 or 24 bits";
        throw std::runtime_error(err_message);
    }

    // -----------------------------------------------------------
    // DATA CHUNK
    int d = dataChunkIndex;
    std::string dataChunkID (fileData.begin() + d, fileData.begin() + d + 4);
    int32_t dataChunkSize = fourBytesToInt (fileData, d + 4);
    
    int numSamples = dataChunkSize / (numChannels * bitDepth / 8);
    int samplesStartIndex = dataChunkIndex + 8;

    samples.resize (numChannels);

    for (int i = 0; i < numSamples; i++)
    {
        for (int channel = 0; channel < numChannels; channel++)
        {
            int sampleIndex = samplesStartIndex + (numBytesPerBlock * i) + channel * numBytesPerSample;
            
            if (bitDepth == 8)
            {
                AudioBufferType sample = singleByteToSample<AudioBufferType>(fileData[sampleIndex]);
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 16)
            {
                int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
                AudioBufferType sample = sixteenBitIntToSample<AudioBufferType>(sampleAsInt);
                samples[channel].push_back (sample);
            }
            else if (bitDepth == 24)
            {
                int32_t sampleAsInt = 0;
                sampleAsInt = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex];
                
                if (sampleAsInt & 0x800000) //  if the 24th bit is set, this is a negative number in 24-bit world
                    sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

                AudioBufferType sample = (AudioBufferType)sampleAsInt / (AudioBufferType)8388608.;
                samples[channel].push_back (sample);
            }
            else
            {
                std::string err_message = "This file has a bit depth that is not 8, 16 or 24 bits";
        		throw std::runtime_error(err_message);
            }
        }
    }

    int numChannelsInt = static_cast<int>(numChannels);
    bool mono = numChannels == 1;
    bool stereo = numChannels == 2;
    int numSamplesPerChannel = 0;
    if (samples.size() > 0)
        numSamplesPerChannel = static_cast<int>(samples[0].size());
    double lengthInSeconds = static_cast<double>(numSamplesPerChannel) / static_cast<double>(sampleRate);
    wavDecodedData["sample_rate"] = sampleRate;
    wavDecodedData["bit_depth"] = bitDepth;
    wavDecodedData["channels"] = numChannelsInt;
    wavDecodedData["mono"] = mono;
    wavDecodedData["stereo"] = stereo;
    wavDecodedData["samples_per_channel"] = numSamplesPerChannel;
    wavDecodedData["length_in_seconds"] = lengthInSeconds;
}


#endif /* __MUSHER_LIBRARY_H__ */