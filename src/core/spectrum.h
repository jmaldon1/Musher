#pragma once

#include <vector>
#include <pocketfft/pocketfft.h>

namespace musher {
namespace core {

/**
 * @brief Calculate the magnitude (absolute value or modulus) of a complex number.
 *
 * @param complex_pair Complex number. Contains 1 real and 1 imaginary number.
 * @return double The magnitude of a complex number.
 */
double Magnitude(const std::complex<double> complex_pair);

using ldbl_t = typename std::conditional<sizeof(long double) == sizeof(double), double, long double>::type;
double NormFct(int inorm, size_t N);
double NormFct(int inorm,
               const pocketfft::shape_t &shape,
               const pocketfft::shape_t &axes,
               size_t fct = 1,
               int delta = 0);

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

/**
 * @brief Computes the frequency spectrum of an array of Reals.
 * 
 * The resulting spectrum has a size which is half the size of the input array plus one.
 * Bins contain raw (linear) magnitude values.
 *
 * @param frame Input audio frame.
 * @return std::vector<double> Frequency spectrum of the input audio signal.
 */
std::vector<double> ConvertToFrequencySpectrum(const std::vector<double> &audio_frame);

}  // namespace core
}  // namespace musher