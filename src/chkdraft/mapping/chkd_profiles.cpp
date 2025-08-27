#include "chkd_profiles.h"
#include "common_files/debug.h"
#include <cross_cut/simple_icu.h>
#include <mapping_core/system_io.h>
#include <filesystem>
#include <fstream>

std::optional<std::string> GetChkdPath()
{
    char* cChkdDir = std::getenv("CHKD_DIR");
    std::string chkdDir(cChkdDir != nullptr ? cChkdDir : "");
    if ( !chkdDir.empty() )
        return chkdDir;
    else if ( auto moduleDirectory = getModuleDirectory() )
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
    if ( auto chkdPath = GetChkdPath() )
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
    if ( auto chkdPath = GetChkdPath() )
    {
        if ( makeDirectory(*chkdPath + "\\Settings\\") )
            return *chkdPath + "\\Settings\\";
    }
    return std::nullopt;
}

void ChkdProfile::fixPathsToForwardSlash()
{
    auto fixPathToForwardSlash = [](std::string & path) {
        for ( auto & c : path )
        {
            if ( c == '\\' )
                c = '/';
        }
    };
    
    fixPathToForwardSlash(remastered.starCraftPath);
    fixPathToForwardSlash(remastered.cascPath);
    for ( auto & dataFile : remastered.dataFiles )
        fixPathToForwardSlash(dataFile);
    
    fixPathToForwardSlash(classic.starCraftPath);
    fixPathToForwardSlash(classic.starDatPath);
    fixPathToForwardSlash(classic.brooDatPath);
    fixPathToForwardSlash(classic.patchRtPath);
    for ( auto & dataFile : classic.dataFiles )
        fixPathToForwardSlash(dataFile);
}

void ChkdProfiles::saveCurrProfile()
{
    current->fixPathsToForwardSlash();
    std::ofstream outFile(icux::toFilestring(current->profilePath), std::ios_base::out | std::ios_base::binary);
    outFile << Json::pretty(*current);
    if ( !outFile.good() )
        Error("Failed to save profile \"" + current->profilePath + "\"");

    outFile.close();
}

ChkdProfile* ChkdProfiles::loadProfile(const std::string & filePath)
{
    std::ifstream inFile(icux::toFilestring(filePath), std::ios_base::in | std::ios_base::binary);
    auto & chkdProfile = this->profiles.emplace_back(std::make_unique<ChkdProfile>());
    chkdProfile->profilePath = filePath;
    try {

        inFile >> Json::in(*chkdProfile);
        if ( !current && chkdProfile->isDefaultProfile )
            current = chkdProfile.get();

        return chkdProfile.get();

    } catch ( const std::exception & e ) {
        chkdProfile = nullptr;
        this->profiles.pop_back();
        Error("Failed to read profile \"" + filePath + "\": " + e.what());
    }
    return nullptr;
}

void ChkdProfiles::loadProfiles()
{
    if ( auto settingsPath = GetSettingsPath() )
    {
        std::unordered_set<std::string> visitedPaths {};
        for ( const auto & item : std::filesystem::directory_iterator{std::filesystem::path(*settingsPath)} )
        {
            auto pathString = item.path().string();
            if ( !item.is_directory() && pathString.ends_with(".profile.json") && !visitedPaths.contains(pathString) )
            {
                visitedPaths.insert(pathString);
                loadProfile(pathString);
            }
        }
        bool loadedAdditionalProfiles = false;
        do
        {
            for ( auto & profile : this->profiles )
            {
                for ( auto & additionalProfileDirectory : profile->additionalProfileDirectories )
                {
                    if ( !visitedPaths.contains(additionalProfileDirectory) )
                    {
                        visitedPaths.insert(additionalProfileDirectory);
                        for ( const auto & item : std::filesystem::directory_iterator{std::filesystem::path(additionalProfileDirectory)} )
                        {
                            auto pathString = item.path().string();
                            if ( !item.is_directory() && pathString.ends_with(".profile.json") && !visitedPaths.contains(pathString) )
                            {
                                visitedPaths.insert(pathString);
                                loadProfile(pathString);
                            }
                        }
                    }
                }
            }
        } while ( loadedAdditionalProfiles ); 
        if ( this->profiles.empty() ) // Create a profile and make it the default
        {
            auto & defaultProfile = profiles.emplace_back(std::make_unique<ChkdProfile>());
            defaultProfile->profilePath = makeSystemFilePath(*settingsPath, "Default.profile.json");
            defaultProfile->profileName = "Default";
            defaultProfile->isDefaultProfile = true;
            defaultProfile->autoLoadOnStart = true;
            this->current = this->profiles[0].get();
            this->saveCurrProfile();
        }
        else if ( this->current == nullptr ) // No default profile located, make the first profile found the default
        {
            this->current = this->profiles[0].get();
            this->current->isDefaultProfile = true;
            this->saveCurrProfile();
        }
    }
    else
        Error("Failed to get settings path!");
}

ChkdProfiles::ChkdProfiles()
{
    loadProfiles();
}
