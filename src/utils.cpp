#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>


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