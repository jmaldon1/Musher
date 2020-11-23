import numpy as np
import musher


def test_hpcp():
    tone = 100.
    frequencies = [tone, tone * 2, tone * 3, tone * 4]
    magnitudes = [1., 1., 1., 1.]

    harmonics = 3
    band_preset = False
    min_frequency = 50.0
    max_frequency = 500.0

    actual_hpcp = musher.hpcp(frequencies,
                              magnitudes,
                              harmonics=harmonics,
                              band_preset=band_preset,
                              min_frequency=min_frequency,
                              max_frequency=max_frequency)

    expected_hpcp = [0.,
                     0.,
                     0.,
                     0.13404962,
                     0.,
                     0.24760914,
                     0.,
                     0.,
                     0.,
                     0.,
                     1.,
                     0.]

    assert np.allclose(actual_hpcp, expected_hpcp, rtol=1e-8)


def test_hpcp_from_peaks():
    buffer = [0.] * (400 + 1)
    buffer[100] = 1.
    buffer[200] = 1.
    buffer[300] = 1.
    buffer[400] = 1.

    harmonics = 3
    band_preset = False
    min_frequency = 50.0
    max_frequency = 500.0

    spectral_peaks = musher.spectral_peaks(buffer, sample_rate=0)
    actual_hpcp = musher.hpcp_from_peaks(spectral_peaks,
                                         harmonics=harmonics,
                                         band_preset=band_preset,
                                         min_frequency=min_frequency,
                                         max_frequency=max_frequency)

    expected_hpcp = [0.,
                     0.,
                     0.,
                     0.13404962,
                     0.,
                     0.24760914,
                     0.,
                     0.,
                     0.,
                     0.,
                     1.,
                     0.]

    assert np.allclose(actual_hpcp, expected_hpcp, rtol=1e-8)
