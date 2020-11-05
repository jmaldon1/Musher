#include <math.h>

#include <complex>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <string>
#include <unordered_map>

#include "gtest/gtest.h"
#include "src/core/musher_library.h"
#include "src/core/utils.h"

/* TEST INCLUDES */
// #include "test_load_audio_file.h"

using namespace musher::core;

// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(PrintFunctionalMessage, PrintsMessage) {
  CPrintFunctionalMessage("This is a message for a functional program");
  EXPECT_EQ(0, 0);
}

TEST(AudioFileDecoding, AudioFileNotFoundTest) {
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

TEST(AudioFileDecoding, LoadsAudioFileTest) {
  std::vector<uint8_t> fileData;
  std::string filePath = TEST_DATA_DIR + std::string("audio_files/CantinaBand3sec.wav");
  fileData = LoadAudioFile(filePath);

  std::string outputFile = "test_include/filedata.txt";
  // outputVectorToFile<uint8_t>(fileData, outputFile);

  std::string fileDataHex = Uint8VectorToHexString(fileData);
  // ASSERT_TRUE(fileData == fileDataExpected);
}

TEST(AudioFileDecoding, DecodeWav) {
  const std::string filePath = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.wav");
  WavDecoded wav_decoded = DecodeWav(filePath);

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

// TEST(AudioFileDecoding, BeatDetection) {
//   const std::string filePath = TEST_DATA_DIR + std::string("audio_files/CantinaBand3sec.wav");

//   WavDecoded wav_decoded = DecodeWav(filePath);
//   uint32_t sampleRate = wav_decoded.sample_rate;
//   std::vector<double> interleaved_normalized_samples = wav_decoded.interleaved_normalized_samples;

//   double bpm = BPMOverWindow(interleaved_normalized_samples, sampleRate, 3);
//   EXPECT_EQ(bpm, 80.);
// }

// TEST(AudioFileDecoding, MP3BPM) {
//   std::vector<double> NormalizedSamples;
//   const std::string filePathMp3 = TEST_DATA_DIR + std::string("audio_files/126bpm.mp3");

//   Mp3Decoded mp3_decoded = CDecodeMp3(filePathMp3);
//   uint32_t sampleRate = mp3_decoded.sample_rate;
//   std::vector<double> interleaved_normalized_samples = mp3_decoded.interleaved_normalized_samples;

//   double bpm = BPMOverWindow(interleaved_normalized_samples, sampleRate, 3);
//   // std::cout << bpm << std::endl;
//   EXPECT_DOUBLE_EQ(bpm, 125.);
// }

TEST(AudioFileDecoding, DecodeMp3) {
  std::vector<double> NormalizedSamples;
  const std::string filePathMp3 = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.mp3");

  Mp3Decoded mp3_decoded = CDecodeMp3(filePathMp3);

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

  // TODO: This is POSSIBLY wrong!
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
