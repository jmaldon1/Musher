#pragma once

#define PY_SSIZE_T_CLEAN  // It is recommended to always define this before Python.h
#include <Python.h>

#include <vector>
// #define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
// #include "numpy/arrayobject.h"
// #include <variant>
#include <iostream>
#include <typeinfo>
#include <unordered_map>

#include "src/core/utils.h"

namespace musher {
namespace python {

const char* CppTypeToPythonType(const std::string type_name);

/**
 * @brief convert basic c++ type to basic Pyobject type
 * @details Only allow arithmetic types into this function
 *
 * @param var any arithmetic type
 * @return PyObject of the same type as c++ type
 */
template <typename BasicType, typename = std::enable_if_t<std::is_arithmetic<BasicType>::value>>
PyObject* BasicTypeToPyobject(const BasicType& var) {
  /* Get type of variable passed in */
  std::string type_name = typeid(BasicType).name();

  /* Bool has a special case */
  if (type_name == typeid(bool).name()) {
    if (var) return Py_True;
    return Py_False;
  }

  return Py_BuildValue(CppTypeToPythonType(type_name), var);
}

/**
 * @brief convert basic c++ type to basic Pyobject type
 * @details strings have are a special case, so we overload the template function BasicTypeToPyobject for the string
 * case
 *
 * @param var string type
 * @return PyObject of the same type as c++ type
 */
PyObject* BasicTypeToPyobject(const std::string& var);

/**
 * @brief Create a key, value pair that will be used to create a python dictionary.
 *
 * @tparam T
 * @param key Key name.
 * @param val Value name.
 * @return std::pair<PyObject*, PyObject*> A key, value tuple.
 */
template <typename T>
std::pair<PyObject*, PyObject*> CreateKVPair(const char* key, T val) {
  PyObject* pykey = BasicTypeToPyobject(key);
  PyObject* pyval = BasicTypeToPyobject(val);
  return std::make_pair(pykey, pyval);
}

/**
 * @brief Create a key, value pair where the val is already a PyObject
 * that will be used to create a python dictionary.
 *
 * @tparam T
 * @param key Key name.
 * @param val Value name.
 * @return std::pair<PyObject*, PyObject*> A key, value tuple.
 */
std::pair<PyObject*, PyObject*> CreateKVPairFromPyObject(const char* key, PyObject* val);

template <class T>
T pyObjectToBasicType(PyObject* pyObj) {
  T BasicType;
  /* Get type of template */
  std::string type_name = typeid(BasicType).name();

  std::string py_type = CppTypeToPythonType(type_name);

  /* Parse PyObject into corresponding c++ type */
  if (!PyArg_Parse(pyObj, py_type.c_str(), &BasicType)) {
    PyTypeObject* type = pyObj->ob_type;
    const char* p = type->tp_name;

    std::string err_message = "Template of type ";
    err_message += "'";
    err_message += type_name;
    err_message += "'";
    err_message += " does not match PyObject of type ";
    err_message += "'";
    err_message += p;
    err_message += "'";
    err_message += ".";
    throw std::runtime_error(err_message);
  }

  return BasicType;
}

template <typename T>
PyObject* VectorToList(const std::vector<T>& vec) {
  PyObject* list_obj = PyList_New(vec.size());

  for (unsigned int i = 0; i < vec.size(); i++) {
    PyObject* item = BasicTypeToPyobject(vec[i]);
    if (!item) {
      Py_DECREF(list_obj);
      throw std::logic_error("Unable to allocate memory for Python list");
    }
    PyList_SetItem(list_obj, i, item);
  }
  return list_obj;
}

template <typename T>
PyObject* VectorToList2D(const std::vector<std::vector<T>>& vec_2d) {
  unsigned int vec_2d_size = vec_2d.size();
  PyObject* list_obj_2d = PyList_New(vec_2d_size);

  for (unsigned int i = 0; i < vec_2d_size; i++) {
    PyObject* list_item = VectorToList(vec_2d[i]);
    PyList_SetItem(list_obj_2d, i, list_item);
  }
  return list_obj_2d;
}

template <typename T>
std::vector<T> ListToVector(PyObject*& listObj) {
  std::vector<T> data;
  if (!PyList_Check(listObj)) {
    std::string err_message = "PyObject passed is not a list";
    throw std::runtime_error(err_message);
  }
  for (Py_ssize_t i = 0; i < PyList_Size(listObj); i++) {
    PyObject* value = PyList_GetItem(listObj, i);
    data.push_back(pyObjectToBasicType<T>(value));
  }
  return data;
}

// /** Convert a c++ vector into a numpy array
//  *
//  * @param const vector<T>& vec : 1D vector data
//  * @return PyArrayObject* array : converted numpy array
//  *
//  * Transforms an arbitrary C++ vector into a numpy array. Throws in case of
//  * unregular shape. The array may contain empty columns or something else, as
//  * long as it's shape is square.
//  *
//  * Warning this routine makes a copy of the memory!
//  */
// template <typename T>
// static PyArrayObject* VectorToNpArray(const std::vector<T>& vec, int type_num = PyArray_FLOAT) {
//   // rows not empty
//   if (!vec.empty()) {
//     size_t n_rows = vec.size();
//     npy_intp dims[1] = { n_rows };

//     PyArrayObject* vec_array = (PyArrayObject*)PyArray_SimpleNew(1, dims, type_num);
//     T* vec_array_pointer = (T*)PyArray_DATA(vec_array);

//     std::copy(vec.begin(), vec.end(), vec_array_pointer);
//     return vec_array;

//     // no data at all
//   } else {
//     npy_intp dims[1] = { 0 };
//     return (PyArrayObject*)PyArray_ZEROS(1, dims, PyArray_FLOAT, 0);
//   }
// }

}  // namespace python
}  // namespace musher
