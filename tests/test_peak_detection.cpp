#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include "gtest/gtest.h"

#include "utils.h"

using namespace musher;


/**
 * @brief Check that we can get a peak at the final position of the input.
 * 
 */
TEST(PeakDetection, LastPositionPeak) {
    std::vector<double> inp{1.0, 1.0, 1.0, 1.0, 2.0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_EQ(actual_position, 4.0);
    EXPECT_EQ(actual_value, 2.0);
}


/**
 * @brief Check that we get the middle value of a flat peak with interpolation on.
 * This was made to test a bug with interpolation that has since been fixed.
 * 
 */
TEST(PeakDetection, FlatPeakMiddle1) {
    std::vector<double> inp{1.0, 2.0, 2.0, 2.0, 1.0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_EQ(actual_position, 2.0);
    EXPECT_EQ(actual_value, 2.0);
}


/**
 * @brief Check that we get the middle value of a flat peak with interpolation on.
 * 
 */
TEST(PeakDetection, FlatPeakMiddle2) {
    std::vector<double> inp{1.0, 2.0, 2.0, 2.0, 1.0, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_EQ(actual_position, 2.0);
    EXPECT_EQ(actual_value, 2.0);
    // std::cout << "POSITIONS: " << std::endl;
    // for (auto & element : peaks) {
    //     std::cout << std::get<0>(element) << std::endl;
    // }

    // std::cout << "VALUES: " << std::endl;
    // for (auto & element : peaks) {
    //     std::cout << std::get<1>(element) << std::endl;
    // }
}


/**
 * @brief Check that we can interpolate a peak after some flatness.
 * 
 */
TEST(PeakDetection, FlatToPeakInterpolation) {
    std::vector<double> inp{1, 2, 2, 2, 3, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_NEAR(actual_position, 3.75, .01);
    EXPECT_NEAR(actual_value, 3.125, .001);
}


/**
 * @brief Check that we can find a peak after some flatness.
 * 
 */
TEST(PeakDetection, FlatToPeakNoInterpolation) {
    /* This test also shows how to convert peaks and values into seperate vectors */

    std::vector<double> inp{1, 2, 2, 2, 3, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false);
    std::vector<double> positions(peaks.size());
    std::vector<double> values(peaks.size());

    std::transform( peaks.begin(),
                    peaks.end(),
                    positions.begin(),
                    [](auto const& pair){ return std::get<0>(pair); }
                    );

    std::transform( peaks.begin(),
                    peaks.end(),
                    values.begin(),
                    [](auto const& pair){ return std::get<1>(pair); }
                    );
    
    EXPECT_EQ(positions[0], 4.0);
    EXPECT_EQ(values[0], 3.0);
}


/**
 * @brief Check that interpolation works with many peaks.
 * 
 */
TEST(PeakDetection, ManyPeaksWithInterpolation) {
    std::vector<double> inp{0, 2, 1, 2, 1, 2, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);
    
    auto [ actual_position_0, actual_value_0 ] = peaks[0];
    EXPECT_NEAR(actual_position_0, 1.16667, 0.00001);
    EXPECT_NEAR(actual_value_0, 2.04167, 0.00001);

    auto [ actual_position_1, actual_value_1 ] = peaks[1];
    EXPECT_DOUBLE_EQ(actual_position_1, 3);
    EXPECT_DOUBLE_EQ(actual_value_1, 2);

    auto [ actual_position_2, actual_value_2 ] = peaks[2];
    EXPECT_NEAR(actual_position_2, 4.83333, 0.00001);
    EXPECT_NEAR(actual_value_2, 2.04167, 0.00001);
}


/**
 * @brief Check that the peaks are ordered by position.
 * 
 */
TEST(PeakDetection, SortByPosition) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position");
    
    auto [ actual_position_0, actual_value_0 ] = peaks[0];
    EXPECT_EQ(actual_position_0, 1);
    EXPECT_EQ(actual_value_0, 2);

    auto [ actual_position_1, actual_value_1 ] = peaks[1];
    EXPECT_EQ(actual_position_1, 3);
    EXPECT_EQ(actual_value_1, 4);

    auto [ actual_position_2, actual_value_2 ] = peaks[2];
    EXPECT_EQ(actual_position_2, 5);
    EXPECT_EQ(actual_value_2, 6);
}


/**
 * @brief Check that the peaks are ordered by height.
 * 
 */
TEST(PeakDetection, SortByHeight) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "height");
    
    auto [ actual_position_0, actual_value_0 ] = peaks[0];
    EXPECT_EQ(actual_position_0, 5);
    EXPECT_EQ(actual_value_0, 6);

    auto [ actual_position_1, actual_value_1 ] = peaks[1];
    EXPECT_EQ(actual_position_1, 3);
    EXPECT_EQ(actual_value_1, 4);

    auto [ actual_position_2, actual_value_2 ] = peaks[2];
    EXPECT_EQ(actual_position_2, 1);
    EXPECT_EQ(actual_value_2, 2);
}


/**
 * @brief Check that we only get the first 2 peaks.
 * 
 */
TEST(PeakDetection, MaxPeaks) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position", 2);
    
    size_t actual_num_of_peaks = 2;
    EXPECT_EQ( actual_num_of_peaks, peaks.size() );
}


