#include "Basics.h"
#include <iomanip>
#include <sstream>

#ifndef CHKDRAFT
void IgnoreErr(const std::string & file, unsigned int line, const std::string msg, ...)
{

}

void PrintError(const std::string & file, unsigned int line, const std::string msg, ...)
{

}
#endif

std::string to_zero_padded_str(int width, int number)
{
    std::stringstream ss {};
    ss << std::setfill('0') << std::setw(width) << number;
    return ss.str();
}
