#pragma once

#define _USE_MATH_DEFINES

#include <math.h>
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
 *
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
 * @code
 *  interweaved_vector = {1, 9, 2, 8, 3, 7, 4, 6}
 *
 *  deinterweaved_vector = {
 *      {1, 2, 3, 4},
 *      {9, 8, 7, 6}
 *  }
 * @endcode
 *
 * @param interweaved_vector Interleaved vector.
 * @return std::vector<std::vector<double>> Deinterweaved vector.
 */
template <typename T>
std::vector<std::vector<T>> Deinterweave(const std::vector<T> &interweaved_vector) {
  size_t interweaved_vec_size = interweaved_vector.size();
  std::vector<T> channel1;
  std::vector<T> channel2;

  for (size_t i = 0; i < interweaved_vec_size; ++i) {
    if (i % 2 == 0) {
      channel1.push_back(interweaved_vector[i]);
    } else {
      channel2.push_back(interweaved_vector[i]);
    }
  };

  std::vector<std::vector<T>> deinterweaved_vectors({
      channel1,
      channel2,
  });

  return deinterweaved_vectors;
}

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

}  // namespace core
}  // namespace musher