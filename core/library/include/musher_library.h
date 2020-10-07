#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>

#include "utils.h"
#include "wavelib.h"
#include "minimp3_ex.h"

namespace musher {
    
struct WavDecoded {
    uint32_t sample_rate;
    int bit_depth;
    int channels;
    bool mono;
    bool stereo;
    int samples_per_channel;
    double length_in_seconds;
    std::string file_type;
    int avg_bitrate_kbps;
    std::vector<std::vector<double>> samples;
    std::vector<double> normalized_samples;
};

struct Mp3Decoded {
    uint32_t sample_rate;
    int channels;
    bool mono;
    bool stereo;
    int samples_per_channel;
    double length_in_seconds;
    std::string file_type;
    int avg_bitrate_kbps;
};


void CPrintFunctionalMessage(const char* message);

std::vector<uint8_t> CLoadAudioFile(const std::string& filePath);

WavDecoded CDecodeWavDualChannel(const std::vector<uint8_t>& file_data);
WavDecoded CDecodeWav(const std::vector<uint8_t>& file_data);
WavDecoded CDecodeWav(const std::string& file_path);

std::vector<double> CDecodeMp3(Mp3Decoded& mp3_decoded, const std::string file_path);


double bpmDetection(std::vector<double> &flattened_normalized_samples, uint32_t sample_rate);

double bpmsOverWindow(std::vector<double> &flattened_normalized_samples, size_t num_samples, uint32_t sample_rate, int windowSeconds);

// bool CDecodeAudio(const char* message, bool (*decodef)(const char*))
// {
//     // *decodef("hello")
//     std::cout << "Hello from Accept Decode!" << std::endl;
//     return decodef(message);
// }

}  // namespace musher
