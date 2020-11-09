#include "src/core/utils.h"

#include <assert.h>

#include <complex>
// #include <fplus/fplus.hpp>
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

std::string Uint8VectorToHexString(const std::vector<uint8_t> &v) {
  std::stringstream ss;
  ss << std::hex << std::setfill('0');
  std::vector<uint8_t>::const_iterator it;

  for (it = v.begin(); it != v.end(); it++) {
    ss << std::setw(2) << static_cast<unsigned>(*it);
  }

  return ss.str();
}

std::string StrBetweenSQuotes(const std::string &s) {
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

int16_t TwoBytesToInt(const std::vector<uint8_t> &source, const int startIndex) {
  int16_t result;

  if (!IsBigEndian())
    result = (source[startIndex + 1] << 8) | source[startIndex];
  else
    result = (source[startIndex] << 8) | source[startIndex + 1];

  return result;
}

int32_t FourBytesToInt(const std::vector<uint8_t> &source, const int startIndex) {
  int32_t result;

  if (!IsBigEndian())
    result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) |
             source[startIndex];
  else
    result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) |
             source[startIndex + 3];

  return result;
}

double NormalizeInt8_t(const uint8_t sample) { return static_cast<double>(sample - 128) / static_cast<double>(128.); }

double NormalizeInt16_t(const int16_t sample) { return static_cast<double>(sample) / static_cast<double>(32768.); }

double NormalizeInt32_t(const int32_t sample) { return static_cast<double>(sample) / static_cast<double>(8388608.); }

bool IsBigEndian(void) {
  union {
    uint32_t i;
    char c[4];
  } bint = { 0x01020304 };

  return bint.c[0] == 1;
}

std::vector<std::vector<double>> Deinterweave(const std::vector<double> &interweaved_vector) {
  size_t interweaved_vec_size = interweaved_vector.size();
  std::vector<double> channel1;
  std::vector<double> channel2;

  for (size_t i = 0; i < interweaved_vec_size; ++i) {
    if (i % 2 == 0) {
      channel1.push_back(interweaved_vector[i]);
    } else {
      channel2.push_back(interweaved_vector[i]);
    }
  };

  std::vector<std::vector<double>> deinterweaved_vectors({
      channel1,
      channel2,
  });

  return deinterweaved_vectors;
}

double Median(std::vector<double> &inVec) {
  std::vector<double> vec(inVec);
  std::sort(vec.begin(), vec.end());
  double median;
  if (vec.size() % 2 == 0)
    median = 0.5 * (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]);
  else
    median = vec[vec.size() / 2];

  return median;
}

std::vector<double> OnePoleFilter(const std::vector<double> &vec) {
  double a = 0.99;
  double y = 0.0;

  std::vector<double> filtered_signal;
  auto one_pool_equation = [a, &y](const double x) { return y += (1 - a) * x - a * y; };
  std::transform(vec.begin(), vec.end(), std::back_inserter(filtered_signal), one_pool_equation);

  return filtered_signal;
}

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

double NormFct(int inorm, size_t N) {
  if (inorm == 0) return double(1);
  if (inorm == 2) return double(1 / ldbl_t(N));
  if (inorm == 1) return double(1 / sqrt(ldbl_t(N)));
  throw std::runtime_error("invalid value for inorm (must be 0, 1, or 2)");
}

double NormFct(int inorm, const pocketfft::shape_t &shape, const pocketfft::shape_t &axes, size_t fct, int delta) {
  if (inorm == 0) return double(1);
  size_t N(1);
  for (auto a : axes) N *= fct * size_t(int64_t(shape[a]) + delta);
  return NormFct(inorm, N);
}

std::vector<double> CenterVector(const std::vector<double> &vec, size_t new_shape) {
  size_t curr_shape = vec.size();
  size_t start_index = (curr_shape - new_shape) / 2;
  size_t end_index = start_index + new_shape;
  std::vector<double> sliced(vec.begin() + start_index, vec.begin() + end_index);
  return sliced;
}

