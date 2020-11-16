project_library(musher-core
    SOURCES
        utils.h
        utils.cpp
        key.h
        key.cpp
        hpcp.h
        hpcp.cpp
        framecutter.h
        framecutter.cpp
        windowing.h
        windowing.cpp
        peak_detect.h
        peak_detect.cpp
        spectral_peaks.h
        spectral_peaks.cpp
        spectrum.h
        spectrum.cpp
        mono_mixer.h
        mono_mixer.cpp
        audio_decoders.h
        audio_decoders.cpp
    DEPENDENCIES
        # CONAN
        #     functionalplus
        # OTHER
        #     wavelib
)


# TODO: Go through code and fix warnings
# relaxed_compile_options(musher-core PUBLIC)

if(ENABLE_TESTS)
    add_subdirectory(test)
endif()
