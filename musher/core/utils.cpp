#include "musher/core/utils.h"

#include <assert.h>

#include <complex>
#include <fplus/fplus.hpp>
#include <functional>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <type_traits>
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
std::string uint8VectorToHexString(const std::vector<uint8_t> &v) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  std::vector<uint8_t>::const_iterator it;

  for (it = v.begin(); it != v.end(); it++) {
    ss << std::setw(2) << static_cast<unsigned>(*it);
  }

  return ss.str();
}

/**
 * @brief Get string between two single quotes.
 * NOTE: There must only be 2 quotes in the entire string.
 *
 * @param s String that contains 2 single quotes
 * @return string between single quotes
 */
std::string strBetweenSQuotes(const std::string &s) {
  std::string squote = "'";
  // Find 1st quote starting from beginning of string
  std::size_t pos = s.find(squote) + 1;
  // Find 2nd quote starting from end of string
  std::size_t endpos = s.rfind(squote);
  // Find length of string between single quotes
  std::size_t len = endpos - pos;

  // Return the substring between single quotes
  return s.substr(pos, len);
}

int16_t twoBytesToInt(const std::vector<uint8_t> &source, const int startIndex) {
  int16_t result;

  if (!isBigEndian())
    result = (source[startIndex + 1] << 8) | source[startIndex];
  else
    result = (source[startIndex] << 8) | source[startIndex + 1];

  return result;
}

int32_t fourBytesToInt(const std::vector<uint8_t> &source, const int startIndex) {
  int32_t result;

  if (!isBigEndian())
    result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) |
             source[startIndex];
  else
    result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) |
             source[startIndex + 3];

  return result;
}

double normalizeInt8_t(const uint8_t sample) { return static_cast<double>(sample - 128) / static_cast<double>(128.); }

double normalizeInt16_t(const int16_t sample) { return static_cast<double>(sample) / static_cast<double>(32768.); }

double normalizeInt32_t(const int32_t sample) { return static_cast<double>(sample) / static_cast<double>(8388608.); }

/**
 * @brief Check if the architecture of the machine running the code is big endian.
 * 
 * @return true If big endian.
 * @return false If not big endian.
 */
bool isBigEndian(void) {
  union {
    uint32_t i;
    char c[4];
  } bint = { 0x01020304 };

  return bint.c[0] == 1;
}

/**
 * @brief Interleave two vectors.
 * 
 * Example:
 *  vec1 = {1, 2, 3, 4}
 *  vec2 = {9, 8, 7, 6}
 * 
 *  interleaved_vector = {1, 9, 2, 8, 3, 7, 4, 6}
 * 
 * @param vec1 Vector 1.
 * @param vec2 Vector 2.
 * @return std::vector<double> Interleaved vectors.
 */
std::vector<double> interleave2Vectors(const std::vector<double> &vec1, const std::vector<double> &vec2) {
  std::vector<double> result;

  auto m = std::min(vec1.size(), vec2.size());

  for (size_t i = 0; i < m; ++i) {
    result.push_back(vec1[i]);
    result.push_back(vec2[i]);
  };
  if (m < vec1.size()) result.insert(result.end(), vec1.begin() + m, vec1.end());
  if (m < vec2.size()) result.insert(result.end(), vec2.begin() + m, vec2.end());

  return result;
}

/**
 * @brief Compute the median of a vector.
 * 
 * @param inVec Input vector.
 * @return double Median.
 */
double median(std::vector<double> &inVec) {
  std::vector<double> vec(inVec);
  std::sort(vec.begin(), vec.end());
  double median;
  if (vec.size() % 2 == 0)
    median = 0.5 * (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]);
  else
    median = vec[vec.size() / 2];

  return median;
}

/**
 * @brief Compute a one pole filter on an audio signal.
 * 
 * @param vec Audio signal.
 * @return std::vector<double> Filtered audio signal.
 */
std::vector<double> onePoleFilter(const std::vector<double> &vec) {
  double a = 0.99;
  double y = 0.0;

  std::vector<double> filtered_signal;
  auto one_pool_equation = [a, &y](const double x) { return y += (1 - a) * x - a * y; };
  std::transform(vec.begin(), vec.end(), std::back_inserter(filtered_signal), one_pool_equation);

  return filtered_signal;
}

