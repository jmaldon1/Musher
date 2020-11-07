#include "src/python/wrapper.h"

#include "src/python/utils.h"

using namespace musher::core;

namespace musher {
namespace python {

int add(int i, int j) { return i + j; }

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

py::array_t<double> _MonoMixer(const std::vector<std::vector<double>>& normalized_samples) {
  std::vector<double> mixed_audio = MonoMixer(normalized_samples);
  return ConvertSequenceToPyarray(mixed_audio);
}

py::array_t<double> _Windowing(const std::vector<double>& audio_frame,
                               const std::function<std::vector<double>(const std::vector<double>&)>& window_type_func,
                               unsigned int size,
                               unsigned int zero_padding_size,
                               bool zero_phase,
                               bool _normalize) {
  const std::vector<double> vec = Windowing(audio_frame, window_type_func, size, zero_padding_size, zero_phase, _normalize);
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

}  // namespace python
}  // namespace musher