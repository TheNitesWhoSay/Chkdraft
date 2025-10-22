#ifndef CHKDPROFILES_H
#define CHKDPROFILES_H
#include <rarecpp/reflect.h>
#include <rarecpp/json.h>
#include <chkdraft/mapping/chkd_skin.h>
#include <cross_cut/logger.h>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>

std::optional<std::string> getPreSavePath(); // Gets path holding assets to be written to the map file on save

std::optional<std::string> GetLoggerPath(); // Gets the path at which logs are stored

std::optional<std::string> GetBackupsPath(); // Gets the path at which backups are stored

std::optional<std::string> GetSettingsPath();

std::optional<std::string> GetToolPath(const std::string & toolName);

struct RemasteredSettings
{
    std::string starCraftPath = "";
    std::string cascPath = "";
    std::vector<std::string> dataFiles {};

    NOTE(defaultSkin, Json::Stringify)
    ChkdSkin defaultSkin = ChkdSkin::Classic;

    REFLECT(RemasteredSettings, starCraftPath, cascPath, dataFiles, defaultSkin)
};

struct ClassicSettings
{
    std::string starCraftPath = "";
    std::string starDatPath = "";
    std::string brooDatPath = "";
    std::string patchRtPath = "";
    std::vector<std::string> dataFiles {};

    NOTE(defaultSkin, Json::Stringify)
    ChkdSkin defaultSkin = ChkdSkin::Classic;

    REFLECT(ClassicSettings, starCraftPath, starDatPath, brooDatPath, patchRtPath, dataFiles, defaultSkin)
};

struct HistorySettings
{
    int maxMemoryUseMb = 500;
    int maxActions = 500;

    REFLECT(HistorySettings, maxMemoryUseMb, maxActions)
};

struct LoggerSettings
{
    NOTE(defaultLogLevel, Json::EnumInt)
    LogLevel defaultLogLevel = LogLevel::Info;

    REFLECT(LoggerSettings, defaultLogLevel)
};

struct TriggerSettings
{
    bool useAddressesForMemory = true;
    std::uint64_t deathTableStart = 0x58A364;

    REFLECT(TriggerSettings, useAddressesForMemory, deathTableStart)
};

struct TreeGroup
{
    std::string label;
    Json::ObjectArray subGroups;
    std::vector<std::uint32_t> items;

    // At least for now, subGroups gets parsed into this field to dodge infinite compile-time recursion issues
    std::vector<TreeGroup> parsedSubGroups;

    void parseSubGroups();
    void serializeSubGroups();

    REFLECT(TreeGroup, label, subGroups, items)
};

struct UnitSettings
{
    std::vector<TreeGroup> customTree;

    REFLECT(UnitSettings, customTree)
};

struct SpriteSettings
{
    std::vector<TreeGroup> customTree;

    REFLECT(SpriteSettings, customTree)
};

struct ChkdProfile
{
    NOTE(profilePath, Json::Ignore)
    std::string profilePath {};

    NOTE(menuId, Json::Ignore)
    std::uint64_t menuId = 0;

    std::string profileName {};
    std::vector<std::string> additionalProfileDirectories {};
    bool isDefaultProfile = false;
    bool autoLoadOnStart = false;
    bool useRemastered = true;
    RemasteredSettings remastered {};
    ClassicSettings classic {};
    HistorySettings history {};
    LoggerSettings logger {};
    TriggerSettings triggers {};
    UnitSettings units {};
    SpriteSettings sprites {};

    REFLECT(ChkdProfile, profileName, additionalProfileDirectories, isDefaultProfile, autoLoadOnStart, useRemastered,
        remastered, classic, history, logger, triggers, units, sprites)

    void fixPathsToForwardSlash();
    void saveProfile();
};

class ChkdProfiles
{
    ChkdProfile* current;

public:
    std::vector<std::unique_ptr<ChkdProfile>> profiles {};
    ChkdProfile* getCurrProfile() { return current; }
    void setCurrProfile(std::size_t index) {
        if ( index < profiles.size() )
            current = profiles[index].get();
        else
            throw std::out_of_range("Index out of range for the profiles array!");
    }

    inline ChkdProfile & operator[](const std::size_t & i) { return *(profiles[i]); } // Returns the ith profile from profiles
    inline ChkdProfile & operator()() const { return *current; } // Returns the current profile

    void saveCurrProfile();
    ChkdProfile* loadProfile(const std::string & filePath);
    void loadProfiles();
    ChkdProfiles();
    bool nameUsed(const std::string & profileName) const;
};

#endif