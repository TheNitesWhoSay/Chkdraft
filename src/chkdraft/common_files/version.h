#ifndef VERSION_H
#define VERSION_H
#include <cstdint>
#include <string>
using u16 = uint16_t;

// Major version is a variable length base-10 number
constexpr u16 MajorVersionNumber = 1;

// Minor version is a 2-digit base-10 number
constexpr u16 MinorVersionNumber = 8;

u16 GetDateShort(); // Returns 384*(year-2000) + 32*month + day based on the date compiled

u16 GetTimeShort(); // Returns (3600*hour + 60*minute + second) based on the time compiled

std::string GetShortVersionString(); // Returns the major version number followed by a dot, followed by a two-digit minor version

std::string GetFullVersionString(); // Returns: major.minor.dateShort.timeShort

#endif