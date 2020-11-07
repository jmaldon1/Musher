#include "src/python/wrapper.h"
#include "src/python/utils.h"
// #include "src/core/test/utils.h"
// using namespace musher::core;

using namespace musher::core;
using namespace musher::python;

PYBIND11_MODULE(musher_python, m) {
  m.doc() = "pybind11 musher_python plugin";  // optional module docstring

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
}