/**
 * @brief Calculate an efficient length to pad the inputs of the FFT.
 * 
 * Copied from Peter Bell.
 * https://gdoc.pub/doc/e/2PACX-1vR6iXXG1uS9ds47GvDgQk6XtpYzVTtYepu5B8onBrMmoorfKHhnHbN0ArDoXgoA23nZrcrm_DSFMW45
 * 
 * @param n Original input size.
 * @return size_t Efficient FFT input size.
 */
size_t NextFastLen(size_t n) {
  if (n <= 6) return n;

  size_t bestfac = 2 * n;
  for (size_t f5 = 1; f5 < bestfac; f5 *= 5) {
    size_t x = f5;
    while (x < n) x *= 2;
    for (;;) {
      if (x < n)
        x *= 3;
      else if (x > n) {
        if (x < bestfac) bestfac = x;
        if (x & 1) break;
        x >>= 1;
      } else
        return n;
    }
  }
  return bestfac;
}

double normFct(int inorm, size_t N) {
  if (inorm == 0) return double(1);
  if (inorm == 2) return double(1 / ldbl_t(N));
  if (inorm == 1) return double(1 / sqrt(ldbl_t(N)));
  throw std::runtime_error("invalid value for inorm (must be 0, 1, or 2)");
}

double normFct(int inorm, const pocketfft::shape_t &shape, const pocketfft::shape_t &axes, size_t fct, int delta) {
  if (inorm == 0) return double(1);
  size_t N(1);
  for (auto a : axes) N *= fct * size_t(int64_t(shape[a]) + delta);
  return normFct(inorm, N);
}

/**
 * @brief Centered a vector with respect to the full discrete linear convolution of the input.
 *
 * @param vec Vector
 * @param new_shape New shape of of vector.
 * @return std::vector<double> Centered vector.
 */
std::vector<double> centerVector(const std::vector<double> &vec, size_t new_shape) {
  size_t curr_shape = vec.size();
  size_t start_index = (curr_shape - new_shape) / 2;
  size_t end_index = start_index + new_shape;
  std::vector<double> sliced(vec.begin() + start_index, vec.begin() + end_index);
  return sliced;
}

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
std::vector<double> fftConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2) {
  std::vector<double> v1(vec1);
  std::vector<double> v2(vec2);
  std::vector<double> ret;

  if (v1.empty() || v2.empty()) return ret;

  size_t s1 = v1.size();
  size_t s2 = v2.size();
  size_t shape = s1 + s2 - 1;

  // Pad inputs to an efficient length
  size_t good_size = NextFastLen(shape);
  if (good_size < v1.size() || good_size < v2.size())
    std::runtime_error("Something went wrong calculating efficient FFT size.");
  v1.resize(good_size, 0.0);
  v2.resize(good_size, 0.0);

  // FFT common variables
  bool forward = true;
  int inorm = 0;
  int nthreads = 1;
  pocketfft::shape_t axes{ 1 };  // 1 axis on 1D array

  // vec1 FFT
  pocketfft::shape_t v1_dims_in{ 1, v1.size() };  // {width, height}
  pocketfft::shape_t v1_dims_out(v1_dims_in);
  v1_dims_out[axes.back()] = (v1_dims_out[axes.back()] >> 1) + 1;  // Get length of output vector
  size_t v1OutSize = v1_dims_out[axes.back()];
  std::vector<std::complex<double>> v1_out(v1OutSize);
  long int s1_in_shape = v1.size() * sizeof(double);
  pocketfft::stride_t s1_in{ s1_in_shape, sizeof(double) };  // {height * sizeof(type), sizeof(type)}
  /* NOTE: Putting the size of the wrong type will produce wrong results */
  long int s1_out_shape = v1.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s1_out{ s1_out_shape, sizeof(std::complex<double>) };
  auto d1_in = reinterpret_cast<const double *>(v1.data());
  auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
  double v1_fct = normFct(inorm, v1_dims_in, axes);
  pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct, nthreads);

  // vec2 FFT
  pocketfft::shape_t v2_dims_in{ 1, v2.size() };  // {width, height}
  pocketfft::shape_t v2_dims_out(v2_dims_in);
  v2_dims_out[axes.back()] = (v2_dims_out[axes.back()] >> 1) + 1;  // Get length of output vector
  size_t v2OutSize = v2_dims_out[axes.back()];
  std::vector<std::complex<double>> v2_out(v2OutSize);
  long int s2_in_shape = v2.size() * sizeof(double);
  pocketfft::stride_t s2_in{ s2_in_shape, sizeof(double) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int s2_out_shape = v2.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s2_out{ s2_out_shape, sizeof(std::complex<double>) };
  auto d2_in = reinterpret_cast<const double *>(v2.data());
  auto d2_out = reinterpret_cast<std::complex<double> *>(v2_out.data());
  double v2_fct = normFct(inorm, v2_dims_in, axes);
  pocketfft::r2c(v2_dims_in, s2_in, s2_out, axes, forward, d2_in, d2_out, v2_fct, nthreads);

  // Element-wise multiplication of 2 complex vectors
  std::vector<std::complex<double>> products(v1_out.size());
  std::transform(v1_out.begin(), v1_out.end(), v2_out.begin(), products.begin(),
                 std::multiplies<std::complex<double>>());

  // Save size of products before padding
  size_t product_size_pre_pad = products.size();

  // Pad products to efficient size
  products.resize(good_size, 0.0);

  // Inverse FFT of complex to real
  forward = false;
  inorm = 2;
  size_t axis = axes.back();
  size_t lastsize = products.size();
  pocketfft::shape_t conv_dims_in{ 1, product_size_pre_pad };  // {width, height}
  pocketfft::shape_t conv_dims_out(conv_dims_in);
  // if (lastsize==0) lastsize = 2 * conv_dims_in[axis] - 1;
  if ((lastsize / 2) + 1 != conv_dims_in[axis]) throw std::runtime_error("bad lastsize");
  conv_dims_out[axis] = lastsize;
  size_t conv_out_size = conv_dims_out[axis];
  std::vector<double> conv_out(conv_out_size);
  long int conv_in_shape = products.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s_conv_in{ conv_in_shape,
                                 sizeof(std::complex<double>) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int conv_out_shape = conv_out_size * sizeof(double);
  pocketfft::stride_t s_conv_out{ conv_out_shape, sizeof(double) };
  auto d_conv_in = reinterpret_cast<const std::complex<double> *>(products.data());
  auto d_conv_out = reinterpret_cast<double *>(conv_out.data());
  double conv_fct = normFct(inorm, conv_dims_out, axes);
  pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct, nthreads);

  std::vector<double> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

  // Perform 'same' part of the convolve
  std::vector<double> centered_conv_out = centerVector(sliced_conv_out, s1);
  return centered_conv_out;
}