std::vector<double> FFTConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2) {
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
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int s1_out_shape = v1.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s1_out{ s1_out_shape, sizeof(std::complex<double>) };
  auto d1_in = reinterpret_cast<const double *>(v1.data());
  auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
  double v1_fct = NormFct(inorm, v1_dims_in, axes);
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
  double v2_fct = NormFct(inorm, v2_dims_in, axes);
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
  double conv_fct = NormFct(inorm, conv_dims_out, axes);
  pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct, nthreads);

  std::vector<double> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

  // Perform 'same' part of the convolve
  std::vector<double> centered_conv_out = CenterVector(sliced_conv_out, s1);
  return centered_conv_out;
}

std::vector<double> Normalize(const std::vector<double> &input) {
  int size = input.size();
  double sum = 0.0;
  std::vector<double> Normalized_output(input);

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
    Normalized_output[i] *= scale;
  }

  return Normalized_output;
}

std::vector<double> BlackmanHarris(const std::vector<double> &window, double a0, double a1, double a2, double a3) {
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

std::vector<double> BlackmanHarris62dB(const std::vector<double> &window) {
  double a0 = .44959, a1 = .49364, a2 = .05677, a3 = 0.;
  return BlackmanHarris(window, a0, a1, a2, a3);
}

std::vector<double> BlackmanHarris92dB(const std::vector<double> &window) {
  double a0 = .35875, a1 = .48829, a2 = .14128, a3 = .01168;
  return BlackmanHarris(window, a0, a1, a2, a3);
}

std::vector<double> Windowing(const std::vector<double> &audio_frame,
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
    window = Normalize(window_type_func(window));
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
    for (unsigned int j = 0; j < zero_padding_size; j++) {
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
    for (unsigned int j = 0; j < zero_padding_size; j++) {
      windowed_signal[i++] = 0.0;
    }
  }

  return windowed_signal;
}

double Magnitude(const std::complex<double> complex_pair) {
  return std::sqrt(std::pow(complex_pair.real(), 2) + std::pow(complex_pair.imag(), 2));
}

std::vector<double> ConvertToFrequencySpectrum(const std::vector<double> &audio_frame) {
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
  double v1_fct = NormFct(inorm, v1_dims_in, axes);
  pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct, nthreads);

  // Get element-wise absolute value of a complex vector
  ret.resize(v1_out.size());
  auto calculate_magnitude = [](const std::complex<double> x) { return Magnitude(x); };
  std::transform(v1_out.begin(), v1_out.end(), ret.begin(), calculate_magnitude);

  return ret;
}

std::tuple<double, double> QuadraticInterpolation(double a, double b, double y, int middle_point_index) {
  double p = 0.5 * ((a - y) / (a - 2 * b + y));
  double peak_location = static_cast<double>(middle_point_index) + p;
  double peak_height_estimate = b - 0.25 * (a - y) * p;
  return std::make_tuple(peak_location, peak_height_estimate);
}

