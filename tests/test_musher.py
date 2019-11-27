import musher

def test_print_functional_message():
    musher.PrintFunctionalMessage("hi")


def test_load_audio_file():
    musher.LoadAudioFile("./audio_files/WAV_1MG.wav")
    # musher.DecodeWav("hello")


# def test_decode_with_func_call():
    # musher.AcceptDecode("hello", musher.DecodeWav)
