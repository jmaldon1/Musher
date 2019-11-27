#ifndef __UTILS_H__
#define __UTILS_H__

#include <string>
#include <vector>


std::string uint8_vector_to_hex_string(const std::vector<uint8_t>& v);
std::string get_str_between_two_squotes(const std::string &s);

#endif /* __UTILS_H__ */