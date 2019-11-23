#ifndef __FUNCTIONAL_TEST_H__
#define __FUNCTIONAL_TEST_H__

#include <string>

#include "musher_core.h"

extern "C"
{
    void MUSHER_API CPrintFunctionalMessage(const char* message);
    bool MUSHER_API LoadAudioFile(const char* filePath);
    char* MUSHER_API getFileAbsPath(const char* filePath);
}
#endif