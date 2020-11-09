import musher
import os

# from musher import framecutter

TESTS_DIR_PATH = os.path.abspath(os.path.dirname(__file__))

# def test_print_functional_message():
#     # help(musher)
#     musher.PrintFunctionalMessage("hi")


def test_load_audio_file():
    abs_audio_file_path = os.path.join(
        TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)
    # print(file_data)


def test_decode_wav_from_data():
    abs_audio_file_path = os.path.join(
        TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)
    d = musher.decode_wav_from_data(file_data)
    # print(d)


def test_decode_wav_from_file():
    abs_audio_file_path = os.path.join(
        TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    # file_data = musher.load_audio_file(abs_audio_file_path)
    d = musher.decode_wav_from_file(abs_audio_file_path)
    # print(d)


def test_mono_mixer():
    abs_audio_file_path = os.path.join(
        TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    # file_data = musher.load_audio_file(abs_audio_file_path)
    d = musher.decode_wav_from_file(abs_audio_file_path)
    t = musher.mono_mixer(d["normalized_samples"])
    # print(t)


def test_framecutter():
    buffer = [1, 2, 3, 4, 5, 6, 7]
    frame_size = 4
    hop_size = 2
    start_from_center = False
    last_frame_to_end_of_file = True
    valid_frame_threshold_ratio = 0.

    framecutter = musher.Framecutter(buffer,
                                     frame_size,
                                     hop_size,
                                     start_from_center,
                                     last_frame_to_end_of_file,
                                     valid_frame_threshold_ratio)
    for _ in framecutter:
        # print(_)
        pass


def test_windowing():
    buffer = [1., 2., 3., 4., 5., 6., 7.]
    windowed_frame = musher.windowing(buffer, musher.blackmanharris62dB)
    # print(windowed_frame)


def test_convert_to_frequency_spectrum():
    buffer = [1., 2., 3., 4., 5., 6., 7.]
    spec = musher.convert_to_frequency_spectrum(buffer)
    # print(spec)


def test_spectral_peaks():
    inp = [1.0, 1.0, 1.0, 1.0, 2.0]
    peaks = musher.spectral_peaks(inp)
    hp = musher.hpcp(peaks)
    print(hp)
    # print(peaks)
    # print(list(peaks))
    # print(len(peaks))
    # print(type(peaks))


def test_detect_key():
    abs_audio_file_path = os.path.join(
        TESTS_DIR_PATH, "..", "data", "audio_files", "mozart_c_major_30sec.wav")
    pcp_size = 36
    sample_rate = 44100.
    num_harmonics = 4
    wav_decoded = musher.decode_wav_from_file(abs_audio_file_path)
    mixed_audio = musher.mono_mixer(wav_decoded["normalized_samples"])
    count = 0
    sums = [0] * pcp_size
    for frame in musher.Framecutter(mixed_audio, 4096, 512):
        windowed_frame = musher.windowing(frame, musher.blackmanharris62dB)
        spectrum = musher.convert_to_frequency_spectrum(windowed_frame)
        spectral_peaks = musher.spectral_peaks(spectrum, -1000.0, "height",
                                               100, sample_rate, 0, int(sample_rate / 2))
        hpcp = musher.hpcp(spectral_peaks, pcp_size, 440.0, num_harmonics - 1, True, 500.0, 40.0,
                           5000.0, "squared cosine", .5)
        for idx, _ in enumerate(hpcp):
            sums[idx] += hpcp[idx]
        count += 1

    avgs = [x/count for x in sums]

    out = musher.detect_key(avgs, True, True, 4, 0.6, "Temperley")
    print(out)
