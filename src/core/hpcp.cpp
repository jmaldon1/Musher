#include "src/core/hpcp.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <fplus/fplus.hpp>
#include <stdexcept>
#include <string>
#include <vector>

namespace musher {
namespace core {

int ArgMax(const std::vector<double> &vec) {
  if (vec.empty()) throw std::runtime_error("Trying to get max vector element of empty vector.");
  return std::max_element(vec.begin(), vec.end()) - vec.begin();
}

void AddContributionWithWeight(double freq,
                               double mag_lin,
                               double reference_frequency,
                               double window_size,
                               WeightType weight_type,
                               double harmonic_weight,
                               std::vector<double> &hpcp) {
  // TODO Change function from editing vector reference.
  int pcp_size = hpcp.size();
  int semitone = 12;
  double resolution = pcp_size / semitone;  // # of bins / semitone

  // Convert frequency in Hz to frequency in pcp_bin index.
  // note: this can be a negative value
  double pcp_bin_F = std::log2(freq / reference_frequency) * static_cast<double>(pcp_size);

  // Which bins are covered by the window centered at this frequency
  // note: this is not wrapped.
  int left_bin = static_cast<int>(std::ceil(pcp_bin_F - resolution * window_size / 2.0));
  int right_bin = static_cast<int>(std::floor(pcp_bin_F + resolution * window_size / 2.0));

  assert(right_bin - left_bin >= 0);

  // Apply weight to all bins in the window
  for (int i = left_bin; i <= right_bin; i++) {
    double distance = std::abs(pcp_bin_F - static_cast<double>(i)) / resolution;
    double Normalized_distance = distance / window_size;
    double weight = 0.;

    if (weight_type == COSINE) {
      weight = std::cos(M_PI * Normalized_distance);
    } else if (weight_type == SQUARED_COSINE) {
      weight = std::cos(M_PI * Normalized_distance);
      weight *= weight;
    }

    int iwrapped = i % pcp_size;
    if (iwrapped < 0) iwrapped += pcp_size;

    hpcp[iwrapped] += weight * (fplus::square(mag_lin) * fplus::square(harmonic_weight));
  }
}

void AddContributionWithoutWeight(double freq,
                                  double mag_lin,
                                  double reference_frequency,
                                  double harmonic_weight,
                                  std::vector<double> &hpcp) {
  // TODO Change function from editing vector reference.
  if (freq <= 0) return;

  // Original Fujishima algorithm, basically places the contribution in the
  // bin nearest to the given frequency
  int pcpsize = hpcp.size();

  double octave = std::log2(freq / reference_frequency);
  int pcpbin = static_cast<int>(std::round(pcpsize * octave));  // bin distance from ref frequency

  pcpbin %= pcpsize;
  if (pcpbin < 0) pcpbin += pcpsize;

  hpcp[pcpbin] += fplus::square(mag_lin) * fplus::square(harmonic_weight);
}

void AddContribution(double freq,
                     double mag_lin,
                     double reference_frequency,
                     double window_size,
                     WeightType weight_type,
                     std::vector<HarmonicPeak> harmonic_peaks,
                     std::vector<double> &hpcp) {
  // TODO Change function from editing vector reference.
  std::vector<HarmonicPeak>::const_iterator it;

  for (it = harmonic_peaks.begin(); it != harmonic_peaks.end(); it++) {
    // Calculate the frequency of the hypothesized fundmental frequency. The
    // harmonic_peaks data structure always includes at least one element,
    // whose semitone value is 0, thus making this first iteration be freq == f
    double f = freq * std::pow(2., -(*it).semitone / 12.0);
    double harmonic_weight = (*it).harmonic_strength;

    if (weight_type != NONE) {
      AddContributionWithWeight(f, mag_lin, reference_frequency, window_size, weight_type, harmonic_weight, hpcp);
    } else {
      AddContributionWithoutWeight(f, mag_lin, reference_frequency, harmonic_weight, hpcp);
    }
  }
}

std::vector<HarmonicPeak> InitHarmonicContributionTable(int harmonics) {
  std::vector<HarmonicPeak> harmonic_peaks;
  const double precision = 0.00001;

  // Populate harmonic_peaks with the semitonal positions of each of the harmonics.
  for (int i = 0; i <= harmonics; i++) {
    double semitone = 12.0 * std::log2(i + 1.0);
    double octweight = std::max(1.0, (semitone / 12.0) * 0.5);

    // Get the semitone within the range (0-precision, 12.0-precision]
    while (semitone >= 12.0 - precision) {
      semitone -= 12.0;
    }

    // Check to see if the semitone has already been added to harmonic_peaks
    std::vector<HarmonicPeak>::iterator it;
    for (it = harmonic_peaks.begin(); it != harmonic_peaks.end(); it++) {
      if ((*it).semitone > semitone - precision && (*it).semitone < semitone + precision) break;
    }

    if (it == harmonic_peaks.end()) {
      // No harmonic peak found for this frequency; add it
      harmonic_peaks.push_back(HarmonicPeak(semitone, (1.0 / octweight)));
    } else {
      // Else, add the weight
      (*it).harmonic_strength += (1.0 / octweight);
    }
  }
  return harmonic_peaks;
}

std::vector<double> HPCP(const std::vector<double> &frequencies,
                         const std::vector<double> &magnitudes,
                         unsigned int size,
                         double reference_frequency,
                         unsigned int harmonics,
                         bool band_preset,
                         double band_split_frequency,
                         double min_frequency,
                         double max_frequency,
                         std::string _weight_type,
                         double window_size,
                         bool max_shifted,
                         bool non_linear,
                         std::string _normalized) {
  // Input validation
  if (size % 12 != 0) {
    throw std::runtime_error("HPCP: The size parameter is not a multiple of 12.");
  }

  if ((max_frequency - min_frequency) < 200.0) {
    throw std::runtime_error("HPCP: Minimum and maximum frequencies are too close");
  }

  if (band_preset) {
    if ((band_split_frequency - min_frequency) < 200.0) {
      throw std::runtime_error("HPCP: Low band frequency range too small");
    }
    if ((max_frequency - band_split_frequency) < 200.0) {
      throw std::runtime_error("HPCP: High band frequency range too small");
    }
  }

  if (magnitudes.size() != frequencies.size()) {
    throw std::runtime_error("HPCP: Frequency and magnitude input vectors are not of equal size");
  }

  if (window_size * size / 12 < 1.0) {
    throw std::runtime_error("HPCP: Your window_size needs to span at least one hpcp bin (window_size >= 12/size)");
  }

  WeightType weight_type;
  if (_weight_type == "none")
    weight_type = NONE;
  else if (_weight_type == "cosine")
    weight_type = COSINE;
  else if (_weight_type == "squared cosine")
    weight_type = SQUARED_COSINE;
  else {
    std::string err_message = "HPCP: Invalid weight type of: ";
    err_message += _weight_type;
    throw std::runtime_error(err_message);
  }

  NormalizeType Normalized;
  if (_normalized == "none")
    Normalized = N_NONE;
  else if (_normalized == "unit sum")
    Normalized = N_UNIT_SUM;
  else if (_normalized == "unit max")
    Normalized = N_UNIT_MAX;
  else {
    std::string err_message = "HPCP: Invalid Normalize type of: ";
    err_message += _normalized;
    throw std::runtime_error(err_message);
  }

  if (non_linear && Normalized != N_UNIT_MAX) {
    throw std::runtime_error("HPCP: Cannot apply non-linear filter when HPCP vector is not Normalized to unit max.");
  }
  // ========

  std::vector<HarmonicPeak> harmonic_peaks = InitHarmonicContributionTable(harmonics);
  std::vector<double> hpcp(size);

  std::vector<double> hpcp_LO;
  std::vector<double> hpcp_HI;

  if (band_preset) {
    hpcp_LO.resize(size);
    std::fill(hpcp_LO.begin(), hpcp_LO.end(), static_cast<double>(0.0));

    hpcp_HI.resize(size);
    std::fill(hpcp_HI.begin(), hpcp_HI.end(), static_cast<double>(0.0));
  }

  // Add each contribution of the spectral frequencies to the HPCP
  for (int i = 0; i < static_cast<int>(frequencies.size()); i++) {
    double freq = frequencies[i];
    double mag_lin = magnitudes[i];

    // Filter out frequencies not between min and max
    if (freq >= min_frequency && freq <= max_frequency) {
      if (band_preset) {
        AddContribution(freq, mag_lin, reference_frequency, window_size, weight_type, harmonic_peaks,
                        (freq < band_split_frequency) ? hpcp_LO : hpcp_HI);
      } else {
        AddContribution(freq, mag_lin, reference_frequency, window_size, weight_type, harmonic_peaks, hpcp);
      }
    }
  }

  if (band_preset) {
    if (Normalized == N_UNIT_MAX) {
      NormalizeInPlace(hpcp_LO);
      NormalizeInPlace(hpcp_HI);
    } else if (Normalized == N_UNIT_SUM) {
      // TODO does it makes sense to apply band preset together with unit sum normalization?
      NormalizeSumInPlace(hpcp_LO);
      NormalizeSumInPlace(hpcp_HI);
    }

    for (int i = 0; i < static_cast<int>(hpcp.size()); i++) {
      hpcp[i] = hpcp_LO[i] + hpcp_HI[i];
    }
  }

  if (Normalized == N_UNIT_MAX) {
    NormalizeInPlace(hpcp);
  } else if (Normalized == N_UNIT_SUM) {
    NormalizeSumInPlace(hpcp);
  }

  /* Perform the Jordi non-linear post-processing step
   This makes small values (below 0.6) even smaller
   while boosting further values close to 1. */
  if (non_linear) {
    for (int i = 0; i < static_cast<int>(hpcp.size()); i++) {
      hpcp[i] = std::sin(hpcp[i] * M_PI * 0.5);
      hpcp[i] *= hpcp[i];
      if (hpcp[i] < 0.6) {
        hpcp[i] *= hpcp[i] / 0.6 * hpcp[i] / 0.6;
      }
    }
  }

  /* Shift all of the elements so that the largest HPCP value is at index 0,
   only if this option is enabled. */
  if (max_shifted) {
    int idx_max = ArgMax(hpcp);
    std::vector<double> hpcp_bak = hpcp;
    for (int i = idx_max; i < static_cast<int>(hpcp.size()); i++) {
      hpcp[i - idx_max] = hpcp_bak[i];
    }
    int offset = hpcp.size() - idx_max;
    for (int i = 0; i < idx_max; i++) {
      hpcp[i + offset] = hpcp_bak[i];
    }
  }
  return hpcp;
}

std::vector<double> HPCP(const std::vector<std::tuple<double, double>> &peaks,
                         unsigned int size,
                         double reference_frequency,
                         unsigned int harmonics,
                         bool band_preset,
                         double band_split_frequency,
                         double min_frequency,
                         double max_frequency,
                         std::string _weight_type,
                         double window_size,
                         bool max_shifted,
                         bool non_linear,
                         std::string _normalized) {
  std::vector<double> frequencies(peaks.size());
  std::vector<double> magnitudes(peaks.size());

  std::transform(peaks.begin(), peaks.end(), frequencies.begin(), [](auto const &pair) { return std::get<0>(pair); });

  std::transform(peaks.begin(), peaks.end(), magnitudes.begin(), [](auto const &pair) { return std::get<1>(pair); });

  return HPCP(frequencies, magnitudes, size, reference_frequency, harmonics, band_preset, band_split_frequency,
              min_frequency, max_frequency, _weight_type, window_size, max_shifted, non_linear, _normalized);
}

}  // namespace core
}  // namespace musher
