#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include "gtest/gtest.h"

#include "musher_library.h"


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
	// this tests that the expected exception is thrown
    EXPECT_THROW({
        try
        {
            LoadAudioFile("/unknown/abs/file/path.wav");
        }
        catch( const std::runtime_error& e )
        {
            EXPECT_STREQ( "No file found at /unknown/abs/file/path.wav" , e.what() );
            throw;
        }
    }, std::runtime_error );
}

TEST(AudioFileDecoding, LoadsAudioFileTest) {
	// LoadAudioFile("/Users/JJMALD1/Desktop/Musher/tests/audio_files/WAV_1MG.wav");

	LoadAudioFile("./tests/audio_files/WAV_1MG.wav");
	// getFileAbsPath("./test/audio_files/WAV_1MG.wav");

	// DecodeWav("Hello");

	// AcceptDecode("Hello", DecodeWav);

	EXPECT_EQ(0, 0);
}