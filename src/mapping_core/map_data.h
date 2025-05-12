#ifndef MAPDATA_H
#define MAPDATA_H
#include "basics.h"
#include "chk.h"
#include "escape_strings.h"
#include <rarecpp/editor.h>
#include <array>
#include <optional>
#include <vector>

/*
    Versions [VER, TYPE, IVER, IVE2, VCOD] - versioning and validation
    Strings [SPRP, STR, OSTR, KSTR] - map title and description, game and editor string data
    Players [SIDE, COLR, FORC, OWNR, IOWN] - players, color, force, and slot information
    Terrain [ERA, DIM, MTXM, TILE, ISOM] - tilesets, dimensions, and terrain
    Layers:Terrain [MASK, THG2, DD2, UNIT, MRGN] - fog of war, sprites, doodads, units, and locations
    Properties [UNIS, UNIx, PUNI, UPGS, UPGx, UPGR, PUPx, TECS, TECx, PTEC, PTEx] - unit, upgrade, and technology properties
    Triggers [UPRP, UPUS, TRIG, MBRF, SWNM, WAV] - triggers, briefing, and trigger/briefing specific data
*/

enum class SaveType // The types of files a map can be saved as
{
    StarCraftScm = 0,
    HybridScm = 1,
    ExpansionScx = 2,
    RemasteredScx = 3,
    StarCraftChk = 4,
    HybridChk = 5,
    ExpansionChk = 6,
    RemasteredChk = 7,
    AllScm = 8,
    AllScx = 9,
    AllChk = 10,
    AllMaps = 11,
    AllFiles = 12,
    Unknown // Have this higher than all other SaveTypes
};

struct MapData
{
    Chk::Version version {Chk::Version::StarCraft_Hybrid};
    Chk::Type type {Chk::Type::RAWS}; // Redundant
    Chk::IVersion iVersion {Chk::IVersion::Current}; // Redundant
    Chk::I2Version i2Version {Chk::I2Version::StarCraft_1_04}; // Redundant
    Chk::VCOD validation {};

    std::vector<std::optional<ScStr>> strings {}; // STR section, Index 0 is unused
    std::vector<std::optional<ScStr>> editorStrings {}; // Index 0 is unused
    Chk::KstrVersion editorStringsVersion {Chk::KstrVersion::Current};
    Chk::OSTR editorStringOverrides {};
    Chk::SPRP scenarioProperties {};

    Chk::Race playerRaces[Sc::Player::Total] {};
    Chk::PlayerColor playerColors[Sc::Player::TotalSlots] {};
    Chk::CRGB customColors {};
    Chk::FORC forces {};

    Sc::Player::SlotType slotTypes[Sc::Player::Total] {};
    Sc::Player::SlotType iownSlotTypes[Sc::Player::Total] {}; // Redundant slot owners

    std::vector<Chk::Sprite> sprites {};
    std::vector<Chk::Doodad> doodads {};
    std::vector<Chk::Unit> units {};
    std::vector<Chk::Location> locations {};
        
    Chk::DIM dimensions {};
    Sc::Terrain::Tileset tileset {Sc::Terrain::Tileset::Badlands};
    std::vector<u16> tiles {};
    std::vector<u16> editorTiles {};
    std::vector<u8> tileFog {};
    std::vector<Chk::IsomRect> isomRects {};
        
    Chk::PUNI unitAvailability {};
    Chk::UNIx unitSettings {};
    Chk::UPGx upgradeCosts {};
    Chk::PUPx upgradeLeveling {};
    Chk::TECx techCosts {};
    Chk::PTEx techAvailability {};

    Chk::UNIS origUnitSettings {};
    Chk::UPGS origUpgradeCosts {};
    Chk::UPGR origUpgradeLeveling {};
    Chk::TECS origTechnologyCosts {};
    Chk::PTEC origTechnologyAvailability {};

    Chk::Cuwp createUnitProperties[Sc::Unit::MaxCuwps] {};
    Chk::CuwpUsed createUnitPropertiesUsed[Sc::Unit::MaxCuwps] {};
    std::vector<Chk::Trigger> triggers {};
    std::vector<Chk::Trigger> briefingTriggers {};
    u32 switchNames[Chk::TotalSwitches] {};
    u32 soundPaths[Chk::TotalSounds] {};
    std::vector<Chk::ExtendedTrigData> triggerExtensions {};
    std::vector<Chk::TriggerGroup> triggerGroupings {};

    struct Section {
        Chk::SectionName sectionName;
        std::optional<std::vector<u8>> sectionData {}; // If not present, section data is found in the fields of Scenario
        REFLECT(Section, sectionName, sectionData)
    };
    
    SaveType saveType = SaveType::HybridScm;
    std::vector<u8> strTailData {}; // Any data that comes after the regular STR section data, and after any padding
    std::vector<Section> saveSections {}; // Maintains the order of sections in the map and stores data for any sections that are not parsed
    std::array<u8, 7> tailData {}; // The 0-7 bytes just before the Scenario file ends, after the last valid section
    u8 tailLength {0}; // 0 for no tail data, must be less than 8
    bool mapIsProtected {false}; // Flagged if map is protected
    bool jumpCompress {false}; // If true, the map will attempt to compress using jump sections when saving
    
    NOTE(sprites, RareEdit::IndexSize<u32>{})
    NOTE(units, RareEdit::IndexSize<u32>{})
    REFLECT(MapData, version, type, iVersion, i2Version, validation, strings, editorStrings, editorStringOverrides, scenarioProperties,
        playerRaces, playerColors, customColors, forces, slotTypes, iownSlotTypes, sprites, doodads, units, locations,
        dimensions, tileset, tileFog, tiles, editorTiles, isomRects,
        unitAvailability, unitSettings, upgradeCosts, upgradeLeveling, techCosts, techAvailability,
        origUnitSettings, origUpgradeCosts, origUpgradeLeveling, origTechnologyCosts, origTechnologyAvailability,
        createUnitProperties, createUnitPropertiesUsed, triggers, briefingTriggers, switchNames, soundPaths, triggerExtensions, triggerGroupings,
        saveType, strTailData, saveSections, tailData, tailLength, mapIsProtected, jumpCompress)
};

enum class SoundStatus
{
    PendingMatch,
    CurrentMatch,
    VirtualFile,
    NoMatch,
    NoMatchExtended,
    FileInUse,
    Unknown
};

#endif