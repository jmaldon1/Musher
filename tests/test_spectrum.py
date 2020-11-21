import numpy as np
import musher


def test_spectrum():
    inp_size = 100
    inp = [1.] * inp_size
    actual_spectrum = musher.convert_to_frequency_spectrum(inp)

    expected_spectrum = [100.] + [0.] * int((inp_size / 2))

    assert np.array_equal(actual_spectrum, expected_spectrum)
