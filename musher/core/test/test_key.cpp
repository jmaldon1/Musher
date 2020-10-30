#include "musher/core/key.h"
#include "musher/core/musher_library.h"
#include "musher/core/test/gtest_extras.h"
#include "musher/core/test/test_utils.h"
#include "musher/core/utils.h"

using namespace musher::core;
using namespace musher::core::test;

/**
 * @brief Test C Major Classical
 *
 */
TEST(Key, TestCMajorClassical) {
    const std::string filePath = TEST_DATA_DIR + std::string("audio_files/mozart_c_major_30sec.wav");
    // const std::string filePath = TEST_DATA_DIR + std::string("audio_files/1356281_Eb_major.wav");

    int pcp_size = 36;
    double sample_rate = 44100.;
    int num_harmonics = 4;

    WavDecoded wav_decoded = CDecodeWav(filePath);
    std::vector<std::vector<double>> normalized_samples = wav_decoded.normalized_samples;
    std::vector<double> mixed_audio = monoMixer(normalized_samples);

    Framecutter framecutter(mixed_audio, 4096, 512);

    int count = 0;
    std::vector<double> sums((size_t)pcp_size, 0.);

    for (const std::vector<double> &frame : framecutter) {
        std::vector<double> windowed_frame = windowing(frame, blackmanHarris62dB);
        std::vector<double> spectrum = convertToFrequencySpectrum(windowed_frame);
        std::vector<std::tuple<double, double>> spectral_peaks =
            spectralPeaks(spectrum, -1000.0, true, "height", 100, sample_rate, 0, sample_rate / 2);
        std::vector<double> hpcp =
            HPCP(spectral_peaks, pcp_size, 440.0, num_harmonics - 1, true, 500.0, 40.0, 5000.0, "squared cosine", .5);

        for (int i = 0; i < (int)hpcp.size(); i++) {
            sums[i] += hpcp[i];
        }
        count += 1;
    }
    std::vector<double> avgs(sums.size());
    std::transform(sums.begin(), sums.end(), avgs.begin(), [&count](auto const &sum) { return sum / count; });
    KeyOutput key_output = detectKey(avgs, true, true, 4, 0.6, PolyphicProfile::Temperley);

    EXPECT_EQ(key_output.key, "C");
    EXPECT_EQ(key_output.scale, "major");
    EXPECT_NEAR(key_output.strength, 0.760322, 0.000001);
    EXPECT_NEAR(key_output.first_to_second_relative_strength, 0.607807, 0.000001);
}

/**
 * @brief Test Eb Major EDM
 *
 */
TEST(Key, TestEbMajorEDM) {
    const std::string filePath = TEST_DATA_DIR + std::string("audio_files/EDM_Eb_major_2min.wav");

    int pcp_size = 36;
    int num_harmonics = 4;

    WavDecoded wav_decoded = CDecodeWav(filePath);
    double sample_rate = wav_decoded.sample_rate;
    std::vector<std::vector<double>> normalized_samples = wav_decoded.normalized_samples;
    std::vector<double> mixed_audio = monoMixer(normalized_samples);

    Framecutter framecutter(mixed_audio, 4096, 512);

    int count = 0;
    std::vector<double> sums((size_t)pcp_size, 0.);

    for (const std::vector<double> &frame : framecutter) {
        std::vector<double> windowed_frame = windowing(frame, blackmanHarris62dB);
        std::vector<double> spectrum = convertToFrequencySpectrum(windowed_frame);
        std::vector<std::tuple<double, double>> spectral_peaks =
            spectralPeaks(spectrum, -1000.0, true, "height", 100, sample_rate, 0, sample_rate / 2);
        std::vector<double> hpcp =
            HPCP(spectral_peaks, pcp_size, 440.0, num_harmonics - 1, true, 500.0, 40.0, 5000.0, "squared cosine", .5);

        for (int i = 0; i < (int)hpcp.size(); i++) {
            sums[i] += hpcp[i];
        }
        count += 1;
    }
    std::vector<double> avgs(sums.size());
    std::transform(sums.begin(), sums.end(), avgs.begin(), [&count](auto const &sum) { return sum / count; });
    KeyOutput key_output = detectKey(avgs, true, true, 4, 0.6, PolyphicProfile::Edmm);

    EXPECT_EQ(key_output.key, "Eb");
    EXPECT_EQ(key_output.scale, "major");
    EXPECT_NEAR(key_output.strength, 0.613304, 0.000001);
    EXPECT_NEAR(key_output.first_to_second_relative_strength, 0.516593, 0.000001);
}
