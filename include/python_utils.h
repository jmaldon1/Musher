#include <Python.h> // Must be first
#include <vector>

PyObject* vector_to_list_float(const std::vector<float> &data);
PyObject* vector_to_list_int(const std::vector<int> &data);
PyObject* vector_to_list_uint8_t(const std::vector<uint8_t> &data);

template <class AnyVariant>
PyObject* variantToPyobject(AnyVariant var)
{
	if (std::holds_alternative<int>(var))
        return std::visit([](int arg) {
        	// return arg;
        	return Py_BuildValue("i", arg);
        }, var);

	if (std::holds_alternative<uint32_t>(var))
        return std::visit([](uint32_t arg) {
        	return Py_BuildValue("I", arg);
        }, var);

    if (std::holds_alternative<bool>(var))
        return std::visit([](bool arg) {
        	return Py_BuildValue("p", arg);
        }, var);

    if (std::holds_alternative<double>(var))
        return std::visit([](double arg) {return Py_BuildValue("d", arg);}, var);

    throw;
}