#pragma once

#include <vector>

namespace musher {
namespace core {

/**
 * @brief Downmixes the signal into a single channel given a stereo signal.
 * 
 * If the signal was already a monoaural, it is left unchanged.
 *
 * @param input Stereo or mono audio signal
 * @return std::vector<double> Downmixed audio signal
 */
std::vector<double> MonoMixer(const std::vector<std::vector<double>> &input);

}  // namespace core
}  // namespace musher
