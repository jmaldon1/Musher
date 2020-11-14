
namespace musher {
namespace core {

// double BPMDetection(std::vector<double>& samples, uint32_t sample_rate) {
//   wave_object obj;
//   wt_object wt;
//   int J = 1;

//   const int total_levels = 4;
//   const int max_decimation = std::pow(2, (total_levels - 1));

//   double min_index = 60. / 220 * (sample_rate / max_decimation);
//   double max_index = 60. / 40 * (sample_rate / max_decimation);

//   const char* name = "db4";
//   obj = wave_init(name);  // Initialize the wavelet

//   size_t cD_min_len;
//   double decimated_signal_sum, decimated_signal_mean;
//   std::vector<double> cD, cD_sum, cD_filtered, cD_decimated_signal, cD_mean_removed_signal,
//       cD_mean_removed_signal_partial;
//   std::vector<double> cA, cA_filtered, cA_mean_removed_signal_partial;
//   for (int level = 0; level < total_levels; level++) {
//     // Discrete Wavelet Transform
//     if (level == 0) {
//       // Initialize the wavelet transform object on input
//       wt = wt_init(obj, (char*)"dwt", samples.size(), J);
//       setDWTExtension(wt, (char*)"sym");
//       setWTConv(wt, (char*)"direct");

//       // Perform DWT
//       dwt(wt, samples.data());

//       cD_min_len = wt->length[1] / max_decimation + 1;
//       cD_sum.resize(cD_min_len, 0.0);
//       cD_mean_removed_signal_partial.resize(cD_min_len);
//     } else {
//       // Initialize the wavelet transform object
//       wt = wt_init(obj, (char*)"dwt", cA.size(), J);
//       setDWTExtension(wt, (char*)"sym");
//       setWTConv(wt, (char*)"direct");

//       // Perform remaining DWT's on cA
//       dwt(wt, cA.data());
//     }

//     // Fill cA
//     cA.clear();
//     for (int i = 0; i < wt->length[0]; ++i) {
//       cA.push_back(wt->output[i]);
//     }

//     // Fill cD
//     for (int i = wt->length[1]; i < wt->outlength; ++i) {
//       cD.push_back(wt->output[i]);
//     }

//     // Perform One Pole filter on cD
//     cD_filtered = OnePoleFilter(cD);

//     // Decimate
//     int dc = std::pow(2, (total_levels - level - 1));
//     for (int ax = 0; ax < cD_filtered.size(); ax += dc) {
//       cD_decimated_signal.push_back(std::abs(cD_filtered[ax]));
//     }

//     decimated_signal_sum = std::accumulate(cD_decimated_signal.begin(), cD_decimated_signal.end(), 0.0);
//     decimated_signal_mean = decimated_signal_sum / static_cast<double>(cD_decimated_signal.size());

//     // Remove the mean
//     cD_mean_removed_signal.resize(cD_decimated_signal.size());
//     auto remove_mean = [decimated_signal_mean](const double x) { return x - decimated_signal_mean; };
//     std::transform(cD_decimated_signal.begin(), cD_decimated_signal.end(), cD_mean_removed_signal.begin(),
//     remove_mean);

//     // Reconstruct
//     std::copy_n(cD_mean_removed_signal.begin(), cD_min_len, cD_mean_removed_signal_partial.begin());
//     // Perform element-wise sum of 2 vectors and store into cD_sum
//     std::transform(cD_sum.begin(), cD_sum.end(), cD_mean_removed_signal_partial.begin(), cD_sum.begin(),
//                    std::plus<double>());

//     // Clear variables
//     wt_free(wt);
//     cD.clear();
//     cD_filtered.clear();
//     cD_decimated_signal.clear();
//     cD_mean_removed_signal.clear();
//     // cD_mean_removed_signal_partial.clear();
//   }
//   wave_free(obj);

//   // Check if cA has any useful data
//   bool zeros = std::all_of(cA.begin(), cA.end(), [](const double d) { return d == 0.0; });
//   if (zeros) return 0.0;

//   // Filter cA
//   cA_filtered = OnePoleFilter(cA);

//   // Make cA_filtered absolute
//   std::vector<double> cA_absolute(cA_filtered.size());
//   auto absolute_val = [](const double x) { return std::abs(x); };
//   std::transform(cA_filtered.begin(), cA_filtered.end(), cA_absolute.begin(), absolute_val);

//   // Get mean
//   double cA_absolute_sum = std::accumulate(cA_absolute.begin(), cA_absolute.end(), 0.0);
//   double cA_absolute_mean = cA_absolute_sum / static_cast<double>(cA_absolute.size());

//   std::vector<double> cA_mean_removed_signal(cA_absolute.size());
//   auto remove_mean = [cA_absolute_mean](const double x) { return x - cA_absolute_mean; };
//   std::transform(cA_absolute.begin(), cA_absolute.end(), cA_mean_removed_signal.begin(), remove_mean);

//   cA_mean_removed_signal_partial.resize(cD_min_len);
//   std::copy_n(cA_mean_removed_signal.begin(), cD_min_len, cA_mean_removed_signal_partial.begin());
//   // Add elements of cD_sum and cD_mean_removed_signal_partial together and store into cD_sum
//   std::transform(cD_sum.begin(), cD_sum.end(), cA_mean_removed_signal_partial.begin(), cD_sum.begin(),
//                  std::plus<double>());

//   size_t data_len = cD_sum.size();
//   std::vector<double> b(data_len * 2);

//   // Fill a section of b with cD_sum data
//   int k = 0;
//   for (int i = data_len / 2; i < (data_len / 2) + data_len; ++i) {
//     b[i] = cD_sum[k];
//     k += 1;
//   }

//   // Reverse cD_sum
//   std::vector<double> reverse_cD(cD_sum);
//   std::reverse(reverse_cD.begin(), reverse_cD.end());

//   // Perform an array flipped convolution, which is the same as a cross-correlation on the samples.
//   std::vector<double> correl = FFTConvolve(b, reverse_cD);
//   correl.pop_back();  // We don't need the last element
//   size_t midpoint = correl.size() / 2;
//   std::vector<double> correl_midpoint_tmp(correl.begin() + midpoint, correl.end());
//   std::vector<double> sliced_correl_midpoint_tmp(correl_midpoint_tmp.begin() + std::floor(min_index),
//                                                  correl_midpoint_tmp.begin() + std::floor(max_index));

//   // Peak Detection
//   std::vector<double> sliced_correl_midpoint_tmp_abs(sliced_correl_midpoint_tmp.size());
//   std::transform(sliced_correl_midpoint_tmp.begin(), sliced_correl_midpoint_tmp.end(),
//                  sliced_correl_midpoint_tmp_abs.begin(), [](const double x) { return std::abs(x); });
//   std::vector<std::tuple<double, double>> peaks;
//   double threshold = -1000.0;
//   bool interpolate = true;
//   std::string sort_by = "height";
//   peaks = PeakDetect(sliced_correl_midpoint_tmp_abs, threshold, interpolate, sort_by);

//   // Get the first item from peaks because we want the highest peak
//   const double peak_index = std::get<0>(peaks[0]);

//   if (peak_index == 0.0) return 0.0;
//   double peak_index_adjusted = peak_index + min_index;
//   double bpm = 60. / peak_index_adjusted * (sample_rate / max_decimation);

//   return bpm;
// }

// double BPMOverWindow(std::vector<double>& samples, uint32_t sample_rate, unsigned int window_seconds) {
//   int num_samples = samples.size();
//   int window_samples = window_seconds * sample_rate;
//   int sample_index = 0;
//   int max_windows_index = num_samples / window_samples;
//   std::vector<double> bpms(max_windows_index, 0.0);
//   std::vector<double> seconds_mid(max_windows_index, 0.0);

//   for (int window_index = 0; window_index < max_windows_index; window_index++) {
//     typename std::vector<double>::iterator samp_it = samples.begin() + sample_index;
//     std::vector<double> sliced_samples(samp_it, samp_it + window_samples);

//     double bpm = BPMDetection(sliced_samples, sample_rate);
//     bpms[window_index] = bpm;

//     sample_index += window_samples;
//   }

//   return std::round(Median(bpms));
// }

}  // namespace core
}  // namespace musher
