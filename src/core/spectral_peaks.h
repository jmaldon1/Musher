#pragma once

#include <vector>
#include <string>

namespace musher {
namespace core {

/**
 * @brief Extracts peaks from a spectrum.
 * 
 * It is important to note that the peak algorithm is independent of an input that is linear or in dB, so one has to
 * adapt the threshold to fit with the type of data fed to it. The algorithm relies on PeakDetect algorithm which is
 * run with parabolic interpolation [1]. The exactness of the peak-searching depends heavily on the windowing type. It
 * gives best results with dB input, a blackman-harris 92dB window and interpolation set to true. According to [1],
 * spectral peak frequencies tend to be about twice as accurate when dB magnitude is used rather than just linear
 * magnitude. For further information about the peak detection, see the description of the PeakDetection algorithm.
 *
 * References:
 *  [1] Peak Detection, http://ccrma.stanford.edu/~jos/parshl/Peak_Detection_Steps_3.html
 *
 * @param input_spectrum Input spectrum.
 * @param threshold Peaks below this given threshold are not outputted.
 * @param sort_by Ordering type of the outputted peaks (ascending by frequency (position)
 * or descending by magnitude (height)).
 * @param max_num_peaks Maximum number of returned peaks (set to 0 to return all peaks).
 * @param sample_rate Sampling rate of the audio signal \[Hz\].
 * @param min_pos Maximum frequency (position) of the range to evaluate \[Hz\].
 * @param max_pos Minimum frequency (position) of the range to evaluate \[Hz\].
 * @return std::vector<std::tuple<double, double>> Vector of spectral peaks, each peak being a tuple (frequency,
 * magnitude).
 */
std::vector<std::tuple<double, double>> SpectralPeaks(const std::vector<double> &input_spectrum,
                                                      double threshold = -1000.0,
                                                      std::string sort_by = "position",
                                                      unsigned int max_num_peaks = 100,
                                                      double sample_rate = 44100.,
                                                      int min_pos = 0,
                                                      int max_pos = 0);

}  // namespace core
}  // namespace musher