#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <complex>
#include <valarray>
#include <assert.h>

#include "utils.h"

namespace musher {
/**
 * @brief Convert uint8_t vector to hex string
 * @details Convert uint8_t vector to hex string
 * 
 * @param uint8_t::vector vector of uint8_t
 * @return std::string string of hex
 */
std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v)
{
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    std::vector<uint8_t>::const_iterator it;

    for (it = v.begin(); it != v.end(); it++) {
        ss << std::setw(2) << static_cast<unsigned>(*it);
    }

    return ss.str();
}

/**
 * @brief Get string between two single quotes
 * @details Get string between two single quotes
 * 
 * @param s String that contains 2 single quotes
 * @return string between single quotes
 */
std::string strBetweenSQuotes(const std::string &s)
{
    std::string squote = "'";
    // Find 1st quote starting from beginning of string
    std::size_t pos = s.find(squote) + 1;
    // Find 2nd quote starting from end of string
    std::size_t endpos = s.rfind(squote);
    // Find length of string between single quotes
    std::size_t len = endpos - pos;

    // Return the substring between single quotes
    return s.substr(pos, len);
}

int16_t twoBytesToInt(const std::vector<uint8_t>& source, const int startIndex)
{
    int16_t result;
    
    if (!isBigEndian())
        result = (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 8) | source[startIndex + 1];
    
    return result;
}

int32_t fourBytesToInt(const std::vector<uint8_t>& source, const int startIndex)
{
    int32_t result;
    
    if (!isBigEndian())
        result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) | source[startIndex + 3];
    
    return result;
}

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

bool isBigEndian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
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

double median(std::vector<double> &inVec)
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

size_t next_fast_len(size_t n)
{
    /* 
    Copied from Peter Bell. 
    https://gdoc.pub/doc/e/2PACX-1vR6iXXG1uS9ds47GvDgQk6XtpYzVTtYepu5B8onBrMmoorfKHhnHbN0ArDoXgoA23nZrcrm_DSFMW45 
    */
    if (n<=6) return n;

    size_t bestfac=2*n;
    for (size_t f5=1; f5<bestfac; f5*=5)
    {
        size_t x = f5;
        while (x<n) x *= 2;
            for (;;)
                {
                    if (x<n)
                        x*=3;
                    else if (x>n)
                    {
                        if (x<bestfac) bestfac=x;
                        if (x&1) break;
                        x>>=1;
                    }
                    else
                        return n;
                }
    }
    return bestfac;
}

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
                size_t fct,
                int delta)
{
    if (inorm==0) return double(1);
    size_t N(1);
    for (auto a: axes)
        N *= fct * size_t(int64_t(shape[a])+delta);
    return normFct(inorm, N);
}

std::vector<double> centerVector(std::vector<double> arr, size_t newShape)
{
    size_t currShape = arr.size();
    size_t startInd = (currShape - newShape) / 2;
    size_t endind = startInd + newShape;
    std::vector<double> sliced(arr.begin() + startInd, arr.begin() + endind);
    return sliced;
}

std::vector<double> fftConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2)
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
    double v1_fct = normFct(inorm, v1_dims_in, axes);
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
    double v2_fct = normFct(inorm, v2_dims_in, axes);
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
    double conv_fct = normFct(inorm, conv_dims_out, axes);
    pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct,
        nthreads);

    std::vector<double> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

    /* Perform 'same' part of the convolve */
    std::vector<double> centered_conv_out = centerVector(sliced_conv_out, s1);
    return centered_conv_out;
}

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
    double v1_fct = normFct(inorm, v1_dims_in, axes);
    pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct,
        nthreads);
    
    /* Get element-wise absolute value of complex vector */
    ret.resize(v1_out.size());
    auto calculate_magnitude = []( const std::complex< double > x ) { return magnitude(x); };
    std::transform(v1_out.begin(),
                    v1_out.end(),
                    ret.begin(),
                    calculate_magnitude );

    return ret;
}


std::tuple<double, double> quadraticInterpolation(double a, double b, double y, int middle_point_index)
{
    /* 
    Smith, J.O. "Quadratic Interpolation of Spectral Peaks", in 
    Spectral Audio Signal Processing,
    https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html, online book, 
    2011 edition,
    accessed 12/18/2019.

    α(a) = left point value of parabola
    β(b) = middle point value of parabola
    γ(y) = right point value of parabola

    Interpolated peak location is given in bins (spectral samples) by:
    p = 1/2 ((α - γ) / (α - 2β + γ))

    The peak magnitude estimate is:
    y(p) = β - 1/4(α - γ)p
    */

    double p = 0.5 * ((a - y) / (a - 2 * b + y));
    double peak_location = static_cast<double>(middle_point_index) + p;
    double peak_height_estimate = b - 0.25 * (a - y) * p;
    return std::make_tuple(peak_location, peak_height_estimate);
}

