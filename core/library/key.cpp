#include "key.h"

#include <cmath>

#include "utils.h"

namespace musher {

std::vector<std::vector<double>> selectKeyProfile(PolyphicProfile profile_type) {
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

    /* -- Profiles with other -- */

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
        /* -- Profiles with other -- */
        case PolyphicProfile::Bgate:
            return bgate;
        case PolyphicProfile::Braw:
            return braw;
        case PolyphicProfile::Edma:
            return edma;
    }
}

/**
  Function that adds the contribution of a chord with root note 'root' to its major triad
  A major triad includes notes from three different classes of pitch: the root, the major 3rd and perfect 5th.
  This is the most relaxed, most consonant chord in all of harmony.
  @see http://www.songtrellis.com/directory/1146/chordTypes/majorChordTypes/majorTriad
  The three notes of the chord have the same weight
*/
std::vector<double> addMajorTriad(const std::vector<double>& chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope) {
    // Root
    const std::vector<double> chords_root =
        addContributionHarmonics(chords, root, contribution, num_harmonics, slope);

    // Major 3rd
    int third = root + 4;
    if (third > 11) third -= 12;
    const std::vector<double> chords_third =
        addContributionHarmonics(chords_root, third, contribution, num_harmonics, slope);

    // Perfect 5th
    int fifth = root + 7;
    if (fifth > 11) fifth -= 12;
    return addContributionHarmonics(chords_third, fifth, contribution, num_harmonics, slope);
}

/**
  Function that adds the contribution of a chord with root note 'root' to its major triad
  A major triad includes notes from three different classes of pitch: the root, the major 3rd and perfect 5th.
  This is the most relaxed, most consonant chord in all of harmony.
  @see http://www.songtrellis.com/directory/1146/chordTypes/majorChordTypes/majorTriad
  The three notes of the chord have the same weight
*/
std::vector<double> addMinorTriad(const std::vector<double>& chords,
                                  const int root,
                                  const double contribution,
                                  const int num_harmonics,
                                  const double slope) {
    // Root
    const std::vector<double> chords_root =
        addContributionHarmonics(chords, root, contribution, num_harmonics, slope);

    // Minor 3rd
    int third = root + 3;
    if (third > 11) third -= 12;
    const std::vector<double> chords_third =
        addContributionHarmonics(chords_root, third, contribution, num_harmonics, slope);

    // Perfect 5th
    int fifth = root + 7;
    if (fifth > 11) fifth -= 12;
    return addContributionHarmonics(chords_third, fifth, contribution, num_harmonics, slope);
}

