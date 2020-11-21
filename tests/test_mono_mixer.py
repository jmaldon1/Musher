import os

import musher


def test_mono_mixer(test_data_dir: str):
    audio_file_path = os.path.join(
        test_data_dir, "audio_files", "impulses_1second_44100.wav")

    wav_decoded = musher.decode_wav_from_file(audio_file_path)
    mixed_audio = musher.mono_mixer(wav_decoded["normalized_samples"])

    actual_mixed_audio_len = len(mixed_audio)
    actual_mixed_audio_sum = sum(mixed_audio)

    expected_mixed_audio_len = 441000
    expected_mixed_audio_sum = 8.999725341796875

    assert actual_mixed_audio_len == expected_mixed_audio_len
    assert actual_mixed_audio_sum == expected_mixed_audio_sum
