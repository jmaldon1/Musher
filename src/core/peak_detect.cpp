#include "src/core/peak_detect.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

namespace musher {
namespace core {

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
  int _max_pos = max_pos;
  const int inp_size = inp.size();
  if (inp_size < 2) {
    std::string err_msg = "Peak detection input vector must be greater than 2.";
    throw std::runtime_error(err_msg);
  }

  if (min_pos != 0 && _max_pos != 0 && min_pos >= _max_pos) {
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
    i = static_cast<int>(std::ceil(min_pos / scale));
  }

  if (_max_pos == 0) {
    _max_pos = (inp_size - 1) * scale;
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
      double scale_removed_max_pos = _max_pos / scale;
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

      if (pos * scale > _max_pos) break;

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

}  // namespace core
}  // namespace musher