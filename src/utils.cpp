#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include <type_traits>
#include <complex>
#include <valarray>

#include "utils.h"

namespace musher
{
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
    std::string strBetweenSQuotes(const std::string &s)
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
        
        if (!isBigEndian())
            result = (source[startIndex + 1] << 8) | source[startIndex];
        else
            result = (source[startIndex] << 8) | source[startIndex + 1];
        
        return result;
    }


    int32_t fourBytesToInt(const std::vector<uint8_t>& source, const int startIndex)
    {
        int32_t result;
        
        if (!isBigEndian())
            result = (source[startIndex + 3] << 24) | (source[startIndex + 2] << 16) | (source[startIndex + 1] << 8) | source[startIndex];
        else
            result = (source[startIndex] << 24) | (source[startIndex + 1] << 16) | (source[startIndex + 2] << 8) | source[startIndex + 3];
        
        return result;
    }


    bool isBigEndian(void)
    {
        union {
            uint32_t i;
            char c[4];
        } bint = {0x01020304};

        return bint.c[0] == 1; 
    }

    const double PI = 3.141592653589793238460;

    // void fft(CArray &x)
    void fft(std::vector<Complex> &x)
    {
        // DFT
        unsigned int N = x.size(), k = N, n;
        double thetaT = 3.14159265358979323846264338328L / N;
        Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
        while (k > 1)
        {
            n = k;
            k >>= 1;
            phiT = phiT * phiT;
            T = 1.0L;
            for (unsigned int l = 0; l < k; l++)
            {
                for (unsigned int a = l; a < N; a += n)
                {
                    unsigned int b = a + k;
                    Complex t = x[a] - x[b];
                    x[a] += x[b];
                    x[b] = t * T;
                }
                T *= phiT;
            }
        }
        // Decimate
        unsigned int m = (unsigned int)log2(N);
        for (unsigned int a = 0; a < N; a++)
        {
            unsigned int b = a;
            // Reverse bits
            b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
            b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
            b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
            b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
            b = ((b >> 16) | (b << 16)) >> (32 - m);
            if (b > a)
            {
                Complex t = x[a];
                x[a] = x[b];
                x[b] = t;
            }
        }
        //// Normalize (This section make it not working correctly)
        //Complex f = 1.0 / sqrt(N);
        //for (unsigned int i = 0; i < N; i++)
        //	x[i] *= f;
    }
}