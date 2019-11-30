#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <variant>

namespace Musher
{
    std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v);
    std::string get_str_between_two_squotes(const std::string &s);
    auto unint8_t_to_int = [](uint8_t num) { return static_cast<int>(num); };
    auto int_to_unint8_t = [](int num) { return static_cast<uint8_t>(num); };
    bool is_big_endian(void);
    int16_t twoBytesToInt (const std::vector<uint8_t>& source, const int startIndex);
    int32_t fourBytesToInt (const std::vector<uint8_t>& source, const int startIndex);
    template <class AudioBufferType>
    AudioBufferType singleByteToSample(uint8_t sample)
    {
        return static_cast<AudioBufferType> (sample - 128) / static_cast<AudioBufferType> (128.);
    }

    template <class AudioBufferType>
    AudioBufferType sixteenBitIntToSample(int16_t sample)
    {
        return static_cast<AudioBufferType> (sample) / static_cast<AudioBufferType> (32768.);
    }
}
#endif /* __UTILS_H__ */