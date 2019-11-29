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

    uint32_t sampleRate;
    if (std::holds_alternative<uint32_t>(wavDecodedData["sample_rate"]))
        sampleRate = (uint32_t)std::visit([](uint32_t arg) {return arg;}, wavDecodedData["sample_rate"]);
    std::cout << sampleRate << std::endl;

    int bitDepth;
    if (std::holds_alternative<int>(wavDecodedData["bit_depth"]))
        bitDepth = (int)std::visit([](int arg) {return arg;}, wavDecodedData["bit_depth"]);
    std::cout << bitDepth << std::endl;

    int channels;
    if (std::holds_alternative<int>(wavDecodedData["channels"]))
        channels = (int)std::visit([](int arg) {return arg;}, wavDecodedData["channels"]);
    std::cout << channels << std::endl;

    bool mono;
    if (std::holds_alternative<bool>(wavDecodedData["mono"]))
        mono = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["mono"]);
    std::cout << mono << std::endl;

    bool stereo;
    if (std::holds_alternative<bool>(wavDecodedData["stereo"]))
        stereo = (bool)std::visit([](bool arg) {return arg;}, wavDecodedData["stereo"]);
    std::cout << stereo << std::endl;

    double lengthInSeconds;
    if (std::holds_alternative<double>(wavDecodedData["length_in_seconds"]))
        lengthInSeconds = (double)std::visit([](double arg) {return arg;}, wavDecodedData["length_in_seconds"]);
    std::cout << lengthInSeconds << std::endl;

    int numSamplesPerChannel;
    if (std::holds_alternative<int>(wavDecodedData["samples_per_channel"]))
        numSamplesPerChannel = (int)std::visit([](int arg) {return arg;}, wavDecodedData["samples_per_channel"]);
    std::cout << numSamplesPerChannel << std::endl;

    // struct {
    //     void operator()(int a) { std::cout << "int!\n" << a; }
    //     void operator()(double) { std::cout << "double!\n"; }
    //     void operator()(uint32_t) { std::cout << "uint32!\n"; }
    //     void operator()(bool) { std::cout << "bool!\n"; }
    //   } visitor;


    // int k2 = (int)std::visit([](int arg) {return arg;}, wavDecodedData["sample_rate"]);
    // int k3 = (int)k2;
    // auto k2 = std::visit(
    //     overload(
    //       [](const int a){return a;},
    //       // [](const uint32_t b){return b;}
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
