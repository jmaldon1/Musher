#include "key.h"

namespace musher
{

std::vector<std::vector<double>> selectKeyProfile(PolyphicProfile profile_type)
{
    const std::vector<std::vector<double>> diatonic{
        {1, 0, 1, 0, 1, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1, 1, 0, 1, 0, 1, 1, 0, 0, 1}};

    const std::vector<std::vector<double>> krumhansl{
        {6.35, 2.23, 3.48, 2.33, 4.38, 4.09, 2.52, 5.19, 2.39, 3.66, 2.29, 2.88},
        {6.33, 2.68, 3.52, 5.38, 2.60, 3.53, 2.54, 4.75, 3.98, 2.69, 3.34, 3.17}};

    const std::vector<std::vector<double>> temperley{
        {5.0, 2.0, 3.5, 2.0, 4.5, 4.0, 2.0, 4.5, 2.0, 3.5, 1.5, 4.0},
        {5.0, 2.0, 3.5, 4.5, 2.0, 4.0, 2.0, 4.5, 3.5, 2.0, 1.5, 4.0}};

    const std::vector<std::vector<double>> weichai{
        {81302, 320, 65719, 1916, 77469, 40928, 2223, 83997, 1218, 39853, 1579, 28908},
        {39853, 1579, 28908, 81302, 320, 65719, 1916, 77469, 40928, 2223, 83997, 1218}};

    const std::vector<std::vector<double>> tonictriad{
        {1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
        {1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0}};

    const std::vector<std::vector<double>> temperley2005{
        {0.748, 0.060, 0.488, 0.082, 0.67, 0.46, 0.096, 0.715, 0.104, 0.366, 0.057, 0.4},
        {0.712, 0.084, 0.474, 0.618, 0.049, 0.46, 0.105, 0.747, 0.404, 0.067, 0.133, 0.33}};

    const std::vector<std::vector<double>> thpcp{
        {0.95162, 0.20742, 0.71758, 0.22007, 0.71341, 0.48841, 0.31431, 1.00000, 0.20957, 0.53657, 0.22585, 0.55363},
        {0.94409, 0.21742, 0.64525, 0.63229, 0.27897, 0.57709, 0.26428, 1.0000, 0.26428, 0.30633, 0.45924, 0.35929}};

    const std::vector<std::vector<double>> shaath{
        {6.6, 2.0, 3.5, 2.3, 4.6, 4.0, 2.5, 5.2, 2.4, 3.7, 2.3, 3.4},
        {6.5, 2.7, 3.5, 5.4, 2.6, 3.5, 2.5, 5.2, 4.0, 2.7, 4.3, 3.2}};

    const std::vector<std::vector<double>> gomez{
        {0.82, 0.00, 0.55, 0.00, 0.53, 0.30, 0.08, 1.00, 0.00, 0.38, 0.00, 0.47},
        {0.81, 0.00, 0.53, 0.54, 0.00, 0.27, 0.07, 1.00, 0.27, 0.07, 0.10, 0.36}};

    const std::vector<std::vector<double>> noland{
        {0.0629, 0.0146, 0.061, 0.0121, 0.0623, 0.0414, 0.0248, 0.0631, 0.015, 0.0521, 0.0142, 0.0478},
        {0.0682, 0.0138, 0.0543, 0.0519, 0.0234, 0.0544, 0.0176, 0.067, 0.0349, 0.0297, 0.0401, 0.027}};

    const std::vector<std::vector<double>> edmm{
        {0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083, 0.083},
        {0.17235348, 0.04, 0.0761009, 0.12, 0.05621498, 0.08527853, 0.0497915, 0.13451001, 0.07458916, 0.05003023, 0.09187879, 0.05545106}};

    const std::vector<std::vector<double>> bgate{
        {1.00, 0.00, 0.42, 0.00, 0.53, 0.37, 0.00, 0.77, 0.00, 0.38, 0.21, 0.30},
        {1.00, 0.00, 0.36, 0.39, 0.00, 0.38, 0.00, 0.74, 0.27, 0.00, 0.42, 0.23},
        {1.00, 0.26, 0.35, 0.29, 0.44, 0.36, 0.21, 0.78, 0.26, 0.25, 0.32, 0.26}};

    const std::vector<std::vector<double>> braw{
        {1.0000, 0.1573, 0.4200, 0.1570, 0.5296, 0.3669, 0.1632, 0.7711, 0.1676, 0.3827, 0.2113, 0.2965},
        {1.0000, 0.2330, 0.3615, 0.3905, 0.2925, 0.3777, 0.1961, 0.7425, 0.2701, 0.2161, 0.4228, 0.2272},
        {1.0000, 0.2608, 0.3528, 0.2935, 0.4393, 0.3580, 0.2137, 0.7809, 0.2578, 0.2539, 0.3233, 0.2615}};

    const std::vector<std::vector<double>> edma{
        {1.00, 0.29, 0.50, 0.40, 0.60, 0.56, 0.32, 0.80, 0.31, 0.45, 0.42, 0.39},
        {1.00, 0.31, 0.44, 0.58, 0.33, 0.49, 0.29, 0.78, 0.43, 0.29, 0.53, 0.32},
        {1.00, 0.26, 0.35, 0.29, 0.44, 0.36, 0.21, 0.78, 0.26, 0.25, 0.32, 0.26}};

    switch (profile_type)
    {
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
    case PolyphicProfile::Bgate:
        return bgate;
    case PolyphicProfile::Braw:
        return braw;
    case PolyphicProfile::Edma:
        return edma;
    }
}

void detectKey(const std::vector<double> &pcp,
                bool use_polphony,
                bool use_three_chords,
                unsigned int num_harmonics,
                double slope,
                PolyphicProfile profile_type,
                unsigned int pcp_size,
                bool use_maj_min)
{
}

}; // namespace musher