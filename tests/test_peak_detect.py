import musher


def test_peak_detect():
    r"""
    Visual of input:
                       /\
                      /  \
               /\    /    \
              /  \  /      \
          /\ /    \/        \
        _/                   \_
    """
    inp = [0., 2., 1., 4., 1., 6., 0.]
    #          ^       ^       ^
    # ^ = peak
    #
    actual_peaks = musher.peak_detect(inp, interpolate=False)

    expected_peaks = [
        # (position, height)
        (1.0, 2.0),
        (3.0, 4.0),
        (5.0, 6.0)
    ]

    assert list(actual_peaks) == expected_peaks
