import musher
import os

TEST_DIR_PATH = os.path.abspath(os.path.dirname(__file__))

def test_print_functional_message():
    musher.PrintFunctionalMessage("hi")


def test_load_audio_file():
    # print(test_dir_path)
    abs_audio_file_path = os.path.join(TEST_DIR_PATH, "audio_files/CantinaBand3sec.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)
    # print(t)
    # musher.DecodeWav("hello")


# def test_decode_with_func_call():
    # musher.AcceptDecode("hello", musher.DecodeWav)
