#include "Settings.h"
#include "../../CrossCutLib/Logger.h"
#include <fstream>

std::string Settings::starCraftPath("");
std::string Settings::starDatPath("");
std::string Settings::brooDatPath("");
std::string Settings::patchRtPath("");
u32 Settings::logLevel(LogLevel::Info);
u32 Settings::deathTableStart(Sc::Address::Patch_1_16_1::DeathTable);
bool Settings::useAddressesForMemory(true);
bool Settings::isRemastered(false);

bool ParseLong(const std::string & text, u32 & dest, size_t pos, size_t end)
{
    size_t size = end - pos;
    if ( size < 12 )
    {
        if ( size == 1 && text[pos] == '0' )
        {
            dest = 0;
            return true;
        }
        else if ( size > 2 && text[pos] == '0' && (text[pos + 1] == 'x' || text[pos + 1] == 'X') )
        {
            char potentialLong[12] = {};
            std::memcpy(potentialLong, &text[pos + 2], size - 2);
            potentialLong[size - 2] = '\0';
            try {
                dest = (u32)std::stoll(potentialLong, nullptr, 16);
                return true;
            }
            catch (std::exception e) {}
        }
        else
        {
            char potentialLong[12] = { };
            std::memcpy(potentialLong, &text[pos], size);
            potentialLong[size] = '\0';
            return ( (dest = (u32)std::atoll(potentialLong)) > 0 );
        }
    }
    return false;
}

std::optional<std::string> GetChkdPath()
{
    if ( auto moduleDirectory = getModuleDirectory() )
    {
        if ( makeDirectory(*moduleDirectory + "\\chkd") )
            return *moduleDirectory + "\\chkd";
    }
    return std::nullopt;
}

std::optional<std::string> getPreSavePath()
{
    if ( auto chkdPath = GetChkdPath() )
    {
        if ( makeDirectory(*chkdPath + "\\Pre-Save") )
            return *chkdPath + "\\Pre-Save\\";
    }
    return std::nullopt;
}

std::optional<std::string> GetLoggerPath()
{
    if ( auto chkdPath = GetChkdPath() )
    {
        if ( makeDirectory(*chkdPath + "\\Logs") )
            return *chkdPath + "\\Logs\\";
    }
    return std::nullopt;
}

std::optional<std::string> GetBackupsPath()
{
    if (auto chkdPath = GetChkdPath())
    {
        if ( makeDirectory(*chkdPath + "\\Backups") )
            return *chkdPath + "\\Backups\\";
    }
    return std::nullopt;
}

std::optional<std::string> GetToolsPath()
{
    if ( auto chkdPath = GetChkdPath() )
    {
        if ( makeDirectory(*chkdPath + "\\Tools") )
            return *chkdPath + "\\Tools\\";
    }
    return std::nullopt;
}

std::optional<std::string> GetToolPath(const std::string & toolName)
{
    if ( auto toolsPath = GetToolsPath() )
    {
        if ( makeDirectory(*toolsPath + toolName) )
            return *toolsPath + toolName + "\\";
    }
    return std::nullopt;
}

std::optional<std::string> GetSettingsPath()
{
    if ( auto moduleDirectory = getModuleDirectory() )
    {
        if ( makeDirectory(*moduleDirectory + "\\chkd") && makeDirectory(*moduleDirectory + "\\chkd\\Settings") )
            return *moduleDirectory + "\\chkd\\Settings\\";
    }
    return std::nullopt;
}

u32 Settings::getLogLevel()
{
    readSettingsFile();
    return logLevel;
}

bool Settings::readSettingsFile()
{
    bool success = false;
    if ( auto settingsPath = GetSettingsPath() )
    {
        std::ifstream loadFile;
        loadFile.open(*settingsPath + "settings.ini");

        std::string line = "";
        while (std::getline(loadFile, line))
        {
            size_t equalsPos = line.find_first_of('=');
            if ( equalsPos > 0 && equalsPos+1 < line.length() )
            {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos+1, line.length()-1);

                bool foundValue = true;
                if ( key == "starCraftPath")
                    starCraftPath = value;
                if ( key == "starDatPath" )
                    starDatPath = value;
                else if ( key == "brooDatPath" )
                    brooDatPath = value;
                else if ( key == "patchRtPath" )
                    patchRtPath = value;
                else if ( key == "logLevel" )
                {
                    u32 temp = 0;
                    if ( ParseLong(value, temp, 0, value.length()) )
                        logLevel = temp;
                }
                else if ( key == "deathTableStart" )
                {
                    u32 temp = 0;
                    if ( ParseLong(value, temp, 0, value.length()) )
                        deathTableStart = temp;
                }
                else if ( key == "useAddressesForMemory" )
                {
                    for ( auto & character : value )
                        character = toupper(character);
                    if ( value == "FALSE" || value == "0" )
                        useAddressesForMemory = false;
                    else
                        useAddressesForMemory = true;
                }
                else if ( key == "isRemastered" )
                {
                    for ( auto & character : value )
                        character = toupper(character);
                    if ( value == "FALSE" || value == "0" )
                        useAddressesForMemory = false;
                    else
                        useAddressesForMemory = true;
                }
                else
                    foundValue = false;

                if ( foundValue )
                    success = true;
            }
        }
    }
    return success;
}

bool Settings::updateSettingsFile()
{
    if ( auto settingsPath = GetSettingsPath() )
    {
        std::ofstream loadFile;
        loadFile.open(*settingsPath + "settings.ini");
        loadFile
            << "starCraftPath=" << starCraftPath << std::endl
            << "starDatPath=" << starDatPath << std::endl
            << "brooDatPath=" << brooDatPath << std::endl
            << "patchRtPath=" << patchRtPath << std::endl
            << "logLevel=" << logLevel << std::endl
            << "deathTableStart=0x" << std::hex << std::uppercase << deathTableStart << std::dec << std::nouppercase << std::endl
            << "useAddressesForMemory=" << (useAddressesForMemory?"TRUE":"FALSE") << std::endl
            << "isRemastered=" << (isRemastered?"TRUE":"FALSE") << std::endl;
        loadFile.close();
        return true;
    }
    return false;
}

Settings::~Settings()
{

}
