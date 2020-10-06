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
#include <iostream>

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

    
    double normalizeInt8_t(const uint8_t sample)
    {
        return static_cast<double> (sample - 128) / static_cast<double> (128.);
    }

    
    double normalizeInt16_t(const int16_t sample)
    {
        return static_cast<double> (sample) / static_cast<double> (32768.);
    }

    
    double normalizeInt32_t(const int32_t sample)
    {
        return static_cast<double> (sample) / static_cast<double> (8388608.);
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

    std::vector<double> interleave2DVector(const std::vector<std::vector<double>> &orig)
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
        std::vector<double> a(orig[0]);
        std::vector<double> b(orig[1]);
        std::vector<double> result;

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

    
    double median(std::vector< double > &inVec)
    {
        std::vector<double> vec(inVec);
        std::sort(vec.begin(), vec.end());
        double median;
        if (vec.size() % 2 == 0)
            median = 0.5 * (vec[vec.size() / 2 - 1] + vec[vec.size() / 2]);
        else
            median = vec[vec.size() / 2];
        
        return median;
    }

    std::vector<double> onePoleFilter(const std::vector< double > &vec)
    {
        double a = 0.99;
        double y = 0.0;

        std::vector<double> filteredSignal;
        auto onePoolEquation = [a, &y]( const double x ) { return y += (1 - a) * x - a * y; };
        std::transform(
               vec.begin(),
               vec.end(),
               std::back_inserter(filteredSignal),
               onePoolEquation );

        return filteredSignal;
    }

    // template< typename double,
    //         typename = std::enable_if_t< std::is_floating_point<double>::value> >
    // double peakDetect(std::vector< double > &correlVec)
    // {
    //     std::vector<double> vecAbs(correlVec.size());
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

    double normFct(int inorm, size_t N)
    {
        if (inorm==0) return double(1);
        if (inorm==2) return double(1/ldbl_t(N));
        if (inorm==1) return double(1/sqrt(ldbl_t(N)));
        throw std::runtime_error("invalid value for inorm (must be 0, 1, or 2)");
    }

    double normFct(int inorm,
            const pocketfft::shape_t &shape,
            const pocketfft::shape_t &axes,
            size_t fct=1,
            int delta=0)
    {
        if (inorm==0) return double(1);
        size_t N(1);
        for (auto a: axes)
            N *= fct * size_t(int64_t(shape[a])+delta);
        return normFct<double>(inorm, N);
    }

    std::vector<double> centerVector(std::vector<double> arr, size_t newShape)
    {
        size_t currShape = arr.size();
        size_t startInd = (currShape - newShape) / 2;
        size_t endind = startInd + newShape;
        std::vector<double> sliced(arr.begin() + startInd, arr.begin() + endind);
        return sliced;
    }

    std::vector<double> fftConvolve(const std::vector< double > &vec1, const std::vector< double > &vec2)
    {
        /* 
            This function computes a 'same' FFT Convolve
    
            This function was heavily inspired by:
            https://github.com/scipy/scipy/blob/12fa74e97d3d18ca3a4e6991327663e88462f238/scipy/signal/signaltools.py#L551
            https://github.com/scipy/scipy/blob/master/scipy/fft/_pocketfft/pypocketfft.cxx
        */

        std::vector<double> v1(vec1);
        std::vector<double> v2(vec2);
        std::vector<double> ret;

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
        std::vector< std::complex< double> > v1_out(v1OutSize);
        long int s1_in_shape = v1.size() * sizeof(double);
        pocketfft::stride_t s1_in {s1_in_shape, sizeof(double)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s1_out_shape = v1.size() * sizeof(std::complex< double >);
        pocketfft::stride_t s1_out {s1_out_shape, sizeof(std::complex< double >)};
        auto d1_in = reinterpret_cast<const double *>(v1.data());
        auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
        double v1_fct = normFct<double>(inorm, v1_dims_in, axes);
        pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct,
            nthreads);

        /* vec2 FFT */
        pocketfft::shape_t v2_dims_in {1, v2.size()}; // {width, height}
        pocketfft::shape_t v2_dims_out(v2_dims_in);
        v2_dims_out[axes.back()] = (v2_dims_out[axes.back()]>>1) + 1; // Get length of output vector
        size_t v2OutSize = v2_dims_out[axes.back()];
        std::vector< std::complex< double> > v2_out(v2OutSize);
        long int s2_in_shape = v2.size() * sizeof(double);
        pocketfft::stride_t s2_in {s2_in_shape, sizeof(double)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s2_out_shape = v2.size() * sizeof(std::complex< double >);
        pocketfft::stride_t s2_out {s2_out_shape, sizeof(std::complex< double >)};
        auto d2_in = reinterpret_cast<const double *>(v2.data());
        auto d2_out = reinterpret_cast<std::complex<double> *>(v2_out.data());
        double v2_fct = normFct<double>(inorm, v2_dims_in, axes);
        pocketfft::r2c(v2_dims_in, s2_in, s2_out, axes, forward, d2_in, d2_out, v2_fct,
            nthreads);

        /* Element-wise multiplication of 2 complex vectors */
        std::vector< std::complex< double> > products(v1_out.size());
        std::transform(
                v1_out.begin(),
                v1_out.end(),
                v2_out.begin(),
                products.begin(),
                std::multiplies< std::complex< double> >() );

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
        std::vector< double > conv_out(conv_out_size);
        long int conv_in_shape = products.size() * sizeof(std::complex< double >);
        pocketfft::stride_t s_conv_in {conv_in_shape, sizeof(std::complex< double >)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int conv_out_shape = conv_out_size * sizeof(double);
        pocketfft::stride_t s_conv_out {conv_out_shape, sizeof(double)};
        auto d_conv_in = reinterpret_cast<const std::complex<double> *>(products.data());
        auto d_conv_out = reinterpret_cast<double *>(conv_out.data());
        double conv_fct = normFct<double>(inorm, conv_dims_out, axes);
        pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct,
            nthreads);

        std::vector<double> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

        /* Perform 'same' part of the convolve */
        std::vector<double> centered_conv_out = centerVector(sliced_conv_out, s1);
        return centered_conv_out;
    }

    template< typename double,
            typename = std::enable_if_t< std::is_arithmetic<double>::value> >
    std::vector<double> blackmanHarris92dB(const std::vector<double> &window)
    {   
        /* window functions help control spectral leakage when doing Fourier Analysis */
        std::vector<double> ret(window);
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

    double magnitude(const std::complex< double > complex_pair)
    {   
        /* Calculate absolute value (magnitude or modulus) of a complex number */
        return sqrt(pow(complex_pair.real(), 2) + pow(complex_pair.imag(), 2));

    }

    
    std::vector<double> convertToFrequencySpectrum(const std::vector<double> &flattened_normalized_samples)
    {
        std::vector<double> v1(flattened_normalized_samples);
        std::vector<double> ret;

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
        std::vector< std::complex< double> > v1_out(v1OutSize);
        long int s1_in_shape = v1.size() * sizeof(double);
        pocketfft::stride_t s1_in {s1_in_shape, sizeof(double)}; // {height * sizeof(type), sizeof(type)}
        /* NOTE: Putting the size of the wrong type will produce wrong results */
        long int s1_out_shape = v1.size() * sizeof(std::complex< double >);
        pocketfft::stride_t s1_out {s1_out_shape, sizeof(std::complex< double >)};
        auto d1_in = reinterpret_cast<const double *>(v1.data());
        auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
        double v1_fct = normFct<double>(inorm, v1_dims_in, axes);
        pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct,
            nthreads);
        
        /* Get element-wise absolute value of complex vector */
        ret.resize(v1_out.size());
        auto calculate_magnitude = []( const std::complex< double > x ) { return magnitude(x); };
        std::transform(
                v1_out.begin(),
                v1_out.end(),
                ret.begin(),
                calculate_magnitude );

        return ret;
    }

    std::tuple< double , double > quadraticInterpolation(double a, double b, double y, int middle_point_index);

    
    std::vector< std::tuple< double, double > > peakDetect(const std::vector<double> &inp,
                                                             double threshold=-1000.0,
                                                             bool interpolate=true,
                                                             std::string sort_by="position",
                                                             int max_num_peaks=0,
                                                             double range=0,
                                                             int min_pos=0,
                                                             int max_pos=0 )
    {   
        const int inp_size = inp.size();
        if (inp_size < 2){
            std::string err_msg = "Peak detection input vector must be greater than 2.";
            throw std::runtime_error(err_msg);
        }

        if (min_pos != 0 && max_pos !=0 && min_pos >= max_pos) {
            std::string err_msg = "Peak detection max position must be greater than min position.";
            throw std::runtime_error(err_msg);
        }

        std::vector< std::tuple< double, double > > estimated_peaks;

        double scale = 1;
        if ( range > 0 ){
            scale = range / static_cast<double>(inp_size - 1);
        }
        /* Start at minimum position */
        int i = 0;
        if (min_pos > 0){
            /* We are dividing by scale because the scale should have been accounted for when the user input the value */
            i = (int) ceil(min_pos / scale);
        }

        if (max_pos == 0){
            max_pos = (inp_size - 1) * scale;
        }
        
        /* Check if lower bound is a peak */
        if (inp[i] > inp[i+1] && inp[i] > threshold) {
            std::tuple<double, double> peak(i * scale, inp[i]);
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
            while (j + 1 < inp_size - 1 && (inp[j] == inp[j+1])) {
                j++;
            }

            /* Check element right before the last element */
            if (i + 1 >= inp_size - 1) {
                if (i == inp_size - 2 && inp[i-1] < inp[i] && inp[i+1] < inp[i] && inp[i] > threshold) {
                    double pos;
                    double val;
                    
                    if (interpolate) {
                        std::tie(pos, val) = quadraticInterpolation(inp[i-1], inp[i], inp[i+1], j);
                    }
                    else {
                        pos = i;
                        val = inp[i];
                    }
                    std::tuple<double, double> peak(pos * scale, val);
                    estimated_peaks.push_back(peak);
                }

                /* We are dividing by scale because the scale should have been accounted for when the user input the value */
                double scale_removed_max_pos = max_pos / scale;
                /* Check if element before last is a peak right before breaking the loop */
                int last_index = inp_size - 1;
                if (scale_removed_max_pos > inp_size - 2 && scale_removed_max_pos <= inp_size - 1 && inp[inp_size-1] > inp[inp_size-2] && inp[inp_size-1] > threshold) {
                    std::tuple<double, double> peak((inp_size - 1) * scale, inp[inp_size-1]);
                    estimated_peaks.push_back(peak);
                }
                break;
            }

            /* Flat peak ends, check if we are going down */
            if ((j+1 <= inp_size - 1) && inp[j] > inp[j + 1] && inp[j] > threshold) {
                double pos;
                double val;

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

                if (pos * scale > max_pos)
                    break;

                std::tuple<double, double> peak(pos * scale, val);
                estimated_peaks.push_back(peak);
            }
        
            /* No flat peak... We continue up, so we start loop again */
            i = j;
        }

        /* Sorting */
        std::transform( sort_by.begin(),
                        sort_by.end(),
                        sort_by.begin(),
                        [](unsigned char c){ return std::tolower(c); }
                    );
        
        std::vector< std::tuple< double, double > > sorted_estimated_peaks = estimated_peaks;
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
            std::vector< std::tuple< double, double > > limited_num_of_sorted_estimated_peaks = sorted_estimated_peaks;
            limited_num_of_sorted_estimated_peaks.resize(num_peaks);
            limited_num_of_sorted_estimated_peaks.shrink_to_fit();
 
            return limited_num_of_sorted_estimated_peaks;
        }

        return sorted_estimated_peaks;
    }

    
    std::vector< std::tuple< double, double > > spectralPeaks(const std::vector<double> &inp,
                                                                double threshold=-1000.0,
                                                                bool interpolate=true,
                                                                std::string sort_by="position",
                                                                int max_num_peaks=0,
                                                                double sample_rate=44100.,
                                                                int min_pos=0,
                                                                int max_pos=0 )
    {
        std::vector<std::tuple<double, double>> peaks = peakDetect(inp,
                                                                   threshold,
                                                                   interpolate,
                                                                   sort_by,
                                                                   max_num_peaks,
                                                                   sample_rate/2.0,
                                                                   min_pos,
                                                                   max_pos);
        return peaks;
    }

    void HPCP(const std::vector<double>& frequencies,
              const std::vector<double>& magnitudes,
              int size=12,
              double reference_frequency=440.0,
              int harmonics=0,
              bool band_preset=true,
              double band_split_frequency=500.0,
              double min_frequency=40.0,
              double max_frequency=5000.0,
              std::string weigh_type="squared cosine",
              bool non_linear=false,
              double window_size=1.0,
              double sample_rate=44100.,
              bool max_shifted=false,
              std::string normalized="unit max")
    {
        // Check inputs
        if (magnitudes.size() != frequencies.size()) {
            throw std::runtime_error("HPCP: Frequency and magnitude input vectors are not of equal size");
        }

        std::vector<double> hpcp;

        hpcp.resize(size);
        std::fill(hpcp.begin(), hpcp.end(), (double)0.0);

        std::vector<double> hpcp_LO;
        std::vector<double> hpcp_HI;

        if (band_preset) {
            hpcp_LO.resize(size);
            std::fill(hpcp_LO.begin(), hpcp_LO.end(), (double)0.0);

            hpcp_HI.resize(size);
            std::fill(hpcp_HI.begin(), hpcp_HI.end(), (double)0.0);
        }

        std::cout << "hi" << std::endl;
    }

    void HPCP(const std::vector<std::tuple<double, double>>& peaks,
              int size=12,
              double reference_frequency=440.0,
              int harmonics=0,
              bool band_preset=true,
              double band_split_frequency=500.0,
              double min_frequency=40.0,
              double max_frequency=5000.0,
              std::string weigh_type="squared cosine",
              bool non_linear=false,
              double window_size=1.0,
              double sample_rate=44100.,
              bool max_shifted=false,
              std::string normalized="unit max")
    {
        std::vector<double> frequencies(peaks.size());
        std::vector<double> magnitudes(peaks.size());

        std::transform( peaks.begin(),
                        peaks.end(),
                        frequencies.begin(),
                        [](auto const& pair){ return std::get<0>(pair); }
                        );

        std::transform( peaks.begin(),
                        peaks.end(),
                        magnitudes.begin(),
                        [](auto const& pair){ return std::get<1>(pair); }
                        );

        return HPCP(frequencies,
                    magnitudes,
                    size,
                    reference_frequency,
                    harmonics,
                    band_preset,
                    band_split_frequency,
                    min_frequency,
                    max_frequency,
                    weigh_type,
                    non_linear,
                    window_size,
                    sample_rate,
                    max_shifted,
                    normalized);
    }
}
#endif /* __UTILS_H__ */