#pragma once

#define _USE_MATH_DEFINES
#include <math.h>

#include <complex>
#include <fplus/fplus.hpp>
#include <fstream>
#include <functional>
#include <iostream>
#include <numeric>
#include <string>
#include <tuple>
#include <valarray>
#include <vector>

#include "src/third-party/pocketfft/pocketfft.h"

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

/**
 * @brief Convert uint8_t vector to hex string
 *
 * @param uint8_t::vector vector of uint8_t
 * @return std::string string of hex
 */
std::string Uint8VectorToHexString(const std::vector<uint8_t> &v);

/**
 * @brief Get string between two single quotes.
 * NOTE: There must only be 2 quotes in the entire string.
 *
 * @param s String that contains 2 single quotes
 * @return string between single quotes
 */
std::string StrBetweenSQuotes(const std::string &s);

/**
 * @brief Check if the architecture of the machine running the code is big endian.
 *
 * @return true If big endian.
 * @return false If not big endian.
 */
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

/**
 * @brief Deinterweave a vector in alternating order to form two vectors.
 * Example:
 *  interweaved_vector = {1, 9, 2, 8, 3, 7, 4, 6}
 *
 *  deinterweaved_vector = {
 *      {1, 2, 3, 4},
 *      {9, 8, 7, 6}
 *  }
 *
 * @param interweaved_vector Interleaved vector.
 * @return std::vector<std::vector<double>> Deinterweaved vector.
 */
std::vector<std::vector<double>> Deinterweave(const std::vector<double> &interweaved_vector);

/**
 * @brief Compute the median of a vector.
 *
 * @param inVec Input vector.
 * @return double Median.
 */
double Median(std::vector<double> &inVec);

/**
 * @brief Compute a one pole filter on an audio signal.
 *
 * @param vec Audio signal.
 * @return std::vector<double> Filtered audio signal.
 */
std::vector<double> OnePoleFilter(const std::vector<double> &vec);

/**
 * @brief Calculate an efficient length to pad the inputs of the FFT.
 *
 * Copied from Peter Bell.
 * https://gdoc.pub/doc/e/2PACX-1vR6iXXG1uS9ds47GvDgQk6XtpYzVTtYepu5B8onBrMmoorfKHhnHbN0ArDoXgoA23nZrcrm_DSFMW45
 *
 * @param n Original input size.
 * @return size_t Efficient FFT input size.
 */
size_t NextFastLen(size_t n);
using ldbl_t = typename std::conditional<sizeof(long double) == sizeof(double), double, long double>::type;
double NormFct(int inorm, size_t N);
double NormFct(int inorm,
               const pocketfft::shape_t &shape,
               const pocketfft::shape_t &axes,
               size_t fct = 1,
               int delta = 0);

/**
 * @brief Centered a vector with respect to the full discrete linear convolution of the input.
 *
 * @param vec Vector
 * @param new_shape New shape of of vector.
 * @return std::vector<double> Centered vector.
 */
std::vector<double> CenterVector(const std::vector<double> &vec, size_t new_shape);

/**
 * @brief Perform 'same' convolve of two 1-dimensional arrays using FFT.
 * Convolve `vec1` and `vec2` using the fast Fourier transform method.
 * The output is the same size as `in1`, centered with respect to the
 * full discrete linear convolution of the inputs.
 *
 * This function was heavily inspired by:
 *  https://github.com/scipy/scipy/blob/12fa74e97d3d18ca3a4e6991327663e88462f238/scipy/signal/signaltools.py#L551
 *  https://github.com/scipy/scipy/blob/master/scipy/fft/_pocketfft/pypocketfft.cxx
 *
 * @param vec1 Vector 1
 * @param vec2 Vector 2
 * @return std::vector<double> A 1-dimensional array containing a subset of the discrete linear
 * convolution of `in1` with `in2`.
 */
std::vector<double> FFTConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2);

/**
 * @brief Normalize a vector (to have an area of 1) and then scale by a factor of 2.
 *
 * @param input Input vector.
 * @return std::vector<double> Normalized vector.
 */
std::vector<double> Normalize(const std::vector<double> &input);

/**
 * @brief Blackmanharris windowing algorithm.
 * Window functions help control spectral leakage when doing Fourier Analysis.
 *
 * @param window Audio signal window.
 * @param a0 Constant a0.
 * @param a1 Constant a1.
 * @param a2 Constant a2.
 * @param a3 Constant a3.
 * @return std::vector<double> BlackmanHarris window.
 */
