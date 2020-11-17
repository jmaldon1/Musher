#pragma once

#include <vector>

namespace musher {
namespace core {

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
 * 
 * Convolve `vec1` and `vec2` using the fast Fourier transform method.
 * The output is the same size as `vec1`, centered with respect to the
 * full discrete linear convolution of the inputs.
 *
 * This function was heavily inspired by:
 *  https://github.com/scipy/scipy/blob/12fa74e97d3d18ca3a4e6991327663e88462f238/scipy/signal/signaltools.py#L551
 *  https://github.com/scipy/scipy/blob/master/scipy/fft/_pocketfft/pypocketfft.cxx
 *
 * @param vec1 Vector 1
 * @param vec2 Vector 2
 * @return std::vector<double> A 1-dimensional array containing a subset of the discrete linear
 * convolution of `vec1` with `vec2`.
 */
std::vector<double> FFTConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2);

}  // namespace core
}  // namespace musher