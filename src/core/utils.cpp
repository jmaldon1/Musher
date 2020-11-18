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

}  // namespace core
}  // namespace musher