std::vector<double> BlackmanHarris(const std::vector<double> &window, double a0, double a1, double a2, double a3);
std::vector<double> BlackmanHarris62dB(const std::vector<double> &window);
std::vector<double> BlackmanHarris92dB(const std::vector<double> &window);

/**
 * @brief Applies windowing to an audio signal.
 * It optionally applies zero-phase windowing and optionally adds zero-padding. The resulting windowed frame size is
 * equal to the incoming frame size plus the number of padded zeros. By default, the available windows are Normalized
 * (to have an area of 1) and then scaled by a factor of 2.
 *
 * References:
 *  [1] F. J. Harris, On the use of windows for harmonic analysis with the discrete Fourier transform,
 *  Proceedings of the IEEE, vol. 66, no. 1, pp. 51-83, Jan. 1978
 *  [2] Window function - Wikipedia, the free encyclopedia, http://en.wikipedia.org/wiki/Window_function
 *
 * @param audio_frame Input audio frame.
 * @param window_type_func The window type function. Examples: BlackmanHarris92dB, BlackmanHarris62dB...
 * @param size Window size.
 * @param zero_padding_size Size of the zero-padding.
 * @param zero_phase Enables zero-phase windowing.
 * @param _Normalize Specify whether to Normalize windows (to have an area of 1) and then scale by a factor of 2.
 * @return std::vector<double> Windowed audio frame.
 */
std::vector<double> Windowing(
    const std::vector<double> &audio_frame,
    const std::function<std::vector<double>(const std::vector<double> &)> &window_type_func = BlackmanHarris62dB,
    unsigned size = 1024,
    unsigned zero_padding_size = 0,
    bool zero_phase = true,
    bool _Normalize = true);

/**
 * @brief Calculate the magnitude (absolute value or modulus) of a complex number.
 *
 * @param complex_pair Complex number. Contains 1 real and 1 imaginary number.
 * @return double The magnitude of a complex number.
 */
double Magnitude(const std::complex<double> complex_pair);

/**
 * @brief Computes the frequency spectrum of an array of Reals.
 * The resulting spectrum has a size which is half the size of the input array plus one.
 * Bins contain raw (linear) magnitude values.
 *
 * @param frame Input audio frame.
 * @return std::vector<double> Frequency spectrum of the input audio signal.
 */
std::vector<double> ConvertToFrequencySpectrum(const std::vector<double> &audio_frame);

/**
 * @brief Interpolate the peak of a parabola given 3 points on the parabola.
 * α(a) = left point value of parabola
 * β(b) = middle point value of parabola
 * γ(y) = right point value of parabola
 *
 * Interpolated peak location is given in bins (spectral samples) by:
 * p = 1/2 ((α - γ) / (α - 2β + γ))
 *
 * The peak magnitude estimate is:
 * y(p) = β - 1/4(α - γ)p
 *
 * Smith, J.O. "Quadratic Interpolation of Spectral Peaks", in
 * Spectral Audio Signal Processing,
 * https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html, online book,
 * 2011 edition,
 * accessed 12/18/2019.
 *
 * @param a Left point value of parabola.
 * @param b Middle point value of parabola.
 * @param y Right point value of parabola.
 * @param middle_point_index Position of the middle point in the parabola.
 * @return std::tuple<double, double> Tuple of (location (position) of the peak, peak height estimate).
 */
std::tuple<double, double> QuadraticInterpolation(double a, double b, double y, int middle_point_index);

/**
 * @brief Detects local maxima (peaks) in a vector.
 * The algorithm finds positive slopes and detects a peak when the slope changes sign and the peak is above the
 * threshold.
 *
 * @param inp Input vector.
 * @param threshold Peaks below this given threshold are not outputted.
 * @param interpolate Enables interpolation.
 * @param sort_by Ordering type of the outputted peaks (ascending by position
 * or descending by height).
 * @param max_num_peaks Maximum number of returned peaks.
 * @param range Input range.
 * @param min_pos Maximum position of the range to evaluate.
 * @param max_pos Minimum position of the range to evaluate.
 * @return std::vector<std::tuple<double, double>> Vector of peaks,
 * each peak being a tuple (positions, heights).
 */
std::vector<std::tuple<double, double>> PeakDetect(const std::vector<double> &inp,
                                                   double threshold = -1000.0,
                                                   bool interpolate = true,
                                                   std::string sort_by = "position",
                                                   int max_num_peaks = 0,
                                                   double range = 0,
                                                   int min_pos = 0,
                                                   int max_pos = 0);

