#pragma once

#define _USE_MATH_DEFINES
#include <algorithm>
#include <string>
#include <vector>

namespace musher {
namespace core {

enum WeightType { NONE, COSINE, SQUARED_COSINE };
enum NormalizeType { N_NONE, N_UNIT_MAX, N_UNIT_SUM };
struct HarmonicPeak {
  double semitone;
  double harmonic_strength;

  // Constructor
  HarmonicPeak(double semitone_, double harmonic_strength_ = 0.0)
      : semitone(semitone_), harmonic_strength(harmonic_strength_){};
};

/**
 * @brief Get the arg max of a vector.
 *
 * Checks if the vector is empty first.
 *
 * @param vec Vector
 * @return int Arg max
 */
int ArgMax(const std::vector<double> &input);

/**
 * @brief Normalize a vector so its largest value gets mapped to 1.
 *
 * If zero, the vector isn't touched.
 *
 * @tparam T
 * @param vec Vector to normalize.
 */
template <typename T>
void NormalizeInPlace(std::vector<T> &vec) {
  if (vec.empty()) return;

  T maxElement = *std::max_element(vec.begin(), vec.end());
  // T maxElement = fplus::maximum(vec);

  if (maxElement != (T)0.0) {
    for (size_t i = 0; i < vec.size(); i++) {
      vec[i] /= maxElement;
    }
  }
}

/**
 * @brief Normalize a vector so it's sum is equal to 1.
 *
 * The vector is not touched if it contains negative elements or the sum is zero.
 *
 * @tparam T
 * @param vec Vector to normalize.
 */
template <typename T>
void NormalizeSumInPlace(std::vector<T> &vec) {
  if (vec.empty()) return;

  // T sumElements = std::accumulate(vec.begin(), vec.end(), (T) 0.0);
  T sumElements = (T)0.;
  for (size_t i = 0; i < vec.size(); ++i) {
    if (vec[i] < 0) return;
    sumElements += vec[i];
  }

  if (sumElements != (T)0.0) {
    for (size_t i = 0; i < vec.size(); ++i) {
      vec[i] /= sumElements;
    }
  }
}

/**
 * @brief Add contribution to the HPCP with weight.
 *
 * @param freq Frequency \[Hz\]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 \[Hz\].
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param weight_type Type of weighting function for determining frequency contribution.
 * @param harmonic_weight Strength/weight of the harmonic.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContributionWithWeight(double freq,
                               double mag_lin,
                               double reference_frequency,
                               double window_size,
                               WeightType weight_type,
                               double harmonic_weight,
                               std::vector<double> &hpcp);

/**
 * @brief Add contribution to the HPCP without weight.
 *
 * @param freq Frequency \[Hz\]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 \[Hz\].
 * @param harmonic_weight Strength/weight of the harmonic.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContributionWithoutWeight(double freq,
                                  double mag_lin,
                                  double reference_frequency,
                                  double harmonic_weight,
                                  std::vector<double> &hpcp);

/**
 * @brief Adds the magnitude contribution of the given frequency as the tonic semitone.
 *
 * As well as its possible contribution as a harmonic of another pitch.
 *
 * @param freq Frequency \[Hz\]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 \[Hz\].
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param weight_type Type of weighting function for determining frequency contribution.
 * @param harmonic_peaks Weighting table of harmonic contribution.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContribution(double freq,
                     double mag_lin,
                     double reference_frequency,
                     double window_size,
                     WeightType weight_type,
                     std::vector<HarmonicPeak> harmonic_peaks,
                     std::vector<double> &hpcp);

/**
 * @brief Builds a weighting table of harmonic contribution.
 *
 * Higher harmonics contribute less and the fundamental frequency has a full harmonic strength of 1.0.
 *
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @return std::vector<HarmonicPeak> Weighting table of harmonic contribution.
 */
std::vector<HarmonicPeak> InitHarmonicContributionTable(int harmonics);

/**
 * @brief Computes a Harmonic Pitch Class Profile (HPCP) from the spectral peaks of a signal.
 *
 * HPCP is a k*12 dimensional vector which represents the intensities of the twelve (k==1) semitone pitch classes
 * (corresponsing to notes from A to G#), or subdivisions of these (k>1).
 *
 * @param frequencies Frequencies (positions) of the spectral peaks \[Hz\].
 * @param magnitudes Magnitudes (heights) of the spectral peaks.
 * @param size Size of the output HPCP (must be a positive nonzero multiple of 12).
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 \[Hz\].
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @param band_preset Enables whether to use a band preset.
 * @param band_split_frequency Split frequency for low and high bands, not used if bandPreset is false \[Hz\].
 * @param min_frequency Minimum frequency that contributes to the HPCP \[Hz\] (the difference between the min and split
 * frequencies must not be less than 200.0 Hz).
 * @param max_frequency Maximum frequency that contributes to the HPCP \[Hz\] (the difference between the max and split
 * frequencies must not be less than 200.0 Hz).
 * @param _weight_type Type of weighting function for determining frequency contribution.
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param max_shifted Whether to shift the HPCP vector so that the maximum peak is at index 0.
 * @param non_linear Apply non-linear post-processing to the output (use with _normalized='unit max'). Boosts values
 * close to 1, decreases values close to 0.
 * @param _normalized Whether to normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
std::vector<double> HPCP(const std::vector<double> &frequencies,
                         const std::vector<double> &magnitudes,
                         unsigned int size = 12,
                         double reference_frequency = 440.0,
                         unsigned int harmonics = 0,
                         bool band_preset = true,
                         double band_split_frequency = 500.0,
                         double min_frequency = 40.0,
                         double max_frequency = 5000.0,
                         std::string _weight_type = "squared cosine",
                         double window_size = 1.0,
                         bool max_shifted = false,
                         bool non_linear = false,
                         std::string _normalized = "unit max");

/**
 * @brief Overloaded function for HPCP that accepts a vector of peaks.
 *
 * Refer to original HPCP function for more details.
 *
 * @param peaks Vector of spectral peaks, each peak being a tuple (frequency, magnitude).
 * @param size Size of the output HPCP (must be a positive nonzero multiple of 12).
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 \[Hz\].
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @param band_preset Enables whether to use a band preset.
 * @param band_split_frequency Split frequency for low and high bands, not used if bandPreset is false \[Hz\].
 * @param min_frequency Minimum frequency that contributes to the HPCP \[Hz\] (the difference between the min and split
 * frequencies must not be less than 200.0 Hz).
 * @param max_frequency Maximum frequency that contributes to the HPCP \[Hz\] (the difference between the max and split
 * frequencies must not be less than 200.0 Hz).
 * @param _weight_type Type of weighting function for determining frequency contribution.
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param max_shifted Whether to shift the HPCP vector so that the maximum peak is at index 0.
 * @param non_linear Apply non-linear post-processing to the output (use with _normalized='unit max'). Boosts values
 * close to 1, decreases values close to 0.
 * @param _normalized Whether to normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
std::vector<double> HPCP(const std::vector<std::tuple<double, double>> &peaks,
                         unsigned int size = 12,
                         double reference_frequency = 440.0,
                         unsigned int harmonics = 0,
                         bool band_preset = true,
                         double band_split_frequency = 500.0,
                         double min_frequency = 40.0,
                         double max_frequency = 5000.0,
                         std::string _weight_type = "squared cosine",
                         double window_size = 1.0,
                         bool max_shifted = false,
                         bool non_linear = false,
                         std::string _normalized = "unit max");

}  // namespace core
}  // namespace musher
