#define PY_SSIZE_T_CLEAN // It is recommended to always define this before Python.h
// #include <Python.h>
#include <vector>
#include <stdexcept>

#include "src/python/utils.h"


namespace musher {
namespace python {

const char* CppTypeToPythonType(const std::string type_name) {
    std::unordered_map<std::string, std::string> cpp_to_python_types({
        {typeid(int).name(),         "i"},
        {typeid(uint32_t).name(),    "I"},
        {typeid(uint16_t).name(),    "H"},
        {typeid(uint8_t).name(),     "B"},
        {typeid(double).name(),      "d"},
        {typeid(float).name(),       "f"},
        {typeid(std::string).name(), "s"},
        {typeid(char*).name(),       "s"},
        {typeid(char).name(),        "C"},
    });

    std::unordered_map<std::string, std::string>::const_iterator got = cpp_to_python_types.find(type_name);

    if ( got == cpp_to_python_types.end() )
        {
            std::string err_message = "Could not match C++ data type to Pyobject type";
            throw std::runtime_error(err_message);
        }

    return got->second.c_str();
}

PyObject* BasicTypeToPyobject(const std::string &var)
{ 
    return Py_BuildValue("s", var.c_str());
}

std::pair<PyObject*, PyObject*> CreateKVPairFromPyObject(const char* key, PyObject* val)
{
    PyObject* pykey = BasicTypeToPyobject(key);
    return std::make_pair(pykey, val);
}

}  // namespace python
}  // namespace musher