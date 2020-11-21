#pragma once

#include <tuple>
#include <vector>
#include <string>

namespace musher {
namespace core {

/**
 * @brief Interpolate the peak of a parabola given 3 points on the parabola.
 * 
 * α(a) = left point value of parabola
 * 
 * β(b) = middle point value of parabola
 * 
 * γ(y) = right point value of parabola
 *
 * Interpolated peak location is given in bins (spectral samples) by:
 * 
 * p = 1/2 ((α - γ) / (α - 2β + γ))
 *
 * The peak magnitude estimate is:
 * 
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
 * 
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
                                                   double range = 0.,
                                                   int min_pos = 0,
                                                   int max_pos = 0);

}  // namespace core
}  // namespace musher