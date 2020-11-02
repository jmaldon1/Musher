#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <complex>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <valarray>
#include <variant>
#include <vector>

#include "musher/third-party/pocketfft/pocketfft.h"

namespace musher {
namespace core {

enum WeightType { NONE, COSINE, SQUARED_COSINE };
enum NormalizeType { N_NONE, N_UNIT_MAX, N_UNIT_SUM };

struct HarmonicPeak {
  double semitone;
  double harmonic_strength;

  /* Constructor */
  HarmonicPeak(double semitone, double harmonic_strength = 0.0)
      : semitone(semitone), harmonic_strength(harmonic_strength){};
};

std::string Uint8VectorToHexString(const std::vector<uint8_t> &v);
std::string StrBetweenSQuotes(const std::string &s);
bool IsBigEndian(void);
int16_t TwoBytesToInt(const std::vector<uint8_t> &source, const int startIndex);
int32_t FourBytesToInt(const std::vector<uint8_t> &source, const int startIndex);
double NormalizeInt8_t(const uint8_t sample);
double NormalizeInt16_t(const int16_t sample);
double NormalizeInt32_t(const int32_t sample);

// template <typename T>
// void outputVectorToFile(const std::vector<T>& vec, std::string& filename)
// {
//     std::ofstream os;
//     os.open(filename);

//     os << "{\n";
//     for (int i = 0; i < vec.size(); ++i)
//     {
//         os << vec[i] << ", ";
//         if (i % 32 == 0)
//             os << "\n";
//     }
//     os << "};";
// }

// /* Allow std::visit to be overloaded with as many lambda funcs as we want */
// template <class ...Fs>
// struct overload : Fs... {
//     overload(Fs const&... fs) : Fs{fs}...
//     {}

//     using Fs::operator()...;
// };

std::vector<double> Interleave2Vectors(const std::vector<double> &vec1, const std::vector<double> &vec2);
double Median(std::vector<double> &inVec);
std::vector<double> OnePoleFilter(const std::vector<double> &vec);
size_t NextFastLen(size_t n);

using ldbl_t = typename std::conditional<sizeof(long double) == sizeof(double), double, long double>::type;

double NormFct(int inorm, size_t N);
double NormFct(int inorm,
               const pocketfft::shape_t &shape,
               const pocketfft::shape_t &axes,
               size_t fct = 1,
               int delta = 0);

std::vector<double> CenterVector(const std::vector<double> &vec, size_t new_shape);
std::vector<double> FFTConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2);
std::vector<double> Normalize(const std::vector<double> &input);
std::vector<double> BlackmanHarris(const std::vector<double> &window, double a0, double a1, double a2, double a3);
std::vector<double> BlackmanHarris62dB(const std::vector<double> &window);
std::vector<double> BlackmanHarris92dB(const std::vector<double> &window);
std::vector<double> Windowing(
    const std::vector<double> &audio_frame,
    const std::function<std::vector<double>(const std::vector<double> &)> &window_type_func = BlackmanHarris62dB,
    unsigned size = 1024,
    unsigned zero_padding_size = 0,
    bool zero_phase = true,
    bool _Normalize = true);

double Magnitude(const std::complex<double> complex_pair);
std::vector<double> ConvertToFrequencySpectrum(const std::vector<double> &audio_frame);
std::tuple<double, double> QuadraticInterpolation(double a, double b, double y, int middle_point_index);

std::vector<std::tuple<double, double>> PeakDetect(const std::vector<double> &inp,
                                                   double threshold = -1000.0,
                                                   bool interpolate = true,
                                                   std::string sort_by = "position",
                                                   int max_num_peaks = 0,
                                                   double range = 0,
                                                   int min_pos = 0,
                                                   int max_pos = 0);

std::vector<std::tuple<double, double>> SpectralPeaks(const std::vector<double> &input_spectrum,
                                                      double threshold = -1000.0,
                                                      std::string sort_by = "position",
                                                      unsigned int max_num_peaks = 100,
                                                      double sample_rate = 44100.,
                                                      int min_pos = 0,
                                                      int max_pos = 0);

void AddContributionWithWeight(double freq,
                               double mag_lin,
                               double reference_frequency,
                               double window_size,
                               WeightType weight_type,
                               double harmonic_weight,
                               std::vector<double> &hpcp);
void AddContribution(double freq,
                     double mag_lin,
                     double reference_frequency,
                     double window_size,
                     WeightType weight_type,
                     std::vector<HarmonicPeak> harmonic_peaks,
                     std::vector<double> &hpcp);
std::vector<HarmonicPeak> InitHarmonicContributionTable(int harmonics);

// Normalize a vector so its largest value gets mapped to 1
// if zero, the vector isn't touched
template <typename T>
void NormalizeInPlace(std::vector<T> &array) {
  if (array.empty()) return;

  T maxElement = *std::max_element(array.begin(), array.end());

  if (maxElement != (T)0.0) {
    for (size_t i = 0; i < array.size(); i++) {
      array[i] /= maxElement;
    }
  }
}

// Normalize a vector so it's sum is equal to 1. the vector is not touched if
// it contains negative elements or the sum is zero
template <typename T>
void NormalizeSum(std::vector<T> &array) {
  if (array.empty()) return;

  // T sumElements = std::accumulate(array.begin(), array.end(), (T) 0.0);
  T sumElements = (T)0.;
  for (size_t i = 0; i < array.size(); ++i) {
    if (array[i] < 0) return;
    sumElements += array[i];
  }

  if (sumElements != (T)0.0) {
    for (size_t i = 0; i < array.size(); ++i) {
      array[i] /= sumElements;
    }
  }
}

int MaxVectorElement(const std::vector<double> &input);

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
                         double sample_rate = 44100.,
                         bool max_shifted = false,
                         bool non_linear = false,
                         std::string _Normalized = "unit max");

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
                         double sample_rate = 44100.,
                         bool max_shifted = false,
                         bool non_linear = false,
                         std::string _Normalized = "unit max");