/**
 * @brief Normalize a vector (to have an area of 1) and then scale by a factor of 2.
 *
 * @param input Input vector.
 * @return std::vector<double> Normalized vector.
 */
std::vector<double> normalize(const std::vector<double> &input) {
  int size = input.size();
  double sum = 0.0;
  std::vector<double> normalized_output(input);

  for (int i = 0; i < size; i++) {
    sum += std::abs(input[i]);
  }
  if (sum == 0.0) {
    return std::vector<double>();
  }

  // As we have half of the energy in negative frequencies, we need to scale, but
  // multiply by two. Otherwise a sinusoid at 0db will result in 0.5 in the spectrum.
  double scale = 2.0 / sum;

  for (int i = 0; i < size; i++) {
    normalized_output[i] *= scale;
  }

  return normalized_output;
}

/**
 * @brief Blackmanharris windowing algorithm.
 * Window functions help control spectral leakage when doing Fourier Analysis
 *
 * @param window Audio signal window.
 * @param a0 Constant a0.
 * @param a1 Constant a1.
 * @param a2 Constant a2.
 * @param a3 Constant a3.
 * @return std::vector<double> blackmanharris window.
 */
std::vector<double> blackmanHarris(const std::vector<double> &window, double a0, double a1, double a2, double a3) {
  std::vector<double> ret(window);
  int window_size = window.size();
  double f = 2.0 * M_PI / (window_size - 1);

  if (window_size % 2 != 0) {
    ret[window_size / 2] = a0 - a1 * cos(f * (window_size / 2)) + a2 * cos(f * 2 * (window_size / 2)) -
                           a3 * cos(f * 3 * (window_size / 2));
  }

  for (int i = 0; i < window_size / 2; i++) {
    ret[i] = ret[window_size - i - 1] = a0 - a1 * cos(f * i) + a2 * cos(f * 2 * i) - a3 * cos(f * 3 * i);
  }

  return ret;
}

std::vector<double> blackmanHarris62dB(const std::vector<double> &window) {
  double a0 = .44959, a1 = .49364, a2 = .05677, a3 = 0.;
  return blackmanHarris(window, a0, a1, a2, a3);
}

