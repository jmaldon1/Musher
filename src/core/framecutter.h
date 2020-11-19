#include <vector>

namespace musher {
namespace core {

/**
 * @brief This class should be treated like an iterator.
 *
 * @code
 *   Framecutter framecutter(audio_signal);
 *
 *   for (const std::vector<double> &frame : framecutter) {
 *       perform_work_on_frame(frame);
 *   }
 * @endcode
 */
class Framecutter {
 private:
  const std::vector<double> buffer_;
  const int frame_size_;
  const int hop_size_;
  const bool start_from_center_;
  const bool last_frame_to_end_of_file_;
  const double valid_frame_threshold_ratio_;
  int start_index_;
  bool last_frame_;
  std::vector<double> frame_;

 public:
  /**
   * @brief Construct a new Framecutter object
   *
   * @param buffer Buffer from which to read data.
   * @param frame_size Output frame size.
   * @param hop_size Hop size between frames.
   * @param start_from_center If true start from the center of the buffer (zero-centered at -frameSize/2) or
   * if false the first frame at time 0 (centered at frameSize/2).
   * @param last_frame_to_end_of_file Whether the beginning of the last frame should reach the end of file. Only
   * applicable if start_from_center is false.
   * @param valid_frame_threshold_ratio Frames smaller than this ratio will be discarded, those larger will be
   * zero-padded to a full frame. (i.e. a value of 0 will never discard frames and a value of 1 will only keep frames
   * that are of length 'frameSize')
   */
  Framecutter(const std::vector<double> buffer,
              int frame_size = 1024,
              int hop_size = 512,
              bool start_from_center = true,
              bool last_frame_to_end_of_file = false,
              double valid_frame_threshold_ratio = 0.)
      : buffer_(buffer),
        frame_size_(frame_size),
        hop_size_(hop_size),
        start_from_center_(start_from_center),
        last_frame_to_end_of_file_(last_frame_to_end_of_file),
        valid_frame_threshold_ratio_(valid_frame_threshold_ratio),
        start_index_(0),
        last_frame_(false),
        frame_(compute()) {}

  ~Framecutter() {}

  // Iterable functions
  const Framecutter &begin() const { return *this; }
  const Framecutter &end() const { return *this; }

  // Iterator functions
  // Keep iterating while frame is not empty.
  bool operator!=(const Framecutter &) const { return !frame_.empty(); }
  bool operator==(const Framecutter &) const { return frame_.empty(); }
  void operator++() { frame_ = compute(); }
  /**
   * @brief Each iteration returns a frame.
   *
   * @return std::vector<double> Cut frame.
   */
  std::vector<double> operator*() const { return frame_; }

  /**
   * @brief Computes the actual slicing of the frames, this function is run on each iteration to calculate the next
   * frame.
   *
   * This function should not be called by the user, it will be called internally while iterating.
   *
   * @return std::vector<double> Sliced frame.
   */
  std::vector<double> compute();
};

}  // namespace core
}  // namespace musher