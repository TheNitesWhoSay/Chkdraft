#include "Settings.h"

std::string Settings::starDatPath("");
std::string Settings::brooDatPath("");
std::string Settings::patchRtPath("");
u32 Settings::deathTableStart(0x0058A364);
bool Settings::useAddressesForMemory(true);

bool ParseLong(const char* text, u32& dest, u32 pos, u32 end)
{
    int size = end - pos;
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

bool GetChkdPath(std::string &outChkdPath)
{
    std::string moduleDirectory;
    if ( GetModuleDirectory(moduleDirectory) )
    {
        MakeDirectory(moduleDirectory + "\\chkd");
        outChkdPath = moduleDirectory + std::string("\\chkd");
        return true;
    }
    return false;
}

bool GetPreSavePath(std::string &outPreSavePath)
{
    std::string chkdPath;
    if ( GetChkdPath(chkdPath) )
    {
        MakeDirectory(chkdPath + "\\Pre-Save");
        outPreSavePath = chkdPath + std::string("\\Pre-Save\\");
        return true;
    }
    return false;
}

bool GetLoggerPath(std::string &outLoggerPath)
{
    std::string chkdPath;
    if ( GetChkdPath(chkdPath) )
    {
        MakeDirectory(chkdPath + "\\Logs");
        outLoggerPath = chkdPath + std::string("\\Logs\\");
        return true;
    }
    return false;
}

bool GetSettingsPath(std::string &outFilePath)
{
    std::string moduleDirectory;
    if ( GetModuleDirectory(moduleDirectory) )
    {
        MakeDirectory(moduleDirectory + "\\chkd");
        MakeDirectory(moduleDirectory + "\\chkd\\Settings");
        outFilePath = moduleDirectory + std::string("\\chkd\\Settings\\");
        return true;
    }
    return false;
}

bool Settings::readSettingsFile()
{
    bool success = false;
    std::string settingsPath = "";
    if ( GetSettingsPath(settingsPath) )
    {
        std::ifstream loadFile;
        loadFile.open(settingsPath + "settings.ini");

        std::string line = "";
        while (std::getline(loadFile, line))
        {
            size_t equalsPos = line.find_first_of('=');
            if ( equalsPos > 0 && equalsPos+1 < line.length() )
            {
                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos+1, line.length()-1);

                bool foundValue = true;
                if ( key == "starDatPath" )
                    starDatPath = value;
                else if ( key == "brooDatPath" )
                    brooDatPath = value;
                else if ( key == "patchRtPath" )
                    patchRtPath = value;
                else if ( key == "deathTableStart" )
                {
                    u32 temp = 0;
                    if ( ParseLong(value.c_str(), temp, 0, value.length()) )
                        deathTableStart = temp;
                }
                else if ( key == "useAddressesForMemory" )
                {
                    for (auto &character: value)
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
    std::string settingsPath = "";
    if ( GetSettingsPath(settingsPath) )
    {
        std::ofstream loadFile;
        loadFile.open(settingsPath + "settings.ini");
        loadFile << "starDatPath=" << starDatPath << std::endl
            << "brooDatPath=" << brooDatPath << std::endl
            << "patchRtPath=" << patchRtPath << std::endl
            << "deathTableStart=0x" << std::hex << std::uppercase << deathTableStart << std::dec << std::nouppercase << std::endl
            << "useAddressesForMemory=" << (useAddressesForMemory?"TRUE":"FALSE") << std::endl;
        loadFile.close();
        return true;
    }
    return false;
}
