import musher


def test_print_functional_message():
    musher.PrintFunctionalMessage("hi")


def test_decode():
    musher.DecodeWav("hello")


def test_decode_with_func_call():
    musher.AcceptDecode("hello", musher.DecodeWav)
