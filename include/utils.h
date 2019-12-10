#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <variant>
#include <math.h>
#include <numeric>
#include <valarray>
#include <complex>

#include "FftComplex.h"

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

    template< typename T >
    std::vector<T> flatten2DVector(const std::vector<std::vector<T>> &orig)
    {   
        std::vector<T> ret;
        for(const auto &v: orig)
            ret.insert(ret.end(), v.begin(), v.end());                                                                                         
        return ret;
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

    typedef std::complex<double> Complex;
    typedef std::valarray<Complex> CArray;
    void fft(CArray &x);

    template< typename vecType,
            typename = std::enable_if_t<std::is_arithmetic<vecType>::value> >
    std::vector<vecType> fftConvolve(const std::vector< vecType > &vec1, const std::vector< vecType > &vec2)
    {
        std::vector<vecType> ret;

        if (vec1.empty() || vec2.empty())
            return ret;
        
        // axes = [0]
        // other_axes = []

        size_t s1 = vec1.size();
        size_t s2 = vec2.size();

        /* Convert vector of doubles to vector of complex */
        // std::vector< std::complex<double> > cvec(s1);
        // auto makeComplex = []( const double x ) { return std::complex<double>(x, 0.0); };
        // std::transform(
        //     vec1.begin(),
        //     vec1.end(),
        //     std::back_inserter(cvec),
        //     makeComplex );

        // const Complex test[10];
        // std::vector<Complex> a(vec1.begin(), vec1.end());
        // const std::vector<Complex> sp1(vec1);
        // const std::vector<Complex> sp2(vec2);
        // CArray data(test, 8);
        // std::vector<std::complex<double>> a(10);

        // Complex *a = reinterpret_cast<Complex *>(vec1.data());
        // CArray data(a, s1);

        // fft(data);

        // complex_result = False

        return ret;
    }
    
}
#endif /* __UTILS_H__ */