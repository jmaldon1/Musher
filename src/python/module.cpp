#include "src/python/utils.h"
#include "src/python/wrapper.h"
// #include "src/core/test/utils.h"
// using namespace musher::core;

PYBIND11_MAKE_OPAQUE(std::vector<std::tuple<double, double>>);
using namespace musher::core;
using namespace musher::python;

PYBIND11_MODULE(musher_python, m) {
  m.doc() = "pybind11 musher_python plugin";  // optional module docstring

  py::bind_vector<std::vector<std::tuple<double, double>>>(m, "peaks");
  m.def("add", &add, "A function which adds two numbers");
  m.def("load_audio_file", &_LoadAudioFile, "description");
  m.def("decode_wav_from_data", &_DecodeWavFromData, "description");
  m.def("decode_wav_from_file", &_DecodeWavFromFile, "description");
  m.def("mono_mixer", &_MonoMixer, "description");
  // Framecutter will be treated like an iterator in python.
  py::class_<Framecutter>(m, "Framecutter")
      .def(py::init<const std::vector<double>, int, int, bool, bool, double>(), py::arg("buffer"),
           py::arg("frame_size") = 1024, py::arg("hop_size") = 512, py::arg("start_from_center") = true,
           py::arg("last_frame_to_end_of_file") = false, py::arg("valid_frame_threshold_ratio") = 0.)
      .def(
          "__iter__", [](const Framecutter& fcutter) { return py::make_iterator(fcutter.begin(), fcutter.end()); },
          py::keep_alive<0, 1>());
  m.def("windowing", &_Windowing, "description", py::arg("audio_frame"),
        py::arg("window_type_func") = py::cpp_function(BlackmanHarris92dB), py::arg("size") = 1024,
        py::arg("zero_padding_size") = 0, py::arg("zero_phase") = true, py::arg("_normalize") = true);
  m.def("blackmanharris", &_BlackmanHarris, "description");
  m.def("blackmanharris62dB", &_BlackmanHarris62dB, "description");
  m.def("blackmanharris92dB", &_BlackmanHarris92dB, "description");
  m.def("convert_to_frequency_spectrum", &_ConvertToFrequencySpectrum, "description");
  m.def("spectral_peaks", &_SpectralPeaks, "description", py::arg("input_spectrum"), py::arg("threshold") = -1000.0,
        py::arg("sort_by") = "position", py::arg("max_num_peaks") = 100, py::arg("sample_rate") = 44100.,
        py::arg("min_pos") = 0, py::arg("max_pos") = 0);
  m.def("hpcp", &_HPCP, "description", py::arg("peaks"), py::arg("size") = 12, py::arg("reference_frequency") = 440.0,
        py::arg("harmonics") = 0, py::arg("band_preset") = true, py::arg("band_split_frequency") = 500.0,
        py::arg("min_frequency") = 40.0, py::arg("max_frequency") = 5000.0, py::arg("_weight_type") = "squared cosine",
        py::arg("window_size") = 1.0, py::arg("sample_rate") = 44100., py::arg("max_shifted") = false,
        py::arg("non_linear") = false, py::arg("_normalized") = "unit max");
  m.def("estimate_key", &_EstimateKey, "description", py::arg("pcp"), py::arg("use_polphony") = true,
        py::arg("use_three_chords") = true, py::arg("num_harmonics") = 4, py::arg("slope") = .6,
        py::arg("profile_type") = "Bgate", py::arg("use_maj_min") = false);
  m.def("detect_key", &_DetectKey, "description", py::arg("normalized_samples"), py::arg("sample_rate")=44100.,
        py::arg("profile_type")="Bgate", py::arg("pcp_size")=36, py::arg("num_harmonics")=4,
        py::arg("frame_size")=4096, py::arg("hop_size")=512, py::arg("window_type_func")=py::cpp_function(BlackmanHarris62dB),
        py::arg("max_num_peaks")=0, py::arg("hop_size")=.5);
}
