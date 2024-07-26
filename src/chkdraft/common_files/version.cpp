#include "Version.h"

u16 GetDateShort()
{
    const std::string sDate(__DATE__); // "Mmm dd yyyy"

    u16 month = 0;
    const std::string sMonth(sDate.substr(0, 3)); // mmm
    const std::string sMonths[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
    for ( u16 i = 0; i < 12; i++ )
    {
        if ( sMonth.compare(sMonths[i]) == 0 )
        {
            month = i;
            break;
        }
    }

    const u16 day = (u16)std::stoi(sDate.substr(4, 2)); // dd
    const u16 year = (u16)std::stoi(sDate.substr(7, 4)); // yyyy

    return 384 * (year - 2000) + 32 * month + day;
}

u16 GetTimeShort()
{
    const std::string sTime(__TIME__); // "hh:mm:ss"

    const u16 hour = (u16)std::stoi(sTime.substr(0, 2));
    const u16 minute = (u16)std::stoi(sTime.substr(3, 2));
    const u16 second = (u16)std::stoi(sTime.substr(6, 2));

    return 3600 * hour + 60 * minute + second;
}

std::string GetShortVersionString()
{
    return
        std::to_string(MajorVersionNumber) + "." +
        (MinorVersionNumber < 10 ? std::string("0") + std::to_string(MinorVersionNumber) : std::to_string(MinorVersionNumber));
}

std::string GetFullVersionString()
{
    return
        std::to_string(MajorVersionNumber) + "." +
        (MinorVersionNumber < 10 ? std::string("0") + std::to_string(MinorVersionNumber) : std::to_string(MinorVersionNumber)) + "." +
        std::to_string(GetDateShort()) + "." +
        std::to_string(GetTimeShort());
}
// Returns: major.minor.dateAppendage.timeAppendage