std::vector<double> addContributionHarmonics(const std::vector<double>& chords,
                                             const int pitch_class,
                                             const double contribution,
                                             const int num_harmonics,
                                             const double slope) {
    double weight = contribution;
    std::vector<double> chords_with_harmonics(chords);

    for (int iHarm = 1; iHarm <= num_harmonics; iHarm++) {
        double index = pitch_class + 12 * std::log2((double)iHarm);

        double before = std::floor(index);
        double after = std::ceil(index);

        int ibefore = (int)std::fmod((double)before, (double)12.0);
        int iafter = (int)std::fmod((double)after, (double)12.0);

        // weight goes proportionally to ibefore & iafter
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

double mean(const std::vector<double>& vec) {
    double sum = std::accumulate(std::begin(vec), std::end(vec), 0.0);
    return sum / vec.size();
}

std::tuple<std::vector<double>, double, double> resize_profile_to_pcp_size(int pcp_size,
                                                                           const std::vector<double>& profile_scale) {
    int n = pcp_size / 12;

    std::vector<double> profile_do(static_cast<size_t>(pcp_size));

    for (int i = 0; i < 12; i++) {
        profile_do[i * n] = profile_scale[i];

        // Two interpolated values
        // Real incr_M, incr_m, incr_O;
        double incr;
        if (i == 11) {
            incr = (profile_scale[11] - profile_scale[0]) / n;
        }

        else {
            incr = (profile_scale[i] - profile_scale[i + 1]) / n;
        }

        for (int j = 1; j <= (n - 1); j++) {
            profile_do[i * n + j] = profile_scale[i] - j * incr;
        }
    }

    double mean_profile = mean(profile_do);

    // Computer standard devation
    double accum = 0.0;
    std::for_each(std::begin(profile_do), std::end(profile_do),
                  [&](const double d) { accum += (d - mean_profile) * (d - mean_profile); });
    double std_profile = sqrt(accum / (profile_do.size() - 1));

    return std::tuple<std::vector<double>, double, double>{ profile_do, mean_profile, std_profile };
}

void detectKey(const std::vector<double>& pcp,
               bool use_polphony,
               bool use_three_chords,
               unsigned int num_harmonics,
               double slope,
               PolyphicProfile profile_type,
               unsigned int pcp_size,
               bool use_maj_min) {
    std::vector<double> M;
    std::vector<double> m;
    std::vector<double> O(12, (double)0.0);

    const std::vector<std::vector<double>> key_profile = selectKeyProfile(profile_type);
    M = key_profile[0];
    m = key_profile[1];

    if (key_profile.size() == 3) {
        O = key_profile[2];
    }

    std::vector<double> M_chords(12, (double)0.0);
    std::vector<double> m_chords(12, (double)0.0);

    std::vector<double> major_tonic;
    std::vector<double> major_I = addMajorTriad(M_chords, 0, M[0], num_harmonics, slope);

    if (use_three_chords) {
        major_tonic = major_I;
    } else {
        std::vector<double> major_II = addMinorTriad(major_I, 2, M[2], num_harmonics, slope);
        std::vector<double> major_III = addMinorTriad(major_II, 4, M[4], num_harmonics, slope);
        major_tonic = major_III;
    }

    std::vector<double> major_subdominant_IV = addMajorTriad(major_tonic, 5, M[5], num_harmonics, slope);
    std::vector<double> major_dominant_V = addMajorTriad(major_subdominant_IV, 7, M[7], num_harmonics, slope);
    std::vector<double> major_VI;
    std::vector<double> major_VII;

    if (use_three_chords) {
        major_VI = addMinorTriad(major_dominant_V, 9, M[9], num_harmonics, slope);

        major_VII = major_VI;
        std::vector<int> major_VII_pitch_classes({ 11, 2, 5 });

        for (const int& pitch_class : major_VII_pitch_classes) {
            major_VII = addContributionHarmonics(major_VII, pitch_class, M[11], num_harmonics, slope);
        }
    }

    /* Minor Key */
    std::vector<double> minor_tonic_I = addMinorTriad(m_chords, 0, m[0], num_harmonics, slope);
    std::vector<double> minor_II;
    std::vector<double> minor_III;

    if (!use_three_chords) {
        minor_II = minor_tonic_I;
        std::vector<int> minor_II_pitch_classes({ 2, 5, 8 });

        for (const int& pitch_class : minor_II_pitch_classes) {
            minor_II = addContributionHarmonics(minor_II, pitch_class, m[2], num_harmonics, slope);
        }

        minor_III = minor_II;
        std::vector<int> minor_III_pitch_classes({ 3, 7, 11 });

        for (const int& pitch_class : minor_III_pitch_classes) {
            minor_III = addContributionHarmonics(minor_III, pitch_class, m[3], num_harmonics, slope);
        }
    }

    // std::vector<double> minor_subdominant_IV = addMinorTriad(minor_III, 5, m[5], num_harmonics, slope);
    // std::vector<double> minor_dominant_V = addMajorTriad(minor_subdominant_IV, 7, M[7], num_harmonics, slope);
    // std::vector<double> minor_VI;
    // std::vector<double> minor_VII;

    // if (!use_three_chords) {
    //     minor_VI = addMajorTriad(minor_dominant_V, 8, M[8], num_harmonics, slope);

    //     minor_VII = minor_VI;
    //     std::vector<int> minor_VII_pitch_classes({ 11, 2, 5 });

    //     for (const int& pitch_class : minor_VII_pitch_classes) {
    //         minor_VII = addContributionHarmonics(minor_VII, pitch_class, M[8], num_harmonics, slope);
    //     }
    // }

    // if (use_polphony) {
    //     M = major_VII;
    //     m = minor_VII;
    // }

    // // RESIZE
    // std::vector<double> profile_doM;
    // double mean_profile_M;
    // double std_profile_M;
    // std::tie(profile_doM, mean_profile_M, std_profile_M) = resize_profile_to_pcp_size(pcp_size, M);

    // std::vector<double> profile_dom;
    // double mean_profile_m;
    // double std_profile_m;
    // std::tie(profile_dom, mean_profile_m, std_profile_m) = resize_profile_to_pcp_size(pcp_size, m);

    // std::vector<double> profile_doO;
    // double mean_profile_O;
    // double std_profile_O;
    // std::tie(profile_doO, mean_profile_O, std_profile_O) = resize_profile_to_pcp_size(pcp_size, O);
}

};  // namespace musher