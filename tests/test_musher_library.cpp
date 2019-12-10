#include <iostream>
#include <string>
#include <stdexcept>
#include <filesystem>
#include <variant>
#include <unordered_map>
#include <math.h>
#include <numeric>
#include "gtest/gtest.h"

#include "musher_library.h"
#include "utils.h"
#include "wavelib.h"

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

double RMS_Error(double *data, double *rec, int N) {
    int i;
    double sum = 0;
    for (i = 0; i < N; ++i) {
        sum += (data[i] - rec[i])*(data[i] - rec[i]);
    }
    return sqrt(sum/((double)N-1));
}

TEST(WaveletTransform, DiscreteWaveleteTransform) {
    wave_object obj;
    wt_object wt;
    double *inp, *out;
    // int i, J;
    int J = 1;
    double epsilon = 1e-15;

    const int totalLevels = 4;
    const int maxDecimation = pow(2, (totalLevels - 1));

    std::vector<uint8_t> fileData;
    const std::string filePath = "./tests/audio_files/CantinaBand3sec.wav";
    fileData = CLoadAudioFile(filePath);
    std::unordered_map< std::string, std::variant<int, uint32_t, double, bool> > wavDecodedData;
    std::vector< std::vector<double> > normalizedSamples;
    std::vector<double> flattenedNormalizedSamples;

    normalizedSamples = CDecodeWav<double>(wavDecodedData, fileData);
    flattenedNormalizedSamples = flatten2DVector(normalizedSamples);
    int numSamplesPerChannel = variantToType<int>(wavDecodedData["samples_per_channel"]);

    // std::vector< std::vector<double> >::const_iterator row; 
    // std::vector<double>::const_iterator col; 

    // std::cout << "LOOPING" << std::endl;
    // for (row = normalizedSamples.begin(); row != normalizedSamples.end(); ++row)
    // { 
    //      for (col = row->begin(); col != row->end(); ++col)
    //      { 
    //         std::cout << *col << " "; 
    //      } 
    // } 

    // inp = (double*)malloc(sizeof(double)* numSamplesPerChannel);
    out = (double*)malloc(sizeof(double)* numSamplesPerChannel);
    //wmean = mean(temp, N);

    // for (int i = 0; i < numSamplesPerChannel; ++i) {
    //     inp[i] = normalizedSamples[0][i];
    // }

    const char *name = "db4";
    obj = wave_init(name);// Initialize the wavelet

    // int loop = 0;
    int cDMinLen;
    size_t cALen;
    double decimatedSignalSum, decimatedSignalMean;
    std::vector<double> cD, cDSum, cDFiltered, cDDecimatedSignal, cDMeanRemovedSignal, cDMeanRemovedSignalPartial;
    std::vector<double> cA, cAFiltered, cAAbsolute, cAMeanRemovedSignal, cAMeanRemovedSignalPartial;
    for (int level = 0; level < totalLevels; level++)
    {
        // J = 1;

        /* DWT */
        if (level == 0){
            wt = wt_init(obj,(char*) "dwt", numSamplesPerChannel, J); // Initialize the wavelet transform object
            setDWTExtension(wt, (char*) "sym");
            setWTConv(wt, (char*) "direct");

            /* Input is the samples */
            inp = (double*)malloc(sizeof(double)* numSamplesPerChannel);
            for (int i = 0; i < numSamplesPerChannel; ++i) {
                inp[i] = flattenedNormalizedSamples[i];
            }
            dwt(wt, inp); // Perform DWT

            cDMinLen = wt->length[1] / maxDecimation + 1;
            cDSum.resize(cDMinLen, 0.0);

            cDMeanRemovedSignalPartial.resize(cDMinLen);
        }
        else{
            cALen = cA.size();
            wt = wt_init(obj,(char*) "dwt", cALen, J);// Initialize the wavelet transform object
            setDWTExtension(wt, (char*) "sym");
            setWTConv(wt, (char*) "direct");

            /* Input is cA */
            inp = (double*)malloc(sizeof(double)* cALen);
            for (size_t i = 0; i < cALen; ++i) {
                inp[i] = cA[i];
            }

            dwt(wt, inp);// Perform DWT
        }

        /* Fill cA */
        cA.clear();
        for (int i = 0; i < wt->length[0]; ++i) {
                cA.push_back(wt->output[i]);
        }

        /* Fill cD */
        for (int i = wt->length[1]; i < wt->outlength; ++i) {
                cD.push_back(wt->output[i]);
        }

        /* Perform One Pole filter on cD */
        cDFiltered = onePoolFilter(cD);

        /* Decimate */
        int dc = pow(2, (totalLevels - level - 1));
        for (int ax = 0; ax < cDFiltered.size(); ax += dc)
        {
            cDDecimatedSignal.push_back(std::abs(cDFiltered[ax]));
        }

        decimatedSignalSum = std::accumulate(cDDecimatedSignal.begin(), cDDecimatedSignal.end(), 0.0);
        decimatedSignalMean =  decimatedSignalSum / static_cast<double>(cDDecimatedSignal.size());

        /* Remove the mean */
        auto removeMean = [decimatedSignalMean]( const double x ) { return x - decimatedSignalMean; };
        std::transform(
               cDDecimatedSignal.begin(),
               cDDecimatedSignal.end(),
               std::back_inserter(cDMeanRemovedSignal),
               removeMean );

        /* Reconstruct */
        // cDMeanRemovedSignalPartial.resize(cDMinLen);
        std::copy_n ( cDMeanRemovedSignal.begin(), cDMinLen, cDMeanRemovedSignalPartial.begin() );
        /* Add elements of cDSum and cDMeanRemovedSignalPartial together and store into cDSum */
        std::transform ( 
                    cDSum.begin(),
                    cDSum.end(),
                    cDMeanRemovedSignalPartial.begin(),
                    cDSum.begin(),
                    std::plus<double>() );

        // cDSum = performSignalTransforms(cD, cDSum, cDFiltered, cDDecimatedSignal, cDMeanRemovedSignal, cDMeanRemovedSignalPartial, cDMinLen, totalLevels, level);
        // cDSum.clear();
        // cDSum = cDTemp;    
        // for (auto & element : cDSum) {
        //     std::cout << element << std::endl;
        // }
        // for(std::vector<double>::iterator it = cDSum.begin(); it != cDSum.begin() + 5; ++it) {
        //     // it->doSomething();
        //     std::cout << *it << std::endl;
        // }
        // copy only positive numbers:
        // std::copy_if (
        //     foo.begin(),
        //     foo.end(),
        //     std::back_inserter(bar),
        //     [](int i){return i>=0;} )
        
        // for (auto & element : cD) {
        //     // element.doSomething ();
        //     std::cout << element << std::endl;
        // }

        // for (auto & element : cDFiltered) {
        //     std::cout << element << std::endl;
        // }

        // for (i = 0; i < wt->outlength; ++i) {
        //     std::cout << wt->output[i] << " ";
        // }

        // std::cout << "RUNNING OUTPUT" << std::endl;
        // std::cout << wt->length[0] << std::endl;
        // std::cout << wt->length[1] << std::endl;
        // for (i = wt->length[1]; i < wt->outlength; ++i) {
        //         std::cout << wt->output[i] << std::endl;
        // }

        // printf("Level %d Access : output[%d] Length : %d \n", J, 0, wt->length[0]);
        // printf("\n");
        // printf("Detail Coefficients \n");
        // t = wt->length[0];
        // for (i = 0; i < J; ++i) {
        //     printf("Level %d Access : output[%d] Length : %d \n", J - i,t,wt->length[i+1]);
        //     t += wt->length[i+1];
        // }
        // printf("\n");

        // idwt(wt, out);// Perform IDWT (if needed)
        // Test Reconstruction

        // if (convolutionalMethod == "direct")
        //     epsilon = 1e-8;
        // else
        //     epsilon = 1e-10;
        
        // printf("%g ",RMS_Error(out, inp, wt->siglength));
        // if (RMS_Error(out, inp, wt->siglength) > epsilon) {
        //     std::cout << "\n ERROR : DWT Reconstruction Unit Test Failed. Exiting. \n" << std::endl;
        //     throw;
        // }

        /* Clear variables */
        wt_free(wt);
        cD.clear();
        cDFiltered.clear();
        cDDecimatedSignal.clear();
        cDMeanRemovedSignal.clear();
        cDMeanRemovedSignalPartial.clear();
        free(inp);
    }

    // for (auto & element : cA) {
    //     std::cout << element << std::endl;
    // }

    cAFiltered = onePoolFilter(cA);

    // for (auto & element : cA) {
    //     std::cout << element << std::endl;
    // }

    auto absoluteVal = [decimatedSignalMean]( const double x ) { return std::abs(x); };
    std::transform(
            cAFiltered.begin(),
            cAFiltered.end(),
            std::back_inserter(cAAbsolute),
            absoluteVal );
    
    double cAAbsoluteSum = std::accumulate(cAAbsolute.begin(), cAAbsolute.end(), 0.0);
    double cAAbsoluteMean =  cAAbsoluteSum / static_cast<double>(cAAbsolute.size());

    auto removeMean = [cAAbsoluteMean]( const double x ) { return x - cAAbsoluteMean; };
    std::transform(
            cAAbsolute.begin(),
            cAAbsolute.end(),
            std::back_inserter(cAMeanRemovedSignal),
            removeMean );
    
    // for (auto & element : cAMeanRemovedSignal) {
    //     std::cout << element << std::endl;
    // }
    

    cAMeanRemovedSignalPartial.resize(cDMinLen);
    std::copy_n ( cAMeanRemovedSignal.begin(), cDMinLen, cAMeanRemovedSignalPartial.begin() );
    /* Add elements of cDSum and cDMeanRemovedSignalPartial together and store into cDSum */
    std::transform ( 
                cDSum.begin(),
                cDSum.end(),
                cAMeanRemovedSignalPartial.begin(),
                cDSum.begin(),
                std::plus<double>() );
    
    std::vector<double> b(cDSum.size() * 2, 0.0);

    std::vector<double> reversecDSum(cDSum);
    std::reverse(reversecDSum.begin(), reversecDSum.end());
    

    // for (auto & element : cDSum) {
    //     std::cout << element << std::endl;
    // }


    fftConvolve(b, reversecDSum);

    // for (i = wt->length[0]; i < wt->outlength; ++i) {
    //         std::cout << wt->output[i] << std::endl;
    // }
    wt_summary(wt);
    wave_free(obj);
    free(out);
    // free(inp);
}
