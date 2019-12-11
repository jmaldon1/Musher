#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>
#include <variant>
#include <math.h>
#include <numeric>
#include <valarray>
#include <complex>
#include <fstream>

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

    // template <typename T>
    // void outputVectorToFile(const std::vector<T>& vec, std::string& filename)
    // {
    //     std::ofstream os;
    //     os.open(filename);

    //     os << "{\n";
    //     for (int i = 0; i < vec.size(); ++i)
    //     {
    //         os << vec[i] << ", ";
    //         if (i % 32 == 0)
    //             os << "\n";
    //     }
    //     os << "};";
    // }

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
    size_t next_fast_len(size_t n);

    template< typename vecType,
            typename = std::enable_if_t<std::is_arithmetic<vecType>::value> >
    std::vector<vecType> fftConvolve(const std::vector< vecType > &vec1, const std::vector< vecType > &vec2)
    {
        std::vector<vecType> v1(vec1);
        std::vector<vecType> v2(vec2);
        std::vector<vecType> ret;

        if (v1.empty() || v2.empty())
            return ret;

        size_t s1 = v1.size();
        size_t s2 = v2.size();
        size_t shape = s1 + s2 - 1;

        /* Pad inputs to an efficient length */
        size_t good_size = next_fast_len(shape);
        while (v1.size() != good_size)
            v1.push_back(0.0);
        
        while (v2.size() != good_size)
            v2.push_back(0.0);

        /* Convert vector of doubles to vector of complex */
        auto makeComplex = []( const vecType x ) { return std::complex<vecType>(x, 0.0); };
        std::vector< std::complex< vecType> > cv1(v1.size());
        std::transform(
            v1.begin(),
            v1.end(),
            cv1.begin(),
            makeComplex );

        std::vector< std::complex< vecType> > cv2(v2.size());
        std::transform(
            v2.begin(),
            v2.end(),
            cv2.begin(),
            makeComplex );

        /* Fast fourier transform */
        Fft::transform(cv1);
        Fft::transform(cv2);

        /* Element-wise multiplication of 2 vectors */
        std::vector< std::complex< vecType> > products(cv1.size());
        std::transform(
                cv1.begin(),
                cv1.end(),
                cv2.begin(),
                products.begin(),
                std::multiplies< std::complex< vecType> >() );

        /* Inverse fast fourier transform */
        Fft::inverseTransform(products);

        // for (auto & element : products) {
        //     std::cout << element << std::endl;
        // }

        return ret;
    }
    
}
#endif /* __UTILS_H__ */