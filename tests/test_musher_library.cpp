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


// struct
// {
//     auto operator()(int a)
//     {
//         return a;
//         //Called if variant holds an int
//     }
//     auto operator()(uint16_t a)
//     {
//         return a;
//         //Called if variant holds a float
//     }
//     auto operator()(double a)
//     {
//         return a;
//         //Called if variant holds a char
//     }
//     auto operator()(bool a)
//     {
//         return a;
//         //Called if variant holds a char
//     }
// } Vistor;


// template <class ...Fs>
// struct overload : Fs... {
//   overload(Fs const&... fs) : Fs{fs}...
//   {}

//   using Fs::operator()...;
// };

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
    // std::cout << actualSampleRate << std::endl;

    EXPECT_EQ( expectedSampleRate, actualSampleRate );

    int expectedBitDepth = 16;
    int actualBitDepth;
    if (std::holds_alternative<int>(wavDecodedData["bit_depth"]))
        actualBitDepth = (int)std::visit([](int arg) {return arg;}, wavDecodedData["bit_depth"]);
    // std::cout << actualBitDepth << std::endl;

    EXPECT_EQ( expectedBitDepth, actualBitDepth );

    int expectedChannels = 1;
    int actualChannels;
    if (std::holds_alternative<int>(wavDecodedData["channels"]))
        actualChannels = (int)std::visit([](int arg) {return arg;}, wavDecodedData["channels"]);
    // std::cout << actualChannels << std::endl;

    EXPECT_EQ( expectedChannels, actualChannels );

    bool expectedMono = true;
    bool acutalMono;
    if (std::holds_alternative<bool>(wavDecodedData["mono"]))
        acutalMono = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["mono"]);
    // std::cout << acutalMono << std::endl;

    EXPECT_EQ( expectedMono, acutalMono );

    bool expectedStereo = false;
    bool actualStereo;
    if (std::holds_alternative<bool>(wavDecodedData["stereo"]))
        actualStereo = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["stereo"]);
    // std::cout << actualStereo << std::endl;

    EXPECT_EQ( expectedStereo, actualStereo );

    double expectedLengthInSeconds = 3.0;
    double actualLengthInSeconds;
    if (std::holds_alternative<double>(wavDecodedData["length_in_seconds"]))
        actualLengthInSeconds = (double)std::visit([](double arg) {return arg;}, wavDecodedData["length_in_seconds"]);
    // std::cout << actualLengthInSeconds << std::endl;

    EXPECT_EQ( expectedLengthInSeconds, actualLengthInSeconds );

    int expectedNumSamplesPerChannel = 66150;
    int actualNumSamplesPerChannel;
    if (std::holds_alternative<int>(wavDecodedData["samples_per_channel"]))
        actualNumSamplesPerChannel = (int)std::visit([](int arg) {return arg;}, wavDecodedData["samples_per_channel"]);
    // std::cout << actualNumSamplesPerChannel << std::endl;

    EXPECT_EQ( expectedNumSamplesPerChannel, actualNumSamplesPerChannel );



    // struct {
    //     void operator()(int a) { std::cout << "int!\n" << a; }
    //     void operator()(double) { std::cout << "double!\n"; }
    //     void operator()(uint32_t) { std::cout << "uint32!\n"; }
    //     void operator()(bool) { std::cout << "bool!\n"; }
    //   } visitor;


    // int k2 = (int)std::visit([](int arg) {return arg;}, wavDecodedData["sample_rate"]);
    // int k3 = (int)k2;
    // std::variant<int, uint32_t, double, bool> my_var;
    // std::variant<int, uint32_t, double, bool> k2 = std::visit(
    //     overload(
    //       [](const int a){return a;},
    //       [](const uint32_t b){return b;}
    //       // [](const double a){return a;},
    //       // [](const bool a){return a;}
    //     ),
    //     wavDecodedData["sample_rate"]
    // );

    // std::visit([](auto arg) {
    //     if (std::holds_alternative<int>(arg))
    //         auto v_int = std::get<int>(arg);
    //     else if (std::holds_alternative<char>(arg))
    //         auto v_chart = std::get<char>(arg);
    //     else if (std::holds_alternative<double>(arg))
    //         auto v_double = std::get<double>(arg);
    //     else if (std::holds_alternative<bool>(arg))
    //         auto v_bool = std::get<bool>(arg);
    //     else if (std::holds_alternative<std::string>(arg))
    //         auto v_str = std::get<std::string>(arg);
    //     }, wavDecodedData["sample_rate"]);
}
