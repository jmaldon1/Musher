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
    std::vector<double> actual_frame;
    int frame_size = 100;
    int hop_size = 60;
    bool start_from_center = false;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame({});
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test empty centered.
 * 
 */
TEST(Framecutter, TestEmptyCentered) {
    const std::vector<double> buffer({});
    std::vector<double> actual_frame;
    int frame_size = 100;
    int hop_size = 60;
    bool start_from_center = true;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame({});
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test One.
 * 
 */
TEST(Framecutter, TestOne) {
    const std::vector<double> buffer({23});
    std::vector<double> actual_frame;
    int frame_size = 100;
    int hop_size = 60;
    bool start_from_center = false;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame(100);
    expected_frame[0] = 23;
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test Centered.
 * 
 */
TEST(Framecutter, TestOneCentered) {
    const std::vector<double> buffer({23});
    std::vector<double> actual_frame;
    int frame_size = 100;
    int hop_size = 60;
    bool start_from_center = true;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame(100);
    expected_frame[50] = 23;
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test Last Frame.
 * 
 */
TEST(Framecutter, TestLastFrame) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    std::vector<double> actual_frame;
    int frame_size = 100;
    int hop_size = 60;
    bool start_from_center = false;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame(100);
    std::iota(std::begin(expected_frame), std::end(expected_frame), (double)0.);
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test Last Frame.
 * 
 */
TEST(Framecutter, TestLastFrame2) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    std::vector<double> actual_frame;
    int frame_size = 101;
    int hop_size = 60;
    bool start_from_center = false;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               hop_size,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame(101);
    // print_vector("frame", actual_frame);
    std::iota(std::begin(expected_frame), std::end(expected_frame)-1, (double)0.);
    expected_frame[101] = 0;
    EXPECT_VEC_EQ(actual_frame, expected_frame);
}

/**
 * @brief Test Last Frame Centered.
 * 
 */
TEST(Framecutter, TestLastFrameCentered) {
    std::vector<double> buffer(100);
    /* Fill buffer with increasing numbers of 0 to 100 */
    std::iota(std::begin(buffer), std::end(buffer), (double)0.);
    std::vector<double> actual_frame;
    int frame_size = 100;
    // int hop_size = 60;
    bool start_from_center = true;
    actual_frame = framecutter(buffer,
                               0,
                               frame_size,
                               60,
                               start_from_center,
                               false,
                               0.);
    std::vector<double> expected_frame(100);
    int hop_size = 0;
    while (actual_frame.size() != 0)
    {
        print_vector("frame", actual_frame);
        hop_size += 60;
        actual_frame = framecutter(buffer,
                                   hop_size,
                                   frame_size,
                                   hop_size,
                                   start_from_center,
                                   false,
                                   0.);
    }
    print_vector("frame", actual_frame);
    // std::iota(std::begin(expected_frame), std::end(expected_frame)-1, (double)0.);
    // expected_frame[101] = 0;
    // EXPECT_VEC_EQ(actual_frame, expected_frame);
}
