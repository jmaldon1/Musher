#define PY_SSIZE_T_CLEAN  // It is recommended to always define this before Python.h
// #include <Python.h>
#include "src/python/utils.h"

#include <stdexcept>
#include <vector>

using namespace musher::core;

namespace musher {
namespace python {

py::dict ConvertWavDecodedToPyDict(WavDecoded wav_decoded) {
  py::dict output_dict;
  output_dict["sample_rate"] = wav_decoded.sample_rate;
  output_dict["bit_depth"] = wav_decoded.bit_depth;
  output_dict["channels"] = wav_decoded.channels;
  output_dict["mono"] = wav_decoded.mono;
  output_dict["stereo"] = wav_decoded.stereo;
  output_dict["samples_per_channel"] = wav_decoded.samples_per_channel;
  output_dict["length_in_seconds"] = wav_decoded.length_in_seconds;
  output_dict["file_type"] = wav_decoded.file_type;
  output_dict["avg_bitrate_kbps"] = wav_decoded.avg_bitrate_kbps;
  // Convert vector to numpy array without copying.
  std::vector<double> interleaved_normalized_samples = wav_decoded.interleaved_normalized_samples;
  output_dict["interleaved_normalized_samples"] = ConvertSequenceToPyarray(interleaved_normalized_samples);

  // Convert 2D vector to 2D numpy array with copy.
  std::vector<std::vector<double>> normalized_samples = wav_decoded.normalized_samples;
  size_t rows = normalized_samples.size();
  size_t cols = normalized_samples[0].size();
  py::array_t<float, py::array::c_style> numpy_arr({ rows, cols });
  auto ra = numpy_arr.mutable_unchecked();
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      ra(i, j) = normalized_samples[i][j];
    };
  };
  output_dict["normalized_samples"] = numpy_arr;

  return output_dict;
}

}  // namespace python
}  // namespace musher