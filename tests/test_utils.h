#include <vector>

#include "utils.h"

using namespace musher;

const std::vector<std::vector<double>> allCutFrames(const std::vector<double> buffer,
                                                    int frame_size,
                                                    int hop_size,
                                                    bool start_from_center,
                                                    bool last_frame_to_end_of_file,
                                                    double valid_frame_threshold_ratio)
{
    Framecutter framecutter(buffer,
                            frame_size,
                            hop_size,
                            start_from_center,
                            last_frame_to_end_of_file,
                            valid_frame_threshold_ratio);
    
    std::vector<std::vector<double>> all_frames;
    
    for(const std::vector<double>& frame: framecutter)
    {
        all_frames.push_back(frame);
    }

    if (all_frames.empty())
    {
        // Return an empty 2D vector. Done this way mainly for testing purposes.
        return std::vector<std::vector<double>> ({{}});
    }

    return all_frames;
}

// std::vector<std::vector<double>> allCutFrames(const std::vector<double> buffer,
//                                               int _hop_size,
//                                               int frame_size,
//                                               bool start_from_center,
//                                               bool last_frame_to_end_of_file,
//                                               double valid_frame_threshold_ratio)
// {
//     if (_hop_size <= 0) throw std::runtime_error("allCutFrames: Hop size is too small.");

//     std::vector<std::vector<double>> actual_frames;
//     std::vector<double> actual_frame;
//     int hop_size = 0;
//     actual_frame = framecutter(buffer,
//                                hop_size,
//                                frame_size,
//                                start_from_center,
//                                last_frame_to_end_of_file,
//                                valid_frame_threshold_ratio);

//     while (!actual_frame.empty())
//     {
//         hop_size += _hop_size;
//         actual_frames.push_back(actual_frame);
//         actual_frame = framecutter(buffer,
//                                    hop_size,
//                                    frame_size,
//                                    start_from_center,
//                                    last_frame_to_end_of_file,
//                                    valid_frame_threshold_ratio);
//     }
//     return actual_frames;
// }

// std::vector<std::vector<double>> allCutFrames()