#define PY_SSIZE_T_CLEAN  // It is recommended to always define this before Python.h
#include <Python.h>

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_map>

// #include "src/python/temp.h"
#include "src/core/musher_library.h"
// #include "src/core/utils.h"
#include "src/python/utils.h"

using namespace musher::core;
using namespace musher::python;

PyObject* PrintFunctionalMessage(PyObject* self, PyObject* args) {
  // Arguments passed in from Python
  const char* message;

  // Process arguments from Python
  PyArg_ParseTuple(args, "s", &message);

  // Call function
  CPrintFunctionalMessage(message);

  // Return nothing
  return Py_BuildValue("");
}

PyObject* LoadAudioFile(PyObject* self, PyObject* args) {
  // Arguments passed in from Python
  const char* filePath;

  // Process arguments from Python
  if (!PyArg_ParseTuple(args, "s", &filePath)) return NULL;

  // Must convert all C++ exceptions to python exceptions to prevent seg faults
  std::vector<uint8_t> fileData;
  try {
    fileData = CLoadAudioFile(filePath);
    PyObject* pyIntList = VectorToList(fileData);
    return pyIntList;
  } catch (const std::runtime_error& e) {
    const std::string unknownFilePath = StrBetweenSQuotes(e.what());
    // Raise a filenotfounderror in python
    PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, unknownFilePath.c_str());
    return NULL;
  } catch (const std::exception& e) {
    // Catch all standard exceptions
    PyErr_SetString(PyExc_Exception, e.what());
    return NULL;
  } catch (...) {
    // Catch all other exceptions
    PyErr_SetString(PyExc_Exception, "Unknown error occured.");
    return NULL;
  }
}

PyObject* DecodeWav(PyObject* self, PyObject* args) {
  // Arguments passed in from Python
  PyObject* listObj;

  // Process arguments from Python
  if (!PyArg_ParseTuple(args, "O!", &PyList_Type, &listObj)) return NULL;

  try {
    PyObject* wav_decoded_data_dict = PyDict_New();
    std::vector<uint8_t> file_data = ListToVector<uint8_t>(listObj);

    WavDecoded wav_decoded = CDecodeWav(file_data);

    std::vector<std::pair<PyObject*, PyObject*>> kv_pair;
    kv_pair.push_back(CreateKVPair("sample_rate", wav_decoded.sample_rate));
    kv_pair.push_back(CreateKVPair("bit_depth", wav_decoded.bit_depth));
    kv_pair.push_back(CreateKVPair("channels", wav_decoded.channels));
    kv_pair.push_back(CreateKVPair("mono", wav_decoded.mono));
    kv_pair.push_back(CreateKVPair("stereo", wav_decoded.stereo));
    kv_pair.push_back(CreateKVPair("length_in_seconds", wav_decoded.length_in_seconds));
    kv_pair.push_back(CreateKVPair("file_type", wav_decoded.file_type));
    kv_pair.push_back(CreateKVPair("avg_bitrate_kbps", wav_decoded.avg_bitrate_kbps));

    PyObject* py_interleaved_normalized_samples = VectorToList(wav_decoded.interleaved_normalized_samples);
    kv_pair.push_back(CreateKVPairFromPyObject("interleaved_normalized_samples", py_interleaved_normalized_samples));

    PyObject* py_normalized_samples = VectorToList2D(wav_decoded.normalized_samples);
    kv_pair.push_back(CreateKVPairFromPyObject("normalized_samples", py_normalized_samples));

    for (std::pair<PyObject*, PyObject*>& key_val : kv_pair) {
      auto key = key_val.first;
      auto val = key_val.second;
      PyDict_SetItem(wav_decoded_data_dict, key, val);
    }

    return wav_decoded_data_dict;
  } catch (const std::exception& e) {
    // Catch all standard exceptions
    PyErr_SetString(PyExc_Exception, e.what());
    return NULL;
  } catch (...) {
    // Catch all other exceptions
    PyErr_SetString(PyExc_Exception, "Unknown error occured.");
    return NULL;
  }
}

// Define the functions provided by the module
static PyMethodDef cFuncs[] = {
  {
      "PrintFunctionalMessage",
      PrintFunctionalMessage,
      METH_VARARGS,
      "Print a message from a function",
  },
  {
      "load_audio_file",
      LoadAudioFile,
      METH_VARARGS,
      "Load audio file from path",
  },
  {
      "decode_wav",
      DecodeWav,
      METH_VARARGS,
      "Decode Wav file",
  },
  // last one must be empty
  { NULL, NULL, 0, NULL },
};

// define the module
static struct PyModuleDef cModule = { PyModuleDef_HEAD_INIT, "musher_python", NULL, -1, cFuncs };

// Initialize the Python module
PyMODINIT_FUNC PyInit_musher_python(void) { return PyModule_Create(&cModule); }