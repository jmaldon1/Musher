#pragma once

#define _USE_MATH_DEFINES
#include <functional>
#include <vector>

namespace musher {
namespace core {

/**
 * @brief Square windowing function.
 * 
 * @param window Audio signal window.
 * @return std::vector<double> Square window.
 */
std::vector<double> Square(const std::vector<double> &window);

/**
 * @brief Blackmanharris windowing algorithm.
 * 
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

/**
 * @brief Blackmanharris62db windowing algorithm.
 * 
 * @param window Audio signal window.
 * @return std::vector<double> Blackmanharris62db window.
 */
std::vector<double> BlackmanHarris62dB(const std::vector<double> &window);

/**
 * @brief Blackmanharris92db windowing algorithm.
 * 
 * @param window Audio signal window.
 * @return std::vector<double> Blackmanharris92db window.
 */
std::vector<double> BlackmanHarris92dB(const std::vector<double> &window);

/**
 * @brief Normalize a vector (to have an area of 1) and then scale by a factor of 2.
 *
 * @param input Input vector.
 * @return std::vector<double> normalized vector.
 */
std::vector<double> Normalize(const std::vector<double> &input);

/**
 * @brief Applies windowing to an audio signal.
 * 
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
 * @param window_type_func The window type function. Examples: BlackmanHarris92dB, BlackmanHarris62dB...
 * @param zero_padding_size Size of the zero-padding.
 * @param zero_phase Enables zero-phase windowing.
 * @param _normalize Specify whether to normalize windows (to have an area of 1) and then scale by a factor of 2.
 * @return std::vector<double> Windowed audio frame.
 */
std::vector<double> Windowing(
    const std::vector<double> &audio_frame,
    const std::function<std::vector<double>(const std::vector<double> &)> &window_type_func = BlackmanHarris62dB,
    unsigned zero_padding_size = 0,
    bool zero_phase = true,
    bool _normalize = true);

}  // namespace core
}  // namespace musher