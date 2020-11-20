#pragma once

#include <string>
#include <vector>

namespace musher {
namespace core {

/**
 * @brief Decoded audio file information that is common across all audio files.
 * 
 */
struct AudioDecoded {
  uint32_t sample_rate;     //!< Sampling rate of the audio signal \[Hz\].
  int channels;             //!< Number of audio channels in the buffer.
  bool mono;                //!< True is audio is mono.
  bool stereo;              //!< True if audio is stereo.
  int samples_per_channel;  //!< Number of samples per channel.
  double length_in_seconds; /*!< Detailed description after the member.
                                Based on the number of samples and the sample rate.*/
  std::string file_type;    //!< Type of the file decoded.
  int avg_bitrate_kbps;     //!< Average bitrate of the buffer \[kbps\]
  std::vector<std::vector<double>>
      normalized_samples; /*!< Normalized samples of the audio file.

                              normalized_samples[0] holds channel 1

                              normalized_samples[1] holds channel 2 (Will not exist if mono audio)
                              */
};

/**
 * @brief Decoded WAV file information.
 * 
 * Contains same attributes as AudioDecoded.
 *
 */
struct WavDecoded : AudioDecoded {
  int bit_depth;  //!< Bit depth of each sample.
};

/**
 * @brief Decoded Mp3 file information.
 * 
 * Contains same attributes as AudioDecoded.
 * 
 */
struct Mp3Decoded : AudioDecoded {};

/**
 * @brief Load the data from an audio file.
 *
 * @param file_path File path to a .wav file.
 * @return std::vector<uint8_t> Audio file data.
 */
std::vector<uint8_t> LoadAudioFile(const std::string& file_path);

/**
 * @brief Decode a wav file.
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
 * @brief Decode an mp3 file.
 *
 * @param file_path File path to a .mp3 file.
 * @return Mp3Decoded .mp3 file information.
 */
Mp3Decoded DecodeMp3(const std::string file_path);

}  // namespace core
}  // namespace musher
