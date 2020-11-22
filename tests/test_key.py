import os
import math

import musher


def test_detect_key(test_data_dir: str):
    """Detect the key of a music file.
    """
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "mozart_c_major_30sec.wav")
    wav_decoded = musher.decode_wav_from_file(audio_file_path)
    normalized_samples = wav_decoded["normalized_samples"]
    sample_rate = wav_decoded["sample_rate"]

    actual_key_output = musher.detect_key(
        normalized_samples, sample_rate, "Temperley")

    expected_key_output = {
        'key': 'C',
        'scale': 'major',
        'strength': 0.760322,
        'first_to_second_relative_strength': 0.607807
    }

    assert actual_key_output['key'] == expected_key_output['key']
    assert actual_key_output['scale'] == expected_key_output['scale']
    assert math.isclose(actual_key_output['strength'],
                        expected_key_output['strength'], rel_tol=1e-6)
    assert math.isclose(actual_key_output['first_to_second_relative_strength'],
                        expected_key_output['first_to_second_relative_strength'], rel_tol=1e-6)


def test_estimate_key(test_data_dir: str):
    """Manually performing the detect_key function.

    Performance is slower than detect_key because of multiple copies of large
    arrays between C++ and Python.
    """
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "mozart_c_major_30sec.wav")
    pcp_size = 36
    sample_rate = 44100.
    num_harmonics = 4
    wav_decoded = musher.decode_wav_from_file(audio_file_path)
    mixed_audio = musher.mono_mixer(wav_decoded["normalized_samples"])
    count = 0
    sums = [0] * pcp_size
    for frame in musher.Framecutter(mixed_audio, frame_size=4096, hop_size=512):
        windowed_frame = musher.windowing(frame, musher.blackmanharris62dB)
        spectrum = musher.convert_to_frequency_spectrum(windowed_frame)
        spectral_peaks = musher.spectral_peaks(spectrum,
                                               sort_by="height",
                                               sample_rate=sample_rate,
                                               max_pos=int(sample_rate / 2))
        hpcp = musher.hpcp_from_peaks(spectral_peaks,
                                      pcp_size,
                                      harmonics=num_harmonics - 1,
                                      window_size=.5)
        for idx, _ in enumerate(hpcp):
            sums[idx] += hpcp[idx]
        count += 1

    avgs = [x/count for x in sums]

    actual_key_output = musher.estimate_key(avgs, profile_type="Temperley")

    expected_key_output = {
        'key': 'C',
        'scale': 'major',
        'strength': 0.7603224296919142,
        'first_to_second_relative_strength': 0.6078072169442225
    }

    assert actual_key_output['key'] == expected_key_output['key']
    assert actual_key_output['scale'] == expected_key_output['scale']
    assert math.isclose(actual_key_output['strength'],
                        expected_key_output['strength'], rel_tol=1e-6)
    assert math.isclose(actual_key_output['first_to_second_relative_strength'],
                        expected_key_output['first_to_second_relative_strength'], rel_tol=1e-6)