std::vector<double> blackmanHarris92dB(const std::vector<double> &window) {
  double a0 = .35875, a1 = .48829, a2 = .14128, a3 = .01168;
  return blackmanHarris(window, a0, a1, a2, a3);
}

/**
 * @brief Applies windowing to an audio signal.
 * It optionally applies zero-phase windowing and optionally adds zero-padding. The resulting windowed frame size is
 * equal to the incoming frame size plus the number of padded zeros. By default, the available windows are normalized
 * (to have an area of 1) and then scaled by a factor of 2.
 *
 * References:
 *  [1] F. J. Harris, On the use of windows for harmonic analysis with the discrete Fourier transform,
 *  Proceedings of the IEEE, vol. 66, no. 1, pp. 51-83, Jan. 1978
 *  [2] Window function - Wikipedia, the free encyclopedia, http://en.wikipedia.org/wiki/Window_function
 *
 * @param audio_frame Input audio frame.
 * @param window_type_func The window type function. Examples: blackmanHarris92dB, blackmanHarris62dB...
 * @param size Window size.
 * @param zero_padding_size Size of the zero-padding.
 * @param zero_phase Enables zero-phase windowing.
 * @param _normalize Specify whether to normalize windows (to have an area of 1) and then scale by a factor of 2.
 * @return std::vector<double> Windowed audio frame.
 */
std::vector<double> windowing(const std::vector<double> &audio_frame,
                              const std::function<std::vector<double>(const std::vector<double> &)> &window_type_func,
                              unsigned int size,
                              unsigned int zero_padding_size,
                              bool zero_phase,
                              bool _normalize) {
  int signal_size = audio_frame.size();
  int total_size = signal_size + zero_padding_size;

  if (signal_size <= 1) {
    throw std::runtime_error("windowing: frame (signal) size should be larger than 1");
  }

  std::vector<double> windowed_signal(static_cast<size_t>(total_size));
  std::vector<double> window(static_cast<size_t>(signal_size));
  if (_normalize) {
    window = normalize(window_type_func(window));
  } else {
    window = window_type_func(window);
  }

  int i = 0;

  if (zero_phase) {
    // first half of the windowed signal is the
    // second half of the signal with windowing!
    for (int j = signal_size / 2; j < signal_size; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }

    // zero padding
    for (int j = 0; j < zero_padding_size; j++) {
      windowed_signal[i++] = 0.0;
    }

    // second half of the signal
    for (int j = 0; j < signal_size / 2; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }
  } else {
    // windowed signal
    for (int j = 0; j < signal_size; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }

    // zero padding
    for (int j = 0; j < zero_padding_size; j++) {
      windowed_signal[i++] = 0.0;
    }
  }

  return windowed_signal;
}

/**
 * @brief Calculate magnitude (absolute value or modulus) of a complex number.
 *
 * @param complex_pair Complex number. Contains 1 real and 1 imaginary number.
 * @return double The magnitude of a complex number.
 */
double Magnitude(const std::complex<double> complex_pair) {
  return std::sqrt(std::pow(complex_pair.real(), 2) + std::pow(complex_pair.imag(), 2));
}

/**
 * @brief Computes the frequency spectrum of an array of Reals.
 * The resulting spectrum has a size which is half the size of the input array plus one.
 * Bins contain raw (linear) magnitude values.
 *
 * @param frame Input audio frame.
 * @return std::vector<double> Frequency spectrum of the input audio signal.
 */
