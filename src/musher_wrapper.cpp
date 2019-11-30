#define PY_SSIZE_T_CLEAN // It is recommended to always define this before Python.h
#include <Python.h>
#include <iostream>
#include <string>
#include <unordered_map> 
#include <stdexcept>
#include <algorithm>

#include "musher_library.h"
#include "utils.h"
#include "python_utils.h"


/* Unordered map of python decode functions mapped to their C++ function equivalent */
// typedef bool (*DecodeFunction)(const char*); // function pointer type
// std::unordered_map<std::string, DecodeFunction> uMapDecodeFuncs({
//     {"DecodeWav", &CDecodeWav}
// });


PyObject* PrintFunctionalMessage(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* message;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "s",
                    &message);

    /* Call function */
    CPrintFunctionalMessage(message);

    /* Return nothing */
    return Py_BuildValue("");
}


PyObject* DecodeWav(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* message;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "s",
                    &message);

    PyObject *wavDecodedDataDict = PyDict_New();
    try{
        std::vector<uint8_t> fileData;
        const std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
        fileData = CLoadAudioFile(filePath);

        std::unordered_map< std::string, std::variant<int, uint32_t, double, bool> > wavDecodedData;
        std::vector< std::vector<double> > audioBuffer;
        CDecodeWav(wavDecodedData, fileData, audioBuffer);

        for (const auto & [ key, value ] : wavDecodedData) {
            // TODO: Possibly fix this to not have to use const_cast
            char* key_copy = new char[key.length()]();;
            strcpy(key_copy, key.c_str());
            // const_cast<char *>(key.c_str())
            PyObject* k = Py_BuildValue("s", key_copy);
            PyObject* v = variantToPyobject(value);
            PyDict_SetItem(wavDecodedDataDict, k, v);
        }
    }
    catch( const std::exception& e )
    { /* Catch all standard exceptions */
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }
    catch ( ... ) 
    { /* Catch all other exceptions */
        PyErr_SetString(PyExc_Exception, "Unknown error occured.");
        return NULL;
    }

    /* Call function */
    // CDecodeWav(message);

    /* Return nothing */
    return wavDecodedDataDict;
}


PyObject* LoadAudioFile(PyObject* self, PyObject* args)
{
    /* Arguments passed in from Python */
    const char* filePath;

    /* Process arguments from Python */
    PyArg_ParseTuple(args, "s", &filePath);

    /* 
    Must convert all c++ exceptions to python exceptions to prevent seg faults
    */
    std::vector<uint8_t> fileData;
    try{
        fileData = CLoadAudioFile(filePath);

        /* convert uint8_t vector to int vector */
        // std::vector<int> fileDataInt;
        // std::transform(fileData.begin(), fileData.end(), std::back_inserter(fileDataInt), unint8_t_to_int);

        // for (std::vector<int>::const_iterator i = fileDataInt.begin(); i != fileDataInt.end(); ++i)
        //     std::cout << *i << ' ';

        // PyObject* pyIntList = vector_to_list_int(fileDataInt);
        PyObject* pyIntList = vector_to_list_uint8_t(fileData);
        return pyIntList;

    }
    catch( const std::runtime_error& e )
    {
        const std::string unknownFilePath = get_str_between_two_squotes(e.what());
        const char* unknownFilePathChar = unknownFilePath.c_str();
        /* Raise a filenotfounderror in python */
        PyErr_SetFromErrnoWithFilename(PyExc_FileNotFoundError, unknownFilePathChar);
        return NULL;
    }
    catch( const std::exception& e )
    { /* Catch all standard exceptions */
        PyErr_SetString(PyExc_Exception, e.what());
        return NULL;
    }
    catch ( ... ) 
    { /* Catch all other exceptions */
        PyErr_SetString(PyExc_Exception, "Unknown error occured.");
        return NULL;
    }


    // fileData = CLoadAudioFile(filePath);

    // PyObject* decodeFunc;
    /* Process arguments from Python */
    // PyArg_ParseTuple(args, "sO",
    //                 &message, &decodeFunc);

    // if(!PyCallable_Check(decodeFunc)){
    //     PyErr_SetString(PyExc_TypeError, "A callable is required");
    //     return 0;
    // }

    // /* Get name of python function that was passed in */
    // PyObject* funcNameUnicode = PyObject_GetAttrString(decodeFunc, "__name__");
    // PyObject* funcNamePythonBytes = PyUnicode_AsEncodedString(funcNameUnicode, "utf-8", "~E~");
    // const char *funcName = PyBytes_AsString(funcNamePythonBytes);
    // // std::cout << funcName << std::endl;

    // Py_XDECREF(funcNameUnicode);
    // Py_XDECREF(funcNamePythonBytes);

    // /* Find decode function in umap */
    // std::unordered_map<std::string, DecodeFunction>::const_iterator got = uMapDecodeFuncs.find(funcName);

    // if ( got == uMapDecodeFuncs.end() ) {
    //     std::cout << "decode function not found";
    //     return 0;
    // }
    // else {
    //     /* Get decode function from umap */
    //     bool (*decodeFunc)(const char*) = got->second;
    //     CAcceptDecode("Hello from Decode wav", decodeFunc);
    // }
    /* Call function */
    // PyObject* cbArgs = PyTuple_New(1);
    // PyTuple_SetItem(cbArgs, 0, Py_BuildValue("s", message));
    // PyObject *rv = PyObject_CallObject(decodeFunc, cbArgs);

    /* Return nothing */
    // return Py_BuildValue("");
}


/* define the functions provided by the module */
static PyMethodDef cFuncs[] =
{
    /* Wrapper for PrintFunctionalMessage */
    {
        "PrintFunctionalMessage",
        PrintFunctionalMessage,
        METH_VARARGS,
        "Print a message from a function"
    },
    {
        "load_audio_file",
        LoadAudioFile,
        METH_VARARGS,
        "Load audio file from path"
    },
    {
        "decode_wav",
        DecodeWav,
        METH_VARARGS,
        "Decode Wav file"
    },
    /* last one must be empty */
    {NULL, NULL, 0, NULL}
};


/* define the module */
static struct PyModuleDef cModule =
{
    PyModuleDef_HEAD_INIT,
    "musher",
    NULL,
    -1,
    cFuncs
};


/* Initialize the Python module */
PyMODINIT_FUNC PyInit_musher(void)
{
    return PyModule_Create(&cModule);
}