/**
 * @brief Extracts peaks from a spectrum.
 * It is important to note that the peak algorithm is independent of an input that is linear or in dB, so one has to
 * adapt the threshold to fit with the type of data fed to it. The algorithm relies on PeakDetection algorithm which is
 * run with parabolic interpolation [1]. The exactness of the peak-searching depends heavily on the windowing type. It
 * gives best results with dB input, a blackman-harris 92dB window and interpolation set to true. According to [1],
 * spectral peak frequencies tend to be about twice as accurate when dB magnitude is used rather than just linear
 * magnitude. For further information about the peak detection, see the description of the PeakDetection algorithm.
 *
 * References:
 *  [1] Peak Detection, http://ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html
 *
 * @param input_spectrum Input spectrum.
 * @param threshold Peaks below this given threshold are not outputted.
 * @param sort_by Ordering type of the outputted peaks (ascending by frequency (position)
 * or descending by magnitude (height)).
 * @param max_num_peaks Maximum number of returned peaks.
 * @param sample_rate Sampling rate of the audio signal [Hz].
 * @param min_pos Maximum frequency (position) of the range to evaluate [Hz].
 * @param max_pos Minimum frequency (position) of the range to evaluate [Hz].
 * @return std::vector<std::tuple<double, double>> Vector of spectral peaks, each peak being a tuple (frequency,
 * magnitude).
 */
std::vector<std::tuple<double, double>> SpectralPeaks(const std::vector<double> &input_spectrum,
                                                      double threshold = -1000.0,
                                                      std::string sort_by = "position",
                                                      unsigned int max_num_peaks = 100,
                                                      double sample_rate = 44100.,
                                                      int min_pos = 0,
                                                      int max_pos = 0);

/**
 * @brief Add contribution to the HPCP with weight.
 *
 * @param freq Frequency [Hz]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 [Hz].
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param weight_type Type of weighting function for determining frequency contribution.
 * @param harmonic_weight Strength/weight of the harmonic.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContributionWithWeight(double freq,
                               double mag_lin,
                               double reference_frequency,
                               double window_size,
                               WeightType weight_type,
                               double harmonic_weight,
                               std::vector<double> &hpcp);

/**
 * @brief Add contribution to the HPCP without weight.
 *
 * @param freq Frequency [Hz]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 [Hz].
 * @param harmonic_weight Strength/weight of the harmonic.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContributionWithoutWeight(double freq,
                                  double mag_lin,
                                  double reference_frequency,
                                  double harmonic_weight,
                                  std::vector<double> &hpcp);

/**
 * @brief Adds the magnitude contribution of the given frequency as the tonic semitone.
 * As well as its possible contribution as a harmonic of another pitch.
 *
 * @param freq Frequency [Hz]
 * @param mag_lin Magnitude
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 [Hz].
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param weight_type Type of weighting function for determining frequency contribution.
 * @param harmonic_peaks Weighting table of harmonic contribution.
 * @param hpcp Harmonic pitch class profile.
 */
void AddContribution(double freq,
                     double mag_lin,
                     double reference_frequency,
                     double window_size,
                     WeightType weight_type,
                     std::vector<HarmonicPeak> harmonic_peaks,
                     std::vector<double> &hpcp);

/**
 * @brief Builds a weighting table of harmonic contribution.
 * Higher harmonics contribute less and the fundamental frequency has a full harmonic strength of 1.0.
 *
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @return std::vector<HarmonicPeak> Weighting table of harmonic contribution.
 */
std::vector<HarmonicPeak> InitHarmonicContributionTable(int harmonics);

/**
 * @brief Normalize a vector so its largest value gets mapped to 1.
 * If zero, the vector isn't touched.
 *
 * @tparam T
 * @param vec Vector to normalize.
 */
template <typename T>
void NormalizeInPlace(std::vector<T> &vec) {
  if (vec.empty()) return;

  T maxElement = *std::max_element(vec.begin(), vec.end());
  // T maxElement = fplus::maximum(vec);

  if (maxElement != (T)0.0) {
    for (size_t i = 0; i < vec.size(); i++) {
      vec[i] /= maxElement;
    }
  }
}

/**
 * @brief Normalize a vector so it's sum is equal to 1.
 * The vector is not touched if it contains negative elements or the sum is zero.
 *
 * @tparam T
 * @param vec Vector to normalize.
 */
