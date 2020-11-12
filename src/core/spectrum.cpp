#include "src/core/spectrum.h"

#include <pocketfft/pocketfft.h>

#include <algorithm>
#include <complex>
#include <stdexcept>
#include <vector>

namespace musher {
namespace core {

double Magnitude(const std::complex<double> complex_pair) {
  return std::sqrt(std::pow(complex_pair.real(), 2) + std::pow(complex_pair.imag(), 2));
}

double NormFct(int inorm, size_t N) {
  if (inorm == 0) return double(1);
  if (inorm == 2) return double(1 / ldbl_t(N));
  if (inorm == 1) return double(1 / sqrt(ldbl_t(N)));
  throw std::runtime_error("invalid value for inorm (must be 0, 1, or 2)");
}

double NormFct(int inorm, const pocketfft::shape_t &shape, const pocketfft::shape_t &axes, size_t fct, int delta) {
  if (inorm == 0) return double(1);
  size_t N(1);
  for (auto a : axes) N *= fct * size_t(int64_t(shape[a]) + delta);
  return NormFct(inorm, N);
}

size_t NextFastLen(size_t n) {
  if (n <= 6) return n;

  size_t best_fac = 2 * n;
  for (size_t f5 = 1; f5 < best_fac; f5 *= 5) {
    size_t x = f5;
    while (x < n) x *= 2;
    for (;;) {
      if (x < n)
        x *= 3;
      else if (x > n) {
        if (x < best_fac) best_fac = x;
        if (x & 1) break;
        x >>= 1;
      } else
        return n;
    }
  }
  return best_fac;
}

std::vector<double> ConvertToFrequencySpectrum(const std::vector<double> &audio_frame) {
  std::vector<double> v1(audio_frame);
  std::vector<double> ret;

  if (v1.empty()) return ret;

  size_t s1 = v1.size();
  size_t shape = s1 - 1;

  // Pad inputs to an efficient length
  size_t good_size = NextFastLen(shape);
  if (good_size < v1.size()) std::runtime_error("Something went wrong calculating efficient FFT size.");
  v1.resize(good_size, 0.0);

  // FFT common variables
  bool forward = true;
  int inorm = 0;
  int nthreads = 1;
  pocketfft::shape_t axes{ 1 };  // 1 axis on 1D array

  // v1 FFT
  pocketfft::shape_t v1_dims_in{ 1, v1.size() };  // {width, height}
  pocketfft::shape_t v1_dims_out(v1_dims_in);
  v1_dims_out[axes.back()] = (v1_dims_out[axes.back()] >> 1) + 1;  // Get length of output vector
  size_t v1OutSize = v1_dims_out[axes.back()];
  std::vector<std::complex<double>> v1_out(v1OutSize);
  long int s1_in_shape = v1.size() * sizeof(double);
  pocketfft::stride_t s1_in{ s1_in_shape, sizeof(double) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int s1_out_shape = v1.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s1_out{ s1_out_shape, sizeof(std::complex<double>) };
  auto d1_in = reinterpret_cast<const double *>(v1.data());
  auto d1_out = reinterpret_cast<std::complex<double> *>(v1_out.data());
  double v1_fct = NormFct(inorm, v1_dims_in, axes);
  pocketfft::r2c(v1_dims_in, s1_in, s1_out, axes, forward, d1_in, d1_out, v1_fct, nthreads);

  // Get element-wise absolute value of a complex vector
  ret.resize(v1_out.size());
  auto calculate_magnitude = [](const std::complex<double> x) { return Magnitude(x); };
  std::transform(v1_out.begin(), v1_out.end(), ret.begin(), calculate_magnitude);

  return ret;
}

}  // namespace core
}  // namespace musher