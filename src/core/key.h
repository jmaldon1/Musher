#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace musher {
namespace core {

enum class PolyphicProfile {
  Diatonic,
  Krumhansl,
  Temperley,
  Weichai,
  Tonictriad,
  Temperley2005,
  Thpcp,
  Shaath,
  Gomez,
  Noland,
  Edmm,
  Bgate,
  Braw,
  Edma,
};

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

std::vector<double> AddContributionHarmonics(const std::vector<double>& M_chords,
                                             const int pitch_class,
                                             const double contribution,
                                             const int num_harmonics,
                                             const double slope);
std::vector<double> AddMajorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

std::vector<double> AddMinorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

std::vector<std::vector<double>> SelectKeyProfile(const PolyphicProfile profile_type);
std::tuple<std::vector<double>, double, double> ResizeProfileToPcpSize(const unsigned int pcp_size,
                                                                       const std::vector<double>& key_profile);
double Correlation(const std::vector<double>& v1,
                   const double mean1,
                   const double std1,
                   const std::vector<double>& v2,
                   const double mean2,
                   const double std2,
                   const int shift);
KeyOutput DetectKey(const std::vector<double>& pcp,
                    const bool use_polphony = true,
                    const bool use_three_chords = true,
                    const unsigned int num_harmonics = 4,
                    const double slope = 0.6,
                    const PolyphicProfile profile_type = PolyphicProfile::Bgate,
                    const bool use_maj_min = false);

}  // namespace core
}  // namespace musher
