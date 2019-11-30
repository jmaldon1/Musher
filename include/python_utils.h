#include <Python.h> // Must be first
#include <vector>
#include <variant>

PyObject* vector_to_list_float(const std::vector<float> &data);
PyObject* vector_to_list_int(const std::vector<int> &data);
PyObject* vector_to_list_uint8_t(const std::vector<uint8_t> &data);

/*  Accept a variant with any number of types
    The '...' will pack and unpack all variant types
*/
template<typename ...types>
PyObject* variantToPyobject(const std::variant<types...> &var)
{
	if (std::holds_alternative<int>(var))
        return std::visit([](int arg) { return Py_BuildValue("i", arg); }, var);

	if (std::holds_alternative<uint32_t>(var))
        return std::visit([](uint32_t arg) { return Py_BuildValue("I", arg); }, var);

    if (std::holds_alternative<bool>(var))
        return std::visit([](bool arg) {
            if (arg)
        	   return Py_True;
            return Py_False;
        }, var);

    if (std::holds_alternative<double>(var))
        return std::visit([](double arg) { return Py_BuildValue("d", arg); }, var);

    std::string err_message = "Variant data type not found";
    throw std::runtime_error(err_message);
}