/**
 * @brief Make sure we do not get peaks below min position.
 * 
 */
TEST(PeakDetection, MinPosition) {
    std::vector<double> inp{5, 0, 1, 0, 0, 0, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position", 0, 0.0, 2);
    
    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_EQ(actual_position, 2);
    EXPECT_EQ(actual_value, 1);
}


/**
 * @brief Make sure we do not get peaks above max position.
 * 
 */
TEST(PeakDetection, MaxPosition) {
    std::vector<double> inp{0, 0, 0, 0, 1, 0, 2};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position", 0, 0.0, 0, 4);
    
    auto [ actual_position, actual_value ] = peaks[0];
    EXPECT_EQ(actual_position, 4);
    EXPECT_EQ(actual_value, 1);
}


/**
 * @brief Check that our input was scaled to fit the range of 3.
 * 
 */
TEST(PeakDetection, Range) {
    std::vector<double> inp{5, 0, 1, 0, 2, 0, 1};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position", 0, 3.0);
    
    auto [ actual_position_0, actual_value_0 ] = peaks[0];
    EXPECT_EQ(actual_position_0, 0);
    EXPECT_EQ(actual_value_0, 5);

    auto [ actual_position_1, actual_value_1 ] = peaks[1];
    EXPECT_EQ(actual_position_1, 1);
    EXPECT_EQ(actual_value_1, 1);

    auto [ actual_position_2, actual_value_2 ] = peaks[2];
    EXPECT_EQ(actual_position_2, 2);
    EXPECT_EQ(actual_value_2, 2);

    auto [ actual_position_3, actual_value_3 ] = peaks[3];
    EXPECT_EQ(actual_position_3, 3);
    EXPECT_EQ(actual_value_3, 1);
}


/**
 * @brief Check that our input was scaled to fit the range of 3 
 * and that we only get peaks within our min and max positions.
 * 
 */
TEST(PeakDetection, RangeWithMinAndMaxPositions) {
    std::vector<double> inp{5, 0, 1, 0, 2, 0, 1};
    std::vector< std::tuple< double, double > > peaks;
    /* Min and Max positions should be within the range */
    peaks = peakDetect(inp, -1000.0, false, "position", 0, 3.0, 2, 3);

    auto [ actual_position_0, actual_value_0 ] = peaks[0];
    EXPECT_EQ(actual_position_0, 2);
    EXPECT_EQ(actual_value_0, 2);

    auto [ actual_position_1, actual_value_1 ] = peaks[1];
    EXPECT_EQ(actual_position_1, 3);
    EXPECT_EQ(actual_value_1, 1);
}