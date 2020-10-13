#include <vector>

#include "utils.h"

using namespace musher;

template<typename T>
void printVector(const char* name, const std::vector<T>& vec)
{
    if (vec.empty()) {
        std::cout << name << " is empty." << std::endl;
        return;
    }

    std::cout << name << std::endl;
    for (const auto& element : vec)
    {
        std::cout << "  " << element << std::endl;
    }
}

template<typename T>
void printMatrix(T mat) {
    for ( const auto &row : mat )
    {
        for ( const auto &s : row ) std::cout << s << ' ';
        std::cout << std::endl;
    }
}

std::vector<std::vector<double>> allCutFrames(const std::vector<double> buffer,
                                              int _hop_size,
                                              int frame_size,
                                              bool start_from_center,
                                              bool last_frame_to_end_of_file,
                                              double valid_frame_threshold_ratio)
{
    if (_hop_size <= 0) throw std::runtime_error("allCutFrames: Hop size is too small.");

    std::vector<std::vector<double>> actual_frames;
    std::vector<double> actual_frame;
    int hop_size = 0;
    actual_frame = framecutter(buffer,
                               hop_size,
                               frame_size,
                               start_from_center,
                               last_frame_to_end_of_file,
                               valid_frame_threshold_ratio);

    while (!actual_frame.empty())
    {
        hop_size += _hop_size;
        actual_frames.push_back(actual_frame);
        actual_frame = framecutter(buffer,
                                   hop_size,
                                   frame_size,
                                   start_from_center,
                                   false,
                                   0.);
    }
    return actual_frames;
}
