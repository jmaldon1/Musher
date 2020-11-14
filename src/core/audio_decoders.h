#pragma once

#include <string>
#include <vector>

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
  std::vector<std::vector<double>> normalized_samples;
  std::vector<double> interleaved_normalized_samples;
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
  std::vector<std::vector<double>> normalized_samples;
  std::vector<double> interleaved_normalized_samples;
};

/**
 * @brief Load an the data from an audio file.
 *
 * @param file_path File path to a .wav file.
 * @return std::vector<uint8_t> Audio file data.
 */
std::vector<uint8_t> LoadAudioFile(const std::string& file_path);

/**
 * @brief Decode a wav file.
 * WavDecoded.normalized_samples contains:
 *  samples[0] holds channel 1
 *  samples[1] holds channel 2 (Will not exist if mono audio)
 *
 * @param file_data WAV file data.
 * @return WavDecoded .wav file information.
 */
WavDecoded DecodeWav(const std::vector<uint8_t>& file_data);

/**
 * @brief Overloaded wrapper around DecodeWav that accepts a file path to a .wav file.
 *
 * @param file_path File path to a .wav file.
 * @return WavDecoded .wav file information.
 */
WavDecoded DecodeWav(const std::string& file_path);

/**
 * @brief Decode am mp3 file.
 *
 * @param file_path File path to a .mp3 file.
 * @return Mp3Decoded .mp3 file information.
 */
Mp3Decoded CDecodeMp3(const std::string file_path);

}  // namespace core
}  // namespace musher
