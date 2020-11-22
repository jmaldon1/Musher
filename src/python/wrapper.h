#include <pybind11/numpy.h>

#include <functional>
#include <string>
#include <vector>

#include "src/python/utils.h"

using namespace musher::core;
namespace py = pybind11;

namespace musher {
namespace python {

py::array_t<uint8_t> _LoadAudioFile(const std::string& file_path);

py::dict _DecodeWavFromData(std::vector<uint8_t>& file_data);

py::dict _DecodeWavFromFile(const std::string file_path);

py::dict _DecodeMp3FromFile(const std::string file_path);

py::array_t<double> _MonoMixer(const std::vector<std::vector<double>>& normalized_samples);

py::array_t<double> _Windowing(const std::vector<double>& audio_frame,
                               const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func,
                               unsigned zero_padding_size,
                               bool zero_phase,
                               bool _normalize);

py::array_t<double> _Square(const std::vector<double>& window);
py::array_t<double> _BlackmanHarris(const std::vector<double>& window, double a0, double a1, double a2, double a3);
py::array_t<double> _BlackmanHarris62dB(const std::vector<double>& window);
py::array_t<double> _BlackmanHarris92dB(const std::vector<double>& window);

py::array_t<double> _ConvertToFrequencySpectrum(const std::vector<double>& audio_frame);

std::vector<std::tuple<double, double>> _PeakDetect(const std::vector<double>& inp,
                                                    double threshold,
                                                    bool interpolate,
                                                    std::string sort_by,
                                                    int max_num_peaks,
                                                    double range,
                                                    int min_pos,
                                                    int max_pos);

std::vector<std::tuple<double, double>> _SpectralPeaks(const std::vector<double>& input_spectrum,
                                                       double threshold,
                                                       std::string sort_by,
                                                       unsigned int max_num_peaks,
                                                       double sample_rate,
                                                       int min_pos,
                                                       int max_pos);
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
                                   std::string _normalized);

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
                          std::string _normalized);

py::dict _EstimateKey(const std::vector<double>& pcp,
                      const bool use_polphony,
                      const bool use_three_chords,
                      const unsigned int num_harmonics,
                      const double slope,
                      const std::string profile_type,
                      const bool use_maj_min);

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
                    double window_size);
}  // namespace python
}  // namespace musher