std::vector<double> convertToFrequencySpectrum(const std::vector<double> &audio_frame) {
  std::vector<double> v1(audio_frame);
  std::vector<double> ret;

  if (v1.empty()) return ret;

  size_t s1 = v1.size();
  size_t shape = s1 - 1;

  // Pad inputs to an efficient length
  size_t good_size = NextFastLen(shape);
  if (good_size < v1.size()) std::runtime_error("Something went wrong calculating efficient FFT size.");
  v1.resize(good_size, 0.0);

  // FFT common variables
  bool forward = true;
  int inorm = 0;
  int nthreads = 1;
  pocketfft::shape_t axes{ 1 };  // 1 axis on 1D array

  // v1 FFT
  pocketfft::shape_t v1_dims_in{ 1, v1.size() };  // {width, height}
  pocketfft::shape_t v1_dims_out(v1_dims_in);
  v1_dims_out[axes.back()] = (v1_dims_out[axes.back()] >> 1) + 1;  // Get length of output vector
  size_t v1OutSize = v1_dims_out[axes.back()];
  std::vector<std::complex<double>> v1_out(v1OutSize);
  long int s1_in_shape = v1.size() * sizeof(double);
  pocketfft::stride_t s1_in{ s1_in_shape, sizeof(double) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int s1_out_shape = v1.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s1_out{ s1_out_shape, sizeof(std::complex<double>) };
  auto d1_in = reinterpret_cast<const double *>(v1.data());
  auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
  double v1_fct = normFct(inorm, v1_dims_in, axes);
  pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct, nthreads);

  // Get element-wise absolute value of a complex vector
  ret.resize(v1_out.size());
  auto calculate_magnitude = [](const std::complex<double> x) { return Magnitude(x); };
  std::transform(v1_out.begin(), v1_out.end(), ret.begin(), calculate_magnitude);

  return ret;
}

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
std::tuple<double, double> quadraticInterpolation(double a, double b, double y, int middle_point_index) {
  /*
  Smith, J.O. "Quadratic Interpolation of Spectral Peaks", in
  Spectral Audio Signal Processing,
  https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html, online book,
  2011 edition,
  accessed 12/18/2019.

  α(a) = left point value of parabola
  β(b) = middle point value of parabola
  γ(y) = right point value of parabola

  Interpolated peak location is given in bins (spectral samples) by:
  p = 1/2 ((α - γ) / (α - 2β + γ))

  The peak magnitude estimate is:
  y(p) = β - 1/4(α - γ)p
  */

  double p = 0.5 * ((a - y) / (a - 2 * b + y));
  double peak_location = static_cast<double>(middle_point_index) + p;
  double peak_height_estimate = b - 0.25 * (a - y) * p;
  return std::make_tuple(peak_location, peak_height_estimate);
}

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
std::vector<std::tuple<double, double>> peakDetect(const std::vector<double> &inp,
                                                   double threshold,
                                                   bool interpolate,
                                                   std::string sort_by,
                                                   int max_num_peaks,
                                                   double range,
                                                   int min_pos,
                                                   int max_pos) {
  const int inp_size = inp.size();
  if (inp_size < 2) {
    std::string err_msg = "Peak detection input vector must be greater than 2.";
    throw std::runtime_error(err_msg);
  }

  if (min_pos != 0 && max_pos != 0 && min_pos >= max_pos) {
    std::string err_msg = "Peak detection max position must be greater than min position.";
    throw std::runtime_error(err_msg);
  }

  std::vector<std::tuple<double, double>> estimated_peaks;

  double scale = 1;
  if (range > 0) {
    scale = range / static_cast<double>(inp_size - 1);
  }
  // Start at minimum position
  int i = 0;
  if (min_pos > 0) {
    // We are dividing by scale because the scale should have been accounted for when the user input the value
    i = static_cast<int>(ceil(min_pos / scale));
  }

  if (max_pos == 0) {
    max_pos = (inp_size - 1) * scale;
  }

  // Check if lower bound is a peak
  if (inp[i] > inp[i + 1] && inp[i] > threshold) {
    std::tuple<double, double> peak(i * scale, inp[i]);
    estimated_peaks.push_back(peak);
  }

  while (true) {
    //  Down:
    //    [0, 3, 4, 3, 2, 1, 1, 0]
    //           ^  ^  ^  ^  ^
    //
    while (i + 1 < inp_size - 1 && inp[i] >= inp[i + 1]) {
      i++;
    }

    //  Up:
    //    [0, 0, 1, 2, 3, 4, 3, 2]
    //           ^  ^  ^  ^

    while (i + 1 < inp_size - 1 && inp[i] < inp[i + 1]) {
      i++;
    }
    // Do not register a peak here because we need to check for a flat peak

    //  Flat peak:
    //    [0, 0, 1, 1, 1, 1, 0, 0]
    //           ^  ^  ^  ^
    int j = i;
    while (j + 1 < inp_size - 1 && (inp[j] == inp[j + 1])) {
      j++;
    }

    // Check element right before the last element
    if (i + 1 >= inp_size - 1) {
      if (i == inp_size - 2 && inp[i - 1] < inp[i] && inp[i + 1] < inp[i] && inp[i] > threshold) {
        double pos;
        double val;

        if (interpolate) {
          std::tie(pos, val) = quadraticInterpolation(inp[i - 1], inp[i], inp[i + 1], j);
        } else {
          pos = i;
          val = inp[i];
        }
        std::tuple<double, double> peak(pos * scale, val);
        estimated_peaks.push_back(peak);
      }

      // We are dividing by scale because the scale should have been accounted for when the user input the value
      double scale_removed_max_pos = max_pos / scale;
      // Check if element before last is a peak right before breaking the loop
      int last_index = inp_size - 1;
      if (scale_removed_max_pos > inp_size - 2 && scale_removed_max_pos <= inp_size - 1 &&
          inp[inp_size - 1] > inp[inp_size - 2] && inp[inp_size - 1] > threshold) {
        std::tuple<double, double> peak((inp_size - 1) * scale, inp[inp_size - 1]);
        estimated_peaks.push_back(peak);
      }
      break;
    }

    // Flat peak ends, check if we are going down
    if ((j + 1 <= inp_size - 1) && inp[j] > inp[j + 1] && inp[j] > threshold) {
      double pos;
      double val;

      if (j != i) {  // Flat peak between i and j
        if (interpolate) {
          // Get the middle of the flat peak
          pos = (i + j) * 0.5;
        } else {
          // Get rising edge of flat peak
          pos = i;
        }
        val = inp[i];
      } else {  // Interpolate peak at i-1, i and i+1
        if (interpolate) {
          std::tie(pos, val) = quadraticInterpolation(inp[i - 1], inp[i], inp[i + 1], j);
        } else {
          pos = j;
          val = inp[j];
        }
      }

      if (pos * scale > max_pos) break;

      std::tuple<double, double> peak(pos * scale, val);
      estimated_peaks.push_back(peak);
    }

    // No flat peak... We continue up, so we start loop again
    i = j;
  }

  // Sorting
  std::transform(sort_by.begin(), sort_by.end(), sort_by.begin(), [](unsigned char c) { return std::tolower(c); });

  std::vector<std::tuple<double, double>> sorted_estimated_peaks = estimated_peaks;
  if (sort_by == "position") {
    // Already sorted by position (Frequency)
  } else if (sort_by == "height") {
    // height (Magnitude)
    std::sort(sorted_estimated_peaks.begin(), sorted_estimated_peaks.end(),
              [](auto const &t1, auto const &t2) { return std::get<1>(t1) > std::get<1>(t2); });
  } else {
    std::string err_msg = "Sorting by '" + sort_by + "' is not supported.";
    throw std::runtime_error(err_msg);
  }

  // Shrink to max number of peaks
  size_t num_peaks = max_num_peaks;
  if (num_peaks != 0 && num_peaks < sorted_estimated_peaks.size()) {
    std::vector<std::tuple<double, double>> limited_num_of_sorted_estimated_peaks = sorted_estimated_peaks;
    limited_num_of_sorted_estimated_peaks.resize(num_peaks);
    limited_num_of_sorted_estimated_peaks.shrink_to_fit();

    return limited_num_of_sorted_estimated_peaks;
  }

  return sorted_estimated_peaks;
}

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
                                                      double threshold,
                                                      std::string sort_by,
                                                      unsigned int max_num_peaks,
                                                      double sample_rate,
                                                      int min_pos,
                                                      int max_pos) {
  return peakDetect(input_spectrum, threshold, true, sort_by, max_num_peaks, sample_rate / 2.0, min_pos, max_pos);
}

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
    double normalized_distance = distance / window_size;
    double weight = 0.;

    if (weight_type == COSINE) {
      weight = std::cos(M_PI * normalized_distance);
    } else if (weight_type == SQUARED_COSINE) {
      weight = std::cos(M_PI * normalized_distance);
      weight *= weight;
    }

    int iwrapped = i % pcp_size;
    if (iwrapped < 0) iwrapped += pcp_size;

    hpcp[iwrapped] += weight * (fplus::square(mag_lin) * fplus::square(harmonic_weight));
  }
}

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

