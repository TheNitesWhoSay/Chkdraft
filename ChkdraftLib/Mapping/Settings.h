#ifndef SETTINGS_H
#define SETTINGS_H
#include "../CommonFiles/CommonFiles.h"
#include "../../MappingCoreLib/MappingCore.h"

std::optional<std::string> getPreSavePath(); // Gets path holding assets to be written to the map file on save

std::optional<std::string> GetLoggerPath(); // Gets the path at which logs are stored

std::optional<std::string> GetSettingsPath();

std::optional<std::string> GetToolPath(const std::string & toolName);

class Settings
{
    public:
        static std::string starCraftPath;
        static std::string starDatPath;
        static std::string brooDatPath;
        static std::string patchRtPath;
        static u32 logLevel;
        static u32 deathTableStart;
        static bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        static bool isRemastered;

        static u32 getLogLevel();

        static bool readSettingsFile();
        static bool updateSettingsFile();
        virtual ~Settings();
};

#endif