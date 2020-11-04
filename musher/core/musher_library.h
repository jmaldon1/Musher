#pragma once

#include <algorithm>
#include <string>
#include <unordered_map>
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

void CPrintFunctionalMessage(const char* message);

/**
 * @brief Load an the data from an audio file.
 * 
 * @param file_path File path to a .wav file.
 * @return std::vector<uint8_t> Audio file data.
 */
std::vector<uint8_t> CLoadAudioFile(const std::string& file_path);

/**
 * @brief Decode a wav file.
 * WavDecoded.normalized_samples contains:
 *  samples[0] holds channel 1
 *  samples[1] holds channel 2 (Will not exist if mono audio)
 * 
 * @param file_data WAV file data.
 * @return WavDecoded .wav file information.
 */
WavDecoded CDecodeWav(const std::vector<uint8_t>& file_data);

/**
 * @brief Overloaded wrapper around CDecodeWav that accepts a file path to a .wav file.
 * 
 * @param file_path File path to a .wav file.
 * @return WavDecoded .wav file information.
 */
WavDecoded CDecodeWav(const std::string& file_path);

/**
 * @brief Decode am mp3 file.
 * 
 * @param file_path File path to a .mp3 file.
 * @return Mp3Decoded .mp3 file information.
 */
Mp3Decoded CDecodeMp3(const std::string file_path);

/**
 * @brief Calculate the BPM (Beats per minute) of audio samples.
 *
 * @param samples Input audio signal.
 * @param sample_rate Sampling rate of the audio signal [Hz].
 * @return double BPM
 */
double BPMDetection(std::vector<double>& samples, uint32_t sample_rate);

/**
 * @brief Calculate the average BPM (Beats per minute) of samples.
 * This function will slice the samples into windows and calculate the BPM over each
 * window and then average them to achieve a final BPM over all samples.
 *
 * @param samples Input audio samples.
 * @param sample_rate Sampling rate of the audio signal [Hz].
 * @param window_seconds Size of the the window [Seconds] that will be scanned to determine the bpm,
 * typically less than 10 seconds.
 * @return double Average BPM over windows.
 */
double BPMOverWindow(std::vector<double>& samples, uint32_t sample_rate, unsigned int window_seconds = 3);

}  // namespace core
}  // namespace musher
