#include "src/core/fft_convolve.h"

#include <pocketfft/pocketfft.h>

#include <algorithm>
#include <complex>
#include <stdexcept>
#include <vector>

#include "src/core/spectrum.h"

namespace musher {
namespace core {

std::vector<double> CenterVector(const std::vector<double> &vec, size_t new_shape) {
  size_t curr_shape = vec.size();
  size_t start_index = (curr_shape - new_shape) / 2;
  size_t end_index = start_index + new_shape;
  std::vector<double> sliced(vec.begin() + start_index, vec.begin() + end_index);
  return sliced;
}

std::vector<double> FFTConvolve(const std::vector<double> &vec1, const std::vector<double> &vec2) {
  std::vector<double> v1(vec1);
  std::vector<double> v2(vec2);
  std::vector<double> ret;

  if (v1.empty() || v2.empty()) return ret;

  size_t s1 = v1.size();
  size_t s2 = v2.size();
  size_t shape = s1 + s2 - 1;

  // Pad inputs to an efficient length
  size_t good_size = NextFastLen(shape);
  if (good_size < v1.size() || good_size < v2.size())
    std::runtime_error("Something went wrong calculating efficient FFT size.");
  v1.resize(good_size, 0.0);
  v2.resize(good_size, 0.0);

  // FFT common variables
  bool forward = true;
  int inorm = 0;
  int nthreads = 1;
  pocketfft::shape_t axes{ 1 };  // 1 axis on 1D array

  // vec1 FFT
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

  // vec2 FFT
  pocketfft::shape_t v2_dims_in{ 1, v2.size() };  // {width, height}
  pocketfft::shape_t v2_dims_out(v2_dims_in);
  v2_dims_out[axes.back()] = (v2_dims_out[axes.back()] >> 1) + 1;  // Get length of output vector
  size_t v2OutSize = v2_dims_out[axes.back()];
  std::vector<std::complex<double>> v2_out(v2OutSize);
  long int s2_in_shape = v2.size() * sizeof(double);
  pocketfft::stride_t s2_in{ s2_in_shape, sizeof(double) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int s2_out_shape = v2.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s2_out{ s2_out_shape, sizeof(std::complex<double>) };
  auto d2_in = reinterpret_cast<const double *>(v2.data());
  auto d2_out = reinterpret_cast<std::complex<double> *>(v2_out.data());
  double v2_fct = NormFct(inorm, v2_dims_in, axes);
  pocketfft::r2c(v2_dims_in, s2_in, s2_out, axes, forward, d2_in, d2_out, v2_fct, nthreads);

  // Element-wise multiplication of 2 complex vectors
  std::vector<std::complex<double>> products(v1_out.size());
  std::transform(v1_out.begin(), v1_out.end(), v2_out.begin(), products.begin(),
                 std::multiplies<std::complex<double>>());

  // Save size of products before padding
  size_t product_size_pre_pad = products.size();

  // Pad products to efficient size
  products.resize(good_size, 0.0);

  // Inverse FFT of complex to real
  forward = false;
  inorm = 2;
  size_t axis = axes.back();
  size_t lastsize = products.size();
  pocketfft::shape_t conv_dims_in{ 1, product_size_pre_pad };  // {width, height}
  pocketfft::shape_t conv_dims_out(conv_dims_in);
  // if (lastsize==0) lastsize = 2 * conv_dims_in[axis] - 1;
  if ((lastsize / 2) + 1 != conv_dims_in[axis]) throw std::runtime_error("bad lastsize");
  conv_dims_out[axis] = lastsize;
  size_t conv_out_size = conv_dims_out[axis];
  std::vector<double> conv_out(conv_out_size);
  long int conv_in_shape = products.size() * sizeof(std::complex<double>);
  pocketfft::stride_t s_conv_in{ conv_in_shape,
                                 sizeof(std::complex<double>) };  // {height * sizeof(type), sizeof(type)}
  // NOTE: Putting the size of the wrong type will produce wrong results
  long int conv_out_shape = conv_out_size * sizeof(double);
  pocketfft::stride_t s_conv_out{ conv_out_shape, sizeof(double) };
  auto d_conv_in = reinterpret_cast<const std::complex<double> *>(products.data());
  auto d_conv_out = reinterpret_cast<double *>(conv_out.data());
  double conv_fct = NormFct(inorm, conv_dims_out, axes);
  pocketfft::c2r(conv_dims_out, s_conv_in, s_conv_out, axes, forward, d_conv_in, d_conv_out, conv_fct, nthreads);

  std::vector<double> sliced_conv_out(conv_out.begin(), conv_out.begin() + shape);

  // Perform 'same' part of the convolve
  std::vector<double> centered_conv_out = CenterVector(sliced_conv_out, s1);
  return centered_conv_out;
}

}  // namespace core
}  // namespace musher