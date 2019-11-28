import musher
import os


def test_print_functional_message():
    musher.PrintFunctionalMessage("hi")


def test_load_audio_file():
    abs_audio_file_path = os.path.abspath("./tests/audio_files/WAV_1MG.wav")
    file_data = musher.load_audio_file(abs_audio_file_path)
    # print(t)
    # musher.DecodeWav("hello")


# def test_decode_with_func_call():
    # musher.AcceptDecode("hello", musher.DecodeWav)
