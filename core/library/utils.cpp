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

    size_t next_fast_len(size_t n)
    {
        /* 
        Copied from Peter Bell. 
        https://gdoc.pub/doc/e/2PACX-1vR6iXXG1uS9ds47GvDgQk6XtpYzVTtYepu5B8onBrMmoorfKHhnHbN0ArDoXgoA23nZrcrm_DSFMW45 
        */
        if (n<=6) return n;

        size_t bestfac=2*n;
        for (size_t f5=1; f5<bestfac; f5*=5)
        {
            size_t x = f5;
            while (x<n) x *= 2;
                for (;;)
                    {
                        if (x<n)
                            x*=3;
                        else if (x>n)
                        {
                            if (x<bestfac) bestfac=x;
                            if (x&1) break;
                            x>>=1;
                        }
                        else
                            return n;
                    }
        }
        return bestfac;
    }
    
    std::tuple< double , double > quadraticInterpolation(double a, double b, double y, int middle_point_index)
    {
        /* 
        Smith, J.O. "Quadratic Interpolation of Spectral Peaks", in 
        Spectral Audio Signal Processing,
        https://ccrma.stanford.edu/~jos/sasp/Quadratic_Interpolation_Spectral_Peaks.html, online book, 
        2011 edition,
        accessed 12/18/2019.

        α(a) = left point value of parabola
        β(b) = middle point value of parabola
        γ(y) = right point value of parabola

        Interpolated peak location is given in bins (spectral samples) by:
        p = 1/2 ((α - γ) / (α - 2β + γ))

        The peak magnitude estimate is:
        y(p) = β - 1/4(α - γ)p
        */

       double p = 0.5 * ((a - y) / (a - 2 * b + y));
       double peak_location = static_cast<double>(middle_point_index) + p;
       double peak_height_estimate = b - 0.25 * (a - y) * p;
       return std::make_tuple(peak_location, peak_height_estimate);
    }
}