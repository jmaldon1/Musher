#include "src/python/utils.h"

using namespace musher::core;

namespace musher {
namespace python {

int add(int i, int j);

py::array_t<uint8_t> _LoadAudioFile(const std::string& file_path);

py::dict _DecodeWavFromData(std::vector<uint8_t>& file_data);

py::dict _DecodeWavFromFile(const std::string file_path);

py::array_t<double> _MonoMixer(const std::vector<std::vector<double>>& normalized_samples);

py::array_t<double> _Windowing(
    const std::vector<double>& audio_frame,
    const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func = BlackmanHarris62dB,
    unsigned size = 1024,
    unsigned zero_padding_size = 0,
    bool zero_phase = true,
    bool _normalize = true);

py::array_t<double> _BlackmanHarris(const std::vector<double>& window, double a0, double a1, double a2, double a3);
py::array_t<double> _BlackmanHarris62dB(const std::vector<double>& window);
py::array_t<double> _BlackmanHarris92dB(const std::vector<double>& window);
}  // namespace python
}  // namespace musher