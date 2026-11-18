#include "chkd_profiles.h"
#include "common_files/debug.h"
#include <cross_cut/simple_icu.h>
#include <mapping_core/sc.h>
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

    for ( auto & additionalProfileDirectory : additionalProfileDirectories )
        fixPathToForwardSlash(additionalProfileDirectory);
    
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

void ChkdProfile::saveProfile()
{
    units.parsedCustomTree.serialize(units.customTree);
    sprites.parsedCustomTree.serialize(sprites.customTree);

    fixPathsToForwardSlash();
    std::ofstream outFile(icux::toFilestring(this->profilePath), std::ios_base::out | std::ios_base::binary);
    outFile << Json::pretty(*this);
    if ( !outFile.good() )
        Error("Failed to save profile \"" + this->profilePath + "\"");

    outFile.close();
}

void ChkdProfiles::saveCurrProfile()
{
    if ( current != nullptr )
        current->saveProfile();
    else
        throw std::logic_error("Cannot saveCurrProfile when there is no current profile!");
}

ChkdProfile* ChkdProfiles::loadProfile(const std::string & filePath)
{
    std::ifstream inFile(icux::toFilestring(filePath), std::ios_base::in | std::ios_base::binary);
    auto & chkdProfile = this->profiles.emplace_back(std::make_unique<ChkdProfile>());
    chkdProfile->profilePath = filePath;
    try {

        inFile >> Json::in<std::tuple<Json::OrderObjectsType>>(*chkdProfile);
        for ( auto & existingProfile : profiles )
        {
            if ( chkdProfile.get() != existingProfile.get() &&
                 (std::filesystem::path(chkdProfile->profilePath) == std::filesystem::path(existingProfile->profilePath) ||
                 chkdProfile->profileName == existingProfile->profileName) )
            {
                chkdProfile = nullptr;
                this->profiles.pop_back();
                return nullptr;
            }
        }
        if ( !current && chkdProfile->isDefaultProfile )
            current = chkdProfile.get();
        
        chkdProfile->units.parsedCustomTree.parse(chkdProfile->units.customTree);
        chkdProfile->sprites.parsedCustomTree.parse(chkdProfile->sprites.customTree);

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
    current = nullptr;
    profiles.clear();
    if ( auto settingsPath = GetSettingsPath() )
    {
        std::unordered_set<std::string> visitedPaths {};
        for ( const auto & item : std::filesystem::directory_iterator{std::filesystem::path(*settingsPath)} )
        {
            auto pathString = item.path().string();
            if ( !item.is_directory() && pathString.ends_with(".profile.json") && !visitedPaths.contains(fixSystemPathSeparators(pathString)) )
            {
                visitedPaths.insert(fixSystemPathSeparators(pathString));
                loadProfile(pathString);
            }
        }

        bool loadedAdditionalProfiles = false;
        do
        {
            for ( std::size_t i=0; i<this->profiles.size(); ++i )
            {
                auto & profile = this->profiles[i];
                for ( auto & additionalProfileDirectory : profile->additionalProfileDirectories )
                {
                    if ( !visitedPaths.contains(fixSystemPathSeparators(additionalProfileDirectory)) )
                    {
                        visitedPaths.insert(fixSystemPathSeparators(additionalProfileDirectory));
                        for ( const auto & item : std::filesystem::directory_iterator{std::filesystem::path(additionalProfileDirectory)} )
                        {
                            auto pathString = item.path().string();
                            if ( !item.is_directory() && pathString.ends_with(".profile.json") && !visitedPaths.contains(fixSystemPathSeparators(pathString)) )
                            {
                                visitedPaths.insert(fixSystemPathSeparators(pathString));
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
            std::filesystem::current_path(getSystemFileDirectory(this->current->profilePath));
            this->saveCurrProfile();
        }
        else if ( this->current == nullptr ) // No default profile located, make the first profile found the default
        {
            this->current = this->profiles[0].get();
            this->current->isDefaultProfile = true;
            std::filesystem::current_path(getSystemFileDirectory(this->current->profilePath));
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

bool ChkdProfiles::nameUsed(const std::string & profileName) const
{
    for ( auto & profile : profiles )
    {
        if ( profile->profileName == profileName )
            return true;
    }
    return false;
}

bool ChkdProfiles::addRecentMap(const std::string & recentMap)
{
    bool changed = false;
    ChkdProfile* profile = getCurrProfile();
    if ( profile != nullptr )
    {
        std::vector<std::string> & recentMapPaths = profile->recentMaps;
        bool hasExistingPaths = !recentMapPaths.empty();
        if ( recentMapPaths.empty() )
        {
            recentMapPaths.push_back(recentMap);
            changed = true;
        }
        else
        {
            auto existingMatch = std::find(recentMapPaths.begin(), recentMapPaths.end(), recentMap);
            if ( existingMatch == recentMapPaths.end() ) // Not already present
            {
                recentMapPaths.insert(recentMapPaths.begin(), recentMap); // Add at the beginning
                changed = true;
            }
            else if ( existingMatch != recentMapPaths.begin() ) // Already present but not already at the beginning
            {
                recentMapPaths.erase(existingMatch); // Remove from wherever it is
                recentMapPaths.insert(recentMapPaths.begin(), recentMap); // Add at the beginning
                changed = true;
            }
            if ( recentMapPaths.size() > 20 )
            {
                recentMapPaths.resize(20);
                changed = true;
            }
        }
    }
    if ( changed )
        this->saveCurrProfile();

    return changed;
}

const std::vector<std::string> & ChkdProfiles::getRecentMaps()
{
    ChkdProfile* profile = getCurrProfile();
    if ( profile != nullptr )
        return profile->recentMaps;
    else
    {
        static std::vector<std::string> empty {};
        return empty;
    }
}

std::optional<std::string> ChkdProfiles::getRecentMap(std::size_t index)
{
    ChkdProfile* profile = getCurrProfile();
    if ( profile != nullptr && index < profile->recentMaps.size() )
        return profile->recentMaps[index];
    else
        return std::nullopt;
}

void ChkdProfiles::clearRecentMaps()
{
    ChkdProfile* profile = getCurrProfile();
    if ( profile != nullptr )
    {
        profile->recentMaps.clear();
        this->saveCurrProfile();
    }
}
