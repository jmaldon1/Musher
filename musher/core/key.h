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

std::vector<double> addMajorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

std::vector<double> addMinorTriad(const std::vector<double>& M_chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope);

std::vector<double> addContributionHarmonics(const std::vector<double>& M_chords,
                                             const int pitch_class,
                                             const double contribution,
                                             const int num_harmonics,
                                             const double slope);

std::vector<std::vector<double>> selectKeyProfile(PolyphicProfile profile_type);
std::tuple<std::vector<double>, double, double> resize_profile_to_pcp_size(unsigned int pcp_size,
                                                                           const std::vector<double>& profile_scale);
double correlation(const std::vector<double>& v1,
                   const double mean1,
                   const double std1,
                   const std::vector<double>& v2,
                   const double mean2,
                   const double std2,
                   const int shift);
KeyOutput detectKey(const std::vector<double>& pcp,
                    const bool use_polphony = true,
                    const bool use_three_chords = true,
                    const unsigned int num_harmonics = 4,
                    const double slope = 0.6,
                    const PolyphicProfile profile_type = PolyphicProfile::Bgate,
                    const unsigned int pcp_size = 36,
                    const bool use_maj_min = false);

}  // namespace core
}  // namespace musher
