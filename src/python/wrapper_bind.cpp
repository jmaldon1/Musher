#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "src/core/musher_library.h"

namespace py = pybind11;
using namespace musher::core;

int add(int i, int j) { return i + j; }

py::array_t<uint8_t> _LoadAudioFile(const std::string& file_path) {
  std::vector<uint8_t> fileData = LoadAudioFile(file_path);

  return py::array(fileData.size(), fileData.data());
}

py::dict _DecodeWav(py::array_t<uint8_t>& file_data_numpy_arr) {
  // Convert numpy array to c++ vector
  // https://pybind11.readthedocs.io/en/stable/advanced/pycpp/object.html#casting-back-and-forth
  std::vector<uint8_t> file_data = file_data_numpy_arr.cast<std::vector<uint8_t>>();
  WavDecoded wav_decoded = DecodeWav(file_data);
  py::dict output_dict;
  output_dict["sample_rate"] = wav_decoded.sample_rate;
  std::vector<double> interleaved_normalized_samples = wav_decoded.interleaved_normalized_samples;
  output_dict["interleaved_normalized_samples"] =
      py::array(interleaved_normalized_samples.size(), interleaved_normalized_samples.data());

  return output_dict;
}

PYBIND11_MODULE(musher_python, m) {
  m.doc() = "pybind11 musher_python plugin";  // optional module docstring

  m.def("add", &add, "A function which adds two numbers");
  m.def("load_audio_file", &_LoadAudioFile, "description");
  m.def("decode_wav", &_DecodeWav, "description");
}
