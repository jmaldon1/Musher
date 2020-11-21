import musher


def test_windowing():
    buffer = [1., 2., 3., 4., 5., 6., 7.]
    windowed_frame = musher.windowing(buffer, musher.blackmanharris62dB)
    print(windowed_frame)
