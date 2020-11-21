// TEST(AudioFileDecoding, BeatDetection) {
//   const std::string filePath = TEST_DATA_DIR + std::string("audio_files/CantinaBand3sec.wav");

//   WavDecoded wav_decoded = DecodeWav(filePath);
//   uint32_t sampleRate = wav_decoded.sample_rate;
//   std::vector<double> interleaved_normalized_samples = wav_decoded.interleaved_normalized_samples;

//   double bpm = BPMOverWindow(interleaved_normalized_samples, sampleRate, 3);
//   EXPECT_EQ(bpm, 80.);
// }

// TEST(AudioFileDecoding, MP3BPM) {
//   std::vector<double> NormalizedSamples;
//   const std::string filePathMp3 = TEST_DATA_DIR + std::string("audio_files/126bpm.mp3");

//   Mp3Decoded mp3_decoded = CDecodeMp3(filePathMp3);
//   uint32_t sampleRate = mp3_decoded.sample_rate;
//   std::vector<double> interleaved_normalized_samples = mp3_decoded.interleaved_normalized_samples;

//   double bpm = BPMOverWindow(interleaved_normalized_samples, sampleRate, 3);
//   // std::cout << bpm << std::endl;
//   EXPECT_DOUBLE_EQ(bpm, 125.);
// }
