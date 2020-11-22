from typing import List

import numpy as np
import musher


def normalize(window: List[float]) -> List[float]:
    return [2.0*i/sum(window) for i in window]


def triangular(window: List[float]) -> List[float]:
    size = len(window)
    return [2.0/size*(size/2.0-abs(float(i-(size-1.0)/2.0))) for i in range(size)]


def test_windowing_square():
    inp = [1.] * 1024
    actual_window = musher.windowing(
        inp, musher.square, zero_padding_size=0, zero_phase=False)

    expected_window = normalize(inp)
    assert np.array_equal(actual_window, expected_window)


def test_windowing_custom_triangular():
    """Example of using a custom windowing function (triangular window).

    Custom functions must accept a list of floats and return a list of floats.
    """
    inp = [1.] * 1024
    actual_window = musher.windowing(
        inp, triangular, zero_padding_size=0, zero_phase=False)

    expected_window = normalize(triangular(inp))
    assert np.array_equal(actual_window, expected_window)
