#include "src/core/utils.h"

#include <assert.h>

#include <complex>
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
