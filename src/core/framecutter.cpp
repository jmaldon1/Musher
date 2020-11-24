#include "src/core/framecutter.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <stdexcept>
#include <vector>

namespace musher {
namespace core {

std::vector<double> Framecutter::compute() {
  if (valid_frame_threshold_ratio_ > 0.5 && start_from_center_) {
    throw std::runtime_error(
        "FrameCutter: valid_frame_threshold_ratio cannot be "
        "larger than 0.5 if start_from_center is true (this "
        "is to prevent loss of the first frame which would "
        "be only half a valid frame since the first frame "
        "is centered on the beginning of the audio)");
  }

  int valid_frame_threshold = static_cast<int>(std::round(valid_frame_threshold_ratio_ * frame_size_));
  int start_index;
  size_t buffer_size = buffer_.size();

  if (start_from_center_)
    start_index = -(frame_size_ + 1) / 2 + start_index_;
  else
    start_index = start_index_;

  if (last_frame_ || buffer_.empty()) return std::vector<double>();
  if (start_index >= static_cast<int>(buffer_size)) return std::vector<double>();

  std::vector<double> frame(static_cast<size_t>(frame_size_));
  int idx_in_frame = 0;

  // If we're before the beginning of the buffer, fill the frame with 0
  if (start_index < 0) {
    int how_much = std::min(-start_index, frame_size_);
    for (; idx_in_frame < how_much; idx_in_frame++) {
      frame[idx_in_frame] = static_cast<double>(0.0);
    }
  }

  // Now, just copy from the buffer to the frame
  int how_much = std::min(frame_size_, static_cast<int>(buffer_size - start_index)) - idx_in_frame;
  std::memcpy(&frame[0] + idx_in_frame, &buffer_[0] + start_index + idx_in_frame, how_much * sizeof(double));
  idx_in_frame += how_much;

  // Check if the idx_in_frame is below the threshold (this would only happen
  // for the last frame in the stream)
  if (idx_in_frame < valid_frame_threshold) return std::vector<double>();

  if (start_index + idx_in_frame >= static_cast<int>(buffer_size) && !start_from_center_ && !last_frame_to_end_of_file_)
    last_frame_ = true;

  if (idx_in_frame < frame_size_) {
    if (!start_from_center_) {
      if (last_frame_to_end_of_file_) {
        if (start_index >= static_cast<int>(buffer_size)) last_frame_ = true;
      }
      // if we're zero-padding with start_from_center=false, it means we're filling
      // in the last frame, so we'll have to stop after this one
      else
        last_frame_ = true;
    } else {
      // if we're zero-padding and the center of the frame is past the end of the
      // stream, then this is the last frame and we need to stop after this one
      if (start_index + frame_size_ / 2 >= static_cast<int>(buffer_.size())) {
        last_frame_ = true;
      }
    }
    // Fill in the frame with 0 until the end of the buffer
    for (; idx_in_frame < frame_size_; idx_in_frame++) {
      frame[idx_in_frame] = static_cast<double>(0.0);
    }
  }
  start_index_ += hop_size_;
  return frame;
}

}  // namespace core
}  // namespace musher