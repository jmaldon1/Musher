#include <cmath>

#include "src/core/hpcp.h"
#include "src/core/test/gtest_extras.h"

using namespace musher::core;

/**
 * @brief Test Zero inputs
 *
 */
TEST(HPCP, TestZeros) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies(10);
  std::vector<double> magnitudes(10);
  actual_hpcp = HPCP(frequencies, magnitudes);
  std::vector<double> expected_hpcp(12);
  EXPECT_VEC_EQ(actual_hpcp, expected_hpcp);
}

/**
 * @brief Test submediant position
 * @details Make sure that the submediant of a key based on 440 is in the
 * correct location (submediant was randomly selected from all the tones)
 *
 */
TEST(HPCP, SubmediantPosition) {
  std::vector<double> actual_hpcp;
  int tonic = 440;
  double submediant = tonic * std::pow(2, (9. / 12.));
  std::vector<double> submediant_freqs = { submediant };
  std::vector<double> magnitudes = { 1 };
  actual_hpcp = HPCP(submediant_freqs, magnitudes);

  std::vector<double> expected_hpcp = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 1., 0., 0. };
  EXPECT_VEC_EQ(actual_hpcp, expected_hpcp);
}

/**
 * @brief Test harmonics
 *
 */
TEST(HPCP, Harmonics) {
  std::vector<double> actual_hpcp;
  double tone = 100.;
  std::vector<double> frequencies = { tone, tone * 2, tone * 3, tone * 4 };
  std::vector<double> magnitudes(4, 1.);
  int harmonics = 3;
  bool band_preset = false;
  double min_frequency = 50.0;
  double max_frequency = 500.0;

  actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, harmonics, band_preset, 500.0, min_frequency, max_frequency,
                     "squared cosine", 1.0, false);

  std::vector<double> expected_hpcp = { 0., 0., 0., 0.1340538263, 0., 0.2476127148, 0., 0., 0., 0., 1., 0. };
  EXPECT_VEC_NEAR(actual_hpcp, expected_hpcp, 1e-4);
}

/**
 * @brief Test max shifted
 * @details Tests whether a HPCP reading with only the dominant semitone
 * activated is correctly shifted so that the dominant is at the position 0
 *
 */
TEST(HPCP, MaxShifted) {
  std::vector<double> actual_hpcp;
  int tonic = 440;
  double dominant = tonic * std::pow(2, (7. / 12.));
  std::vector<double> dominant_freqs = { dominant };
  std::vector<double> magnitudes = { 1 };
  bool max_shifted = true;
  actual_hpcp =
      HPCP(dominant_freqs, magnitudes, 12, 440.0, 0, true, 500.0, 40.0, 5000.0, "squared cosine", 1.0, max_shifted);

  std::vector<double> expected_hpcp = { 1., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };
  EXPECT_VEC_EQ(actual_hpcp, expected_hpcp);
}

/**
 * @brief Test low frequency
 *
 */
TEST(HPCP, LowFrequency) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = { 99 };
  std::vector<double> magnitudes = { 1 };
  double min_frequency = 100.0;
  double max_frequency = 1000.0;
  actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, 0, true, 500.0, min_frequency, max_frequency, "squared cosine",
                     1.0, false);

  std::vector<double> expected_hpcp = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };
  EXPECT_VEC_EQ(actual_hpcp, expected_hpcp);
}

/**
 * @brief Test high frequency
 *
 */
TEST(HPCP, HighFrequency) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = { 1001 };
  std::vector<double> magnitudes = { 1 };
  double min_frequency = 100.0;
  double max_frequency = 1000.0;
  actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, 0, true, 500.0, min_frequency, max_frequency, "squared cosine",
                     1.0, false);

  std::vector<double> expected_hpcp = { 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0. };
  EXPECT_VEC_EQ(actual_hpcp, expected_hpcp);
}

/**
 * @brief Test too small min range error
 *
 */
TEST(HPCP, TooSmallMinRangeError) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = {};
  std::vector<double> magnitudes = {};
  double band_split_frequency = 200.0;
  double min_frequency = 1.0;
  EXPECT_THROW(
      {
        try {
          actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, 0, true, band_split_frequency, min_frequency, 1000.0,
                             "squared cosine", 1.0, false);
        } catch (const std::runtime_error& e) {
          /* This tests if the error message is equal */
          EXPECT_STREQ("HPCP: Low band frequency range too small", e.what());
          throw;
        }
      },
      std::runtime_error);
}

/**
 * @brief Test too small max range error
 *
 */
TEST(HPCP, TooSmallMaxRangeError) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = {};
  std::vector<double> magnitudes = {};
  double band_split_frequency = 1000.0;
  double max_frequency = 1199.0;
  EXPECT_THROW(
      {
        try {
          actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, 0, true, band_split_frequency, 40.0, max_frequency,
                             "squared cosine", 1.0, false);
        } catch (const std::runtime_error& e) {
          /* This tests if the error message is equal */
          EXPECT_STREQ("HPCP: High band frequency range too small", e.what());
          throw;
        }
      },
      std::runtime_error);
}

/**
 * @brief Test too close min max range error
 *
 */
TEST(HPCP, TooCloseMinMaxRange) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = {};
  std::vector<double> magnitudes = {};
  bool band_present = false;
  double min_frequency = 1.0;
  double max_frequency = 200.0;
  EXPECT_THROW(
      {
        try {
          actual_hpcp = HPCP(frequencies, magnitudes, 12, 440.0, 0, band_present, 1000.0, min_frequency, max_frequency,
                             "squared cosine", 1.0, false);
        } catch (const std::runtime_error& e) {
          /* This tests if the error message is equal */
          EXPECT_STREQ("HPCP: Minimum and maximum frequencies are too close", e.what());
          throw;
        }
      },
      std::runtime_error);
}

/**
 * @brief Test size not a multiple of 12.
 *
 */
TEST(HPCP, SizeNotMultipleOf12) {
  std::vector<double> actual_hpcp;
  std::vector<double> frequencies = {};
  std::vector<double> magnitudes = {};
  int size = 13;
  EXPECT_THROW(
      {
        try {
          actual_hpcp =
              HPCP(frequencies, magnitudes, size, 440.0, 0, true, 500.0, 40.0, 5000.0, "squared cosine", 1.0, false);
        } catch (const std::runtime_error& e) {
          /* This tests if the error message is equal */
          EXPECT_STREQ("HPCP: The size parameter is not a multiple of 12.", e.what());
          throw;
        }
      },
      std::runtime_error);
}