std::vector<std::tuple<double, double>> peakDetect(const std::vector<double> &inp,
                                                   double threshold,
                                                   bool interpolate,
                                                   std::string sort_by,
                                                   int max_num_peaks,
                                                   double range,
                                                   int min_pos,
                                                   int max_pos )
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

std::vector<std::tuple<double, double>> spectralPeaks(const std::vector<double> &inp,
                                                            double threshold,
                                                            bool interpolate,
                                                            std::string sort_by,
                                                            int max_num_peaks,
                                                            double sample_rate,
                                                            int min_pos,
                                                            int max_pos)
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

std::vector<double> addContributionWithWeight(double freq,
                                              double reference_frequency,
                                              double window_size,
                                              WeightType weight_type,
                                              double mag_lin,
                                              double harmonic_weight,
                                              bool band_preset,
                                              int size) {
    std::vector<double> hpcp;
    if (band_preset) {
        hpcp.resize(size);
        std::fill(hpcp.begin(), hpcp.end(), (double)0.0);
    }
    int pcp_size = hpcp.size();
    int semitone = 12;
    double resolution = pcp_size / semitone; // # of bins / semitone

    /* Convert frequency in Hz to frequency in pcp_bin index.
    note: this can be a negative value */
    double pcp_bin_F = log2(freq / reference_frequency) * (double)pcp_size;

    /* Which bins are covered by the window centered at this frequency
    note: this is not wrapped. */
    int left_bin = (int)ceil(pcp_bin_F - resolution * window_size / 2.0);
    int right_bin = (int)floor(pcp_bin_F + resolution * window_size / 2.0);

    assert(right_bin - left_bin >= 0);

    /* Apply weight to all bins in the window */
    for (int i=left_bin; i<=right_bin; i++) {
        double distance = abs(pcp_bin_F - (double)i)/resolution;
        double normalized_distance = distance/window_size;
        double weight = 0.;

        if (weight_type == COSINE) {
            weight = cos(M_PI*normalized_distance);
        }
        else if (weight_type == SQUARED_COSINE) {
            weight = cos(M_PI*normalized_distance);
            weight *= weight;
        }

        int iwrapped = i % pcp_size;
        if (iwrapped < 0) iwrapped += pcp_size;

        hpcp[iwrapped] += weight * (mag_lin*mag_lin) * harmonic_weight * harmonic_weight;
    }
    return hpcp;
}

void HPCP(const std::vector<double>& frequencies,
          const std::vector<double>& magnitudes,
          int size,
          double reference_frequency,
          int harmonics,
          bool band_preset,
          double band_split_frequency,
          double min_frequency,
          double max_frequency,
          std::string _weigh_type,
          bool non_linear,
          double window_size,
          double sample_rate,
          bool max_shifted,
          std::string normalized)
{
    // Check inputs
    if (magnitudes.size() != frequencies.size()) {
        throw std::runtime_error("HPCP: Frequency and magnitude input vectors are not of equal size");
    }

    if      (_weigh_type == "none") weight_type = NONE;
    else if (_weigh_type == "cosine") weight_type = COSINE;
    else if (_weigh_type == "squared cosine") weight_type = SQUARED_COSINE;
    else {
        std::string err_message = "HPCP: Invalid weight type of: ";
        err_message += _weigh_type;
        throw std::runtime_error(err_message);
    }

    std::vector<double> hpcp;

    hpcp.resize(size);
    std::fill(hpcp.begin(), hpcp.end(), (double)0.0);

    std::vector<double> hpcp_LO;
    std::vector<double> hpcp_HI;

    // if (band_preset) {
    //     hpcp_LO.resize(size);
    //     std::fill(hpcp_LO.begin(), hpcp_LO.end(), (double)0.0);

    //     hpcp_HI.resize(size);
    //     std::fill(hpcp_HI.begin(), hpcp_HI.end(), (double)0.0);
    // }

    // Add each contribution of the spectral frequencies to the HPCP
    for (int i=0; i<(int)frequencies.size(); i++) {
        double freq = frequencies[i];
        double mag_lin = magnitudes[i];

        // Filter out frequencies not between min and max
        if (freq >= min_frequency && freq <= max_frequency) {
            if (band_preset) {
                // addContribution(freq, mag_lin, (freq < band_split_frequency) ? hpcp_LO : hpcp_HI);
            }
            else {
                // addContribution(freq, mag_lin, hpcp);
            }
        }
    }

    std::cout << "hi" << std::endl;
}

void HPCP(const std::vector<std::tuple<double, double>>& peaks,
          int size,
          double reference_frequency,
          int harmonics,
          bool band_preset,
          double band_split_frequency,
          double min_frequency,
          double max_frequency,
          std::string weigh_type,
          bool non_linear,
          double window_size,
          double sample_rate,
          bool max_shifted,
          std::string normalized)
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

}  // namespace musher
