/**
 * How to run only these tests: 
 *  python setup.py build_cpp_tests --r --g=--gtest_filter=Framecutter\*.\*
 * 
 */

#include "utils.h"
#include "test_utils.h"
#include "gtest_extras.h"

using namespace musher;

/**
 * @brief Test empty buffer.
 * 
 */
TEST(Framecutter, TestEmptyBuffer) {
    const std::vector<double> buffer({});
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {}
    });
    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Empty Centered
 * 
 */
TEST(Framecutter, TestEmptyCentered) {
    const std::vector<double> buffer({});
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {}
    });
    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test One
 * 
 */
TEST(Framecutter, TestOne) {
    const std::vector<double> buffer({23.});
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame(100, 0.);
    expected_frame[0] = 23;

    std::vector<std::vector<double>> expected_frames({
        expected_frame
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test One Centered
 * 
 */
TEST(Framecutter, TestOneCentered) {
    const std::vector<double> buffer({23.});
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame(100, 0.);
    expected_frame[50] = 23;

    std::vector<std::vector<double>> expected_frames({
        expected_frame
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Last Frame
 * 
 */
TEST(Framecutter, TestLastFrame) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame(100);
    std::iota(std::begin(expected_frame), std::end(expected_frame), (double)0.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Last Frame 2
 * 
 */
TEST(Framecutter, TestLastFrame2) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 101;
    int hop_size                       = 60;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame(101);
    std::iota(std::begin(expected_frame), std::end(expected_frame)-1, (double)0.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Last Frame Centered
 * 
 */
TEST(Framecutter, TestLastFrameCentered) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 100;
    int hop_size                       = 60;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1)+50, std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2)-10, (double)10.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3)-70, (double)70.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Last Frame Centered 2
 * 
 */
TEST(Framecutter, TestLastFrameCentered2) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 102;
    int hop_size                       = 60;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1)+51, std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2)-11, (double)9.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3)-71, (double)69.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Last Frame Centered 3
 * 
 */
TEST(Framecutter, TestLastFrameCentered3) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 101;
    int hop_size                       = 60;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1)+51, std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2)-10, (double)9.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3)-70, (double)69.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Big Hop Size
 * 
 */
TEST(Framecutter, TestBigHopSize) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 20;
    int hop_size                       = 100;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)0.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Big Hop Size 2
 * 
 */
TEST(Framecutter, TestBigHopSize2) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 20;
    int hop_size                       = 40;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)40.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)80.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Big Hop Size Centered
 * 
 */
TEST(Framecutter, TestBigHopSizeCentered) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 20;
    int hop_size                       = 100;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1)+10, std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2)-10, (double)90.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Big Hop Size Centered 2
 * 
 */
TEST(Framecutter, TestBigHopSizeCentered2) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    int frame_size                     = 20;
    int hop_size                       = 40;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1)+10, std::end(expected_frame1), (double)0.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)30.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)70.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Complex
 * 
 */
TEST(Framecutter, TestComplex) {
    std::vector<double> buffer(5);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 3;
    int hop_size                       = 2;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2., 3.},
        {3., 4., 5.}
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Complex 2
 * 
 */
TEST(Framecutter, TestComplex2) {
    std::vector<double> buffer(3);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 2;
    int hop_size                       = 1;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2.},
        {2., 3.}
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Complex Centered
 * 
 */
