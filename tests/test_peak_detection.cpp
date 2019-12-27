#include <iostream>
#include <string>
#include <vector>
#include <variant>
#include <stdexcept>
#include "gtest/gtest.h"

#include "utils.h"

using namespace musher;


TEST(PeakDetection, LastPositionPeak) {
    std::vector<double> inp{1.0, 1.0, 1.0, 1.0, 2.0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ expected_position, expected_value ] = peaks[0];
    EXPECT_EQ(expected_position, 4.0);
    EXPECT_EQ(expected_value, 2.0);
}


TEST(PeakDetection, FlatPeakMiddle1) {
    std::vector<double> inp{1.0, 2.0, 2.0, 2.0, 1.0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ expected_position, expected_value ] = peaks[0];
    EXPECT_EQ(expected_position, 2.0);
    EXPECT_EQ(expected_value, 2.0);
}

TEST(PeakDetection, FlatPeakMiddle2) {
    std::vector<double> inp{1.0, 2.0, 2.0, 2.0, 1.0, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ expected_position, expected_value ] = peaks[0];
    EXPECT_EQ(expected_position, 2.0);
    EXPECT_EQ(expected_value, 2.0);
    // std::cout << "POSITIONS: " << std::endl;
    // for (auto & element : peaks) {
    //     std::cout << std::get<0>(element) << std::endl;
    // }

    // std::cout << "VALUES: " << std::endl;
    // for (auto & element : peaks) {
    //     std::cout << std::get<1>(element) << std::endl;
    // }
}

TEST(PeakDetection, FlatPeakInterpolation) {
    std::vector<double> inp{1, 2, 2, 2, 3, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);

    auto [ expected_position, expected_value ] = peaks[0];
    EXPECT_NEAR(expected_position, 3.75, .01);
    EXPECT_NEAR(expected_value, 3.125, .001);
}

TEST(PeakDetection, FlatPeakNoInterpolation) {
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

TEST(PeakDetection, ManyPeaksWithInterpolation) {
    std::vector<double> inp{0, 2, 1, 2, 1, 2, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);
    
    auto [ expected_position_0, expected_value_0 ] = peaks[0];
    EXPECT_NEAR(expected_position_0, 1.16667, 0.00001);
    EXPECT_NEAR(expected_value_0, 2.04167, 0.00001);

    auto [ expected_position_1, expected_value_1 ] = peaks[1];
    EXPECT_DOUBLE_EQ(expected_position_1, 3);
    EXPECT_DOUBLE_EQ(expected_value_1, 2);

    auto [ expected_position_2, expected_value_2 ] = peaks[2];
    EXPECT_NEAR(expected_position_2, 4.83333, 0.00001);
    EXPECT_NEAR(expected_value_2, 2.04167, 0.00001);
}

TEST(PeakDetection, SortByPosition) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position");
    
    auto [ expected_position_0, expected_value_0 ] = peaks[0];
    EXPECT_EQ(expected_position_0, 1);
    EXPECT_EQ(expected_value_0, 2);

    auto [ expected_position_1, expected_value_1 ] = peaks[1];
    EXPECT_EQ(expected_position_1, 3);
    EXPECT_EQ(expected_value_1, 4);

    auto [ expected_position_2, expected_value_2 ] = peaks[2];
    EXPECT_EQ(expected_position_2, 5);
    EXPECT_EQ(expected_value_2, 6);
}

TEST(PeakDetection, SortByHeight) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "height");
    
    auto [ expected_position_0, expected_value_0 ] = peaks[0];
    EXPECT_EQ(expected_position_0, 5);
    EXPECT_EQ(expected_value_0, 6);

    auto [ expected_position_1, expected_value_1 ] = peaks[1];
    EXPECT_EQ(expected_position_1, 3);
    EXPECT_EQ(expected_value_1, 4);

    auto [ expected_position_2, expected_value_2 ] = peaks[2];
    EXPECT_EQ(expected_position_2, 1);
    EXPECT_EQ(expected_value_2, 2);
}

TEST(PeakDetection, MaxPeaks) {
    std::vector<double> inp{0, 2, 1, 4, 1, 6, 0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, false, "position", 2);
    
    size_t expected_num_of_peaks = 2;
    EXPECT_EQ( expected_num_of_peaks, peaks.size());
}