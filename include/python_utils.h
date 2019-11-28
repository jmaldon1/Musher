#ifndef __PYTHON_UTILS__
#define __PYTHON_UTILS__

#include <Python.h> // Must be first
#include <vector>

PyObject* vector_to_list_float(const std::vector<float> &data);
PyObject* vector_to_list_int(const std::vector<int> &data);
PyObject* vector_to_list_uint8_t(const std::vector<uint8_t> &data);

#endif /* __PYTHON_UTILS__ */