#ifndef __FUNCTIONAL_TEST_H__
#define __FUNCTIONAL_TEST_H__

#include <string>
#include <vector>

#include "musher_core.h"

// extern "C"
// {
void MUSHER_API CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> LoadAudioFile(const char* filePath, bool (*decodeFunc)(const char*)=0);
bool MUSHER_API DecodeWav(const char* message);
bool MUSHER_API AcceptDecode(const char* message, bool (*decodef)(const char*));
// }
#endif