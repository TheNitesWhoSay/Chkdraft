#ifndef CHKDSKIN_H
#define CHKDSKIN_H
#include <cctype>
#include <cstddef>
#include <iostream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <unordered_map>

enum class ChkdSkin
{
    ClassicGDI,
    Classic,
    RemasteredSD,
    RemasteredHD2,
    RemasteredHD,
    CarbotHD2,
    CarbotHD,
    Total,
    Unknown
};

inline std::string_view skinStrings[] {
    "ClassicGDI",
    "Classic",
    "RemasteredSD",
    "RemasteredHD2",
    "RemasteredHD",
    "CarbotHD2",
    "CarbotHD"
};

inline std::string_view skinUserText[] {
    "Classic GDI",
    "Classic OpenGL",
    "Remastered SD",
    "Remastered HD2",
    "Remastered HD",
    "Carbot HD2",
    "Carbot HD"
};

inline std::ostream & operator<<(std::ostream & os, const ChkdSkin & skin)
{
    if ( int(skin) >= 0 && int(skin) < int(ChkdSkin::Total) )
        os << skinStrings[int(skin)];
    else
        os << skinStrings[int(ChkdSkin::Classic)];

    return os;
}

inline std::istream & operator>>(std::istream & is, ChkdSkin & skin)
{
    static auto stringToSkin = [](){
        std::unordered_map<std::string, ChkdSkin> buildStringToSkin {};
        buildStringToSkin.reserve(std::size_t(ChkdSkin::Total));
        for ( int i=0; i<int(ChkdSkin::Total); ++i )
        {
            std::string skinStr = std::string(skinStrings[i]);
            for ( auto & c : skinStr )
                c = std::tolower(c);

            buildStringToSkin.insert(std::make_pair(skinStr, ChkdSkin(i)));
        }
        return buildStringToSkin;
    }();

    if ( !is.good() )
    {
        skin = ChkdSkin::Unknown;
        return is;
    }

    std::string inputString {};
    is >> inputString;
    if ( is.fail() )
        skin = ChkdSkin::Unknown;
    else
    {
        for ( auto & c : inputString )
            c = std::tolower(c);

        auto found = stringToSkin.find(inputString);
        if ( found != stringToSkin.end() )
            skin = found->second;
        else
            skin = ChkdSkin::Unknown;
    }
    return is;
}

inline std::string_view getSkinUserText(ChkdSkin skin)
{
    if ( int(skin) >= 0 && int(skin) < int(ChkdSkin::Total) )
        return skinUserText[int(skin)];
    else
        return skinUserText[int(ChkdSkin::Classic)];
}

#endif