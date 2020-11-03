#include <fplus/fplus.hpp>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#define MINIMP3_IMPLEMENTATION
#include "musher/core/musher_library.h"
#include "musher/core/utils.h"

namespace musher {
namespace core {

void CPrintFunctionalMessage(const char* message) { std::cout << message << std::endl; }

std::vector<uint8_t> CLoadAudioFile(const std::string& file_path) {
  // std::error_code e;
  // fs::path audioFileAbsPath = fs::canonical(filePath, e);
  if (file_path.empty()) {
    throw std::runtime_error("No file provided");
  }
  std::ifstream audio_file(file_path, std::ios::binary);

  // skip white space
  audio_file.unsetf(std::ios::skipws);
  std::istream_iterator<uint8_t> begin(audio_file), end;
  if (audio_file.fail()) {
    std::stringstream ss;
    ss << "Failed to load file '" << file_path << "'";
    throw std::runtime_error(ss.str().c_str());
  }

  std::vector<uint8_t> file_data(begin, end);
  return file_data;
}

WavDecoded CDecodeWav(const std::vector<uint8_t>& file_data) {
  std::vector<std::vector<double>> samples;

  if (!samples.empty()) {
    std::string err_message = "Audio Buffer must be empty";
    throw std::runtime_error(err_message);
  }
  // -----------------------------------------------------------
  // HEADER CHUNK
  std::string header_chunk_id(file_data.begin(), file_data.begin() + 4);
  // int32_t fileSizeInBytes = FourBytesToInt (file_data, 4) + 8;
  std::string format(file_data.begin() + 8, file_data.begin() + 12);
  // -----------------------------------------------------------

  // find data chunk in file_data
  const std::string data_chunk_key = "data";
  int data_chunk_index = -1;
  auto data_chunk_it = std::search(file_data.begin(), file_data.end(), data_chunk_key.begin(), data_chunk_key.end());
  if (data_chunk_it != file_data.end()) {
    data_chunk_index = std::distance(file_data.begin(), data_chunk_it);
  }

  // find format chunk in file_data
  std::string format_chunk_key = "fmt";
  int format_chunk_index = -1;
  auto formatChunkIt =
      std::search(file_data.begin(), file_data.end(), format_chunk_key.begin(), format_chunk_key.end());
  if (formatChunkIt != file_data.end()) {
    format_chunk_index = std::distance(file_data.begin(), formatChunkIt);
  }

  // if we can't find the data or format chunks, or the IDs/formats don't seem to be as expected
  // then it is unlikely we'll able to read this file, so abort
  if (data_chunk_index == -1 || format_chunk_index == -1 || header_chunk_id != "RIFF" || format != "WAVE") {
    std::string err_message = "This doesn't seem to be a valid .WAV file";
    throw std::runtime_error(err_message);
  }

  // -----------------------------------------------------------
  // FORMAT CHUNK
  int f = format_chunk_index;
  std::string format_chunk_id(file_data.begin() + f, file_data.begin() + f + 4);
  // int32_t formatChunkSize = FourBytesToInt (file_data, f + 4);
  int16_t audio_format = TwoBytesToInt(file_data, f + 8);
  int16_t num_channels = TwoBytesToInt(file_data, f + 10);
  uint32_t sample_rate = (uint32_t)FourBytesToInt(file_data, f + 12);
  int32_t num_bytes_per_second = FourBytesToInt(file_data, f + 16);
  int16_t num_bytes_per_block = TwoBytesToInt(file_data, f + 20);
  int bit_depth = static_cast<int>(TwoBytesToInt(file_data, f + 22));

  int num_bytes_per_sample = bit_depth / 8;

  // check that the audio format is PCM
  if (audio_format != 1) {
    std::string err_message =
        "This is a compressed .WAV file and this library does not support decoding them at present";
    throw std::runtime_error(err_message);
  }

  // check the number of channels is mono or stereo
  if (num_channels < 1 || num_channels > 2) {
    std::string err_message = "This WAV file seems to be neither mono nor stereo (perhaps multi-track, or corrupted?";
    throw std::runtime_error(err_message);
  }

  // check header data is consistent
  if ((num_bytes_per_second != (num_channels * sample_rate * bit_depth) / 8) ||
      (num_bytes_per_block != (num_channels * num_bytes_per_sample))) {
    std::string err_message = "The header data in this WAV file seems to be inconsistent";
    throw std::runtime_error(err_message);
  }

  // check bit depth is either 8, 16 or 24 bit
  if (bit_depth != 8 && bit_depth != 16 && bit_depth != 24) {
    std::string err_message = "This file has a bit depth that is not 8, 16 or 24 bits";
    throw std::runtime_error(err_message);
  }

  // -----------------------------------------------------------
  // DATA CHUNK
  int d = data_chunk_index;
  std::string data_chunk_id(file_data.begin() + d, file_data.begin() + d + 4);
  int32_t data_chunk_size = FourBytesToInt(file_data, d + 4);

  int num_samples = data_chunk_size / (num_channels * bit_depth / 8);
  int samples_start_index = data_chunk_index + 8;

  samples.resize(num_channels);

  for (int i = 0; i < num_samples; i++) {
    for (int channel = 0; channel < num_channels; channel++) {
      int sample_index = samples_start_index + (num_bytes_per_block * i) + channel * num_bytes_per_sample;

      if (bit_depth == 8) {
        // Normalize samples to between -1 and 1
        double sample = NormalizeInt8_t(file_data[sample_index]);
        samples[channel].push_back(sample);
      } else if (bit_depth == 16) {
        int16_t sample_as_int = TwoBytesToInt(file_data, sample_index);
        // Normalize samples to between -1 and 1
        double sample = NormalizeInt16_t(sample_as_int);
        samples[channel].push_back(sample);
      } else if (bit_depth == 24) {
        int32_t sample_as_int = 0;
        sample_as_int =
            (file_data[sample_index + 2] << 16) | (file_data[sample_index + 1] << 8) | file_data[sample_index];

        if (sample_as_int & 0x800000)  // if the 24th bit is set, this is a negative number in 24-bit world
          sample_as_int = sample_as_int | ~0xFFFFFF;  // so make sure sign is extended to the 32 bit float

        // Normalize samples to between -1 and 1
        double sample = NormalizeInt32_t(sample_as_int);
        samples[channel].push_back(sample);
      } else {
        std::string err_message =
            "This file has a bit depth that is not 8, 16 or 24 bits, not sure how you got past the first error check.";
        throw std::runtime_error(err_message);
      }
    }
  }

  int num_channels_int = static_cast<int>(num_channels);
  bool mono = num_channels == 1;
  bool stereo = num_channels == 2;
  int num_samples_per_channel = 0;
  if (samples.size() > 0) num_samples_per_channel = static_cast<int>(samples[0].size());
  double length_in_seconds = static_cast<double>(num_samples_per_channel) / static_cast<double>(sample_rate);
  std::string file_type = "wav";
  int avg_bitrate_kbps = (sample_rate * bit_depth * num_channels_int) / 1000;

  WavDecoded wav_decoded;
  wav_decoded.sample_rate = sample_rate;
  wav_decoded.bit_depth = bit_depth;
  wav_decoded.channels = num_channels_int;
  wav_decoded.mono = mono;
  wav_decoded.stereo = stereo;
  wav_decoded.samples_per_channel = num_samples_per_channel;
  wav_decoded.length_in_seconds = length_in_seconds;
  wav_decoded.file_type = file_type;
  wav_decoded.avg_bitrate_kbps = avg_bitrate_kbps;
  wav_decoded.normalized_samples = samples;

  if (stereo) {
    const std::vector<double> channel_one = samples[0];
    const std::vector<double> channel_two = samples[1];
    wav_decoded.interleaved_normalized_samples = fplus::interweave(channel_one, channel_two);
  } else {
    wav_decoded.interleaved_normalized_samples = samples[0];
  }

  return wav_decoded;
}

WavDecoded CDecodeWav(const std::string& file_path) {
  std::vector<uint8_t> file_data = CLoadAudioFile(file_path);
  return CDecodeWav(file_data);
}

Mp3Decoded CDecodeMp3(const std::string file_path) {
  mp3dec_t mp3d;
  mp3dec_file_info_t info;
  if (mp3dec_load(&mp3d, file_path.c_str(), &info, NULL, NULL)) {
    // error
    throw std::runtime_error("Unable to decode MP3.");
  }

  std::vector<int32_t> interleaved_samples(info.buffer, info.buffer + info.samples);
  int num_samples = static_cast<int>(info.samples);
  bool mono = info.channels == 1;
  bool stereo = info.channels == 2;
  int samples_per_channel;
  if (stereo) {
    samples_per_channel = num_samples / 2;
  } else {
    samples_per_channel = info.samples;
  }

  uint32_t sample_rate = info.hz;
  double length_in_seconds = static_cast<double>(samples_per_channel) / static_cast<double>(sample_rate);
  std::string file_type = "mp3";

  std::vector<double> interleaved_normalized_samples(interleaved_samples.size());
  std::transform(interleaved_samples.begin(), interleaved_samples.end(), interleaved_normalized_samples.begin(),
                 [](const int32_t x) { return NormalizeInt32_t(x); });

  Mp3Decoded mp3_decoded;
  mp3_decoded.sample_rate = sample_rate;
  mp3_decoded.channels = info.channels;
  mp3_decoded.mono = mono;
  mp3_decoded.stereo = stereo;
  mp3_decoded.samples_per_channel = samples_per_channel;
  mp3_decoded.length_in_seconds = length_in_seconds;
  mp3_decoded.file_type = file_type;
  mp3_decoded.avg_bitrate_kbps = info.avg_bitrate_kbps;
  mp3_decoded.interleaved_normalized_samples = interleaved_normalized_samples;
  mp3_decoded.normalized_samples = Deinterweave(interleaved_normalized_samples);

  return mp3_decoded;
}

double BPMDetection(std::vector<double>& samples, uint32_t sample_rate) {
  wave_object obj;
  wt_object wt;
  int J = 1;

  const int total_levels = 4;
  const int max_decimation = std::pow(2, (total_levels - 1));

  double min_index = 60. / 220 * (sample_rate / max_decimation);
  double max_index = 60. / 40 * (sample_rate / max_decimation);

  const char* name = "db4";
  obj = wave_init(name);  // Initialize the wavelet

  size_t cD_min_len;
  double decimated_signal_sum, decimated_signal_mean;
  std::vector<double> cD, cD_sum, cD_filtered, cD_decimated_signal, cD_mean_removed_signal,
      cD_mean_removed_signal_partial;
  std::vector<double> cA, cA_filtered, cA_mean_removed_signal_partial;
  for (int level = 0; level < total_levels; level++) {
    // Discrete Wavelet Transform
    if (level == 0) {
      // Initialize the wavelet transform object on input
      wt = wt_init(obj, (char*)"dwt", samples.size(), J);
      setDWTExtension(wt, (char*)"sym");
      setWTConv(wt, (char*)"direct");

      // Perform DWT
      dwt(wt, samples.data());

      cD_min_len = wt->length[1] / max_decimation + 1;
      cD_sum.resize(cD_min_len, 0.0);
      cD_mean_removed_signal_partial.resize(cD_min_len);
    } else {
      // Initialize the wavelet transform object
      wt = wt_init(obj, (char*)"dwt", cA.size(), J);
      setDWTExtension(wt, (char*)"sym");
      setWTConv(wt, (char*)"direct");

      // Perform remaining DWT's on cA
      dwt(wt, cA.data());
    }

    // Fill cA
    cA.clear();
    for (int i = 0; i < wt->length[0]; ++i) {
      cA.push_back(wt->output[i]);
    }

    // Fill cD
    for (int i = wt->length[1]; i < wt->outlength; ++i) {
      cD.push_back(wt->output[i]);
    }

    // Perform One Pole filter on cD
    cD_filtered = OnePoleFilter(cD);

    // Decimate
    int dc = std::pow(2, (total_levels - level - 1));
    for (int ax = 0; ax < cD_filtered.size(); ax += dc) {
      cD_decimated_signal.push_back(std::abs(cD_filtered[ax]));
    }

    decimated_signal_sum = std::accumulate(cD_decimated_signal.begin(), cD_decimated_signal.end(), 0.0);
    decimated_signal_mean = decimated_signal_sum / static_cast<double>(cD_decimated_signal.size());

    // Remove the mean
    cD_mean_removed_signal.resize(cD_decimated_signal.size());
    auto remove_mean = [decimated_signal_mean](const double x) { return x - decimated_signal_mean; };
    std::transform(cD_decimated_signal.begin(), cD_decimated_signal.end(), cD_mean_removed_signal.begin(), remove_mean);

    // Reconstruct
    std::copy_n(cD_mean_removed_signal.begin(), cD_min_len, cD_mean_removed_signal_partial.begin());
    // Perform element-wise sum of 2 vectors and store into cD_sum
    std::transform(cD_sum.begin(), cD_sum.end(), cD_mean_removed_signal_partial.begin(), cD_sum.begin(),
                   std::plus<double>());

    // Clear variables
    wt_free(wt);
    cD.clear();
    cD_filtered.clear();
    cD_decimated_signal.clear();
    cD_mean_removed_signal.clear();
    // cD_mean_removed_signal_partial.clear();
  }
  wave_free(obj);

  // Check if cA has any useful data
  bool zeros = std::all_of(cA.begin(), cA.end(), [](const double d) { return d == 0.0; });
  if (zeros) return 0.0;

  // Filter cA
  cA_filtered = OnePoleFilter(cA);

  // Make cA_filtered absolute
  std::vector<double> cA_absolute(cA_filtered.size());
  auto absolute_val = [](const double x) { return std::abs(x); };
  std::transform(cA_filtered.begin(), cA_filtered.end(), cA_absolute.begin(), absolute_val);

  // Get mean
  double cA_absolute_sum = std::accumulate(cA_absolute.begin(), cA_absolute.end(), 0.0);
  double cA_absolute_mean = cA_absolute_sum / static_cast<double>(cA_absolute.size());

  std::vector<double> cA_mean_removed_signal(cA_absolute.size());
  auto remove_mean = [cA_absolute_mean](const double x) { return x - cA_absolute_mean; };
  std::transform(cA_absolute.begin(), cA_absolute.end(), cA_mean_removed_signal.begin(), remove_mean);

  cA_mean_removed_signal_partial.resize(cD_min_len);
  std::copy_n(cA_mean_removed_signal.begin(), cD_min_len, cA_mean_removed_signal_partial.begin());
  // Add elements of cD_sum and cD_mean_removed_signal_partial together and store into cD_sum
  std::transform(cD_sum.begin(), cD_sum.end(), cA_mean_removed_signal_partial.begin(), cD_sum.begin(),
                 std::plus<double>());

  size_t data_len = cD_sum.size();
  std::vector<double> b(data_len * 2);

  // Fill a section of b with cD_sum data
  int k = 0;
  for (int i = data_len / 2; i < (data_len / 2) + data_len; ++i) {
    b[i] = cD_sum[k];
    k += 1;
  }

  // Reverse cD_sum
  std::vector<double> reverse_cD(cD_sum);
  std::reverse(reverse_cD.begin(), reverse_cD.end());

  // Perform an array flipped convolution, which is the same as a cross-correlation on the samples.
  std::vector<double> correl = FFTConvolve(b, reverse_cD);
  correl.pop_back();  // We don't need the last element
  size_t midpoint = correl.size() / 2;
  std::vector<double> correl_midpoint_tmp(correl.begin() + midpoint, correl.end());
  std::vector<double> sliced_correl_midpoint_tmp(correl_midpoint_tmp.begin() + std::floor(min_index),
                                                 correl_midpoint_tmp.begin() + std::floor(max_index));

  // Peak Detection
  std::vector<double> sliced_correl_midpoint_tmp_abs(sliced_correl_midpoint_tmp.size());
  std::transform(sliced_correl_midpoint_tmp.begin(), sliced_correl_midpoint_tmp.end(),
                 sliced_correl_midpoint_tmp_abs.begin(), [](const double x) { return std::abs(x); });
  std::vector<std::tuple<double, double>> peaks;
  double threshold = -1000.0;
  bool interpolate = true;
  std::string sort_by = "height";
  peaks = PeakDetect(sliced_correl_midpoint_tmp_abs, threshold, interpolate, sort_by);

  // Get the first item from peaks because we want the highest peak
  const double peak_index = std::get<0>(peaks[0]);

  if (peak_index == 0.0) return 0.0;
  double peak_index_adjusted = peak_index + min_index;
  double bpm = 60. / peak_index_adjusted * (sample_rate / max_decimation);

  return bpm;
}

double BPMOverWindow(std::vector<double>& samples, uint32_t sample_rate, unsigned int window_seconds) {
  int num_samples = samples.size();
  int window_samples = window_seconds * sample_rate;
  int sample_index = 0;
  int max_windows_index = num_samples / window_samples;
  std::vector<double> bpms(max_windows_index, 0.0);
  std::vector<double> seconds_mid(max_windows_index, 0.0);

  for (int window_index = 0; window_index < max_windows_index; window_index++) {
    typename std::vector<double>::iterator samp_it = samples.begin() + sample_index;
    std::vector<double> sliced_samples(samp_it, samp_it + window_samples);

    double bpm = BPMDetection(sliced_samples, sample_rate);
    bpms[window_index] = bpm;

    sample_index += window_samples;
  }

  return std::round(Median(bpms));
}

}  // namespace core
}  // namespace musher
