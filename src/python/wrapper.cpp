#include "src/python/wrapper.h"

#include <pybind11/numpy.h>

#include "src/core/audio_decoders.h"
#include "src/core/hpcp.h"
#include "src/core/mono_mixer.h"
#include "src/core/peak_detect.h"
#include "src/core/spectral_peaks.h"
#include "src/core/spectrum.h"
#include "src/core/windowing.h"
#include "src/python/utils.h"

using namespace musher::core;
namespace py = pybind11;

namespace musher {
namespace python {

py::array_t<uint8_t> _LoadAudioFile(const std::string& file_path) {
  std::vector<uint8_t> fileData = LoadAudioFile(file_path);
  return ConvertSequenceToPyarray(fileData);
}

py::dict _DecodeWavFromData(std::vector<uint8_t>& file_data) {
  WavDecoded wav_decoded = DecodeWav(file_data);
  return ConvertWavDecodedToPyDict(wav_decoded);
}

py::dict _DecodeWavFromFile(const std::string file_path) {
  WavDecoded wav_decoded = DecodeWav(file_path);
  return ConvertWavDecodedToPyDict(wav_decoded);
}

py::dict _DecodeMp3FromFile(const std::string file_path) {
  Mp3Decoded mp3_decoded = DecodeMp3(file_path);
  return ConvertMp3DecodedToPyDict(mp3_decoded);
}

py::array_t<double> _MonoMixer(const std::vector<std::vector<double>>& normalized_samples) {
  std::vector<double> mixed_audio = MonoMixer(normalized_samples);
  return ConvertSequenceToPyarray(mixed_audio);
}

py::array_t<double> _Windowing(const std::vector<double>& audio_frame,
                               const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func,
                               unsigned int zero_padding_size,
                               bool zero_phase,
                               bool _normalize) {
  const std::vector<double> vec = Windowing(audio_frame, window_type_func, zero_padding_size, zero_phase, _normalize);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _Square(const std::vector<double>& window) {
  const std::vector<double> vec = Square(window);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _BlackmanHarris(const std::vector<double>& window, double a0, double a1, double a2, double a3) {
  const std::vector<double> vec = BlackmanHarris(window, a0, a1, a2, a3);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _BlackmanHarris62dB(const std::vector<double>& window) {
  const std::vector<double> vec = BlackmanHarris62dB(window);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _BlackmanHarris92dB(const std::vector<double>& window) {
  const std::vector<double> vec = BlackmanHarris92dB(window);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _ConvertToFrequencySpectrum(const std::vector<double>& audio_frame) {
  const std::vector<double> vec = ConvertToFrequencySpectrum(audio_frame);
  return ConvertSequenceToPyarray(vec);
}

std::vector<std::tuple<double, double>> _PeakDetect(const std::vector<double>& inp,
                                                    double threshold,
                                                    bool interpolate,
                                                    std::string sort_by,
                                                    int max_num_peaks,
                                                    double range,
                                                    int min_pos,
                                                    int max_pos) {
  // Figure out how to pass vector of tuples back without copy.
  std::vector<std::tuple<double, double>> vec =
      PeakDetect(inp, threshold, interpolate, sort_by, max_num_peaks, range, min_pos, max_pos);
  return vec;
}

std::vector<std::tuple<double, double>> _SpectralPeaks(const std::vector<double>& input_spectrum,
                                                       double threshold,
                                                       std::string sort_by,
                                                       unsigned int max_num_peaks,
                                                       double sample_rate,
                                                       int min_pos,
                                                       int max_pos) {
  // Figure out how to pass vector of tuples back without copy.
  std::vector<std::tuple<double, double>> vec =
      SpectralPeaks(input_spectrum, threshold, sort_by, max_num_peaks, sample_rate, min_pos, max_pos);
  return vec;
}

py::array_t<double> _HPCPFromPeaks(const std::vector<std::tuple<double, double>>& peaks,
                                   unsigned int size,
                                   double reference_frequency,
                                   unsigned int harmonics,
                                   bool band_preset,
                                   double band_split_frequency,
                                   double min_frequency,
                                   double max_frequency,
                                   std::string _weight_type,
                                   double window_size,
                                   bool max_shifted,
                                   bool non_linear,
                                   std::string _normalized) {
  const std::vector<double> vec =
      HPCP(peaks, size, reference_frequency, harmonics, band_preset, band_split_frequency, min_frequency, max_frequency,
           _weight_type, window_size, max_shifted, non_linear, _normalized);
  return ConvertSequenceToPyarray(vec);
}

py::array_t<double> _HPCP(const std::vector<double>& frequencies,
                          const std::vector<double>& magnitudes,
                          unsigned int size,
                          double reference_frequency,
                          unsigned int harmonics,
                          bool band_preset,
                          double band_split_frequency,
                          double min_frequency,
                          double max_frequency,
                          std::string _weight_type,
                          double window_size,
                          bool max_shifted,
                          bool non_linear,
                          std::string _normalized) {
  const std::vector<double> vec =
      HPCP(frequencies, magnitudes, size, reference_frequency, harmonics, band_preset, band_split_frequency,
           min_frequency, max_frequency, _weight_type, window_size, max_shifted, non_linear, _normalized);
  return ConvertSequenceToPyarray(vec);
}

py::dict _EstimateKey(const std::vector<double>& pcp,
                      const bool use_polphony,
                      const bool use_three_chords,
                      const unsigned int num_harmonics,
                      const double slope,
                      const std::string profile_type,
                      const bool use_maj_min) {
  KeyOutput key_output =
      EstimateKey(pcp, use_polphony, use_three_chords, num_harmonics, slope, profile_type, use_maj_min);
  return ConvertKeyOutputToPyDict(key_output);
}

py::dict _DetectKey(const std::vector<std::vector<double>>& normalized_samples,
                    double sample_rate,
                    const std::string profile_type,
                    const bool use_polphony,
                    const bool use_three_chords,
                    const unsigned int num_harmonics,
                    const double slope,
                    const bool use_maj_min,
                    const unsigned int pcp_size,
                    const int frame_size,
                    const int hop_size,
                    const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func,
                    unsigned int max_num_peaks,
                    double window_size) {
  KeyOutput key_output =
      DetectKey(normalized_samples, sample_rate, profile_type, use_polphony, use_three_chords, num_harmonics, slope,
                use_maj_min, pcp_size, frame_size, hop_size, window_type_func, max_num_peaks, window_size);
  return ConvertKeyOutputToPyDict(key_output);
}

}  // namespace python
}  // namespace musher