/**
 * @brief This class should be treated like an iterator.
 *
 * Usage example:
 *   Framecutter framecutter(audio_signal);
 *
 *   for (const std::vector<double> &frame : framecutter) {
 *       perform_work_on_frame(frame);
 *   }
 *
 */
class Framecutter {
 private:
  const std::vector<double> buffer_;
  const int frame_size_;
  const int hop_size_;
  const bool start_from_center_;
  const bool last_frame_to_end_of_file_;
  const double valid_frame_threshold_ratio_;
  int start_index_;
  bool last_frame_;
  std::vector<double> frame_;

 public:
  Framecutter(const std::vector<double> buffer,
              int frame_size = 1024,
              int hop_size = 512,
              bool start_from_center = true,
              bool last_frame_to_end_of_file = false,
              double valid_frame_threshold_ratio = 0.)
      : buffer_(buffer),
        frame_size_(frame_size),
        hop_size_(hop_size),
        start_from_center_(start_from_center),
        last_frame_to_end_of_file_(last_frame_to_end_of_file),
        valid_frame_threshold_ratio_(valid_frame_threshold_ratio),
        start_index_(0),
        last_frame_(false),
        frame_(compute()) {}

  ~Framecutter() {}

  // Iterable functions
  const Framecutter &begin() const { return *this; }
  const Framecutter &end() const { return *this; }

  // Iterator functions
  // Keep iterating while frame is not empty.
  bool operator!=(const Framecutter &) const { return !frame_.empty(); }
  void operator++() { frame_ = compute(); }
  std::vector<double> operator*() const { return frame_; }

  // Compute
  std::vector<double> compute();
};

std::vector<double> monoMixer(const std::vector<std::vector<double>> &input);
double standard_deviation(double mean, const std::vector<double> &vec);

}  // namespace core
}  // namespace musher