std::vector<std::tuple<double, double>> PeakDetect(const std::vector<double> &inp,
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
          std::tie(pos, val) = QuadraticInterpolation(inp[i - 1], inp[i], inp[i + 1], j);
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
          std::tie(pos, val) = QuadraticInterpolation(inp[i - 1], inp[i], inp[i + 1], j);
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

std::vector<std::tuple<double, double>> SpectralPeaks(const std::vector<double> &input_spectrum,
                                                      double threshold,
                                                      std::string sort_by,
                                                      unsigned int max_num_peaks,
                                                      double sample_rate,
                                                      int min_pos,
                                                      int max_pos) {
  return PeakDetect(input_spectrum, threshold, true, sort_by, max_num_peaks, sample_rate / 2.0, min_pos, max_pos);
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

int ArgMax(const std::vector<double> &vec) {
  if (vec.empty()) throw std::runtime_error("Trying to get max vector element of empty vector.");
  return std::max_element(vec.begin(), vec.end()) - vec.begin();
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
      NormalizeSum(hpcp_LO);
      NormalizeSum(hpcp_HI);
    }

    for (int i = 0; i < static_cast<int>(hpcp.size()); i++) {
      hpcp[i] = hpcp_LO[i] + hpcp_HI[i];
    }
  }

  if (Normalized == N_UNIT_MAX) {
    NormalizeInPlace(hpcp);
  } else if (Normalized == N_UNIT_SUM) {
    NormalizeSum(hpcp);
  }

  /* Perform the Jordi non-linear post-processing step
   This makes small values (below 0.6) even smaller
   while boosting further values close to 1. */
  if (non_linear) {
    for (int i = 0; i < static_cast<int>(hpcp.size()); i++) {
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

std::vector<double> Framecutter::compute() {
  if (valid_frame_threshold_ratio_ > 0.5 && start_from_center_) {
    throw std::runtime_error(
        "FrameCutter: valid_frame_threshold_ratio cannot be "
        "larger than 0.5 if start_from_center is true (this "
        "is to prevent loss of the first frame which would "
        "be only half a valid frame since the first frame "
        "is centered on the beginning of the audio)");
  }

  int valid_frame_threshold = static_cast<int>(std::round(valid_frame_threshold_ratio_ * frame_size_));
  int start_index;
  size_t buffer_size = buffer_.size();

  if (start_from_center_)
    start_index = -(frame_size_ + 1) / 2 + start_index_;
  else
    start_index = start_index_;

  if (last_frame_ || buffer_.empty()) return std::vector<double>();
  if (start_index >= static_cast<int>(buffer_size)) return std::vector<double>();

  std::vector<double> frame(static_cast<size_t>(frame_size_));
  int idx_in_frame = 0;

  // If we're before the beginning of the buffer, fill the frame with 0
  if (start_index < 0) {
    int how_much = std::min(-start_index, frame_size_);
    for (; idx_in_frame < how_much; idx_in_frame++) {
      frame[idx_in_frame] = static_cast<double>(0.0);
    }
  }

  // Now, just copy from the buffer to the frame
  int how_much = std::min(frame_size_, static_cast<int>(buffer_size - start_index)) - idx_in_frame;
  std::memcpy(&frame[0] + idx_in_frame, &buffer_[0] + start_index + idx_in_frame, how_much * sizeof(double));
  idx_in_frame += how_much;

  // Check if the idx_in_frame is below the threshold (this would only happen
  // for the last frame in the stream)
  if (idx_in_frame < valid_frame_threshold) return std::vector<double>();

  if (start_index + idx_in_frame >= static_cast<int>(buffer_size) && !start_from_center_ && !last_frame_to_end_of_file_)
    last_frame_ = true;

  if (idx_in_frame < frame_size_) {
    if (!start_from_center_) {
      if (last_frame_to_end_of_file_) {
        if (start_index >= static_cast<int>(buffer_size)) last_frame_ = true;
      }
      // if we're zero-padding with start_from_center=false, it means we're filling
      // in the last frame, so we'll have to stop after this one
      else
        last_frame_ = true;
    } else {
      // if we're zero-padding and the center of the frame is past the end of the
      // stream, then this is the last frame and we need to stop after this one
      if (start_index + frame_size_ / 2 >= static_cast<int>(buffer_.size())) {
        last_frame_ = true;
      }
    }
    // Fill in the frame with 0 until the end of the buffer
    for (; idx_in_frame < frame_size_; idx_in_frame++) {
      frame[idx_in_frame] = static_cast<double>(0.0);
    }
  }
  start_index_ += hop_size_;
  return frame;
}

std::vector<double> MonoMixer(const std::vector<std::vector<double>> &input) {
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

double standard_deviation(double mean, const std::vector<double> &vec) {
  return fplus::fwd::apply(
      fplus::reduce([&mean](auto total, auto next_val) { return total + fplus::square(next_val - mean); },
                    0,  // Start at 0
                    vec),
      [](auto std) { return std::sqrt(std); });
}

}  // namespace core
}  // namespace musher