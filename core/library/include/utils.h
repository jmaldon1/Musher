#pragma once

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

namespace musher {

enum WeightType { NONE, COSINE, SQUARED_COSINE };
enum NormalizeType { N_NONE, N_UNIT_MAX, N_UNIT_SUM };

struct HarmonicPeak {
    /* Constructor */ 
    HarmonicPeak(double semitone, double harmonic_strength = 0.0)
      : semitone(semitone), harmonic_strength(harmonic_strength) {}

    double semitone;
    double harmonic_strength;
};

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

double normalizeInt8_t(const uint8_t sample);
double normalizeInt16_t(const int16_t sample);
double normalizeInt32_t(const int32_t sample);


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

std::vector<double> interleave2DVector(const std::vector<std::vector<double>> &orig);
double median(std::vector<double> &inVec);
std::vector<double> onePoleFilter(const std::vector< double > &vec);

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

double normFct(int inorm, size_t N);
double normFct(int inorm,
                const pocketfft::shape_t &shape,
                const pocketfft::shape_t &axes,
                size_t fct=1,
                int delta=0);

std::vector<double> centerVector(std::vector<double> arr, size_t newShape);
std::vector<double> fftConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2);
std::vector<double> blackmanHarris92dB(const std::vector<double> &window);
double magnitude(const std::complex< double > complex_pair);
std::vector<double> convertToFrequencySpectrum(const std::vector<double> &flattened_normalized_samples);
std::tuple<double, double> quadraticInterpolation(double a, double b, double y, int middle_point_index);

std::vector<std::tuple<double, double>> peakDetect(const std::vector<double> &inp,
                                                   double threshold=-1000.0,
                                                   bool interpolate=true,
                                                   std::string sort_by="position",
                                                   int max_num_peaks=0,
                                                   double range=0,
                                                   int min_pos=0,
                                                   int max_pos=0 );

std::vector<std::tuple<double, double>> spectralPeaks(const std::vector<double> &inp,
                                                      double threshold=-1000.0,
                                                      bool interpolate=true,
                                                      std::string sort_by="position",
                                                      int max_num_peaks=0,
                                                      double sample_rate=44100.,
                                                      int min_pos=0,
                                                      int max_pos=0);

void addContributionWithWeight(double freq,
                               double mag_lin,
                               double reference_frequency,
                               double window_size,
                               WeightType weight_type,
                               double harmonic_weight,
                               std::vector<double>& hpcp);
void addContribution(double freq,
                     double mag_lin,
                     double reference_frequency,
                     double window_size,
                     WeightType weight_type,
                     std::vector<HarmonicPeak> harmonic_peaks,
                     std::vector<double>& hpcp);
std::vector<HarmonicPeak> initHarmonicContributionTable(int harmonics);

// normalize a vector so its largest value gets mapped to 1
// if zero, the vector isn't touched
template <typename T>
void normalize(std::vector<T>& array)
{
    if (array.empty()) return;

    T maxElement = *std::max_element(array.begin(), array.end());

    if (maxElement != (T) 0.0) {
        for (uint i=0; i<array.size(); i++) {
            array[i] /= maxElement;
        }
    }
}

// normalize a vector so it's sum is equal to 1. the vector is not touched if
// it contains negative elements or the sum is zero
template <typename T>
void normalizeSum(std::vector<T>& array)
{
  if (array.empty()) return;

  //T sumElements = std::accumulate(array.begin(), array.end(), (T) 0.0);
  T sumElements = (T) 0.;
  for (size_t i=0; i<array.size(); ++i) {
    if (array[i] < 0) return;
    sumElements += array[i];
  }

  if (sumElements != (T) 0.0) {
    for (size_t i=0; i<array.size(); ++i) {
      array[i] /= sumElements;
    }
  }
}

int max_vector_element(const std::vector<double>& input);

std::vector<double> HPCP(const std::vector<double>& frequencies,
                         const std::vector<double>& magnitudes,
                         int size=12,
                         double reference_frequency=440.0,
                         int harmonics=0,
                         bool band_preset=true,
                         double band_split_frequency=500.0,
                         double min_frequency=40.0,
                         double max_frequency=5000.0,
                         std::string _weight_type="squared cosine",
                         double window_size=1.0,
                         double sample_rate=44100.,
                         bool max_shifted=false,
                         bool non_linear=false,
                         std::string _normalized="unit max");

std::vector<double> HPCP(const std::vector<std::tuple<double, double>>& peaks,
                         int size=12,
                         double reference_frequency=440.0,
                         int harmonics=0,
                         bool band_preset=true,
                         double band_split_frequency=500.0,
                         double min_frequency=40.0,
                         double max_frequency=5000.0,
                         std::string _weight_type="squared cosine",
                         double window_size=1.0,
                         double sample_rate=44100.,
                         bool max_shifted=false,
                         bool non_linear=false,
                         std::string _normalized="unit max");

std::vector<double> framecutter(const std::vector<double> buffer,
                                int start_index=0,
                                int frame_size=1024,
                                // int hop_size=512,
                                bool start_from_center=false,
                                bool last_frame_to_end_of_file=false,
                                double valid_frame_threshold_ratio=0.);

}  // namespace musher