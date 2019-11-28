#ifndef __MUSHER_LIBRARY_H__
#define __MUSHER_LIBRARY_H__

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>

#include "musher_core.h"


void MUSHER_API CPrintFunctionalMessage(const char* message);
std::vector<uint8_t> MUSHER_API CLoadAudioFile(const std::string& filePath);
bool MUSHER_API CAcceptDecode(const char* message, bool (*decodef)(const char*));
// std::unordered_map<std::string, std::variant<int, uint32_t, double, bool>> 
template <class UnorderedMapIterator>
void MUSHER_API CDecodeAudio(UnorderedMapIterator first, const std::string& filePath=std::string(), const std::vector<uint8_t>& fileData=std::vector<uint8_t>());
template <class UnorderedMapIterator>
void MUSHER_API CDecodeWav(UnorderedMapIterator first, const std::vector<uint8_t>& fileData);

#endif /* __MUSHER_LIBRARY_H__ */