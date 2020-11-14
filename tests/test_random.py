import musher


def test_ran():
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
        print(_)
        # pass


def test_hpcp():
    tone = 100.
    frequencies = [tone, tone * 2, tone * 3, tone * 4]
    magnitudes = [1.] * 4

    harmonics = 3
    band_preset = False
    min_frequency = 50.0
    max_frequency = 500.0
    print()

    hpcp = musher.hpcp(frequencies,
                       magnitudes,
                       harmonics=harmonics,
                       band_preset=band_preset,
                       min_frequency=min_frequency,
                       max_frequency=max_frequency)
    print(hpcp)
