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