/**
 * @brief Builds a weighting table of harmonic contribution.
 * Higher harmonics contribute less and the fundamental frequency has a full harmonic strength of 1.0.
 *
 * @param harmonics Number of harmonics for frequency contribution, 0 indicates exclusive fundamental frequency
 * contribution.
 * @return std::vector<HarmonicPeak> Weighting table of harmonic contribution.
 */
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

/**
 * @brief Get the max element in a vector.
 * Checks if the vector is empty first.
 *
 * @param vec Vector
 * @return int Max element
 */
int MaxVectorElement(const std::vector<double> &vec) {
  if (vec.empty()) throw std::runtime_error("Trying to get max vector element of empty vector.");
  return std::max_element(vec.begin(), vec.end()) - vec.begin();
}

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
 * @param non_linear Apply non-linear post-processing to the output (use with normalized='unitMax'). Boosts values close
 * to 1, decreases values close to 0.
 * @param _normalized Whether to normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
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
                         double sample_rate,
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

  NormalizeType normalized;
  if (_normalized == "none")
    normalized = N_NONE;
  else if (_normalized == "unit sum")
    normalized = N_UNIT_SUM;
  else if (_normalized == "unit max")
    normalized = N_UNIT_MAX;
  else {
    std::string err_message = "HPCP: Invalid normalize type of: ";
    err_message += _normalized;
    throw std::runtime_error(err_message);
  }

  if (non_linear && normalized != N_UNIT_MAX) {
    throw std::runtime_error("HPCP: Cannot apply non-linear filter when HPCP vector is not normalized to unit max.");
  }
  // ========

  std::vector<HarmonicPeak> harmonic_peaks = InitHarmonicContributionTable(harmonics);
  std::vector<double> hpcp(size);

  std::vector<double> hpcp_LO;
  std::vector<double> hpcp_HI;

  if (band_preset) {
    hpcp_LO.resize(size);
    std::fill(hpcp_LO.begin(), hpcp_LO.end(), (double)0.0);

    hpcp_HI.resize(size);
    std::fill(hpcp_HI.begin(), hpcp_HI.end(), (double)0.0);
  }

  // Add each contribution of the spectral frequencies to the HPCP
  for (int i = 0; i < (int)frequencies.size(); i++) {
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
    if (normalized == N_UNIT_MAX) {
      normalizeInPlace(hpcp_LO);
      normalizeInPlace(hpcp_HI);
    } else if (normalized == N_UNIT_SUM) {
      // TODO does it makes sense to apply band preset together with unit sum normalization?
      normalizeSum(hpcp_LO);
      normalizeSum(hpcp_HI);
    }

    for (int i = 0; i < (int)hpcp.size(); i++) {
      hpcp[i] = hpcp_LO[i] + hpcp_HI[i];
    }
  }

  if (normalized == N_UNIT_MAX) {
    normalizeInPlace(hpcp);
  } else if (normalized == N_UNIT_SUM) {
    normalizeSum(hpcp);
  }

  /* Perform the Jordi non-linear post-processing step
   This makes small values (below 0.6) even smaller
   while boosting further values close to 1. */
  if (non_linear) {
    for (int i = 0; i < (int)hpcp.size(); i++) {
      hpcp[i] = sin(hpcp[i] * M_PI * 0.5);
      hpcp[i] *= hpcp[i];
      if (hpcp[i] < 0.6) {
        hpcp[i] *= hpcp[i] / 0.6 * hpcp[i] / 0.6;
      }
    }
  }

  /* Shift all of the elements so that the largest HPCP value is at index 0,
   only if this option is enabled. */
  if (max_shifted) {
    int idx_max = MaxVectorElement(hpcp);
    std::vector<double> hpcp_bak = hpcp;
    for (int i = idx_max; i < (int)hpcp.size(); i++) {
      hpcp[i - idx_max] = hpcp_bak[i];
    }
    int offset = hpcp.size() - idx_max;
    for (int i = 0; i < idx_max; i++) {
      hpcp[i + offset] = hpcp_bak[i];
    }
  }
  return hpcp;
}

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
 * @param non_linear Apply non-linear post-processing to the output (use with normalized='unitMax'). Boosts values close
 * to 1, decreases values close to 0.
 * @param _normalized Whether to normalize the HPCP vector.
 * @return std::vector<double> Resulting harmonic pitch class profile.
 */
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
                         double sample_rate,
                         bool max_shifted,
                         bool non_linear,
                         std::string _normalized) {
  std::vector<double> frequencies(peaks.size());
  std::vector<double> magnitudes(peaks.size());

  std::transform(peaks.begin(), peaks.end(), frequencies.begin(), [](auto const &pair) { return std::get<0>(pair); });

  std::transform(peaks.begin(), peaks.end(), magnitudes.begin(), [](auto const &pair) { return std::get<1>(pair); });

  return HPCP(frequencies, magnitudes, size, reference_frequency, harmonics, band_preset, band_split_frequency,
              min_frequency, max_frequency, _weight_type, window_size, sample_rate, max_shifted, non_linear,
              _normalized);
}

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
std::vector<double> Framecutter::compute() {
  if (valid_frame_threshold_ratio_ > 0.5 && start_from_center_) {
    throw std::runtime_error(
        "FrameCutter: valid_frame_threshold_ratio cannot be "
        "larger than 0.5 if start_from_center is true (this "
        "is to prevent loss of the first frame which would "
        "be only half a valid frame since the first frame "
        "is centered on the beginning of the audio)");
  }

  int valid_frame_threshold = (int)round(valid_frame_threshold_ratio_ * frame_size_);
  int start_index;
  size_t buffer_size = buffer_.size();

  if (start_from_center_)
    start_index = -(frame_size_ + 1) / 2 + start_index_;
  else
    start_index = start_index_;

  if (last_frame_ || buffer_.empty()) return std::vector<double>();
  if (start_index >= (int)buffer_size) return std::vector<double>();

  std::vector<double> frame(static_cast<size_t>(frame_size_));
  int idx_in_frame = 0;

  // If we're before the beginning of the buffer, fill the frame with 0
  if (start_index < 0) {
    int how_much = std::min(-start_index, frame_size_);
    for (; idx_in_frame < how_much; idx_in_frame++) {
      frame[idx_in_frame] = (double)0.0;
    }
  }

  // Now, just copy from the buffer to the frame
  int how_much = std::min(frame_size_, (int)buffer_size - start_index) - idx_in_frame;
  memcpy(&frame[0] + idx_in_frame, &buffer_[0] + start_index + idx_in_frame, how_much * sizeof(double));
  idx_in_frame += how_much;

  // Check if the idx_in_frame is below the threshold (this would only happen
  // for the last frame in the stream)
  if (idx_in_frame < valid_frame_threshold) return std::vector<double>();

  if (start_index + idx_in_frame >= (int)buffer_size && !start_from_center_ && !last_frame_to_end_of_file_)
    last_frame_ = true;

  if (idx_in_frame < frame_size_) {
    if (!start_from_center_) {
      if (last_frame_to_end_of_file_) {
        if (start_index >= (int)buffer_size) last_frame_ = true;
      }
      // if we're zero-padding with start_from_center=false, it means we're filling
      // in the last frame, so we'll have to stop after this one
      else
        last_frame_ = true;
    } else {
      // if we're zero-padding and the center of the frame is past the end of the
      // stream, then this is the last frame and we need to stop after this one
      if (start_index + frame_size_ / 2 >= (int)buffer_.size()) {
        last_frame_ = true;
      }
    }
    // Fill in the frame with 0 until the end of the buffer
    for (; idx_in_frame < frame_size_; idx_in_frame++) {
      frame[idx_in_frame] = (double)0.0;
    }
  }
  start_index_ += hop_size_;
  return frame;
}

