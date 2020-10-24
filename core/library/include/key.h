#pragma once

#include <string>
#include <vector>
#include <iostream>

namespace musher
{

enum class PolyphicProfile
{
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

struct KeyOutput
{
    std::string key;
    std::string scale;
    double strength;
    double first_to_second_relative_strength;
};

std::vector<std::vector<double>> selectKeyProfile(PolyphicProfile profile_type);
void detectKey(const std::vector<double> &pcp,
                bool use_polphony = true,
                bool use_three_chords = true,
                unsigned int num_harmonics = 4,
                double slope = 0.6,
                PolyphicProfile profile_type = PolyphicProfile::Bgate,
                unsigned int pcp_size = 36,
                bool use_maj_min = false);

} // namespace musher
