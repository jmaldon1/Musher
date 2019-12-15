#ifndef __MUSHER_LIBRARY_H__
#define __MUSHER_LIBRARY_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>

#include "musher_core.h"
#include "utils.h"
#include "wavelib.h"

namespace musher
{
    void MUSHER_API CPrintFunctionalMessage(const char* message);

    std::vector<uint8_t> MUSHER_API CLoadAudioFile(const std::string& filePath);

    // bool MUSHER_API CAcceptDecode(const char* message, bool (*decodef)(const char*));

    template <typename AudioBufferType, template <typename ...> class Map, typename K, typename V>
    std::vector< std::vector<AudioBufferType> > MUSHER_API CDecodeWav(Map<K, V>& wavDecodedData, const std::vector<uint8_t>& fileData)
    {   
        std::vector< std::vector<AudioBufferType> > samples;

        if (!samples.empty()){
            std::string err_message = "Audio Buffer must be empty";
            throw std::runtime_error(err_message);
        }
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
        {
            dataChunkIndex = std::distance(fileData.begin(), dataChunkIt);
        }

        /* find format chunk in fileData */
        std::string formatChunkKey = "fmt";
        int formatChunkIndex = -1;
        auto formatChunkIt = std::search(fileData.begin(), fileData.end(), formatChunkKey.begin(), formatChunkKey.end());
        if(formatChunkIt != fileData.end())
        {
            formatChunkIndex = std::distance(fileData.begin(), formatChunkIt);
        }

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
                    /* normalize samples to between -1 and 1 */
                    AudioBufferType sample = normalizeInt8_t<AudioBufferType>(fileData[sampleIndex]);
                    samples[channel].push_back (sample);
                }
                else if (bitDepth == 16)
                {
                    int16_t sampleAsInt = twoBytesToInt(fileData, sampleIndex);
                    /* normalize samples to between -1 and 1 */
                    AudioBufferType sample = normalizeInt16_t<AudioBufferType>(sampleAsInt);
                    // samples[channel].push_back (sample);
                    samples[channel].push_back (sampleAsInt);
                }
                else if (bitDepth == 24)
                {
                    int32_t sampleAsInt = 0;
                    sampleAsInt = (fileData[sampleIndex + 2] << 16) | (fileData[sampleIndex + 1] << 8) | fileData[sampleIndex];
                    
                    if (sampleAsInt & 0x800000) // if the 24th bit is set, this is a negative number in 24-bit world
                        sampleAsInt = sampleAsInt | ~0xFFFFFF; // so make sure sign is extended to the 32 bit float

                    /* normalize samples to between -1 and 1 */
                    AudioBufferType sample = normalizeInt32_t<AudioBufferType>(sampleAsInt);
                    // AudioBufferType sample = (AudioBufferType)sampleAsInt / (AudioBufferType)8388608.;
                    // samples[channel].push_back (sample);
                    samples[channel].push_back (sampleAsInt);
                }
                else
                {
                    std::string err_message = "This file has a bit depth that is not 8, 16 or 24 bits, not sure how you got past the first error check.";
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
        std::string fileType = "wav";

        /* Add the decoded info into the unordered map */
        wavDecodedData["sample_rate"] = sampleRate;
        wavDecodedData["bit_depth"] = bitDepth;
        wavDecodedData["channels"] = numChannelsInt;
        wavDecodedData["mono"] = mono;
        wavDecodedData["stereo"] = stereo;
        wavDecodedData["samples_per_channel"] = numSamplesPerChannel;
        wavDecodedData["length_in_seconds"] = lengthInSeconds;
        wavDecodedData["filetype"] = fileType;

        return samples;
    }

