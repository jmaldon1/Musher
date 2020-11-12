#pragma once

#define NOMINMAX
#define _USE_MATH_DEFINES
#include <math.h>
#include <minimp3/minimp3_ex.h>
#include <pocketfft/pocketfft.h>

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

namespace musher {
namespace core {

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
 * @param max_num_peaks Maximum number of returned peaks (set to 0 to return all peaks).
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
 * @param max_num_peaks Maximum number of returned peaks (set to 0 to return all peaks).
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
 * @brief Downmixes the signal into a single channel given a stereo signal. If the signal was already a monoaural, it is
 * left unchanged.
 *
 * @param input Stereo or mono audio signal
 * @return std::vector<double> Downmixed audio signal
 */
std::vector<double> MonoMixer(const std::vector<std::vector<double>> &input);

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