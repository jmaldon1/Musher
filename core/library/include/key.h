#pragma once

#include <iostream>
#include <string>
#include <vector>

namespace musher {

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
std::tuple<std::vector<double>, double, double> resize_profile_to_pcp_size(int pcp_size,
                                                                           const std::vector<double>& profile_scale);
void detectKey(const std::vector<double>& pcp,
               bool use_polphony = true,
               bool use_three_chords = true,
               unsigned int num_harmonics = 4,
               double slope = 0.6,
               PolyphicProfile profile_type = PolyphicProfile::Bgate,
               unsigned int pcp_size = 36,
               bool use_maj_min = false);

}  // namespace musher
