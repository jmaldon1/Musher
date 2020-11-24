import os
import math

import numpy as np
import musher


# WAV

def test_decode_wav_from_data(test_data_dir: str):
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "mozart_c_major_30sec.wav")
    file_data = musher.load_audio_file(audio_file_path)
    actual_decoded_wav = musher.decode_wav_from_data(file_data)
    actual_normalized_samples_sum = np.sum(
        actual_decoded_wav["normalized_samples"])

    expected_decoded_wav = {
        'avg_bitrate_kbps': 1411,
        'bit_depth': 16,
        'channels': 2,
        'file_type': 'wav',
        'length_in_seconds': 30.0,
        'mono': False,
        'sample_rate': 44100,
        'samples_per_channel': 1323000,
        'stereo': True
    }
    expected_normalized_samples_sum = -996.6949157714844

    # We are not checking normalized_samples.
    assert all(item in actual_decoded_wav.items()
               for item in expected_decoded_wav.items())

    assert actual_normalized_samples_sum == expected_normalized_samples_sum


def test_decode_wav_from_file(test_data_dir: str):
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "mozart_c_major_30sec.wav")
    actual_decoded_wav = musher.decode_wav_from_file(audio_file_path)
    actual_normalized_samples_sum = np.sum(
        actual_decoded_wav["normalized_samples"])

    expected_decoded_wav = {
        'avg_bitrate_kbps': 1411,
        'bit_depth': 16,
        'channels': 2,
        'file_type': 'wav',
        'length_in_seconds': 30.0,
        'mono': False,
        'sample_rate': 44100,
        'samples_per_channel': 1323000,
        'stereo': True
    }
    expected_normalized_samples_sum = -996.6949157714844

    # We are not checking normalized_samples.
    assert all(item in actual_decoded_wav.items()
               for item in expected_decoded_wav.items())

    assert actual_normalized_samples_sum == expected_normalized_samples_sum


# MP3

def test_decode_mp3_from_file(test_data_dir: str):
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "mozart_c_major_30sec.mp3")
    actual_decoded_mp3 = musher.decode_mp3_from_file(audio_file_path)
    actual_normalized_samples_sum = np.sum(
        actual_decoded_mp3["normalized_samples"])

    expected_decoded_mp3 = {
        'avg_bitrate_kbps': 320,
        'channels': 2,
        'file_type': 'mp3',
        'length_in_seconds': 30.066938775510206,
        'mono': False,
        'sample_rate': 44100,
        'samples_per_channel': 1325952,
        'stereo': True
    }

    # linux_i686 produces a slightly different result.
    expected_normalized_samples_sum = -32665783.0
    expected_normalized_samples_sum_linux_i686 = -32665751.0

    # We are not checking normalized_samples.
    assert all(item in actual_decoded_mp3.items()
               for item in expected_decoded_mp3.items())

    assert actual_normalized_samples_sum in (
        expected_normalized_samples_sum, expected_normalized_samples_sum_linux_i686)


# OTHERS

def test_load_audio_file(test_data_dir: str):
    abs_audio_file_path = os.path.join(
        test_data_dir, "audio_files", "impulses_1second_44100.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)

    actual_file_data_len = len(file_data)
    actual_file_data_sum = sum(file_data)

    assert actual_file_data_len == 882044
    assert actual_file_data_sum == 5774
