#include <string>
#include <numeric>

#include "gtest/gtest.h"
#include "src/core/mono_mixer.h"
#include "src/core/audio_decoders.h"
#include "src/core/utils.h"

using namespace musher::core;


/**
 * @brief Basic mono mixer test
 * 
 */
TEST(MonoMixer, MonoMixer44100) {
  const std::string filePath = TEST_DATA_DIR + std::string("audio_files/impulses_1second_44100.wav");
  WavDecoded wav_decoded = DecodeWav(filePath);
  std::vector<double> mixed_audio = MonoMixer(wav_decoded.normalized_samples);

  size_t actual_mixed_audio_size = mixed_audio.size();
  double actual_mixed_audio_sum = std::accumulate(mixed_audio.begin(), mixed_audio.end(), 0.);

  size_t expected_mixed_audio_size = 441000;
  double expected_mixed_audio_sum = 8.999725341796875;

  EXPECT_EQ(actual_mixed_audio_size, expected_mixed_audio_size);
  EXPECT_DOUBLE_EQ(actual_mixed_audio_sum, expected_mixed_audio_sum);
}
