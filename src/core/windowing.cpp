#include "src/core/windowing.h"

#include <cmath>
#include <functional>
#include <vector>
#include <stdexcept>

namespace musher {
namespace core {

std::vector<double> Square(const std::vector<double> &window) {
  std::vector<double> ret(window);
  for (int i=0; i<int(window.size()); i++) {
    ret[i] = 1.0;
  }
  return ret;
}

std::vector<double> BlackmanHarris(const std::vector<double> &window, double a0, double a1, double a2, double a3) {
  std::vector<double> ret(window);
  int window_size = window.size();
  double f = 2.0 * M_PI / (window_size - 1);

  if (window_size % 2 != 0) {
    ret[window_size / 2] = a0 - a1 * std::cos(f * (window_size / 2)) + a2 * std::cos(f * 2 * (window_size / 2)) -
                           a3 * std::cos(f * 3 * (window_size / 2));
  }

  for (int i = 0; i < window_size / 2; i++) {
    ret[i] = ret[window_size - i - 1] = a0 - a1 * std::cos(f * i) + a2 * std::cos(f * 2 * i) - a3 * std::cos(f * 3 * i);
  }

  return ret;
}

std::vector<double> BlackmanHarris62dB(const std::vector<double> &window) {
  double a0 = .44959, a1 = .49364, a2 = .05677, a3 = 0.;
  return BlackmanHarris(window, a0, a1, a2, a3);
}

std::vector<double> BlackmanHarris92dB(const std::vector<double> &window) {
  double a0 = .35875, a1 = .48829, a2 = .14128, a3 = .01168;
  return BlackmanHarris(window, a0, a1, a2, a3);
}

std::vector<double> Normalize(const std::vector<double> &input) {
  int size = input.size();
  double sum = 0.0;
  std::vector<double> Normalized_output(input);

  for (int i = 0; i < size; i++) {
    sum += std::abs(input[i]);
  }
  if (sum == 0.0) {
    return std::vector<double>();
  }

  // As we have half of the energy in negative frequencies, we need to scale, but
  // multiply by two. Otherwise a sinusoid at 0db will result in 0.5 in the spectrum.
  double scale = 2.0 / sum;

  for (int i = 0; i < size; i++) {
    Normalized_output[i] *= scale;
  }

  return Normalized_output;
}

std::vector<double> Windowing(const std::vector<double> &audio_frame,
                              const std::function<std::vector<double>(const std::vector<double> &)> &window_type_func,
                              unsigned int zero_padding_size,
                              bool zero_phase,
                              bool _normalize) {
  int signal_size = audio_frame.size();
  int total_size = signal_size + zero_padding_size;

  if (signal_size <= 1) {
    throw std::runtime_error("Windowing: frame (signal) size should be larger than 1");
  }

  std::vector<double> windowed_signal(static_cast<size_t>(total_size));
  std::vector<double> window(static_cast<size_t>(signal_size));
  if (_normalize) {
    window = Normalize(window_type_func(window));
  } else {
    window = window_type_func(window);
  }

  int i = 0;

  if (zero_phase) {
    // first half of the windowed signal is the
    // second half of the signal with windowing!
    for (int j = signal_size / 2; j < signal_size; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }

    // zero padding
    for (unsigned int j = 0; j < zero_padding_size; j++) {
      windowed_signal[i++] = 0.0;
    }

    // second half of the signal
    for (int j = 0; j < signal_size / 2; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }
  } else {
    // windowed signal
    for (int j = 0; j < signal_size; j++) {
      windowed_signal[i++] = audio_frame[j] * window[j];
    }

    // zero padding
    for (unsigned int j = 0; j < zero_padding_size; j++) {
      windowed_signal[i++] = 0.0;
    }
  }

  return windowed_signal;
}

}  // namespace core
}  // namespace musher