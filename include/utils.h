#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>


std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v);
std::string get_str_between_two_squotes(const std::string &s);
auto unint8_t_to_int = [](uint8_t num) { return static_cast<int>(num); };

#endif /* __UTILS_H__ */