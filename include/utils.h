#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <variant>

namespace musher
{
    std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v);
    std::string strBetweenSQuotes(const std::string &s);
    auto unint8_t_to_int = [](uint8_t num) { return static_cast<int>(num); };
    auto int_to_unint8_t = [](int num) { return static_cast<uint8_t>(num); };
    bool isBigEndian(void);
    int16_t twoBytesToInt (const std::vector<uint8_t>& source, const int startIndex);
    int32_t fourBytesToInt (const std::vector<uint8_t>& source, const int startIndex);
    template <typename AudioBufferType>
    AudioBufferType normalizeInt8_t(const uint8_t sample)
    {
        return static_cast<AudioBufferType> (sample - 128) / static_cast<AudioBufferType> (128.);
    }

    template <typename AudioBufferType>
    AudioBufferType normalizeInt16_t(const int16_t sample)
    {
        return static_cast<AudioBufferType> (sample) / static_cast<AudioBufferType> (32768.);
    }

    template <typename AudioBufferType>
    AudioBufferType normalizeInt32_t(const int32_t sample)
    {
        return static_cast<AudioBufferType> (sample) / static_cast<AudioBufferType> (8388608.);
    }

    /* Allow std::visit to be overloaded with as many lambda funcs as we want */
    template <class ...Fs>
    struct overload : Fs... {
      overload(Fs const&... fs) : Fs{fs}...
      {}

      using Fs::operator()...;
    };

    template< typename basicType, typename ...types >
    basicType variantToType(const std::variant<types...> &var)
    {
        basicType t;
        std::visit(
          overload(
            [&t](basicType arg) { t = arg; },
            [](auto&&) { 
                std::string err_message = "The template type was not found within the variant.";
                throw std::runtime_error(err_message); 
            }
          ),
          var
        );
        
        return t;
    }

    template< typename vecType,
            typename = std::enable_if_t<std::is_floating_point<vecType>::value> >
    std::vector<vecType> onePoolFilter(const std::vector< vecType > &vec)
    {
        vecType a = 0.99;
        vecType y = 0.0;

        std::vector<vecType> filteredSignal;
        auto onePoolEquation = [a, &y]( const vecType x ) { return y += (1 - a) * x - a * y; };
        std::transform(
               vec.begin(),
               vec.end(),
               std::back_inserter(filteredSignal),
               onePoolEquation );

        return filteredSignal;
    }
    
}
#endif /* __UTILS_H__ */