TEST(Framecutter, TestComplexCentered) {
    std::vector<double> buffer(5);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 3;
    int hop_size                       = 2;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {0., 0., 1.},
        {1., 2., 3.},
        {3., 4., 5.},
        {5., 0., 0.}
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Complex Centered2
 * 
 */
TEST(Framecutter, TestComplexCentered2) {
    std::vector<double> buffer(3);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 2;
    int hop_size                       = 1;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {0., 1.},
        {1., 2.},
        {2., 3.},
        {3., 0.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test EOF
 * 
 */
TEST(Framecutter, TestEOF) {
    std::vector<double> buffer(5);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 3;
    int hop_size                       = 2;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = true;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2., 3.},
        {3., 4., 5.},
        {5., 0., 0.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test EOF 2
 * 
 */
TEST(Framecutter, TestEOF2) {
    std::vector<double> buffer(3);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 2;
    int hop_size                       = 1;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = true;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2.},
        {2., 3.},
        {3., 0.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test EOF 3
 * 
 */
TEST(Framecutter, TestEOF3) {
    std::vector<double> buffer(5);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 2;
    int hop_size                       = 3;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = true;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2.},
        {4., 5.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test EOF 4
 * 
 */
TEST(Framecutter, TestEOF4) {
    std::vector<double> buffer(7);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 2;
    int hop_size                       = 3;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = true;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2.},
        {4., 5.},
        {7., 0.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test EOF 5
 * 
 */
TEST(Framecutter, TestEOF5) {
    std::vector<double> buffer(7);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 4;
    int hop_size                       = 2;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = true;
    double valid_frame_threshold_ratio = 0.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<std::vector<double>> expected_frames({
        {1., 2., 3., 4.},
        {3., 4., 5., 6.},
        {5., 6., 7., 0.},
        {7., 0., 0., 0.},
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

class TestDropLastFrameStartFromZeroEvenFrameSizeMultipleParameters :public ::testing::TestWithParam<std::tuple<size_t, double>> {};

/**
 * @brief Test Drop Last Frame | Start From Zero | Even Frame Size
 * 
 */
TEST_P(TestDropLastFrameStartFromZeroEvenFrameSizeMultipleParameters, TestDropLastFrameStartFromZeroEvenFrameSize) {
    int frame_size                 = 10;
    int hop_size                   = 10;
    bool start_from_center         = false;
    bool last_frame_to_end_of_file = false;

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)11.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)21.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)31.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)41.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5
    });

    size_t buffer_size = std::get<0>(GetParam());
    double valid_frame_threshold_ratio = std::get<1>(GetParam());

    std::vector<double> buffer(buffer_size);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

INSTANTIATE_TEST_CASE_P(
        Framecutter,
        TestDropLastFrameStartFromZeroEvenFrameSizeMultipleParameters,
        ::testing::Values(
                std::make_tuple(59, 1.),
                std::make_tuple(58, .9),
                std::make_tuple(51, .2)
                ));

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Even Frame Size
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroEvenFrameSize) {
    std::vector<double> buffer(60);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 10;
    int hop_size                       = 10;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 1.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)11.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)21.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)31.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)41.);

    std::vector<double> expected_frame6(frame_size);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6), (double)51.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Even Frame Size2
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroEvenFrameSize2) {
    std::vector<double> buffer(59);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 10;
    int hop_size                       = 10;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .9;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)11.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)21.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)31.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)41.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-1, (double)51.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Even Frame Size3
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroEvenFrameSize3) {
    std::vector<double> buffer(52);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 10;
    int hop_size                       = 10;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .2;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)11.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)21.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)31.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)41.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-8, (double)51.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

class TestDropLastFrameStartFromZeroOddFrameSizeMultipleParameters :public ::testing::TestWithParam<std::tuple<size_t, double>> {};

/**
 * @brief Test Drop Last Frame | Start From Zero | Odd Frame Size
 * 
 */
TEST_P(TestDropLastFrameStartFromZeroOddFrameSizeMultipleParameters, TestDropLastFrameStartFromZeroOddFrameSize) {
    int frame_size                 = 11;
    int hop_size                   = 11;
    bool start_from_center         = false;
    bool last_frame_to_end_of_file = false;

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)12.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)23.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)34.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)45.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5
    });

    size_t buffer_size = std::get<0>(GetParam());
    double valid_frame_threshold_ratio = std::get<1>(GetParam());

    std::vector<double> buffer(buffer_size);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

INSTANTIATE_TEST_CASE_P(
        Framecutter,
        TestDropLastFrameStartFromZeroOddFrameSizeMultipleParameters,
        ::testing::Values(
                std::make_tuple(65, 1.),
                std::make_tuple(64, .9),
                std::make_tuple(56, .2)
                ));

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Odd Frame Size
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroOddFrameSize) {
    std::vector<double> buffer(66);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 11;
    int hop_size                       = 11;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = 1.;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)12.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)23.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)34.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)45.);

    std::vector<double> expected_frame6(frame_size);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6), (double)56.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Odd Frame Size2
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroOddFrameSize2) {
    std::vector<double> buffer(65);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 11;
    int hop_size                       = 11;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .9;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)12.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)23.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)34.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)45.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-1, (double)56.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Zero | Odd Frame Size3
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromZeroOddFrameSize3) {
    std::vector<double> buffer(58);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 11;
    int hop_size                       = 11;
    bool start_from_center             = false;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .2;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size);
    std::iota(std::begin(expected_frame1), std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)12.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)23.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)34.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)45.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-8, (double)56.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

class TestDropLastFrameStartFromCenterEvenFrameSizeMultipleParameters :public ::testing::TestWithParam<std::tuple<size_t, double>> {};

/**
 * @brief Test Drop Last Frame | Start From Center | Even Frame Size
 * 
 */
TEST_P(TestDropLastFrameStartFromCenterEvenFrameSizeMultipleParameters, TestDropLastFrameStartFromCenterEvenFrameSize) {
    int frame_size                 = 10;
    int hop_size                   = 10;
    bool start_from_center         = true;
    bool last_frame_to_end_of_file = false;

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+5, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)16.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)26.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)36.);

    std::vector<double> expected_frame6(frame_size);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6), (double)46.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6
    });

    size_t buffer_size = std::get<0>(GetParam());
    double valid_frame_threshold_ratio = std::get<1>(GetParam());

    std::vector<double> buffer(buffer_size);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