    template< typename vecType,
            typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    double bpmDetection(std::vector< vecType > &flattenedNormalizedSamples, uint32_t sampleRate)
    {
        wave_object obj;
        wt_object wt;
        int J = 1;

        const int totalLevels = 4;
        const int maxDecimation = pow(2, (totalLevels - 1));

        double minNdx = 60. / 220 * (sampleRate / maxDecimation);
        double maxNdx = 60. / 40 * (sampleRate / maxDecimation);

        const char *name = "db4";
        obj = wave_init(name);// Initialize the wavelet

        size_t cDMinLen;
        double decimatedSignalSum, decimatedSignalMean;
        std::vector<vecType> cD, cDSum, cDFiltered, cDDecimatedSignal, cDMeanRemovedSignal, cDMeanRemovedSignalPartial;
        std::vector<vecType> cA, cAFiltered, cAMeanRemovedSignalPartial;
        for (int level = 0; level < totalLevels; level++)
        {
            /* Discrete Wavelet Transform */
            if (level == 0) {
                wt = wt_init(obj,(char*) "dwt", flattenedNormalizedSamples.size(), J); // Initialize the wavelet transform object
                setDWTExtension(wt, (char*) "sym");
                setWTConv(wt, (char*) "direct");

                dwt(wt, flattenedNormalizedSamples.data()); // Perform DWT

                cDMinLen = wt->length[1] / maxDecimation + 1;
                cDSum.resize(cDMinLen, 0.0);

                cDMeanRemovedSignalPartial.resize(cDMinLen);
            } else {
                wt = wt_init(obj,(char*) "dwt", cA.size(), J);// Initialize the wavelet transform object
                setDWTExtension(wt, (char*) "sym");
                setWTConv(wt, (char*) "direct");

                dwt(wt, cA.data()); // Perform DWT
            }

            /* Fill cA */
            cA.clear();
            for (int i = 0; i < wt->length[0]; ++i) {
                    cA.push_back(wt->output[i]);
            }

            /* Fill cD */
            for (int i = wt->length[1]; i < wt->outlength; ++i) {
                    cD.push_back(wt->output[i]);
            }

            /* Perform One Pole filter on cD */
            cDFiltered = onePoolFilter(cD);

            /* Decimate */
            int dc = pow(2, (totalLevels - level - 1));
            for (int ax = 0; ax < cDFiltered.size(); ax += dc)
            {
                cDDecimatedSignal.push_back(std::abs(cDFiltered[ax]));
            }

            decimatedSignalSum = std::accumulate(cDDecimatedSignal.begin(), cDDecimatedSignal.end(), 0.0);
            decimatedSignalMean =  decimatedSignalSum / static_cast<double>(cDDecimatedSignal.size());

            /* Remove the mean */
            auto removeMean = [decimatedSignalMean]( const vecType x ) { return x - decimatedSignalMean; };
            std::transform(
                cDDecimatedSignal.begin(),
                cDDecimatedSignal.end(),
                std::back_inserter(cDMeanRemovedSignal),
                removeMean );

            /* Reconstruct */
            std::copy_n ( cDMeanRemovedSignal.begin(), cDMinLen, cDMeanRemovedSignalPartial.begin() );
            /* Perform element-wise sum of 2 vectors and store into cDSum */
            std::transform ( 
                        cDSum.begin(),
                        cDSum.end(),
                        cDMeanRemovedSignalPartial.begin(),
                        cDSum.begin(),
                        std::plus<vecType>() );

            /* Clear variables */
            wt_free(wt);
            cD.clear();
            cDFiltered.clear();
            cDDecimatedSignal.clear();
            cDMeanRemovedSignal.clear();
            cDMeanRemovedSignalPartial.clear();
        }
        wave_free(obj);

        /* Check if cA has any useful data */
        bool zeros = std::all_of(cA.begin(), cA.end(), [](const vecType d) { return d == 0.0; });
        if (zeros)
            return 0.0;

        /* Filter cA */
        cAFiltered = onePoolFilter(cA);

        /* Make cAFiltered absolute */
        std::vector<vecType> cAAbsolute(cAFiltered.size());
        auto absoluteVal = []( const vecType x ) { return std::abs(x); };
        std::transform(
                cAFiltered.begin(),
                cAFiltered.end(),
                cAAbsolute.begin(),
                absoluteVal );
        
        double cAAbsoluteSum = std::accumulate(cAAbsolute.begin(), cAAbsolute.end(), 0.0);
        double cAAbsoluteMean =  cAAbsoluteSum / static_cast<double>(cAAbsolute.size());

        std::vector<vecType> cAMeanRemovedSignal(cAAbsolute.size());
        auto removeMean = [cAAbsoluteMean]( const vecType x ) { return x - cAAbsoluteMean; };
        std::transform(
                cAAbsolute.begin(),
                cAAbsolute.end(),
                cAMeanRemovedSignal.begin(),
                removeMean );

        cAMeanRemovedSignalPartial.resize(cDMinLen);
        std::copy_n ( cAMeanRemovedSignal.begin(), cDMinLen, cAMeanRemovedSignalPartial.begin() );
        /* Add elements of cDSum and cDMeanRemovedSignalPartial together and store into cDSum */
        std::transform ( 
                    cDSum.begin(),
                    cDSum.end(),
                    cAMeanRemovedSignalPartial.begin(),
                    cDSum.begin(),
                    std::plus<vecType>() );
        
        size_t dataLen = cDSum.size();
        std::vector<vecType> b(dataLen * 2);

        /* Fill a section of b with cDSum data */
        int g = 0;
        for (int i = dataLen / 2; i < (dataLen / 2) + dataLen; ++i){
            b[i] = cDSum[g];
            g += 1;
        }

        /* Reverse cDSum */
        std::vector<vecType> reversecDSum(cDSum);
        std::reverse(reversecDSum.begin(), reversecDSum.end());

        /* Perform an array flipped convolution, which is a correlation on the data.  */
        std::vector<vecType> correl = fftConvolve<vecType>(b, reversecDSum);
        correl.pop_back(); // We don't need the last element
        size_t midpoint = correl.size() / 2;
        std::vector<vecType> correlMidpointTmp(correl.begin() + midpoint, correl.end());
        std::vector<vecType> slicedCorrelMidpointTmp(correlMidpointTmp.begin() + std::floor(minNdx), correlMidpointTmp.begin() + std::floor(maxNdx));

        /* Simple Peak Detection */
        double peakIndex = peakDetect(slicedCorrelMidpointTmp);
        if (peakIndex == 0.0)
            return 0.0;
        double peakIndexAdjusted = peakIndex + minNdx;
        double bpm = 60. / peakIndexAdjusted * (sampleRate / maxDecimation);

        return bpm;
    }