/**
 * @brief Downmixes the signal into a single channel given a stereo signal. If the signal was already a monoaural, it is
 * left unchanged.
 *
 * @param input Stereo or mono audio signal
 * @return std::vector<double> Downmixed audio signal
 */
std::vector<double> monoMixer(const std::vector<std::vector<double>> &input) {
  int num_channels = input.size();
  if (num_channels > 2 || input.empty()) {
    std::runtime_error("Audio samples must be either mono or stereo.");
  }

  if (num_channels == 1) {
    return input[0];
  }

  const std::vector<double> channel_one = input[0];
  const std::vector<double> channel_two = input[1];

  if (channel_one.size() != channel_two.size()) std::runtime_error("Audio channels must be the same length.");
  int size = channel_one.size();
  std::vector<double> result(size);

  for (int i = 0; i < size; ++i) {
    result[i] = 0.5 * (channel_one[i] + channel_two[i]);
  }
  return result;
}

/**
 * @brief Calculate the standard deviation of a vector.
 *
 * @param mean Mean (Average)
 * @param vec Vector
 * @return double Standard devation
 */
double standard_deviation(double mean, const std::vector<double> &vec) {
  return fplus::fwd::apply(
      fplus::reduce([&mean](auto total, auto next_val) { return total + fplus::square(next_val - mean); },
                    0,  // Start at 0
                    vec),
      [](auto std) { return std::sqrt(std); });
}

}  // namespace core
}  // namespace musher
