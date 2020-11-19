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

  // Convert 2D vector to 2D numpy array with copy.
  std::vector<std::vector<double>> normalized_samples = wav_decoded.normalized_samples;
  size_t rows = normalized_samples.size();
  size_t cols = normalized_samples[0].size();
  py::array_t<double, py::array::c_style> numpy_arr({ rows, cols });
  auto ra = numpy_arr.mutable_unchecked();
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      ra(i, j) = normalized_samples[i][j];
    };
  };
  output_dict["normalized_samples"] = numpy_arr;

  return output_dict;
}

py::dict ConvertMp3DecodedToPyDict(Mp3Decoded mp3_decoded) {
  py::dict output_dict;
  output_dict["sample_rate"] = mp3_decoded.sample_rate;
  output_dict["channels"] = mp3_decoded.channels;
  output_dict["mono"] = mp3_decoded.mono;
  output_dict["stereo"] = mp3_decoded.stereo;
  output_dict["samples_per_channel"] = mp3_decoded.samples_per_channel;
  output_dict["length_in_seconds"] = mp3_decoded.length_in_seconds;
  output_dict["file_type"] = mp3_decoded.file_type;
  output_dict["avg_bitrate_kbps"] = mp3_decoded.avg_bitrate_kbps;

  // Convert 2D vector to 2D numpy array with copy.
  std::vector<std::vector<double>> normalized_samples = mp3_decoded.normalized_samples;
  size_t rows = normalized_samples.size();
  size_t cols = normalized_samples[0].size();
  py::array_t<double, py::array::c_style> numpy_arr({ rows, cols });
  auto ra = numpy_arr.mutable_unchecked();
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < cols; j++) {
      ra(i, j) = normalized_samples[i][j];
    };
  };
  output_dict["normalized_samples"] = numpy_arr;

  return output_dict;
}

py::dict ConvertKeyOutputToPyDict(KeyOutput key_output) {
  py::dict key_output_dict;
  key_output_dict["key"] = key_output.key;
  key_output_dict["scale"] = key_output.scale;
  key_output_dict["strength"] = key_output.strength;
  key_output_dict["first_to_second_relative_strength"] = key_output.first_to_second_relative_strength;
  return key_output_dict;
}

}  // namespace python
}  // namespace musher