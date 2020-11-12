#include <string>
#include <vector>

#include "src/core/peak_detect.h"

namespace musher {
namespace core {

std::vector<std::tuple<double, double>> SpectralPeaks(const std::vector<double> &input_spectrum,
                                                      double threshold,
                                                      std::string sort_by,
                                                      unsigned int max_num_peaks,
                                                      double sample_rate,
                                                      int min_pos,
                                                      int max_pos) {
  return PeakDetect(input_spectrum, threshold, true, sort_by, max_num_peaks, sample_rate / 2.0, min_pos, max_pos);
}

}  // namespace core
}  // namespace musher