#include <vector>

#include "musher/core/utils.h"

namespace musher {
namespace core {
namespace test {

template <typename T>
void printVector(const std::vector<T> &vec) {
  if (vec.empty()) {
    std::cout << "Vector is empty." << std::endl;
    return;
  }

  std::cout << "Printing vector:" << std::endl;
  for (const auto &element : vec) {
    std::cout << "  " << element << std::endl;
  }
}

template <typename T>
void printMatrix(T matrix, const char spacing = ' ') {
  std::cout << "Printing matrix:" << std::endl;
  if (matrix.empty()) {
    std::cout << "Matrix is empty." << std::endl;
    return;
  }

  for (const auto &row : matrix) {
    for (const auto &item : row) std::cout << item << spacing;
    std::cout << std::endl;
  }
}

const std::vector<std::vector<double>> allCutFrames(const std::vector<double> buffer,
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
