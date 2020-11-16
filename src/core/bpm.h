#pragma once

namespace musher {
namespace core {

// /**
//  * @brief Calculate the BPM (Beats per minute) of audio samples.
//  *
//  * @param samples Input audio signal.
//  * @param sample_rate Sampling rate of the audio signal \[Hz\].
//  * @return double BPM
//  */
// double BPMDetection(std::vector<double>& samples, uint32_t sample_rate);

// /**
//  * @brief Calculate the average BPM (Beats per minute) of samples.
//  * This function will slice the samples into windows and calculate the BPM over each
//  * window and then average them to achieve a final BPM over all samples.
//  *
//  * @param samples Input audio samples.
//  * @param sample_rate Sampling rate of the audio signal \[Hz\].
//  * @param window_seconds Size of the the window [Seconds] that will be scanned to determine the bpm,
//  * typically less than 10 seconds.
//  * @return double Average BPM over windows.
//  */
// double BPMOverWindow(std::vector<double>& samples, uint32_t sample_rate, unsigned int window_seconds = 3);

}  // namespace core
}  // namespace musher
