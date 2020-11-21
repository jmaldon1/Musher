import musher


def test_framecutter():
    buffer = [1, 2, 3, 4, 5, 6, 7]
    frame_size = 4
    hop_size = 2
    start_from_center = False
    last_frame_to_end_of_file = True
    valid_frame_threshold_ratio = 0.

    actual_framecutter = musher.Framecutter(buffer,
                                            frame_size,
                                            hop_size,
                                            start_from_center,
                                            last_frame_to_end_of_file,
                                            valid_frame_threshold_ratio)

    expacted_framecutter = [
        [1.0, 2.0, 3.0, 4.0],
        [3.0, 4.0, 5.0, 6.0],
        [5.0, 6.0, 7.0, 0.0],
        [7.0, 0.0, 0.0, 0.0]
    ]

    assert list(actual_framecutter) == expacted_framecutter
