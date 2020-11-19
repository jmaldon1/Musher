#pragma once

#include <pybind11/numpy.h>

#include "src/core/audio_decoders.h"
#include "src/core/key.h"

using namespace musher::core;
namespace py = pybind11;

namespace musher {
namespace python {

/**
 * @brief Convert a sequence C++ type to a numpy array WITHOUT copying.
 * 
 * @tparam Sequence 
 * @param seq A sequence.
 * @return py::array_t<typename Sequence::value_type> py::array_t<typename Sequence::value_type> Numpy array.
 */
template <typename Sequence>
py::array_t<typename Sequence::value_type> ConvertSequenceToPyarray(Sequence& seq) {
    // Move entire object to heap (Ensure is moveable!). Memory handled via Python capsule
    Sequence* seq_ptr = new Sequence(std::move(seq));
    auto capsule = py::capsule(seq_ptr, [](void* p) { delete reinterpret_cast<Sequence*>(p); });
    return py::array(seq_ptr->size(),  // shape of array
                     seq_ptr->data(),  // c-style contiguous strides for Sequence
                     capsule           // numpy array references this parent
    );
}

py::dict ConvertWavDecodedToPyDict(WavDecoded wav_decoded);
py::dict ConvertMp3DecodedToPyDict(Mp3Decoded mp3_decoded);
py::dict ConvertKeyOutputToPyDict(KeyOutput key_output);

}  // namespace python
}  // namespace musher
