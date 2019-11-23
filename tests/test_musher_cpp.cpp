#include <iostream>
#include <string>
#include "gtest/gtest.h"

#include "functional_test.h"


// IndependentMethod is a test case - here, we have 2 tests for this 1 test case
TEST(PrintFunctionalMessage, PrintsMessage) {
	CPrintFunctionalMessage("This is a message for a functional program");

	EXPECT_EQ(0, 0);
}

TEST(LoadAudioFile, LoadsAudioFile) {
	// LoadAudioFile("/Users/JJMALD1/Desktop/Musher/tests/audio_files/WAV_1MG.wav");

	getFileAbsPath("/Users/JJMALD1/Desktop/Musher/tests/audio_files/WAV_1MG.wav");

	EXPECT_EQ(0, 0);
}