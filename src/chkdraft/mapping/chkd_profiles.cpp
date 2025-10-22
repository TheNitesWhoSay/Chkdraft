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

void parseTreeGroup(std::vector<TreeGroup> & treeGroups, const std::vector<Json::ObjectValue> & rawTreeGroups)
{
    treeGroups.assign(rawTreeGroups.size(), TreeGroup{});
    std::size_t i = 0;
    for ( const Json::ObjectValue & rawTreeGroup : rawTreeGroups )
    {
        auto labelIt = rawTreeGroup.find(RareTs::MemberType<TreeGroup>::label::name);
        if ( labelIt != rawTreeGroup.end() )
        {
            const std::shared_ptr<Json::Value> & label = labelIt->second;
            if ( label != nullptr && label->type() == Json::Value::Type::String )
                treeGroups[i].label = label->string();
        }
        auto subGroupsIt = rawTreeGroup.find(RareTs::MemberType<TreeGroup>::subGroups::name);
        if ( subGroupsIt != rawTreeGroup.end() )
        {
            const std::shared_ptr<Json::Value> & subGroups = subGroupsIt->second;
            if ( subGroups != nullptr && subGroups->type() == Json::Value::Type::ObjectArray )
                parseTreeGroup(treeGroups[i].parsedSubGroups, subGroups->objectArray());
        }
        auto itemsIt = rawTreeGroup.find(RareTs::MemberType<TreeGroup>::items::name);
        if ( itemsIt != rawTreeGroup.end() )
        {
            const std::shared_ptr<Json::Value> & items = itemsIt->second;
            if ( items != nullptr && items->type() == Json::Value::Type::NumberArray )
            {
                const auto & numberArray = items->numberArray();
                std::size_t count = numberArray.size();
                for ( const auto & number : numberArray )
                    treeGroups[i].items.push_back(std::stoi(number));
            }
        }
        ++i;
    }
}

void TreeGroup::parseSubGroups()
{
    this->parsedSubGroups.clear();
    parseTreeGroup(this->parsedSubGroups, this->subGroups.objectArray());
}

void serializeSubGroup(std::vector<Json::ObjectValue> & serializedTreeGroups, const std::vector<TreeGroup> & treeGroups)
{
    serializedTreeGroups.assign(treeGroups.size(), Json::ObjectValue{});
    std::size_t i = 0;
    for ( const TreeGroup & treeGroup : treeGroups )
    {
        std::shared_ptr<Json::String> label = std::make_shared<Json::String>(treeGroup.label);
        std::shared_ptr<Json::ObjectArray> subGroups = std::make_shared<Json::ObjectArray>();
        serializeSubGroup(subGroups->objectArray(), treeGroup.parsedSubGroups);
        std::shared_ptr<Json::NumberArray> items = std::make_shared<Json::NumberArray>();
        auto & numberArray = items->numberArray();
        numberArray.reserve(treeGroup.items.size());
        for ( auto & number : treeGroup.items )
            numberArray.push_back(std::to_string(number));

        serializedTreeGroups[i].insert(std::make_pair<std::string, std::shared_ptr<Json::Value>>(
            RareTs::MemberType<TreeGroup>::label::name, std::dynamic_pointer_cast<Json::Value>(label)));
        serializedTreeGroups[i].insert(std::make_pair<std::string, std::shared_ptr<Json::Value>>(
            RareTs::MemberType<TreeGroup>::subGroups::name, std::dynamic_pointer_cast<Json::Value>(subGroups)));
        serializedTreeGroups[i].insert(std::make_pair<std::string, std::shared_ptr<Json::Value>>(
            RareTs::MemberType<TreeGroup>::items::name, std::dynamic_pointer_cast<Json::Value>(items)));

        ++i;
    }
}

void TreeGroup::serializeSubGroups()
{
    this->subGroups.objectArray().clear();
    serializeSubGroup(this->subGroups.objectArray(), this->parsedSubGroups);
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
    for ( auto & group : units.customTree )
        group.serializeSubGroups();

    for ( auto & group : sprites.customTree )
        group.serializeSubGroups();

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

        inFile >> Json::in(*chkdProfile);
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
        
        for ( auto & group : chkdProfile->units.customTree )
            group.parseSubGroups();

        for ( auto & group : chkdProfile->sprites.customTree )
            group.parseSubGroups();

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
