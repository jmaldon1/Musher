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
#include <tuple>

#include "pocketfft.h"

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
            [&t](const basicType arg) { t = arg; },
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
    std::vector<T> interleave2DVector(const std::vector<std::vector<T>> &orig)
    {   
        /* Interleave 2 vectors */
        if (orig.size() > 2){
            std::string err_message = "This is not a 2D vector";
            throw std::runtime_error(err_message);
        } else if (orig.size() == 1) {
            return orig[0];
        } else if (orig.empty()) {
            std::string err_message = "2D Vector is empty";
            throw std::runtime_error(err_message);
        }
        std::vector<T> a(orig[0]);
        std::vector<T> b(orig[1]);
        std::vector<T> result;

        auto m = std::min( a.size(), b.size() );

        for( size_t i = 0 ; i < m ; ++i )
        {
            result.push_back(a[i]);
            result.push_back(b[i]);
        }
        if( m < a.size() ) result.insert( result.end(), a.begin()+m, a.end() );
        if( m < b.size() ) result.insert( result.end(), b.begin()+m, b.end() );

        return result;
    }

    template< typename vecType,
            typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    double median(std::vector< vecType > &inVec)
    {
        std::vector<vecType> vec(inVec);
        std::sort(vec.begin(), vec.end());
        double median;
        if (vec.size() % 2 == 0)
            median = 0.5 * (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]);
        else
            median = vec[vec.size() / 2];
        
        return median;
    }

    template< typename vecType,
            typename = std::enable_if_t<std::is_floating_point<vecType>::value> >
    std::vector<vecType> onePoleFilter(const std::vector< vecType > &vec)
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

    // template< typename vecType,
    //         typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    // double peakDetect(std::vector< vecType > &correlVec)
    // {
    //     std::vector<vecType> vecAbs(correlVec.size());
    //     /* Find the max of the input */
    //     auto absoluteVal = []( const double x ) { return std::abs(x); };
    //     std::transform(
    //             correlVec.begin(),
    //             correlVec.end(),
    //             vecAbs.begin(),
    //             absoluteVal );

    //     double max = *max_element(vecAbs.begin(), vecAbs.end());

    //     /* Check if the max is positive */
    //     auto itPositive = std::find(correlVec.begin(), correlVec.end(), max);
    //     if (itPositive != correlVec.end()){
    //         double peakIndex = itPositive - correlVec.begin();
    //         return peakIndex;
    //     }
        
    //     /* Check if the max is negative */
    //     auto itNegative = std::find(correlVec.begin(), correlVec.end(), -max);
    //     if (itNegative != correlVec.end()){
    //         double peakIndex = itNegative - correlVec.begin();
    //         return peakIndex;
    //     }
    //     // throw std::runtime_error("No Peak Detected...");
    //     return 0.0;
    // }

    size_t next_fast_len(size_t n);

    using ldbl_t = typename std::conditional<
        sizeof(long double)==sizeof(double), double, long double>::type;

    template<typename T> T normFct(int inorm, size_t N)
    {
        if (inorm==0) return T(1);
        if (inorm==2) return T(1/ldbl_t(N));
        if (inorm==1) return T(1/sqrt(ldbl_t(N)));
        throw std::runtime_error("invalid value for inorm (must be 0, 1, or 2)");
    }

    template<typename T> T normFct(int inorm, const pocketfft::shape_t &shape,
    const pocketfft::shape_t &axes, size_t fct=1, int delta=0)
    {
    if (inorm==0) return T(1);
    size_t N(1);
    for (auto a: axes)
        N *= fct * size_t(int64_t(shape[a])+delta);
    return normFct<T>(inorm, N);
    }

    template<typename vecType> 
    std::vector<vecType> centerVector(std::vector<vecType> arr, size_t newShape)
    {
        size_t currShape = arr.size();
        size_t startInd = (currShape - newShape) / 2;
        size_t endind = startInd + newShape;
        std::vector<vecType> sliced(arr.begin() + startInd, arr.begin() + endind);
        return sliced;
    }

    template< typename vecType,
            typename = std::enable_if_t<std::is_arithmetic<vecType>::value> >
    std::vector<vecType> fftConvolve(const std::vector< vecType > &vec1, const std::vector< vecType > &vec2)
    {
        /* 
            This function computes a 'same' FFT Convolve
    
            This function was heavily inspired by:
            https://github.com/scipy/scipy/blob/master/scipy/signal/signaltools.py#L432
            https://github.com/scipy/scipy/blob/master/scipy/fft/_pocketfft/pypocketfft.cxx
        */

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
        if (good_size < v1.size() || good_size < v2.size())
            std::runtime_error("Something went wrong calculating efficient FFT size.");
        v1.resize(good_size, 0.0);
        v2.resize(good_size, 0.0);
        
        /* FFT common variables */
        bool forward = true;
        int inorm = 0;
        int nthreads = 1;
        pocketfft::shape_t axes{1}; // 1 axis on 1D array

        /* vec1 FFT */
        pocketfft::shape_t v1_dims_in {1, v1.size()}; // {width, height}
        pocketfft::shape_t v1_dims_out(v1_dims_in);
        v1_dims_out[axes.back()] = (v1_dims_out[axes.back()]>>1) + 1; // Get length of output vector
        size_t v1OutSize = v1_dims_out[axes.back()];
        std::vector< std::complex< vecType> > v1_out(v1OutSize);
        long int s1_in_shape = v1.size() * sizeof(vecType);
        pocketfft::stride_t s1_in {s1_in_shape, sizeof(vecType)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s1_out_shape = v1.size() * sizeof(std::complex< vecType >);
        pocketfft::stride_t s1_out {s1_out_shape, sizeof(std::complex< vecType >)};
        auto d1_in = reinterpret_cast<const vecType *>(v1.data());
        auto d1_out = reinterpret_cast<std::complex<vecType> *>(v1_out.data());
        vecType v1_fct = normFct<vecType>(inorm, v1_dims_in, axes);
        pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct,
            nthreads);

        /* vec2 FFT */
        pocketfft::shape_t v2_dims_in {1, v2.size()}; // {width, height}
        pocketfft::shape_t v2_dims_out(v2_dims_in);
        v2_dims_out[axes.back()] = (v2_dims_out[axes.back()]>>1) + 1; // Get length of output vector
        size_t v2OutSize = v2_dims_out[axes.back()];
        std::vector< std::complex< vecType> > v2_out(v2OutSize);
        long int s2_in_shape = v2.size() * sizeof(vecType);
        pocketfft::stride_t s2_in {s2_in_shape, sizeof(vecType)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s2_out_shape = v2.size() * sizeof(std::complex< vecType >);
        pocketfft::stride_t s2_out {s2_out_shape, sizeof(std::complex< vecType >)};
        auto d2_in = reinterpret_cast<const vecType *>(v2.data());
        auto d2_out = reinterpret_cast<std::complex<vecType> *>(v2_out.data());
        vecType v2_fct = normFct<vecType>(inorm, v2_dims_in, axes);
        pocketfft::r2c(v2_dims_in, s2_in, s2_out, axes, forward, d2_in, d2_out, v2_fct,
            nthreads);

        /* Element-wise multiplication of 2 complex vectors */
        std::vector< std::complex< vecType> > products(v1_out.size());
        std::transform(
                v1_out.begin(),
                v1_out.end(),
                v2_out.begin(),
                products.begin(),
                std::multiplies< std::complex< vecType> >() );

        /* Save size of products before padding*/
        size_t product_size_pre_pad = products.size();

        /* Pad products to efficient size */
        products.resize(good_size, 0.0);

        /* Inverse FFT of complex to real */
        forward = false;
        inorm = 2;
        size_t axis = axes.back();
        size_t lastsize = products.size();
        pocketfft::shape_t conv_dims_in {1, product_size_pre_pad}; // {width, height}
        pocketfft::shape_t conv_dims_out(conv_dims_in);
        // if (lastsize==0) lastsize = 2 * conv_dims_in[axis] - 1;
        if ((lastsize/2) + 1 != conv_dims_in[axis])
            throw std::runtime_error("bad lastsize");
        conv_dims_out[axis] = lastsize;
        size_t conv_out_size = conv_dims_out[axis];
        std::vector< vecType > conv_out(conv_out_size);
        long int conv_in_shape = products.size() * sizeof(std::complex< vecType >);
        pocketfft::stride_t s_conv_in {conv_in_shape, sizeof(std::complex< vecType >)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int conv_out_shape = conv_out_size * sizeof(vecType);
        pocketfft::stride_t s_conv_out {conv_out_shape, sizeof(vecType)};
        auto d_conv_in = reinterpret_cast<const std::complex<vecType> *>(products.data());
        auto d_conv_out = reinterpret_cast<vecType *>(conv_out.data());
        vecType conv_fct = normFct<vecType>(inorm, conv_dims_out, axes);
        pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct,
            nthreads);

        std::vector<vecType> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

        /* Perform 'same' part of the convolve */
        std::vector<vecType> centered_conv_out = centerVector(sliced_conv_out, s1);
        return centered_conv_out;
    }

    template< typename vecType,
            typename = std::enable_if_t< std::is_arithmetic<vecType>::value> >
    std::vector<vecType> blackmanHarris92dB(const std::vector<vecType> &window)
    {   
        /* window functions help control spectral leakage when doing Fourier Analysis */
        std::vector<vecType> ret(window);
        double a0 = .35875, a1 = .48829, a2 = .14128, a3 = .01168;
        int window_size = window.size();
        double f = 2.0 * M_PI / (window_size - 1);

        if (window_size % 2 !=0) {
            ret[window_size / 2] = a0 - a1 * cos(f * (window_size / 2)) + a2 *
            cos(f * 2 * (window_size / 2)) - a3 * cos(f * 3 * (window_size / 2));
        }

        for (int i=0; i < window_size / 2; i++) {
            ret[i] = ret[window_size - i - 1] = a0 - a1 * cos(f * i) +
            a2 * cos(f * 2 * i) - a3 * cos(f * 3 * i);
        }

        return ret;
    }

    template< typename complexType,
            typename = std::enable_if_t< std::is_arithmetic<complexType>::value> >
    complexType magnitude(const std::complex< complexType > complex_pair)
    {   
        /* Calculate absolute value (magnitude or modulus) of a complex number */
        return sqrt(pow(complex_pair.real(), 2) + pow(complex_pair.imag(), 2));

    }

    template< typename vecType,
            typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    std::vector<vecType> convertToFrequencySpectrum(const std::vector<vecType> &flattened_normalized_samples)
    {
        std::vector<vecType> v1(flattened_normalized_samples);
        std::vector<vecType> ret;

        if (v1.empty())
            return ret;

        size_t s1 = v1.size();
        size_t shape = s1 - 1;

        /* Pad inputs to an efficient length */
        size_t good_size = next_fast_len(shape);
        if (good_size < v1.size())
            std::runtime_error("Something went wrong calculating efficient FFT size.");
        v1.resize(good_size, 0.0);

        /* FFT common variables */
        bool forward = true;
        int inorm = 0;
        int nthreads = 1;
        pocketfft::shape_t axes{1}; // 1 axis on 1D array

        /* v1 FFT */
        pocketfft::shape_t v1_dims_in {1, v1.size()}; // {width, height}
        pocketfft::shape_t v1_dims_out(v1_dims_in);
        v1_dims_out[axes.back()] = (v1_dims_out[axes.back()]>>1) + 1; // Get length of output vector
        size_t v1OutSize = v1_dims_out[axes.back()];
        std::vector< std::complex< vecType> > v1_out(v1OutSize);
        long int s1_in_shape = v1.size() * sizeof(vecType);
        pocketfft::stride_t s1_in {s1_in_shape, sizeof(vecType)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s1_out_shape = v1.size() * sizeof(std::complex< vecType >);
        pocketfft::stride_t s1_out {s1_out_shape, sizeof(std::complex< vecType >)};
        auto d1_in = reinterpret_cast<const vecType *>(v1.data());
        auto d1_out = reinterpret_cast<std::complex<vecType> *>(v1_out.data());
        vecType v1_fct = normFct<vecType>(inorm, v1_dims_in, axes);
        pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct,
            nthreads);
        
        /* Get element-wise absolute value of complex vector */
        ret.resize(v1_out.size());
        auto calculate_magnitude = []( const std::complex< vecType > x ) { return magnitude(x); };
        std::transform(
                v1_out.begin(),
                v1_out.end(),
                ret.begin(),
                calculate_magnitude );

        return ret;
    }

    std::tuple< double , double > quadraticInterpolation(double a, double b, double y, int middle_point_index);

    template< typename vecType,
            typename = std::enable_if_t< std::is_floating_point<vecType>::value> >
    std::vector< std::tuple< vecType, vecType > > peakDetect(const std::vector<vecType> &inp, vecType threshold=-1000.0, bool interpolate=true, std::string sort_by="position", int max_num_peaks=0)
    {   
        int inp_size = inp.size();
        if (inp_size < 2){
            std::string err_msg = "Peak detection input vector must be greater than 2.";
            throw std::runtime_error(err_msg);
        }

        std::vector< std::tuple< vecType, vecType > > estimated_peaks;

        /* Start at the beginning */
        int i = 0;
        
        /* Check if lower bound is a peak */
        if (inp[i] > inp[i+1] && inp[i] > threshold) {
            std::tuple<vecType, vecType> peak(i, inp[i]);
            estimated_peaks.push_back(peak);
        }

        while (true) {
            /*  Down 
                [0, 3, 4, 3, 2, 1, 1, 0]
                       ^  ^  ^  ^  ^
            */
            while (i + 1 < inp_size - 1 && inp[i] >= inp[i + 1]) {
                i++;
            }

            /*  Up 
                [0, 0, 1, 2, 3, 4, 3, 2]
                       ^  ^  ^  ^ 
            */
            while (i + 1 < inp_size - 1 && inp[i] < inp[i + 1]) {
                i++;
            }
            /* Do not register a peak here because we need to check for a flat peak */

            /*  Flat peak 
                [0, 0, 1, 1, 1, 1, 0, 0]
                       ^  ^  ^  ^ 
            */
            int j = i;
            while (j+1 < inp_size-1 && (inp[j] == inp[j+1])) {
                j++;
            }

            if (i+1 >= inp_size-1) { // check the one just before the last position
                if (i == inp_size - 2 && inp[i-1] < inp[i] && inp[i+1] < inp[i] && inp[i] > threshold) {
                    vecType pos;
                    vecType val;
                    
                    if (interpolate) {
                        std::tie(pos, val) = quadraticInterpolation(inp[i-1], inp[i], inp[i+1], j);
                    }
                    else {
                        pos = i;
                        val = inp[i];
                    }
                    // std::cout << estimated_peaks << std::endl;
                    std::tuple<vecType, vecType> peak(pos, val);
                    estimated_peaks.push_back(peak);
                }

                /* Check if element before last is a peak right before breaking the loop */
                int last_index = inp_size - 1;
                if (inp_size - 2 < last_index && last_index <= inp_size-1 && inp[inp_size-1] > inp[inp_size-2] && inp[inp_size-1] > threshold) {
                    std::tuple<vecType, vecType> peak(inp_size-1, inp[inp_size-1]);
                    estimated_peaks.push_back(peak);
                }
                break;
            }

            /* Flat peak ends, check if we are going down */
            if ((j+1 <= inp_size - 1) && inp[j] > inp[j + 1] && inp[j] > threshold) {
                vecType pos;
                vecType val;

                if (j != i) {  /* Flat peak between i and j */
                    if (interpolate) {
                        /* Get the middle of the flat peak */
                        pos = (i + j) * 0.5;
                    }
                    else {
                        /* Get rising edge of flat peak */
                        pos = i;
                    }
                    val = inp[i];
                }
                else {  /* Interpolate peak at i-1, i and i+1 */
                    if (interpolate) {
                        std::tie(pos, val) = quadraticInterpolation(inp[i-1], inp[i], inp[i+1], j);
                    }
                    else {
                        pos = j;
                        val = inp[j];
                    }
                }

                if (pos > inp_size - 1)
                    break;

                std::tuple<vecType, vecType> peak(pos, val);
                estimated_peaks.push_back(peak);
            }
        
            /* No flat peak or we continue up after flat peak, so we start loop again */
            i = j;
        }

        /* Sorting */
        std::transform( sort_by.begin(),
                        sort_by.end(),
                        sort_by.begin(),
                        [](unsigned char c){ return std::tolower(c); }
                    );
        
        std::vector< std::tuple< vecType, vecType > > sorted_estimated_peaks = estimated_peaks;
        if (sort_by == "position") {
            /* Already sorted by position */
        }
        else if(sort_by == "height"){
            std::sort(sorted_estimated_peaks.begin(),
                      sorted_estimated_peaks.end(),
                      [](auto const &t1, auto const &t2) {
                        return std::get<1>(t1) > std::get<1>(t2); }
                    );
        }
        else{
            std::string err_msg = "Sorting by '" + sort_by + "' is not supported.";
            throw std::runtime_error(err_msg);
        }

        /* Shrink to max number of peaks */
        size_t num_peaks = max_num_peaks;
        if ( num_peaks != 0 && num_peaks < sorted_estimated_peaks.size() ){
            std::vector< std::tuple< vecType, vecType > > limited_num_of_sorted_estimated_peaks = sorted_estimated_peaks;
            limited_num_of_sorted_estimated_peaks.resize(num_peaks);
            limited_num_of_sorted_estimated_peaks.shrink_to_fit();
 
            return limited_num_of_sorted_estimated_peaks;
        }

        return sorted_estimated_peaks;
    }
    
}
#endif /* __UTILS_H__ */