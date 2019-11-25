import musher


def test_print_functional_message():
    musher.PrintFunctionalMessage("hi")


def test_decode():
    musher.PyDecodeWav("hello")


def test_decode_with_func_call():
    musher.PyAcceptDecode("hello", musher.PyDecodeWav)