INSTANTIATE_TEST_CASE_P(
        Framecutter,
        TestDropLastFrameStartFromCenterEvenFrameSizeMultipleParameters,
        ::testing::Values(
                std::make_tuple(59, .5),
                std::make_tuple(56, .2)
                ));

/**
 * @brief Test Dont Drop Last Frame | Start From Center | Even Frame Size
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromCenterEvenFrameSize) {
    std::vector<double> buffer(60);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 10;
    int hop_size                       = 10;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .5;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+5, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)16.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)26.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)36.);

    std::vector<double> expected_frame6(frame_size);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6), (double)46.);

    std::vector<double> expected_frame7(frame_size, 0.);
    std::iota(std::begin(expected_frame7), std::end(expected_frame7)-5, (double)56.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
        expected_frame7
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Center | Even Frame Size2
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromCenterEvenFrameSize2) {
    std::vector<double> buffer(57);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 10;
    int hop_size                       = 10;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .2;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+5, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)16.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)26.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)36.);

    std::vector<double> expected_frame6(frame_size);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6), (double)46.);

    std::vector<double> expected_frame7(frame_size, 0.);
    std::iota(std::begin(expected_frame7), std::end(expected_frame7)-8, (double)56.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6,
        expected_frame7
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

class TestDropLastFrameStartFromCenterOddFrameSizeMultipleParameters :public ::testing::TestWithParam<std::tuple<size_t, double>> {};

/**
 * @brief Test Drop Last Frame | Start From Center | Odd Frame Size
 * 
 */
TEST_P(TestDropLastFrameStartFromCenterOddFrameSizeMultipleParameters, TestDropLastFrameStartFromCenterOddFrameSize) {
    int frame_size                 = 11;
    int hop_size                   = 11;
    bool start_from_center         = true;
    bool last_frame_to_end_of_file = false;

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+6, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)17.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)28.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)39.);

    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5
    });

    size_t buffer_size = std::get<0>(GetParam());
    double valid_frame_threshold_ratio = std::get<1>(GetParam());

    std::vector<double> buffer(buffer_size);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

INSTANTIATE_TEST_CASE_P(
        Framecutter,
        TestDropLastFrameStartFromCenterOddFrameSizeMultipleParameters,
        ::testing::Values(
                std::make_tuple(54, .5),
                std::make_tuple(50, .2)
                ));

/**
 * @brief Test Dont Drop Last Frame | Start From Center | Odd Frame Size
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromCenterOddFrameSize) {
    std::vector<double> buffer(55);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 11;
    int hop_size                       = 11;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .5;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+6, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)17.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)28.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)39.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-5, (double)50.);


    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}

/**
 * @brief Test Dont Drop Last Frame | Start From Center | Odd Frame Size2
 * 
 */
TEST(Framecutter, TestDontDropLastFrameStartFromCenterOddFrameSize2) {
    std::vector<double> buffer(52);
    std::iota(std::begin(buffer), std::end(buffer), (double)1.);
    int frame_size                     = 11;
    int hop_size                       = 11;
    bool start_from_center             = true;
    bool last_frame_to_end_of_file     = false;
    double valid_frame_threshold_ratio = .2;
    std::vector<std::vector<double>> actual_frames;
    actual_frames = allCutFrames(buffer,
                                 frame_size,
                                 hop_size,
                                 start_from_center,
                                 last_frame_to_end_of_file,
                                 valid_frame_threshold_ratio);

    std::vector<double> expected_frame1(frame_size, 0.);
    std::iota(std::begin(expected_frame1)+6, std::end(expected_frame1), (double)1.);

    std::vector<double> expected_frame2(frame_size);
    std::iota(std::begin(expected_frame2), std::end(expected_frame2), (double)6.);

    std::vector<double> expected_frame3(frame_size);
    std::iota(std::begin(expected_frame3), std::end(expected_frame3), (double)17.);

    std::vector<double> expected_frame4(frame_size);
    std::iota(std::begin(expected_frame4), std::end(expected_frame4), (double)28.);

    std::vector<double> expected_frame5(frame_size);
    std::iota(std::begin(expected_frame5), std::end(expected_frame5), (double)39.);

    std::vector<double> expected_frame6(frame_size, 0.);
    std::iota(std::begin(expected_frame6), std::end(expected_frame6)-8, (double)50.);


    std::vector<std::vector<double>> expected_frames({
        expected_frame1,
        expected_frame2,
        expected_frame3,
        expected_frame4,
        expected_frame5,
        expected_frame6
    });

    EXPECT_MATRIX_EQ(actual_frames, expected_frames);
}
