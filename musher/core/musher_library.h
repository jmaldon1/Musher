#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "musher/core/utils.h"
#include "musher/third-party/minimp3/minimp3_ex.h"
#include "musher/third-party/wavelib/header/wavelib.h"

namespace musher {
namespace core {

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
  std::vector<std::vector<double>> Normalized_samples;
  std::vector<double> interleaved_Normalized_samples;
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
  std::vector<std::vector<double>> Normalized_samples;
  std::vector<double> interleaved_Normalized_samples;
};

void CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> CLoadAudioFile(const std::string& file_path);
WavDecoded CDecodeWav(const std::vector<uint8_t>& file_data);
WavDecoded CDecodeWav(const std::string& file_path);
std::vector<double> CDecodeMp3(Mp3Decoded& mp3_decoded, const std::string file_path);
double BPMDetection(std::vector<double>& samples, uint32_t sample_rate);
double BPMsOverWindow(std::vector<double>& samples, uint32_t sample_rate, unsigned int window_seconds = 3);

}  // namespace core
}  // namespace musher
