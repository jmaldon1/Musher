#include "utils.h"
#include "gtest_extras.h"

using namespace musher;

/**
 * @brief test
 * 
 */
TEST(HPCP, test) {
    std::vector<double> inp{1.0, 1.0, 1.0, 1.0, 2.0};
    std::vector< std::tuple< double, double > > peaks;
    peaks = peakDetect(inp, -1000.0, true);
    HPCP(peaks);
}