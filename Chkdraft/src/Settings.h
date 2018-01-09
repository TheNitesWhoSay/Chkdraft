#ifndef SETTINGS_H
#define SETTINGS_H
#include "CommonFiles/CommonFiles.h"
#include "MappingCore/MappingCore.h"

bool GetPreSavePath(std::string &outPreSavePath); // Gets path holding assets to be written to the map file on save

bool GetLoggerPath(std::string &outLoggerPath); // Gets the path at which logs are stored

bool GetSettingsPath(std::string &outFilePath);

class Settings
{
    public:
        static std::string starDatPath;
        static std::string brooDatPath;
        static std::string patchRtPath;
        static u32 deathTableStart;
        static bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions

        static bool readSettingsFile();
        static bool updateSettingsFile();
};

#endif