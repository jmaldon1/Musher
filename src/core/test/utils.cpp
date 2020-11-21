#include "src/core/test/utils.h"

#include <vector>

#include "src/core/framecutter.h"

namespace musher {
namespace core {
namespace test {

const std::vector<std::vector<double>> AllCutFrames(const std::vector<double> buffer,
                                                    int frame_size,
                                                    int hop_size,
                                                    bool start_from_center,
                                                    bool last_frame_to_end_of_file,
                                                    double valid_frame_threshold_ratio) {
  Framecutter framecutter(buffer, frame_size, hop_size, start_from_center, last_frame_to_end_of_file,
                          valid_frame_threshold_ratio);

  std::vector<std::vector<double>> all_frames;

  for (const std::vector<double> &frame : framecutter) {
    all_frames.push_back(frame);
  }

  if (all_frames.empty()) {
    // Return an empty 2D vector. Done this way mainly for testing purposes.
    return std::vector<std::vector<double>>({ {} });
  }

  return all_frames;
}

}  // namespace test
}  // namespace core
}  // namespace musher
