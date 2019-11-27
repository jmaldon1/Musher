#ifndef __MUSHER_LIBRARY_H__
#define __MUSHER_LIBRARY_H__

#include <string>
#include <vector>

#include "musher_core.h"


void MUSHER_API CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> MUSHER_API CLoadAudioFile(const std::string filePath);
bool MUSHER_API CDecodeWav(const char* message);
bool MUSHER_API CAcceptDecode(const char* message, bool (*decodef)(const char*));

#endif /* __MUSHER_LIBRARY_H__ */