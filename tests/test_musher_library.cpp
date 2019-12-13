#include <iostream>
#include <string>
#include <stdexcept>
#include <variant>
#include <unordered_map>
#include <math.h>
#include <numeric>
#include "gtest/gtest.h"

/* use experimental filesystem on linux */
#if defined(WIN32) || defined(__APPLE__)
#  include <filesystem>
#else
#  include <experimental/filesystem>
#endif

#include "musher_library.h"
#include "utils.h"
// #include "wavelib.h"

/* TEST INCLUDES */
// #include "test_load_audio_file.h"

/* use experimental filesystem on linux */
#if defined(WIN32) || defined(__APPLE__)
	namespace fs = std::filesystem;
#else
	namespace fs = std::experimental::filesystem;
#endif

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
    
    std::string outputFile = "./tests/test_include/filedata.txt";
    // outputVectorToFile<uint8_t>(fileData, outputFile);

    std::string fileDataHex = uint8_vector_to_hex_string(fileData);
    // ASSERT_TRUE(fileData == fileDataExpected);
}


TEST(AudioFileDecoding, DecodeWav) {
    std::vector<uint8_t> fileData;
    const std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
    fileData = CLoadAudioFile(filePath);

    std::unordered_map< std::string, std::variant<int, uint32_t, double, bool> > wavDecodedData;
    /* Sample should only be of type double or float */
    std::vector< std::vector<double> > normalizedSamples;
    normalizedSamples = CDecodeWav<double>(wavDecodedData, fileData);

    // std::vector< std::vector<double> >::const_iterator row; 
    // std::vector<double>::const_iterator col; 

    // std::cout << "LOOPING" << std::endl;
    // for (row = normalizedSamples.begin(); row != normalizedSamples.end(); ++row)
    // { 
    //      for (col = row->begin(); col != row->end(); ++col)
    //      { 
    //         std::cout << *col; 
    //      } 
    // } 

    uint32_t expectedSampleRate = 22050;
    uint32_t actualSampleRate = variantToType<uint32_t>(wavDecodedData["sample_rate"]);

    EXPECT_EQ( expectedSampleRate, actualSampleRate );

    int expectedBitDepth = 16;
    int actualBitDepth = variantToType<int>(wavDecodedData["bit_depth"]);

    EXPECT_EQ( expectedBitDepth, actualBitDepth );

    int expectedChannels = 1;
    int actualChannels = variantToType<int>(wavDecodedData["channels"]);

    EXPECT_EQ( expectedChannels, actualChannels );

    bool expectedMono = true;
    bool actualMono = variantToType<bool>(wavDecodedData["mono"]);

    EXPECT_EQ( expectedMono, actualMono );

    bool expectedStereo = false;
    bool actualStereo = variantToType<bool>(wavDecodedData["stereo"]);

    EXPECT_EQ( expectedStereo, actualStereo );

    double expectedLengthInSeconds = 3.0;
    double actualLengthInSeconds = variantToType<double>(wavDecodedData["length_in_seconds"]);

    EXPECT_EQ( expectedLengthInSeconds, actualLengthInSeconds );

    int expectedNumSamplesPerChannel = 66150;
    int actualNumSamplesPerChannel = variantToType<int>(wavDecodedData["samples_per_channel"]);

    EXPECT_EQ( expectedNumSamplesPerChannel, actualNumSamplesPerChannel );
}

TEST(AudioFileDecoding, BeatDetection) {
    std::vector<uint8_t> fileData;
    const std::string filePath = "./tests/audio_files/Faded.wav";
    fileData = CLoadAudioFile(filePath);
    std::unordered_map< std::string, std::variant<int, uint32_t, double, bool> > wavDecodedData;
    std::vector< std::vector<double> > normalizedSamples;
    std::vector<double> flattenedNormalizedSamples;

    normalizedSamples = CDecodeWav<double>(wavDecodedData, fileData);
    flattenedNormalizedSamples = interleave2DVector(normalizedSamples);
    uint32_t sampleRate = variantToType<uint32_t>(wavDecodedData["sample_rate"]);

    double bpm = bpmsOverWindow(flattenedNormalizedSamples, flattenedNormalizedSamples.size(), sampleRate, 3);

    std::cout << bpm << std::endl;
    // EXPECT_EQ( bpm, 80.0 );
    // for (auto & element : seconds) {
    //     std::cout << element << std::endl;
    // }
}
