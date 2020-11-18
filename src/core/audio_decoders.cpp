#include "src/core/audio_decoders.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#define MINIMP3_IMPLEMENTATION
#include <minimp3/minimp3_ex.h>

#include "src/core/utils.h"

namespace musher {
namespace core {

std::vector<uint8_t> LoadAudioFile(const std::string& file_path) {
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

WavDecoded DecodeWav(const std::vector<uint8_t>& file_data) {
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
    data_chunk_index = static_cast<int>(std::distance(file_data.begin(), data_chunk_it));
  }

  // find format chunk in file_data
  std::string format_chunk_key = "fmt";
  int format_chunk_index = -1;
  auto formatChunkIt =
      std::search(file_data.begin(), file_data.end(), format_chunk_key.begin(), format_chunk_key.end());
  if (formatChunkIt != file_data.end()) {
    format_chunk_index = static_cast<int>(std::distance(file_data.begin(), formatChunkIt));
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
  uint32_t sample_rate = static_cast<uint32_t>(FourBytesToInt(file_data, f + 12));
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
  if ((num_bytes_per_second != static_cast<int32_t>((num_channels * sample_rate * bit_depth)) / 8) ||
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
        // double sample = NormalizeInt32_t(sample_as_int);
        double sample = static_cast<double>(sample_as_int);
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

  return wav_decoded;
}

WavDecoded DecodeWav(const std::string& file_path) {
  std::vector<uint8_t> file_data = LoadAudioFile(file_path);
  return DecodeWav(file_data);
}

Mp3Decoded DecodeMp3(const std::string file_path) {
  mp3dec_t mp3d;
  mp3dec_file_info_t info;
  if (mp3dec_load(&mp3d, file_path.c_str(), &info, NULL, NULL)) {
    // error
    throw std::runtime_error("Unable to decode MP3.");
  }

  std::vector<int32_t> interleaved_samples(info.buffer, info.buffer + info.samples);
  free(info.buffer);
  int num_samples = static_cast<int>(info.samples);
  bool mono = info.channels == 1;
  bool stereo = info.channels == 2;
  int samples_per_channel;
  if (stereo) {
    samples_per_channel = num_samples / 2;
  } else {
    samples_per_channel = static_cast<int>(info.samples);
  }

  uint32_t sample_rate = info.hz;
  double length_in_seconds = static_cast<double>(samples_per_channel) / static_cast<double>(sample_rate);
  std::string file_type = "mp3";

  std::vector<double> interleaved_normalized_samples(interleaved_samples.size());
  std::transform(interleaved_samples.begin(), interleaved_samples.end(), interleaved_normalized_samples.begin(),
                 [](const int32_t x) { return static_cast<double>(x); });

  Mp3Decoded mp3_decoded;
  mp3_decoded.sample_rate = sample_rate;
  mp3_decoded.channels = info.channels;
  mp3_decoded.mono = mono;
  mp3_decoded.stereo = stereo;
  mp3_decoded.samples_per_channel = samples_per_channel;
  mp3_decoded.length_in_seconds = length_in_seconds;
  mp3_decoded.file_type = file_type;
  mp3_decoded.avg_bitrate_kbps = info.avg_bitrate_kbps;
  mp3_decoded.normalized_samples = Deinterweave(interleaved_normalized_samples);

  return mp3_decoded;
}

}  // namespace core
}  // namespace musher
