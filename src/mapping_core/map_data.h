#ifndef MAPDATA_H
#define MAPDATA_H
#include "basics.h"
#include "chk.h"
#include "escape_strings.h"
#include "render/map_actor.h"
#include <nf/hist.h>
#include <array>
#include <bitset>
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
    Chk::Type mapType {Chk::Type::RAWS}; // Redundant
    Chk::IVersion iVersion {Chk::IVersion::Current}; // Redundant
    Chk::I2Version i2Version {Chk::I2Version::StarCraft_1_04}; // Redundant
    Chk::VCOD validation {};

    std::vector<std::optional<ScStr>> strings {}; // STR and/or STRx section, Index 0 is unused
    std::vector<std::optional<ScStr>> editorStrings {}; // Index 0 is unused
    Chk::KstrVersion editorStringsVersion {Chk::KstrVersion::Current};
    Chk::OSTR editorStringOverrides {};
    Chk::SPRP scenarioProperties {};

    Chk::Race playerRaces[Sc::Player::Total] {
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    };
    Chk::PlayerColor playerColors[Sc::Player::TotalSlots] {
        Chk::PlayerColor::Red, Chk::PlayerColor::Blue, Chk::PlayerColor::Teal, Chk::PlayerColor::Purple,
        Chk::PlayerColor::Orange, Chk::PlayerColor::Brown, Chk::PlayerColor::White, Chk::PlayerColor::Yellow
    };
    Chk::CRGB customColors {};
    Chk::FORC forces {};

    Sc::Player::SlotType slotTypes[Sc::Player::Total] {
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    };
    Sc::Player::SlotType iownSlotTypes[Sc::Player::Total] { // Redundant slot owners
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    };

    std::vector<Chk::Sprite> sprites {}; NOTE(sprites, nf::attach_data<MapActor>, nf::index_size<u32>) // Auto-maintains parallel-array of MapActors
    std::vector<Chk::Doodad> doodads {};
    std::vector<Chk::Unit> units {}; NOTE(units, nf::attach_data<MapActor>, nf::index_size<u32>) // Auto-maintains parallel-array of MapActors
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
    bool jumpCompress {false}; // If true, the map will attempt to compress using jump sections when saving
    
    bool hasSection(SectionName sectionName) const {
        for ( auto & section : saveSections )
        {
            if ( section.sectionName == sectionName )
                return true;
        }
        return false;
    }

    Section & addSaveSection(Section section)
    {
        if ( hasSection(section.sectionName) )
        {
            for ( std::size_t i=0; i<saveSections.size(); ++i )
            {
                if ( section.sectionName == saveSections[i].sectionName )
                    return saveSections[i];
            }
            throw std::logic_error("An internal error occured");
        }
        else
        {
            saveSections.push_back(section);
            return saveSections.back();
        }
    }

    void markUsedForceStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
        {
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( forces.forceString[i] != Chk::StringId::NoString )
                    stringIdUsed[forces.forceString[i]] = true;
            }
        }
    }

    void markUsedUnitStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
        {
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( origUnitSettings.nameStringId[i] > 0 )
                    stringIdUsed[origUnitSettings.nameStringId[i]] = true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
        {
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( unitSettings.nameStringId[i] > 0 )
                    stringIdUsed[unitSettings.nameStringId[i]] = true;
            }
        }
    }

    void markUsedTriggerStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( storageScope == Chk::Scope::Game )
        {
            if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
            {
                for ( size_t i=0; i<Chk::TotalSounds; i++ )
                {
                    if ( soundPaths[i] != Chk::StringId::UnusedSound && soundPaths[i] < Chk::MaxStrings )
                        stringIdUsed[soundPaths[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
            {
                for ( size_t i=0; i<Chk::TotalSwitches; i++ )
                {
                    if ( switchNames[i] > 0 && switchNames[i] < Chk::MaxStrings )
                        stringIdUsed[switchNames[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
            {
                for ( const auto & trigger : triggers )
                    trigger.markUsedStrings(stringIdUsed, userMask);
            }

            if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
            {            
                for ( const auto & briefingTrigger : briefingTriggers )
                    briefingTrigger.markUsedBriefingStrings(stringIdUsed, userMask);
            }
        }
        else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
        {        
            for ( const auto & extendedTrig : triggerExtensions )
            {
                if ( (userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment && extendedTrig.commentStringId != Chk::StringId::NoString )
                    stringIdUsed[extendedTrig.commentStringId] = true;

                if ( (userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes && extendedTrig.notesStringId != Chk::StringId::NoString )
                    stringIdUsed[extendedTrig.notesStringId] = true;
            }
        }
    }

    void markUsedTriggerGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
        {
            for ( const auto & trigger : triggers )
                trigger.markUsedGameStrings(stringIdUsed, userMask);
        }
        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
        {
            for ( const auto & briefingTrigger : briefingTriggers )
                briefingTrigger.markUsedBriefingStrings(stringIdUsed, userMask);
        }
    }

    void markUsedTriggerEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( storageScope == Chk::Scope::Game )
        {
            if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
            {
                for ( size_t i=0; i<Chk::TotalSounds; i++ )
                {
                    if ( soundPaths[i] != Chk::StringId::UnusedSound && soundPaths[i] < Chk::MaxStrings )
                        stringIdUsed[soundPaths[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
            {
                for ( size_t i=0; i<Chk::TotalSwitches; i++ )
                {
                    if ( switchNames[i] > 0 && switchNames[i] < Chk::MaxStrings )
                        stringIdUsed[switchNames[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction )
            {
                for ( const auto & trigger : triggers )
                    trigger.markUsedCommentStrings(stringIdUsed);
            }
        }
        else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
        {
            for ( const auto & extendedTrig : triggerExtensions )
            {
                if ( (userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment && extendedTrig.commentStringId != Chk::StringId::NoString )
                    stringIdUsed[extendedTrig.commentStringId] = true;

                if ( (userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes && extendedTrig.notesStringId != Chk::StringId::NoString )
                    stringIdUsed[extendedTrig.notesStringId] = true;
            }
        }
    }

    void markUsedLocationStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
        {
            for ( const auto & location : locations )
            {
                if ( location.stringId > 0 )
                    stringIdUsed[location.stringId] = true;
            }
        }
    }

    void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either, u32 userMask = Chk::StringUserFlag::All) const
    {
        if ( storageScope == Chk::Scope::Game )
        {
            bool markGameStrings = (usageScope & Chk::Scope::Game) == Chk::Scope::Game;
            bool markEditorStrings = (usageScope & Chk::Scope::Editor) == Chk::Scope::Editor;

            if ( markGameStrings )
            {
                // {SPRP, Game, u16}: Scenario Name and Scenario Description
                if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && scenarioProperties.scenarioNameStringId > 0 )
                    stringIdUsed[scenarioProperties.scenarioNameStringId] = true;

                if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && scenarioProperties.scenarioDescriptionStringId > 0 )
                    stringIdUsed[scenarioProperties.scenarioDescriptionStringId] = true;

                markUsedForceStrings(stringIdUsed, userMask); // {FORC, Game, u16}: Force Names
                markUsedUnitStrings(stringIdUsed, userMask); // {UNIS, Game, u16}: Unit Names (original); {UNIx, Game, u16}: Unit names (expansion)
                if ( markEditorStrings ) // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, ...
                    markUsedTriggerStrings(stringIdUsed, storageScope, userMask); // ... transmission text, next scenario, sound path, comment; {MBRF, Game, u32}: mission objectives, sound, text message
                else
                    markUsedTriggerGameStrings(stringIdUsed, userMask); // {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, transmission text, next scenario, sound path
            }

            if ( markEditorStrings )
            {
                markUsedLocationStrings(stringIdUsed, userMask); // {MRGN, Editor, u16}: location name
                if ( !markGameStrings )
                    markUsedTriggerEditorStrings(stringIdUsed, storageScope, userMask); // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: comment
            }
        }
        else if ( storageScope == Chk::Scope::Editor )
        {
            if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && editorStringOverrides.scenarioName != 0 )
                stringIdUsed[editorStringOverrides.scenarioName] = true;

            if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && editorStringOverrides.scenarioDescription != 0 )
                stringIdUsed[editorStringOverrides.scenarioDescription] = true;

            if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
            {
                for ( size_t i=0; i<Chk::TotalForces; i++ )
                {
                    if ( editorStringOverrides.forceName[i] != 0 )
                        stringIdUsed[editorStringOverrides.forceName[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
            {
                for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
                {
                    if ( editorStringOverrides.unitName[i] != 0 )
                        stringIdUsed[editorStringOverrides.unitName[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
            {
                for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
                {
                    if ( editorStringOverrides.expUnitName[i] != 0 )
                        stringIdUsed[editorStringOverrides.expUnitName[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
            {
                for ( size_t i=0; i<Chk::TotalSounds; i++ )
                {
                    if ( editorStringOverrides.soundPath[i] != 0 )
                        stringIdUsed[editorStringOverrides.soundPath[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
            {
                for ( size_t i=0; i<Chk::TotalSwitches; i++ )
                {
                    if ( editorStringOverrides.switchName[i] != 0 )
                        stringIdUsed[editorStringOverrides.switchName[i]] = true;
                }
            }

            if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
            {
                for ( size_t i=0; i<Chk::TotalLocations; i++ )
                {
                    if ( editorStringOverrides.locationName[i] != 0 )
                        stringIdUsed[editorStringOverrides.locationName[i]] = true;
                }
            }
            markUsedTriggerStrings(stringIdUsed, storageScope, userMask);
        }
    }

    REFLECT(MapData, version, mapType, iVersion, i2Version, validation, strings, editorStrings, editorStringsVersion, editorStringOverrides,
        scenarioProperties, playerRaces, playerColors, customColors, forces, slotTypes, iownSlotTypes, sprites, doodads, units, locations,
        dimensions, tileset, tileFog, tiles, editorTiles, isomRects,
        unitAvailability, unitSettings, upgradeCosts, upgradeLeveling, techCosts, techAvailability,
        origUnitSettings, origUpgradeCosts, origUpgradeLeveling, origTechnologyCosts, origTechnologyAvailability,
        createUnitProperties, createUnitPropertiesUsed, triggers, briefingTriggers, switchNames, soundPaths, triggerExtensions, triggerGroupings,
        saveType, strTailData, saveSections, tailData, tailLength, jumpCompress)
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