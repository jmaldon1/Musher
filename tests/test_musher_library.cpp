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


// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(PrintFunctionalMessage, PrintsMessage) {
	CPrintFunctionalMessage("This is a message for a functional program");

	EXPECT_EQ(0, 0);
}

// TEST(Decode, DecodeTest) {
// 	// LoadAudioFile("/Users/JJMALD1/Desktop/Musher/tests/audio_files/WAV_1MG.wav");

// 	// LoadAudioFile("./test/audio_files/WAV_1MG.wav");
// 	// getFileAbsPath("./test/audio_files/WAV_1MG.wav");

// 	// DecodeWav("Hello");

// 	AcceptDecode("Hello", DecodeWav);

// 	EXPECT_EQ(0, 0);
// }

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
    std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
    fileData = CLoadAudioFile(filePath);

    // std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>> um = {{"hello", 1}};
    std::unordered_map<std::string, int> um = {{"hello", 1}};
    CDecodeWav(um.begin(), fileData);
}
