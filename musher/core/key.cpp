#include "musher/core/key.h"

#include <cmath>
#include <fplus/fplus.hpp>

#include "musher/core/utils.h"

namespace musher {
namespace core {

/**
 * @brief Select a key profile given the type.
 *
 * About the Key Profiles:
 * - 'Diatonic' - binary profile with diatonic notes of both modes. Could be useful for ambient music or diatonic music
 *      which is not strictly 'tonal functional'
 * - 'Tonic Triad' - just the notes of the major and minor chords. Exclusively for testing.
 * - 'Krumhansl' - reference key profiles after cognitive experiments with users. They should work generally fine for
 *      pop music.
 * - 'Temperley' - key profiles extracted from corpus analysis of euroclassical music. Therefore, they perform best on
 *      this repertoire (especially in minor).
 * - 'Shaath' -  profiles based on Krumhansl's specifically tuned to popular and electronic music.
 * - 'Noland' - profiles from Bach's 'Well Tempered Klavier'.
 * - 'edma' - automatic profiles extracted from corpus analysis of electronic dance music [3]. They normally perform
 *      better that Shaath's
 * - 'edmm' - automatic profiles extracted from corpus analysis of electronic dance music and manually tweaked according
 *      to heuristic observation. It will report major modes (which are poorly represented in EDM) as minor, but improve
 *      performance otherwise [3].
 * - 'braw' - profiles obtained by calculating the median profile for each mode from a subset of BeatPort dataset. There
 *      is an extra profile obtained from ambiguous tracks that are reported as minor[4]
 * - 'bgate' - same as braw but zeroing the 4 less relevant elements of each profile[4]
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
std::vector<std::vector<double>> SelectKeyProfile(const PolyphicProfile profile_type) {
  const std::vector<std::vector<double>> diatonic{
    { 1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1 },
    { 1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1 },
  };

  const std::vector<std::vector<double>> krumhansl{
    { 6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88 },
    { 6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17 },
  };

  const std::vector<std::vector<double>> temperley{
    { 5.0, 2.0, 3.5, 2.0, 4.5, 4.0, 2.0, 4.5, 2.0, 3.5, 1.5, 4.0 },
    { 5.0, 2.0, 3.5, 4.5, 2.0, 4.0, 2.0, 4.5, 3.5, 2.0, 1.5, 4.0 },
  };

  const std::vector<std::vector<double>> weichai{
    { 81302, 320, 65719, 1916, 77469, 40928, 2223, 83997, 1218, 39853, 1579, 28908 },
    { 39853, 1579, 28908, 81302, 320, 65719, 1916, 77469, 40928, 2223, 83997, 1218 },
  };

  const std::vector<std::vector<double>> tonictriad{
    { 1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0 },
    { 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  };

  const std::vector<std::vector<double>> temperley2005{
    { 0.748, 0.060, 0.488, 0.082, 0.67, 0.46, 0.096, 0.715, 0.104, 0.366, 0.057, 0.4 },
    { 0.712, 0.084, 0.474, 0.618, 0.049, 0.46, 0.105, 0.747, 0.404, 0.067, 0.133, 0.33 },
  };

  const std::vector<std::vector<double>> thpcp{
    { 0.95162, 0.20742, 0.71758, 0.22007, 0.71341, 0.48841, 0.31431, 1.00000, 0.20957, 0.53657, 0.22585, 0.55363 },
    { 0.94409, 0.21742, 0.64525, 0.63229, 0.27897, 0.57709, 0.26428, 1.0000, 0.26428, 0.30633, 0.45924, 0.35929 },
  };

  const std::vector<std::vector<double>> shaath{
    { 6.6, 2.0, 3.5, 2.3, 4.6, 4.0, 2.5, 5.2, 2.4, 3.7, 2.3, 3.4 },
    { 6.5, 2.7, 3.5, 5.4, 2.6, 3.5, 2.5, 5.2, 4.0, 2.7, 4.3, 3.2 },
  };

  const std::vector<std::vector<double>> gomez{
    { 0.82, 0.00, 0.55, 0.00, 0.53, 0.30, 0.08, 1.00, 0.00, 0.38, 0.00, 0.47 },
    { 0.81, 0.00, 0.53, 0.54, 0.00, 0.27, 0.07, 1.00, 0.27, 0.07, 0.10, 0.36 },
  };

  const std::vector<std::vector<double>> noland{
    { 0.0629, 0.0146, 0.061, 0.0121, 0.0623, 0.0414, 0.0248, 0.0631, 0.015, 0.0521, 0.0142, 0.0478 },
    { 0.0682, 0.0138, 0.0543, 0.0519, 0.0234, 0.0544, 0.0176, 0.067, 0.0349, 0.0297, 0.0401, 0.027 },
  };

  const std::vector<std::vector<double>> edmm{
    { 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083 },
    { 0.17235348, 0.04, 0.0761009, 0.12, 0.05621498, 0.08527853, 0.0497915, 0.13451001, 0.07458916, 0.05003023,
      0.09187879, 0.05545106 },
  };

  // -- Profiles with other --

  const std::vector<std::vector<double>> bgate{
    { 1.00, 0.00, 0.42, 0.00, 0.53, 0.37, 0.00, 0.77, 0.00, 0.38, 0.21, 0.30 },
    { 1.00, 0.00, 0.36, 0.39, 0.00, 0.38, 0.00, 0.74, 0.27, 0.00, 0.42, 0.23 },
    { 1.00, 0.26, 0.35, 0.29, 0.44, 0.36, 0.21, 0.78, 0.26, 0.25, 0.32, 0.26 },
  };

  const std::vector<std::vector<double>> braw{
    { 1.0000, 0.1573, 0.4200, 0.1570, 0.5296, 0.3669, 0.1632, 0.7711, 0.1676, 0.3827, 0.2113, 0.2965 },
    { 1.0000, 0.2330, 0.3615, 0.3905, 0.2925, 0.3777, 0.1961, 0.7425, 0.2701, 0.2161, 0.4228, 0.2272 },
    { 1.0000, 0.2608, 0.3528, 0.2935, 0.4393, 0.3580, 0.2137, 0.7809, 0.2578, 0.2539, 0.3233, 0.2615 },
  };

  const std::vector<std::vector<double>> edma{
    { 1.00, 0.29, 0.50, 0.40, 0.60, 0.56, 0.32, 0.80, 0.31, 0.45, 0.42, 0.39 },
    { 1.00, 0.31, 0.44, 0.58, 0.33, 0.49, 0.29, 0.78, 0.43, 0.29, 0.53, 0.32 },
    { 1.00, 0.26, 0.35, 0.29, 0.44, 0.36, 0.21, 0.78, 0.26, 0.25, 0.32, 0.26 },
  };

  switch (profile_type) {
    case PolyphicProfile::Diatonic:
      return diatonic;
    case PolyphicProfile::Krumhansl:
      return krumhansl;
    case PolyphicProfile::Temperley:
      return temperley;
    case PolyphicProfile::Weichai:
      return weichai;
    case PolyphicProfile::Tonictriad:
      return tonictriad;
    case PolyphicProfile::Temperley2005:
      return temperley2005;
    case PolyphicProfile::Thpcp:
      return thpcp;
    case PolyphicProfile::Shaath:
      return shaath;
    case PolyphicProfile::Gomez:
      return gomez;
    case PolyphicProfile::Noland:
      return noland;
    case PolyphicProfile::Edmm:
      return edmm;
    // -- Profiles with other --
    case PolyphicProfile::Bgate:
      return bgate;
    case PolyphicProfile::Braw:
      return braw;
    case PolyphicProfile::Edma:
      return edma;
  }
}

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
std::vector<double> AddContributionHarmonics(const std::vector<double>& chords,
                                             const int pitch_class,
                                             const double contribution,
                                             const int num_harmonics,
                                             const double slope) {
  double weight = contribution;
  std::vector<double> chords_with_harmonics(chords);

  for (int iHarm = 1; iHarm <= num_harmonics; iHarm++) {
    double index = pitch_class + 12 * std::log2(static_cast<double>(iHarm));

    double before = std::floor(index);
    double after = std::ceil(index);

    int ibefore = static_cast<int>(std::fmod(static_cast<double>(before), static_cast<double>(12.0)));
    int iafter = static_cast<int>(std::fmod(static_cast<double>(after), static_cast<double>(12.0)));

    // Weight goes proportionally to ibefore & iafter
    if (ibefore < iafter) {
      double distance_before = index - before;
      chords_with_harmonics[ibefore] += std::pow(std::cos(0.5 * M_PI * distance_before), 2) * weight;

      double distance_after = after - index;
      chords_with_harmonics[iafter] += std::pow(std::cos(0.5 * M_PI * distance_after), 2) * weight;
    } else {  // equal
      chords_with_harmonics[ibefore] += weight;
    }
    weight *= slope;
  }

  return chords_with_harmonics;
}

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
std::vector<double> AddMajorTriad(const std::vector<double>& chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope) {
  // Root
  const std::vector<double> chords_root = AddContributionHarmonics(chords, root, contribution, num_harmonics, slope);

  // Major 3rd
  int third = root + 4;
  if (third > 11) third -= 12;
  const std::vector<double> chords_third =
      AddContributionHarmonics(chords_root, third, contribution, num_harmonics, slope);

  // Perfect 5th
  int fifth = root + 7;
  if (fifth > 11) fifth -= 12;
  return AddContributionHarmonics(chords_third, fifth, contribution, num_harmonics, slope);
}

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
std::vector<double> AddMinorTriad(const std::vector<double>& chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope) {
  // Root
  const std::vector<double> chords_root = AddContributionHarmonics(chords, root, contribution, num_harmonics, slope);

  // Minor 3rd
  int third = root + 3;
  if (third > 11) third -= 12;
  const std::vector<double> chords_third =
      AddContributionHarmonics(chords_root, third, contribution, num_harmonics, slope);

  // Perfect 5th
  int fifth = root + 7;
  if (fifth > 11) fifth -= 12;
  return AddContributionHarmonics(chords_third, fifth, contribution, num_harmonics, slope);
}

/**
 * @brief Resizes and interpolates the profiles to fit the pcp size.
 *
 * @param pcp_size Number of array elements used to represent a semitone times 12.
 * @param key_profile Key profile.
 * @return std::tuple<std::vector<double>, double, double> Tuple of (resized profile, mean, standard deviation).
 */
std::tuple<std::vector<double>, double, double> ResizeProfileToPcpSize(const unsigned int pcp_size,
                                                                       const std::vector<double>& key_profile) {
  if (pcp_size < 12 || pcp_size % 12 != 0)
    throw std::runtime_error("Key: input PCP size is not a positive multiple of 12");

  unsigned int n = pcp_size / 12;
  std::vector<double> profile_do(static_cast<size_t>(pcp_size));

  for (int i = 0; i < 12; i++) {
    profile_do[i * n] = key_profile[i];

    // Two interpolated values
    double incr;
    if (i == 11) {
      incr = (key_profile[11] - key_profile[0]) / n;
    }

    else {
      incr = (key_profile[i] - key_profile[i + 1]) / n;
    }

    for (int j = 1; j <= (n - 1); j++) {
      profile_do[i * n + j] = key_profile[i] - j * incr;
    }
  }

  double mean_profile = fplus::mean<double, std::vector<double>>(profile_do);
  double std_profile = standard_deviation(mean_profile, profile_do);

  return std::tuple<std::vector<double>, double, double>{ profile_do, mean_profile, std_profile };
}

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
                   const int shift) {
  double r = 0.0;
  int size = static_cast<int>(v1.size());

  for (int i = 0; i < size; i++) {
    int index = (i - shift) % size;

    if (index < 0) {
      index += size;
    }

    r += (v1[i] - mean1) * (v2[index] - mean2);
  }

  r /= std1 * std2;

  return r;
}

/**
 * @brief This algorithm computes key estimate given a pitch class profile (HPCP).
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
 * and braw profiles
 * @return KeyOutput A struct containing the following:
 *      key: Estimated key, from A to G.
 *      scale: Scale of the key (major or minor).
 *      strength: Strength of the estimated key.
 *      first_to_second_relative_strength: The relative strength difference between the best estimate and second best
 *       estimate of the key.
 */
KeyOutput DetectKey(const std::vector<double>& pcp,
                    const bool use_polphony,
                    const bool use_three_chords,
                    const unsigned int num_harmonics,
                    const double slope,
                    const PolyphicProfile profile_type,
                    const bool use_maj_min) {
  unsigned int pcp_size = static_cast<unsigned int>(pcp.size());
  unsigned int n = pcp_size / 12;

  if (pcp_size < 12 || pcp_size % 12 != 0)
    throw std::runtime_error("Key: input PCP size is not a positive multiple of 12");

  std::vector<double> M;
  std::vector<double> m;
  std::vector<double> O(12, static_cast<double>(0.0));

  const std::vector<std::vector<double>> key_profile = SelectKeyProfile(profile_type);
  M = key_profile[0];
  m = key_profile[1];

  if (key_profile.size() == 3) {
    O = key_profile[2];
  }

  /*
  Assumptions:
    - We consider that the tonal hierarchy is kept when dealing with polyphonic sounds.
      That means that Krumhansl profiles are seen as the tonal hierarchy of
      each of the chords of the harmonic scale within a major/minor tonal contest.
    - We compute from these chord profiles the corresponding note (pitch class) profiles,
      which will be compared to HPCP values.

  Rationale:
    - Each note contribute to the different harmonics.
    - All the chords of the major/minor key are considered.

  Procedure:
    - First, profiles are initialized to 0
    - We take _M[i], n[i] as Krumhansl profiles i=1,...12 related to each of the chords
      of the major/minor key.
    - For each chord, we add its contribution to the three notes (pitch classes) of the chord.
      We use the same weight for all the notes of the chord.
    - For each note, we add its contribution to the different harmonics

    --essentia: https://github.com/MTG/essentia/blob/master/src/algorithms/tonal/key.cpp
  */

  std::vector<double> M_chords_empty(12, static_cast<double>(0.0));
  std::vector<double> m_chords_empty(12, static_cast<double>(0.0));

  // MAJOR KEY
  std::vector<double> M_chords = fplus::fwd::apply(
      M_chords_empty,
      // Tonic (I)
      [&M, &num_harmonics, &slope](auto chords) { return AddMajorTriad(chords, 0, M[0], num_harmonics, slope); },
      // II
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddMinorTriad(chords, 2, M[2], num_harmonics, slope);
        return chords;
      },
      // III
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddMinorTriad(chords, 4, M[4], num_harmonics, slope);
        return chords;
      },
      // Subdominant (IV)
      [&M, &num_harmonics, &slope](auto chords) { return AddMajorTriad(chords, 5, M[5], num_harmonics, slope); },
      // Dominant (V)
      [&M, &num_harmonics, &slope](auto chords) { return AddMajorTriad(chords, 7, M[7], num_harmonics, slope); },
      // VI
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddMinorTriad(chords, 9, M[9], num_harmonics, slope);
        return chords;
      },
      // VII (5th diminished)
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 11, M[11], num_harmonics, slope);
        return chords;
      },
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 2, M[11], num_harmonics, slope);
        return chords;
      },
      [&M, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 5, M[11], num_harmonics, slope);
        return chords;
      });

  // MINOR KEY
  std::vector<double> m_chords = fplus::fwd::apply(
      m_chords_empty,
      // Tonica (I)
      [&m, &num_harmonics, &slope](auto chords) { return AddMinorTriad(chords, 0, m[0], num_harmonics, slope); },
      // II (5th diminished)
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 2, m[2], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 5, m[2], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 8, m[2], num_harmonics, slope);
        return chords;
      },
      // III (5th augmented)
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 3, m[3], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 7, m[3], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 11, m[3], num_harmonics, slope);
        return chords;
      },
      // Subdominant (IV)
      [&m, &num_harmonics, &slope](auto chords) { return AddMinorTriad(chords, 5, m[5], num_harmonics, slope); },
      // Dominant (V) (harmonic minor scale)
      [&m, &num_harmonics, &slope](auto chords) { return AddMajorTriad(chords, 7, m[7], num_harmonics, slope); },
      // VI
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddMajorTriad(chords, 8, m[8], num_harmonics, slope);
        return chords;
      },
      // VII (diminished 5th)
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 11, m[8], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 2, m[8], num_harmonics, slope);
        return chords;
      },
      [&m, &num_harmonics, &slope, &use_three_chords](auto chords) {
        if (!use_three_chords) return AddContributionHarmonics(chords, 5, m[8], num_harmonics, slope);
        return chords;
      });

  std::vector<double> M_final;
  std::vector<double> m_final;

  if (use_polphony) {
    M_final = M_chords;
    m_final = m_chords;
  } else {
    M_final = M;
    m_final = m;
  }

  // RESIZE
  std::vector<double> profile_doM;
  double mean_profile_M;
  double std_profile_M = 0.;
  std::tie(profile_doM, mean_profile_M, std_profile_M) = ResizeProfileToPcpSize(pcp_size, M_final);

  std::vector<double> profile_dom;
  double mean_profile_m;
  double std_profile_m = 0.;
  std::tie(profile_dom, mean_profile_m, std_profile_m) = ResizeProfileToPcpSize(pcp_size, m_final);

  std::vector<double> profile_doO;
  double mean_profile_O;
  double std_profile_O = 0.;
  std::tie(profile_doO, mean_profile_O, std_profile_O) = ResizeProfileToPcpSize(pcp_size, O);

  // Compute correlation
  double mean_pcp = fplus::mean<double, std::vector<double>>(pcp);
  double std_pcp = standard_deviation(mean_pcp, pcp);

  // Compute correlation matrix
  int key_index = -1;            // index of the first maximum
  double max = -1;               // first maximum
  double max2 = -1;              // second maximum
  Scales scale = Scales::MAJOR;  // scale

  // Compute maximum for major, minor and other.
  double max_major = -1;
  double max_2_major = -1;
  int key_index_major = -1;

  double max_minor = -1;
  double max_2_minor = -1;
  int key_index_minor = -1;

  double max_other = -1;
  double max_2_other = -1;
  int key_index_other = -1;

  // calculate the correlation between the profiles and the PCP...
  // we shift the profile around to find the best match
  for (int shift = 0; shift < pcp_size; shift++) {
    double corr_major = Correlation(pcp, mean_pcp, std_pcp, profile_doM, mean_profile_M, std_profile_M, shift);
    // Compute maximum value for major keys
    if (corr_major > max_major) {
      max_2_major = max_major;
      max_major = corr_major;
      key_index_major = shift;
    }

    double corr_minor = Correlation(pcp, mean_pcp, std_pcp, profile_dom, mean_profile_m, std_profile_m, shift);
    // Compute maximum value for minor keys
    if (corr_minor > max_minor) {
      max_2_minor = max_minor;
      max_minor = corr_minor;
      key_index_minor = shift;
    }

    double corr_other = 0;
    if (use_maj_min) {
      corr_other = Correlation(pcp, mean_pcp, std_pcp, profile_doO, mean_profile_O, std_profile_O, shift);
      // Compute maximum value for other keys
      if (corr_other > max_other) {
        max_2_other = max_other;
        max_other = corr_other;
        key_index_other = shift;
      }
    }
  }

  if (max_major > max_minor && max_major > max_other) {
    key_index = static_cast<int>((key_index_major * 12 / pcp_size + 0.5));
    scale = Scales::MAJOR;
    max = max_major;
    max2 = max_2_major;
  }

  else if (max_minor >= max_major && max_minor >= max_other) {
    key_index = static_cast<int>((key_index_minor * 12 / pcp_size + 0.5));
    scale = Scales::MINOR;
    max = max_minor;
    max2 = max_2_minor;
  }

  else if (max_other > max_major && max_other > max_minor) {
    key_index = static_cast<int>((key_index_other * 12 / pcp_size + 0.5));
    scale = Scales::MAJMIN;
    max = max_other;
    max2 = max_2_other;
  }

  // In the case of Wei Chai algorithm, the scale is detected in a second step
  // In this point, always the major relative is detected, as it is the first
  // maximum
  if (profile_type == PolyphicProfile::Weichai) {
    if (scale == Scales::MINOR)
      throw std::runtime_error("Key: error in Wei Chai algorithm. Wei Chai algorithm does not support minor scales.");

    int fifth = key_index + 7 * n;
    if (fifth > pcp_size) fifth -= pcp_size;
    int sixth = key_index + 9 * n;
    if (sixth > pcp_size) sixth -= pcp_size;

    if (pcp[sixth] > pcp[fifth]) {
      key_index = sixth;
      key_index = static_cast<int>((key_index * 12 / pcp_size + .5));
      scale = Scales::MINOR;
    }
  }
  if (key_index < 0) {
    throw std::runtime_error("Key: key_index smaller than zero. Could not find key.");
  }

  const char* key_names[] = { "A", "Bb", "B", "C", "C#", "D", "Eb", "E", "F", "F#", "G", "Ab" };
  const char* key = key_names[key_index];

  std::string final_scale;
  if (scale == Scales::MAJOR) {
    final_scale = "major";
  }

  else if (scale == Scales::MINOR) {
    final_scale = "minor";
  }

  else if (scale == Scales::MAJMIN) {
    final_scale = "majmin";
  }

  double strength = max;
  double first_to_second_relative_strength = (max - max2) / max;

  KeyOutput key_output;
  key_output.key = key;
  key_output.scale = final_scale;
  key_output.strength = strength;
  key_output.first_to_second_relative_strength = first_to_second_relative_strength;
  return key_output;
}

}  // namespace core
}  // namespace musher