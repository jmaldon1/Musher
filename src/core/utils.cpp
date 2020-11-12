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
