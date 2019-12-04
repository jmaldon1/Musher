#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <variant>
#include <unordered_map>
#include "gtest/gtest.h"

#include "musher_library.h"
#include "utils.h"

/* TEST INCLUDES */
#include "test_load_audio_file.h"

using namespace musher;


// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(PrintFunctionalMessage, PrintsMessage) {
	CPrintFunctionalMessage("This is a message for a functional program");

	EXPECT_EQ(0, 0);
}


TEST(AudioFileDecoding, AudioFileNotFoundTest) {
	/* This tests that the expected exception is thrown */
    EXPECT_THROW({
        try
        {
            CLoadAudioFile("/unknown/abs/file/path.wav");
        }
        catch( const std::runtime_error& e )
        {   
            /* This tests if the error message is equal */
            EXPECT_STREQ( "No file found at '/unknown/abs/file/path.wav'" , e.what() );
            throw;
        }
    }, std::runtime_error );
}


TEST(AudioFileDecoding, LoadsAudioFileTest) {
    std::vector<uint8_t> fileData;
    std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
	fileData = CLoadAudioFile(filePath);

    std::string fileDataHex = uint8_vector_to_hex_string(fileData);
    EXPECT_EQ( FileDataHexCantinaBand3sec, fileDataHex );
}


TEST(AudioFileDecoding, DecodeWav) {
    std::vector<uint8_t> fileData;
    const std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
    fileData = CLoadAudioFile(filePath);

    std::unordered_map< std::string, std::variant<int, uint32_t, double, bool> > wavDecodedData;
    std::vector< std::vector<double> > audioBuffer;
    CDecodeWav(wavDecodedData, fileData, audioBuffer);

    uint32_t expectedSampleRate = 22050;
    uint32_t actualSampleRate;
    if (std::holds_alternative<uint32_t>(wavDecodedData["sample_rate"]))
        actualSampleRate = (uint32_t)std::visit([](uint32_t arg) {return arg;}, wavDecodedData["sample_rate"]);

    EXPECT_EQ( expectedSampleRate, actualSampleRate );

    int expectedBitDepth = 16;
    int actualBitDepth;
    if (std::holds_alternative<int>(wavDecodedData["bit_depth"]))
        actualBitDepth = (int)std::visit([](int arg) {return arg;}, wavDecodedData["bit_depth"]);

    EXPECT_EQ( expectedBitDepth, actualBitDepth );

    int expectedChannels = 1;
    int actualChannels;
    if (std::holds_alternative<int>(wavDecodedData["channels"]))
        actualChannels = (int)std::visit([](int arg) {return arg;}, wavDecodedData["channels"]);

    EXPECT_EQ( expectedChannels, actualChannels );

    bool expectedMono = true;
    bool actualMono;
    if (std::holds_alternative<bool>(wavDecodedData["mono"]))
        actualMono = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["mono"]);

    EXPECT_EQ( expectedMono, actualMono );

    bool expectedStereo = false;
    bool actualStereo;
    if (std::holds_alternative<bool>(wavDecodedData["stereo"]))
        actualStereo = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["stereo"]);

    EXPECT_EQ( expectedStereo, actualStereo );

    double expectedLengthInSeconds = 3.0;
    double actualLengthInSeconds;
    if (std::holds_alternative<double>(wavDecodedData["length_in_seconds"]))
        actualLengthInSeconds = (double)std::visit([](double arg) {return arg;}, wavDecodedData["length_in_seconds"]);

    EXPECT_EQ( expectedLengthInSeconds, actualLengthInSeconds );

    int expectedNumSamplesPerChannel = 66150;
    int actualNumSamplesPerChannel;
    if (std::holds_alternative<int>(wavDecodedData["samples_per_channel"]))
        actualNumSamplesPerChannel = (int)std::visit([](int arg) {return arg;}, wavDecodedData["samples_per_channel"]);

    EXPECT_EQ( expectedNumSamplesPerChannel, actualNumSamplesPerChannel );
}
