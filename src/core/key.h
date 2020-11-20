#pragma once

#include <iostream>
#include <string>
#include <vector>

#include "src/core/hpcp.h"
#include "src/core/utils.h"
#include "src/core/windowing.h"

using namespace musher::core;

namespace musher {
namespace core {

enum class Scales {
  MAJOR,
  MINOR,
  MAJMIN,
};

struct KeyOutput {
  std::string key;
  std::string scale;
  double strength;
  double first_to_second_relative_strength;
};

/**
 * @brief Select a key profile given the type.
 *
 * About the Key Profiles:
 * - **Diatonic** - Binary profile with diatonic notes of both modes. Could be useful for ambient music or diatonic music
 *      which is not strictly 'tonal functional'
 * - **Tonic Triad** - Just the notes of the major and minor chords. Exclusively for testing.
 * - **Krumhansl** - Reference key profiles after cognitive experiments with users. They should work generally fine for
 *      pop music.
 * - **Temperley** - Key profiles extracted from corpus analysis of euroclassical music. Therefore, they perform best on
 *      this repertoire (especially in minor).
 * - **Shaath** -  Profiles based on Krumhansl's specifically tuned to popular and electronic music.
 * - **Noland** - Profiles from Bach's 'Well Tempered Klavier'.
 * - **Edma** - Automatic profiles extracted from corpus analysis of electronic dance music [3]. They normally perform
 *      better that Shaath's
 * - **Edmm** - Automatic profiles extracted from corpus analysis of electronic dance music and manually tweaked according
 *      to heuristic observation. It will report major modes (which are poorly represented in EDM) as minor, but improve
 *      performance otherwise [3].
 * - **Braw** - Profiles obtained by calculating the median profile for each mode from a subset of BeatPort dataset. There
 *      is an extra profile obtained from ambiguous tracks that are reported as minor[4]
 * - **Bgate** - Same as braw but zeroing the 4 less relevant elements of each profile[4]
 *
 * References:
 *  [1] E. Gómez, "Tonal Description of Polyphonic Audio for Music Content
 *  Processing," INFORMS Journal on Computing, vol. 18, no. 3, pp. 294–304, 2006.
 *  [2] D. Temperley, "What's key for key? The Krumhansl-Schmuckler key-finding algorithm reconsidered", Music
 *  Perception vol. 17, no. 1,  pp. 65-100, 1999.
 *  [3] Á. Faraldo, E. Gómez, S. Jordà, P.Herrera, "Key Estimation in Electronic"
 *  Dance Music. Proceedings of the 38th International Conference on information"
 *  Retrieval, pp. 335-347, 2016.
 *  [4] Faraldo, Á., Jordà, S., & Herrera, P. (2017, June). A multi-profile method"
 *  for key estimation in edm. In Audio Engineering Society Conference: 2017 AES"
 *  International Conference on Semantic Audio. Audio Engineering Society.
 *
 * --essentia: https://github.com/MTG/essentia/blob/master/src/algorithms/tonal/key.cpp
 *
 * @param profile_type Key profile type.
 * @return std::vector<std::vector<double>> Key profile
 */
std::vector<std::vector<double>> SelectKeyProfile(const std::string profile_type);

/**
 * @brief Add contribution harmonics to chords.
 * Each note contribute to the different harmonics:
 * 1.- first  harmonic  f   -> i
 * 2.- second harmonic  2*f -> i
 * 3.- third  harmonic  3*f -> i+7
 * 4.- fourth harmonic  4*f -> i
 * ..
 * The contribution is weighted depending of the slope
 *
 * @param chords Chords
 * @param pitch_class pitch class
 * @param contribution harmonic contribution
 * @param num_harmonics Number of harmonics that should contribute to the polyphonic profile (1 only considers the
 * fundamental harmonic).
 * @param slope Value of the slope of the exponential harmonic contribution to the polyphonic profile.
 * @return std::vector<double> chords with added contribution harmonics
 */
std::vector<double> AddContributionHarmonics(const std::vector<double>& M_chords,
                                             const int pitch_class,
                                             const double contribution,
                                             const int num_harmonics,
                                             const double slope);

/**
 * @brief Adds the contribution of a chord with root note 'root' to its major triad.
 * A major triad includes notes from three different classes of pitch: the root, the major 3rd and perfect 5th.
 * This is the most relaxed, most consonant chord in all of harmony.
 * @see http://www.songtrellis.com/directory/1146/chordTypes/majorChordTypes/majorTriad
 * The three notes of the chord have the same weight
 *
 * @param chords Chords
 * @param root root note
 * @param contribution harmonic contribution
 * @param num_harmonics Number of harmonics that should contribute to the polyphonic profile (1 only considers the
 * fundamental harmonic).
 * @param slope Value of the slope of the exponential harmonic contribution to the polyphonic profile.
 * @return std::vector<double> Chords with contribution added to its major triad.
 */
std::vector<double> AddMajorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

/**
 * @brief Adds the contribution of a chord with root note 'root' to its minor triad
 * A minor triad includes notes from three different classes of pitch: the root, the minor 3rd and perfect 5th.
 * @see http://www.songtrellis.com/directory/1146/chordTypes/minorChordTypes/minorTriadMi
 * The three notes of the chord have the same weight
 *
 * @param chords Chords
 * @param root root note
 * @param contribution harmonic contribution
 * @param num_harmonics Number of harmonics that should contribute to the polyphonic profile (1 only considers the
 * fundamental harmonic).
 * @param slope Value of the slope of the exponential harmonic contribution to the polyphonic profile.
 * @return std::vector<double> Chords with contribution added to its minor triad.
 */
std::vector<double> AddMinorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

/**
 * @brief Resizes and interpolates the profiles to fit the pcp size.
 *
 * @param pcp_size Number of array elements used to represent a semitone times 12.
 * @param key_profile Key profile.
 * @return std::tuple<std::vector<double>, double, double> Tuple of (resized profile, mean, standard deviation).
 */
std::tuple<std::vector<double>, double, double> ResizeProfileToPcpSize(const unsigned int pcp_size,
                                                                       const std::vector<double>& key_profile);

/**
 * @brief Calculate correlation
 * Correlation coefficient with 'shift' on of the vectors is shifted in time,
 * and then the correlation is calculated, just like a cross-correlation.
 *
 * @param v1 Vector
 * @param mean1 Mean
 * @param std1 Standard deviation
 * @param v2 Vector
 * @param mean2 Mean
 * @param std2 Standard deviation
 * @param shift Amount the profile was shifted
 * @return double
 */
double Correlation(const std::vector<double>& v1,
                   const double mean1,
                   const double std1,
                   const std::vector<double>& v2,
                   const double mean2,
                   const double std2,
                   const int shift);

/**
 * @brief Calculate the standard deviation of a vector.
 *
 * @param mean Mean (Average)
 * @param vec Vector
 * @return double Standard devation
 */
double StandardDeviation(double mean, const std::vector<double>& vec);

/**
 * @brief Computes key estimate given a pitch class profile (HPCP).
 *
 * @param pcp The input pitch class profile.
 * @param use_polphony Enables the use of polyphonic profiles to define key profiles (this includes the contributions
 * from triads as well as pitch harmonics).
 * @param use_three_chords Consider only the 3 main triad chords of the key (T, D, SD) to build the polyphonic profiles.
 * @param num_harmonics Number of harmonics that should contribute to the polyphonic profile (1 only considers the
 * fundamental harmonic).
 * @param slope Value of the slope of the exponential harmonic contribution to the polyphonic profile.
 * @param profile_type The type of polyphic profile to use for correlation calculation.
 * @param use_maj_min Use a third profile called 'majmin' for ambiguous tracks [4]. Only available for the edma, bgate
 * and braw profiles.
 * @return KeyOutput A struct containing the following:
 *      key: Estimated key, from A to G.
 *      scale: Scale of the key (major or minor).
 *      strength: Strength of the estimated key.
 *      first_to_second_relative_strength: The relative strength difference between the best estimate and second best
 *       estimate of the key.
 */
KeyOutput EstimateKey(const std::vector<double>& pcp,
                      const bool use_polphony = true,
                      const bool use_three_chords = true,
                      const unsigned int num_harmonics = 4,
                      const double slope = 0.6,
                      const std::string profile_type = "Bgate",
                      const bool use_maj_min = false);

/**
 * @brief Computes key estimate given normalized samples.
 *
 * @param normalized_samples Normalized samples, either stereo or mono.
 * @param sample_rate Sampling rate of the audio signal \[Hz\].
 * @param profile_type The type of polyphic profile to use for correlation calculation.
 * @param use_polphony Enables the use of polyphonic profiles to define key profiles (this includes the contributions
 * from triads as well as pitch harmonics).
 * @param use_three_chords Consider only the 3 main triad chords of the key (T, D, SD) to build the polyphonic profiles.
 * @param num_harmonics Number of harmonics that should contribute to the polyphonic profile (1 only considers the
 * fundamental harmonic).
 * @param slope Value of the slope of the exponential harmonic contribution to the polyphonic profile.
 * @param use_maj_min Use a third profile called 'majmin' for ambiguous tracks [4]. Only available for the edma, bgate
 * and braw profiles.
 * @param pcp_size Number of array elements used to represent a semitone times 12.
 * @param frame_size Output frame size.
 * @param hop_size Hop size between frames.
 * @param window_type_func The window type function. Examples: BlackmanHarris92dB, BlackmanHarris62dB...
 * @param max_num_peaks Maximum number of returned peaks (set to 0 to return all peaks).
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @return KeyOutput A struct containing the following:
 *      key: Estimated key, from A to G.
 *      scale: Scale of the key (major or minor).
 *      strength: Strength of the estimated key.
 *      first_to_second_relative_strength: The relative strength difference between the best estimate and second best
 *       estimate of the key.
 */
KeyOutput DetectKey(
    const std::vector<std::vector<double>>& normalized_samples,
    double sample_rate = 44100.,
    const std::string profile_type = "Bgate",
    const bool use_polphony = true,
    const bool use_three_chords = true,
    const unsigned int num_harmonics = 4,
    const double slope = 0.6,
    const bool use_maj_min = false,
    const unsigned int pcp_size = 36,
    const int frame_size = 4096,
    const int hop_size = 512,
    const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func = BlackmanHarris62dB,
    unsigned int max_num_peaks = 100,
    double window_size = .5);

}  // namespace core
}  // namespace musher