    template< typename vecType,
            typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    double bpmsOverWindow(std::vector< vecType > &flattenedNormalizedSamples, size_t numSamples, uint32_t sampleRate, int windowSeconds)
    {
        int windowSamples = windowSeconds * sampleRate;
        int sampleIndex = 0;
        int maxWindowIndex = numSamples / windowSamples;
        // seconds_mid = numpy.zeros(max_window_ndx)
        std::vector<vecType> bpms(maxWindowIndex, 0.0);
        std::vector<vecType> secondsMid(maxWindowIndex, 0.0);

        // /* Fill vector from 0 to size of samples with increasing numbers */
        // std::vector<vecType> arangeSamples(flattenedNormalizedSamples.size());
        // std::iota (arangeSamples.begin(), arangeSamples.end(), 0);

        // std::vector<vecType> seconds(arangeSamples.size());
        // std::transform(
        //         arangeSamples.begin(),
        //         arangeSamples.end(),
        //         seconds.begin(),
        //         [&sampleRate]( const vecType x ) { return x / static_cast<double>(sampleRate); } );

        for (int windowIndex = 0; windowIndex < maxWindowIndex; windowIndex++)
        {
            typename std::vector<vecType>::iterator sampIt = flattenedNormalizedSamples.begin() + sampleIndex;
            std::vector<vecType> slicesSamples(sampIt, sampIt + windowSamples);

            // std::vector<vecType> slicedSeconds(sampIt, sampIt + windowSamples);
            // double secondsSum = std::accumulate(slicedSeconds.begin(), slicedSeconds.end(), 0.0);
            // double secondsMid =  cAAbsoluteSum / static_cast<double>(slicedSeconds.size());
            
            double bpm = bpmDetection(slicesSamples, sampleRate);
            bpms[windowIndex] = bpm;

            sampleIndex += windowSamples;
        }

        return std::round(median(bpms));
    }

    // bool CDecodeAudio(const char* message, bool (*decodef)(const char*))
    // {
    //     // *decodef("hello")
    //     std::cout << "Hello from Accept Decode!" << std::endl;
    //     return decodef(message);
    // }
}

#endif /* __MUSHER_LIBRARY_H__ */