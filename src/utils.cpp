#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>

#include "utils.h"

/**
 * @brief Convert uint8_t vector to hex string
 * @details Convert uint8_t vector to hex string
 * 
 * @param uint8_t::vector vector of uint8_t
 * @return std::string string of hex
 */
std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v) {
    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    std::vector<uint8_t>::const_iterator it;

    for (it = v.begin(); it != v.end(); it++) {
        ss << std::setw(2) << static_cast<unsigned>(*it);
    }

    return ss.str();
}

/**
 * @brief Get string between two single quotes
 * @details Get string between two single quotes
 * 
 * @param s String that contains 2 single quotes
 * @return string between single quotes
 */
std::string get_str_between_two_squotes(const std::string &s)
{
    std::string squote = "'";
    // Find 1st quote starting from beginning of string
    std::size_t pos = s.find(squote) + 1;
    // Find 2nd quote starting from end of string
	std::size_t endpos = s.rfind(squote);
    // Find length of string between single quotes
	std::size_t len = endpos - pos;

    // Return the substring between single quotes
    return s.substr(pos, len);
}


int16_t twoBytesToInt(const std::vector<uint8_t>& source, const int startIndex)
{
    int16_t result;
    
    if (!is_big_endian())
        result = (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 8) | source[startIndex + 1];
    
    return result;
}


int32_t fourBytesToInt(const std::vector<uint8_t>& source, const int startIndex)
{
    int32_t result;
    
    if (!is_big_endian())
        result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
    else
        result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) | source[startIndex + 3];
    
    return result;
}


bool is_big_endian(void)
{
    union {
        uint32_t i;
        char c[4];
    } bint = {0x01020304};

    return bint.c[0] == 1; 
}
