#include <pybind11/functional.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/stl_bind.h>

#include "src/core/framecutter.h"
#include "src/python/module_descriptions.h"
#include "src/python/wrapper.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<std::tuple<double, double>>);
using namespace musher::core;
using namespace musher::python;

PYBIND11_MODULE(musher_python, m) {
  m.doc() = "pybind11 musher_python plugin";  // optional module docstring

  py::bind_vector<std::vector<std::tuple<double, double>>>(m, "peaks");

  m.def("load_audio_file", &_LoadAudioFile, load_audio_file_description, py::arg("file_path"));

  m.def("decode_wav_from_data", &_DecodeWavFromData, decode_wav_from_data_description, py::arg("file_data"));

  m.def("decode_wav_from_file", &_DecodeWavFromFile, decode_wav_from_file_description, py::arg("file_path"));

  m.def("decode_mp3_from_file", &_DecodeMp3FromFile, decode_mp3_from_file_description, py::arg("file_path"));

  m.def("mono_mixer", &_MonoMixer, mono_mixer_description, py::arg("input"));

  // Framecutter will be treated like an iterator in python.
  py::class_<Framecutter>(m, "Framecutter", framecutter_description)
      .def(py::init<const std::vector<double>, int, int, bool, bool, double>(), framecutter_init_description,
           py::arg("buffer"), py::arg("frame_size") = 1024, py::arg("hop_size") = 512,
           py::arg("start_from_center") = true, py::arg("last_frame_to_end_of_file") = false,
           py::arg("valid_frame_threshold_ratio") = 0.)
      .def(
          "__iter__", [](const Framecutter& fcutter) { return py::make_iterator(fcutter.begin(), fcutter.end()); },
          py::keep_alive<0, 1>());

  m.def("windowing", &_Windowing, windowing_description, py::arg("audio_frame"),
        py::arg("window_type_func") = py::cpp_function(BlackmanHarris92dB), py::arg("zero_padding_size") = 0,
        py::arg("zero_phase") = true, py::arg("_normalize") = true);

  m.def("square", &_Square, square_description, py::arg("window"));
  m.def("blackmanharris", &_BlackmanHarris, blackmanharris_description, py::arg("window"), py::arg("a0"), py::arg("a1"),
        py::arg("a2"), py::arg("a3"));

  m.def("blackmanharris62dB", &_BlackmanHarris62dB, blackmanharris62dB_description, py::arg("window"));

  m.def("blackmanharris92dB", &_BlackmanHarris92dB, blackmanharris92dB_description, py::arg("window"));

  m.def("convert_to_frequency_spectrum", &_ConvertToFrequencySpectrum, convert_to_frequency_spectrum_description,
        py::arg("audio_frame"));

  m.def("peak_detect", &_PeakDetect, peak_detect_description, py::arg("inp"), py::arg("threshold") = -1000.0,
        py::arg("interpolate") = true, py::arg("sort_by") = "position", py::arg("max_num_peaks") = 0,
        py::arg("range") = 0., py::arg("min_pos") = 0, py::arg("max_pos") = 0);

  m.def("spectral_peaks", &_SpectralPeaks, spectral_peaks_description, py::arg("input_spectrum"),
        py::arg("threshold") = -1000.0, py::arg("sort_by") = "position", py::arg("max_num_peaks") = 100,
        py::arg("sample_rate") = 44100., py::arg("min_pos") = 0, py::arg("max_pos") = 0);

  m.def("hpcp", &_HPCP, hpcp_description, py::arg("frequencies"), py::arg("magnitudes"), py::arg("size") = 12,
        py::arg("reference_frequency") = 440.0, py::arg("harmonics") = 0, py::arg("band_preset") = true,
        py::arg("band_split_frequency") = 500.0, py::arg("min_frequency") = 40.0, py::arg("max_frequency") = 5000.0,
        py::arg("_weight_type") = "squared cosine", py::arg("window_size") = 1.0, py::arg("max_shifted") = false,
        py::arg("non_linear") = false, py::arg("_normalized") = "unit max");

  m.def("hpcp_from_peaks", &_HPCPFromPeaks, hpcp_from_peaks_description, py::arg("peaks"), py::arg("size") = 12,
        py::arg("reference_frequency") = 440.0, py::arg("harmonics") = 0, py::arg("band_preset") = true,
        py::arg("band_split_frequency") = 500.0, py::arg("min_frequency") = 40.0, py::arg("max_frequency") = 5000.0,
        py::arg("_weight_type") = "squared cosine", py::arg("window_size") = 1.0, py::arg("max_shifted") = false,
        py::arg("non_linear") = false, py::arg("_normalized") = "unit max");

  m.def("estimate_key", &_EstimateKey, estimate_key_description, py::arg("pcp"), py::arg("use_polphony") = true,
        py::arg("use_three_chords") = true, py::arg("num_harmonics") = 4, py::arg("slope") = .6,
        py::arg("profile_type") = "Bgate", py::arg("use_maj_min") = false);

  m.def("detect_key", &_DetectKey, detect_key_description, py::arg("normalized_samples"),
        py::arg("sample_rate") = 44100., py::arg("profile_type") = "Bgate", py::arg("use_polphony") = true,
        py::arg("use_three_chords") = true, py::arg("num_harmonics") = 4, py::arg("slope") = .6,
        py::arg("use_maj_min") = false, py::arg("pcp_size") = 36, py::arg("frame_size") = 4096,
        py::arg("hop_size") = 512, py::arg("window_type_func") = py::cpp_function(BlackmanHarris62dB),
        py::arg("max_num_peaks") = 100, py::arg("window_size") = .5);
}