template <typename T>
void NormalizeSum(std::vector<T> &vec) {
  if (vec.empty()) return;

  // T sumElements = std::accumulate(vec.begin(), vec.end(), (T) 0.0);
  T sumElements = (T)0.;
  for (size_t i = 0; i < vec.size(); ++i) {
    if (vec[i] < 0) return;
    sumElements += vec[i];
  }

  if (sumElements != (T)0.0) {
    for (size_t i = 0; i < vec.size(); ++i) {
      vec[i] /= sumElements;
    }
  }
}

/**
 * @brief Get the arg max of a vector.
 * Checks if the vector is empty first.
 *
 * @param vec Vector
 * @return int Arg max
 */
int ArgMax(const std::vector<double> &input);

/**
 * @brief Computes a Harmonic Pitch Class Profile (HPCP) from the spectral peaks of a signal.
 * HPCP is a k*12 dimensional vector which represents the intensities of the twelve (k==1) semitone pitch classes
 * (corresponsing to notes from A to G#), or subdivisions of these (k>1).
 *
 * @param frequencies Frequencies (positions) of the spectral peaks [Hz].
 * @param magnitudes Magnitudes (heights) of the spectral peaks.
 * @param size Size of the output HPCP (must be a positive nonzero multiple of 12).
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 [Hz].
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @param band_preset Enables whether to use a band preset.
 * @param band_split_frequency Split frequency for low and high bands, not used if bandPreset is false [Hz].
 * @param min_frequency Minimum frequency that contributes to the HPCP [Hz] (the difference between the min and split
 * frequencies must not be less than 200.0 Hz).
 * @param max_frequency Maximum frequency that contributes to the HPCP [Hz] (the difference between the max and split
 * frequencies must not be less than 200.0 Hz).
 * @param _weight_type Type of weighting function for determining frequency contribution.
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param sample_rate Sampling rate of the audio signal [Hz].
 * @param max_shifted Whether to shift the HPCP vector so that the maximum peak is at index 0.
 * @param non_linear Apply non-linear post-processing to the output (use with Normalized='unitMax'). Boosts values close
 * to 1, decreases values close to 0.
 * @param _Normalized Whether to Normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
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

/**
 * @brief Overloaded function for HPCP that accepts a vector of peaks. Refer to original HPCP function for more details.
 *
 * @param peaks Vector of spectral peaks, each peak being a tuple (frequency, magnitude).
 * @param size Size of the output HPCP (must be a positive nonzero multiple of 12).
 * @param reference_frequency Reference frequency for semitone index calculation, corresponding to A3 [Hz].
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @param band_preset Enables whether to use a band preset.
 * @param band_split_frequency Split frequency for low and high bands, not used if bandPreset is false [Hz].
 * @param min_frequency Minimum frequency that contributes to the HPCP [Hz] (the difference between the min and split
 * frequencies must not be less than 200.0 Hz).
 * @param max_frequency Maximum frequency that contributes to the HPCP [Hz] (the difference between the max and split
 * frequencies must not be less than 200.0 Hz).
 * @param _weight_type Type of weighting function for determining frequency contribution.
 * @param window_size Size, in semitones, of the window used for the weighting.
 * @param sample_rate Sampling rate of the audio signal [Hz].
 * @param max_shifted Whether to shift the HPCP vector so that the maximum peak is at index 0.
 * @param non_linear Apply non-linear post-processing to the output (use with Normalized='unitMax'). Boosts values close
 * to 1, decreases values close to 0.
 * @param _Normalized Whether to Normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
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

  /**
   * @brief Computes the actual slicing of the frames, this function is run on each
   * iteration to calculate the next frame. This function should not be called by the user,
   * it will be called interally while iterating.
   *
   * Usage example:
   *   Framecutter framecutter(audio_signal);
   *
   *   for (const std::vector<double> &frame : framecutter) {
   *       perform_work_on_frame(frame);
   *   }
   *
   * @return std::vector<double> Sliced frame.
   */
  std::vector<double> compute();
};

/**
 * @brief Downmixes the signal into a single channel given a stereo signal. If the signal was already a monoaural, it is
 * left unchanged.
 *
 * @param input Stereo or mono audio signal
 * @return std::vector<double> Downmixed audio signal
 */
std::vector<double> monoMixer(const std::vector<std::vector<double>> &input);

/**
 * @brief Calculate the standard deviation of a vector.
 *
 * @param mean Mean (Average)
 * @param vec Vector
 * @return double Standard devation
 */
double standard_deviation(double mean, const std::vector<double> &vec);

}  // namespace core
}  // namespace musher