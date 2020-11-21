#include <string>

#include "gtest/gtest.h"
#include "src/core/audio_decoders.h"
#include "src/core/utils.h"

using namespace musher::core;

/**
 * @brief File not found error.
 *
 */
TEST(AudioFileDecoding, AudioFileNotFound) {
  /* This tests that the expected exception is thrown */
  EXPECT_THROW(
      {
        try {
          LoadAudioFile("/unknown/abs/file/path.wav");
        } catch (const std::runtime_error& e) {
          /* This tests if the error message is equal */
          EXPECT_STREQ("Failed to load file '/unknown/abs/file/path.wav'", e.what());
          throw;
        }
      },
      std::runtime_error);
}

/**
 * @brief Basic load audio file.
 *
 */
TEST(AudioFileDecoding, LoadAudioFile) {
  const std::string file_path = TEST_DATA_DIR + std::string("audio_files/impulses_1second_44100.wav");
  std::vector<uint8_t> file_data = LoadAudioFile(file_path);

  size_t actual_file_data_size = file_data.size();
  double actual_file_data_sum = std::accumulate(file_data.begin(), file_data.end(), 0.);

  size_t expected_file_data_size = 882044;
  double expected_file_data_sum = 5774.0;

  EXPECT_EQ(actual_file_data_size, expected_file_data_size);
  EXPECT_DOUBLE_EQ(actual_file_data_sum, expected_file_data_sum);
}

/**
 * @brief Decode WAV from data.
 *
 */
TEST(AudioFileDecoding, DecodeWavFromData) {
  const std::string file_path = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.wav");
  std::vector<uint8_t> file_data = LoadAudioFile(file_path);
  WavDecoded wav_decoded = DecodeWav(file_data);

  uint32_t expected_sample_rate = 44100;
  uint32_t actual_sample_rate = wav_decoded.sample_rate;
  EXPECT_EQ(expected_sample_rate, actual_sample_rate);

  int expected_bit_depth = 16;
  int actual_bit_depth = wav_decoded.bit_depth;
  EXPECT_EQ(expected_bit_depth, actual_bit_depth);

  int expectedChannels = 2;
  int actual_channels = wav_decoded.channels;
  EXPECT_EQ(expectedChannels, actual_channels);

  bool expected_mono = false;
  bool actual_mono = wav_decoded.mono;
  EXPECT_EQ(expected_mono, actual_mono);

  bool expected_stereo = true;
  bool actual_stereo = wav_decoded.stereo;
  EXPECT_EQ(expected_stereo, actual_stereo);

  double expected_length_in_seconds = 30.;
  double actual_length_in_seconds = wav_decoded.length_in_seconds;
  EXPECT_DOUBLE_EQ(expected_length_in_seconds, actual_length_in_seconds);

  int expected_samples_per_channel = 1323000;
  int actual_samples_per_channel = wav_decoded.samples_per_channel;
  EXPECT_EQ(expected_samples_per_channel, actual_samples_per_channel);

  std::string expected_file_type = "wav";
  std::string actual_file_type = wav_decoded.file_type;
  EXPECT_EQ(expected_file_type, actual_file_type);

  int expected_avg_bitrate_kbps = 1411;
  int actual_avg_bitrate_kbps = wav_decoded.avg_bitrate_kbps;
  EXPECT_EQ(expected_avg_bitrate_kbps, actual_avg_bitrate_kbps);
}

/**
 * @brief Decode WAV from file.
 *
 */
TEST(AudioFileDecoding, DecodeWavFromFile) {
  const std::string file_path = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.wav");
  WavDecoded wav_decoded = DecodeWav(file_path);

  uint32_t expected_sample_rate = 44100;
  uint32_t actual_sample_rate = wav_decoded.sample_rate;
  EXPECT_EQ(expected_sample_rate, actual_sample_rate);

  int expected_bit_depth = 16;
  int actual_bit_depth = wav_decoded.bit_depth;
  EXPECT_EQ(expected_bit_depth, actual_bit_depth);

  int expectedChannels = 2;
  int actual_channels = wav_decoded.channels;
  EXPECT_EQ(expectedChannels, actual_channels);

  bool expected_mono = false;
  bool actual_mono = wav_decoded.mono;
  EXPECT_EQ(expected_mono, actual_mono);

  bool expected_stereo = true;
  bool actual_stereo = wav_decoded.stereo;
  EXPECT_EQ(expected_stereo, actual_stereo);

  double expected_length_in_seconds = 30.;
  double actual_length_in_seconds = wav_decoded.length_in_seconds;
  EXPECT_DOUBLE_EQ(expected_length_in_seconds, actual_length_in_seconds);

  int expected_samples_per_channel = 1323000;
  int actual_samples_per_channel = wav_decoded.samples_per_channel;
  EXPECT_EQ(expected_samples_per_channel, actual_samples_per_channel);

  std::string expected_file_type = "wav";
  std::string actual_file_type = wav_decoded.file_type;
  EXPECT_EQ(expected_file_type, actual_file_type);

  int expected_avg_bitrate_kbps = 1411;
  int actual_avg_bitrate_kbps = wav_decoded.avg_bitrate_kbps;
  EXPECT_EQ(expected_avg_bitrate_kbps, actual_avg_bitrate_kbps);
}

/**
 * @brief Decode MP3 from file.
 *
 */
TEST(AudioFileDecoding, DecodeMp3FromFile) {
  std::vector<double> NormalizedSamples;
  const std::string file_pathMp3 = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.mp3");

  Mp3Decoded mp3_decoded = DecodeMp3(file_pathMp3);

  uint32_t expected_sample_rate = 44100;
  uint32_t actual_sample_rate = mp3_decoded.sample_rate;
  EXPECT_EQ(expected_sample_rate, actual_sample_rate);

  int expectedChannels = 2;
  int actual_channels = mp3_decoded.channels;
  EXPECT_EQ(expectedChannels, actual_channels);

  bool expected_mono = false;
  bool actual_mono = mp3_decoded.mono;
  EXPECT_EQ(expected_mono, actual_mono);

  bool expected_stereo = true;
  bool actual_stereo = mp3_decoded.stereo;
  EXPECT_EQ(expected_stereo, actual_stereo);

  double expected_length_in_seconds = 30.;
  double actual_length_in_seconds = mp3_decoded.length_in_seconds;
  EXPECT_NEAR(expected_length_in_seconds, actual_length_in_seconds, 0.1);

  int expected_samples_per_channel = 1325952;
  int actual_samples_per_channel = mp3_decoded.samples_per_channel;
  EXPECT_EQ(expected_samples_per_channel, actual_samples_per_channel);

  std::string expected_file_type = "mp3";
  std::string actual_file_type = mp3_decoded.file_type;
  EXPECT_EQ(expected_file_type, actual_file_type);

  int expected_avg_bitrate_kbps = 320;
  int actual_avg_bitrate_kbps = mp3_decoded.avg_bitrate_kbps;
  EXPECT_EQ(expected_avg_bitrate_kbps, actual_avg_bitrate_kbps);
}
