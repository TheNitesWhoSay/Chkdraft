#ifndef SCENARIO_H
#define SCENARIO_H
#include "Buffer.h"
#include "Basics.h"
#include "EscapeStrings.h"
#include "Sections.h"
#include <cstdio>
#include <list>
#include <array>
#include <memory>
#include <string>
#include <map>

/*
    This section defines the scenario file, the API for editing the scenario is split into several groupings...

    Versions [VER, TYPE, IVER, IVE2, VCOD] - versioning and validation
    Strings [SPRP, STR, OSTR, KSTR] - strings (uses Versions - to sometimes auto-determine unis/unix section for unit names, Players - for force names, Layers - location names, Properties - unit names, and Triggers - many strings)
    Players [SIDE, COLR, FORC, OWNR, IOWN] - player, color, force, and slot information (uses Strings - for force names)
    Terrain [ERA, DIM, MTXM, TILE, ISOM] - tilesets, dimensions, and terrain
    Layers:Terrain [MASK, THG2, DD2, UNIT, MRGN] - fog of war, sprites, doodads, units, and locations (uses Strings - for location names)
    Properties [UNIS, UNIx, PUNI, UPGS, UPGx, UPGR, PUPx, TECS, TECx, PTEC, PTEx] - unit, upgrade, and technology properties (uses Versions - to sometimes auto-determine original/expansion section, Strings - for unit names)
    Triggers [UPRP, UPUS, TRIG, MBRF, SWNM, WAV] - triggers, briefing, and trigger/briefing specific data - (uses Layers - for locations, Strings - for many strings)
*/

class Versions; class Strings; class Players; class Terrain; class Layers; class Properties; class Triggers;
typedef std::shared_ptr<Versions> VersionsPtr; typedef std::shared_ptr<Strings> StringsPtr; typedef std::shared_ptr<Players> PlayersPtr; typedef std::shared_ptr<Terrain> TerrainPtr;
typedef std::shared_ptr<Layers> LayersPtr; typedef std::shared_ptr<Properties> PropertiesPtr; typedef std::shared_ptr<Triggers> TriggersPtr;

class Versions
{
    public:
        VerSectionPtr ver; // StarCraft version information
        TypeSectionPtr type; // Redundant versioning
        IverSectionPtr iver; // Redundant versioning
        Ive2SectionPtr ive2; // Redundant versioning
        VcodSectionPtr vcod; // Validation
        
        bool is(Chk::Version version);
        bool isOriginal();
        bool isHybrid();
        bool isExpansion();
        bool isHybridOrAbove();
        void changeTo(Chk::Version version);

        bool hasDefaultValidation();
        void setToDefaultValidation();

    private:
        LayersPtr layers; // For updating location capacity as necessary
};

class Strings : StrSynchronizer
{
    public:

        SprpSectionPtr sprp; // Scenario name and description
        StrSectionPtr str; // StarCraft string data
        OstrSectionPtr ostr; // Overrides for all but trigger and briefing strings
        KstrSectionPtr kstr; // Editor only string data

        enum class RescopeFlag : u32 {
            RescopeSwitchNames = BIT_0,
            RescopeComments = BIT_1,
            Rescope
        };

        enum class StringUserType : u8 {
            None = 0,
            ScenarioName,
            ScenarioDescription = ScenarioName+1,
            Force,
            Location,
            OriginalUnitSettings,
            ExpansionUnitSettings,
            Sound,
            Switch,
            Trigger,
            BriefingTrigger
        };

        Strings();

        size_t getCapacity(Chk::Scope storageScope = Chk::Scope::Game);

        bool stringUsed(size_t stringId, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Game, bool ensureStored = false);
        bool stringStored(size_t stringId, Chk::Scope storageScope = Chk::Scope::Either);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either);
        void markValidUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t stringId, Chk::Scope storageScope = Chk::Scope::EditorOverGame); // Gets the string at stringId with formatting based on StringType

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t findString(const StringType &str, Chk::Scope storageScope = Chk::Scope::Game);

        void setCapacity(size_t stringCapacity, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t addString(const StringType &str, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        bool addStrings(const std::vector<zzStringTableNode> stringsToAdd, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void replaceString(size_t stringId, const StringType &str, Chk::Scope storageScope = Chk::Scope::Game);

        void deleteString(size_t stringId, Chk::Scope storageScope = Chk::Scope::Both, bool deleteOnlyIfUnused = true);
        void moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope = Chk::Scope::Game);
        size_t rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo = Chk::Scope::Editor, bool autoDefragment = true);

        size_t getScenarioNameStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getScenarioDescriptionStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getForceNameStringId(Chk::Force force, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getLocationNameStringId(size_t locationIndex, Chk::Scope storageScope = Chk::Scope::Game);

        void setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        void setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setLocationNameStringId(size_t locationIndex, size_t locationNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioName(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioDescription(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getForceName(Chk::Force force, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getUnitName(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSoundPath(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSwitchName(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getLocationName(size_t locationIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioName(const StringType &scenarioNameString, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioDescription(const StringType &scenarioDescription, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setForceName(Chk::Force force, const StringType &forceName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setUnitName(Sc::Unit::Type unitType, const StringType &unitName, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSoundPath(size_t soundIndex, const StringType &soundPath, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSwitchName(size_t switchIndex, const StringType &switchName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setLocationName(size_t locationIndex, const StringType &locationName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        // Checks whether there's any viable configuration for the STR section given the current allowedCompressionFlags
        // allowedCompressionFlags may be increased as necessary if elevator.elevate() returns true
        bool checkFit(StrCompressionElevatorPtr compressionElevator = StrCompressionElevatorPtr());

        // Creates a viable internal data buffer for the string section using the methods in requestedCompressionFlags
        // If no configuration among requestedCompressionFlags is viable, additional methods through allowedCompressionFlags are added as neccessary
        // allowedCompressionFlags may be increased as neccessary if elevator.elevate() returns true
        virtual void synchronizeToStrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged);
        virtual void synchronzieFromStrBuffer(const buffer &rawData);

        virtual void synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = (u32)StrCompressFlag::Unchanged, u32 allowedCompressionFlags = (u32)StrCompressFlag::Unchanged);
        virtual void synchronzieFromKstrBuffer(const buffer &rawData);

    private:
        Versions versions; // For auto-determining the section for regular or expansion units
        PlayersPtr players; // For finding force string usage
        LayersPtr layers; // For finding location string usage
        PropertiesPtr properties; // For finding unit name string usage
        TriggersPtr triggers; // For finding trigger and briefing string usage

        static const std::vector<u32> compressionFlagsProgression;

        std::deque<ScStrPtr> strings;
        std::deque<ScStrPtr> kStrings;
        
        inline void loadString(std::deque<ScStrPtr> &stringContainer, const buffer &rawData, const u16 &stringOffset, const size_t &sectionSize);
        size_t getNextUnusedStringId(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope storageScope = Chk::Scope::Game, bool checkBeyondScopedCapacity = true, size_t firstChecked = 1);
        void resolveParantage();
        void resolveParantage(ScStrPtr string);
        bool defragmentStr(bool matchCapacityToUsage = true); // Returns true if any fragmented strings are packed
        bool defragmentKstr(bool matchCapacityToUsage = true); // Returns true if any fragmented strings are packed
        void replaceStringId(size_t oldStringId, size_t newStringId);
        void remapStringIds(std::map<u32, u32> stringIdRemappings, Chk::Scope storageScope);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> convert(ScStrPtr string);
};

class Players
{
    public:
        SideSectionPtr side; // Races
        ColrSectionPtr colr; // Player colors
        ForcSectionPtr forc; // Forces
        OwnrSectionPtr ownr; // Slot owners
        IownSectionPtr iown; // Redundant slot owners

        Sc::Player::SlotOwner getSlotOwner(size_t slotIndex, Chk::Scope scope = Chk::Scope::Game);
        void setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner, Chk::Scope scope = Chk::Scope::Both);

        Chk::Race getPlayerRace(size_t playerIndex);
        void setPlayerRace(size_t playerIndex, Chk::Race race);

        Chk::PlayerColor getPlayerColor(Sc::Player::SlotOwner slotOwner);
        void setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color);

        Chk::Force getPlayerForce(Sc::Player::SlotOwner slotOwner);
        size_t getForceStringId(Chk::Force force);
        u8 getForceFlags(Chk::Force force);
        void setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force);
        void setForceStringId(Chk::Force force, u16 forceStringId);
        void setForceFlags(Chk::Force force, u8 forceFlags);
        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating force strings
};

class Terrain
{
    public:
        EraSectionPtr era; // Tileset
        DimSectionPtr dim; // Dimensions
        MtxmSectionPtr mtxm; // Real terrain data
        TileSectionPtr tile; // Intermediate terrain data
        IsomSectionPtr isom; // Isometric terrain data

        Sc::Terrain::Tileset getTileset();
        void setTileset(Sc::Terrain::Tileset tileset);

        size_t getTileWidth();
        size_t getTileHeight();
        size_t getPixelWidth();
        size_t getPixelHeight();
        virtual void setTileWidth(u16 newTileWidth, s32 leftEdge = 0);
        virtual void setTileHeight(u16 newTileHeight, s32 topEdge = 0);
        virtual void setDimensions(u16 newTileWidth, u16 newTileHeight, s32 leftEdge = 0, s32 topEdge = 0);

        u16 getTile(size_t tileXc, size_t tileYc, Chk::Scope scope = Chk::Scope::Game);
		inline u16 getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope = Chk::Scope::Game);
		void setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Both);
		inline void setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Both);
        
        std::shared_ptr<Chk::IsomEntry> getIsomEntry(size_t isomIndex);
};

class Layers : public Terrain
{
    public:
        MaskSectionPtr mask; // Fog of war
        Thg2SectionPtr thg2; // Sprites
        Dd2SectionPtr dd2; // Doodads
        UnitSectionPtr unit; // Units
        MrgnSectionPtr mrgn; // Locations

        enum class SizeValidationFlag : u16
        {
            UpdateAnywhere = BIT_0,
            UpdateAnywhereIfAlreadyStandard = BIT_1,
            UpdateOutOfBoundsLocations = BIT_2, // Not standard, locations larger than the map are useful
            RemoveOutOfBoundsDoodads = BIT_3,
            UpdateOutOfBoundsUnits = BIT_4,
            RemoveOutOfBoundsUnits = BIT_5,
            UpdateOutOfBoundsSprites = BIT_6,
            RemoveOutOfBoundsSprites = BIT_7,
            Default = UpdateAnywhereIfAlreadyStandard | RemoveOutOfBoundsDoodads | UpdateOutOfBoundsUnits | UpdateOutOfBoundsSprites
        };

        virtual void setTileWidth(u16 tileWidth, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 leftEdge = 0);
        virtual void setTileHeight(u16 tileHeight, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 topEdge = 0);
        virtual void setDimensions(u16 tileWidth, u16 tileHeight, u16 sizeValidationFlags = (u16)SizeValidationFlag::Default, s32 leftEdge = 0, s32 topEdge = 0);
        void validateSizes(u16 sizeValidationFlags);

        u8 getFog(size_t tileXc, size_t tileYc);
        inline u8 getFogPx(size_t pixelXc, size_t pixelYc);
        void setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers);
        inline void setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers);

        size_t numSprites();
        std::shared_ptr<Chk::Sprite> getSprite(size_t spriteIndex);
        size_t addSprite(std::shared_ptr<Chk::Sprite> sprite);
        void insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite);
        void deleteSprite(size_t spriteIndex);
        void moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo);
        void updateOutOfBoundsSprites();
        void removeOutOfBoundsSprites();

        size_t numDoodads();
        std::shared_ptr<Chk::Doodad> getDoodad(size_t doodadIndex);
        size_t addDoodad(std::shared_ptr<Chk::Doodad> doodad);
        void insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad);
        void deleteDoodad(size_t doodadIndex);
        void moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo);
        void removeOutOfBoundsDoodads();

        size_t numUnits();
        std::shared_ptr<Chk::Unit> getUnit(size_t unitIndex);
        size_t addUnit(std::shared_ptr<Chk::Unit> unit);
        void insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit);
        void deleteUnit(size_t unitIndex);
        void moveUnit(size_t unitIndexFrom, size_t unitIndexTo);
        void updateOutOfBoundsUnits();
        void removeOutOfBoundsUnits();
        
        size_t numLocations();
        void sizeLocationsToScOriginal();
        void sizeLocationsToScHybridOrExpansion();
        std::shared_ptr<Chk::Location> getLocation(size_t locationIndex);
        size_t addLocation(std::shared_ptr<Chk::Location> location);
        void insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location);
        void deleteLocation(size_t locationIndex);
        void moveLocation(size_t locationIndexFrom, size_t locationIndexTo);
        void downsizeOutOfBoundsLocations();
        
        bool anywhereIsStandardDimensions();
        void matchAnywhereToDimensions();

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating location names
};

class Properties
{
    public:
        UnisSectionPtr unis; // Unit settings
        UnixSectionPtr unix; // Expansion Unit Settings
        PuniSectionPtr puni; // Unit availability
        UpgsSectionPtr upgs; // Upgrade costs
        UpgxSectionPtr upgx; // Expansion upgrade costs
        UpgrSectionPtr upgr; // Upgrade leveling
        PupxSectionPtr pupx; // Expansion upgrade leveling
        TecsSectionPtr tecs; // Technology costs
        TecxSectionPtr tecx; // Expansion technology costs
        PtecSectionPtr ptec; // Technology availability
        PtexSectionPtr ptex; // Expansion technology availability

        bool useExpansionUnitSettings(Chk::UseExpSection useExp);
        bool unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u32 getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u8 getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUnitNameStringId(Sc::Unit::Type unitType, size_t nameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex);
        bool isUnitDefaultBuildable(Sc::Unit::Type unitType);
        bool playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex);
        void setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable);
        void setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable);
        void setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault);
        
        bool useExpansionUpgradeCosts(Chk::UseExpSection useExp);
        bool upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool useExpansionUpgradeLeveling(Chk::UseExpSection useExp);
        size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        
        bool useExpansionTechCosts(Chk::UseExpSection useExp);
        bool techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        
        bool useExpansionTechAvailability(Chk::UseExpSection useExp);
        bool techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool stringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        Versions versions; // For auto-determining the section for regular or expansion units
        StringsPtr strings; // For reading and updating unit names
};

class Triggers
{
    public:
        UprpSectionPtr uprp; // CUWP - Create unit with properties properties
        UpusSectionPtr upus; // CUWP usage
        TrigSectionPtr trig; // Triggers
        MbrfSectionPtr mbrf; // Mission briefing triggers
        SwnmSectionPtr swnm; // Switch names
        WavSectionPtr wav; // Sound names

        Chk::Cuwp getCuwp(size_t cuwpIndex);
        void setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp);
        size_t addCuwp(const Chk::Cuwp &cuwp, bool fixUsageBeforeAdding = true);
        
        void fixCuwpUsage();
        bool cuwpUsed(size_t cuwpIndex);
        void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);

        size_t numTriggers();
        std::shared_ptr<Chk::Trigger> getTrigger(size_t triggerIndex);
        size_t addTrigger(std::shared_ptr<Chk::Trigger> trigger);
        void insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger);
        void deleteTrigger(size_t triggerIndex);
        void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);

        size_t numBriefingTriggers();
        std::shared_ptr<Chk::Trigger> getBriefingTrigger(size_t briefingTriggerIndex);
        size_t addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger);
        void insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger);
        void deleteBriefingTrigger(size_t briefingTriggerIndex);
        void moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo);

        size_t getSwitchNameStringId(size_t switchIndex);
        void setSwitchNameStringId(size_t switchIndex, size_t stringId);

        bool stringIsSound(size_t stringId);
        size_t getSoundStringId(size_t soundIndex);
        void setSoundStringId(size_t soundIndex, size_t soundStringId);
        bool stringUsed(size_t stringId);
        bool gameStringUsed(size_t stringId);
        bool editorStringUsed(size_t stringId);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedEditorStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapStringIds(std::map<u32, u32> stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        StringsPtr strings; // For reading and updating sound paths, next scenario paths, text messages, leader board text, comments, and switch names
        LayersPtr layers; // For reading locations
};

class Scenario;
using ScenarioPtr = std::shared_ptr<Scenario>;

class ScenarioSaver;
using ScenarioSaverPtr = std::shared_ptr<ScenarioSaver>;

enum class ChkVersion : u32;

class Scenario
{
    public:
/*   Sections   */  Versions versions; // All version and validation related information
                    Strings strings; // All string related information: map title and description, strings used by other sections
                    Players players; // All player related information: slot owners, races, player colors, forces
                    Layers layers; // All layers (stuff with coordinates) - terrain, units, fog of war, sprites, doodads, locations
                    Properties properties; // All property sheets: unit settings, unit availability, upgrade costs, upgrade leveling, technology costs, technology leveling
                    Triggers triggers; // All scenario triggers and mission briefing triggers and their metadata - CUWPs, switches, and sounds

/* Constructors */  Scenario();

/* Destructors  */  virtual ~Scenario();

/*  Quick Pass  */  Scenario* scenario(); // Returns pointer to this scenario, useful for classes that inherit 'Scenario'

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*  Expansion   */  bool isExpansion(); // Check if the map uses expansion settings

/*   Version    */  bool HasVerSection();
                    bool IsScOrig();
                    bool IsHybrid();
                    bool IsScExp();
                    void ChangeToScOrig();
                    void ChangeToHybrid();
                    void ChangeToScExp();
                    bool isVersion(ChkVersion chkVersion) { return false; } // Checks whether all sections are valid for the given ChkVersion

/*  Description */  u16 GetMapTitleStrId();
                    u16 GetMapDescriptionStrId();
                    bool getMapTitle(ChkdString &dest);
                    bool getMapDescription(ChkdString &dest);
                    bool SetMapTitle(ChkdString &newMapTitle);
                    bool SetMapDescription(ChkdString &newMapDescription);


/*   Players    */  bool getPlayerOwner(u8 player, u8& owner); // Gets the current owner of a player
                    bool getPlayerRace(u8 player, u8& race); // Gets the players current race
                    bool getPlayerColor(u8 player, u8& color); // Gets the players current color
                    bool setPlayerOwner(u8 player, u8 newOwner); // Sets a new owner for a player
                    bool setPlayerRace(u8 player, u8 newRace); // Sets a new race for a player
                    bool setPlayerColor(u8 player, u8 newColor); // Sets a new color for a player


/*    Forces    */  bool hasForceSection();
                    u32 getForceStringNum(u8 index); // Gets the string number of the force at the given index
                    bool getForceString(ChkdString &str, u8 forceNum); // Gets the name of the given force num
                    bool getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av); // Attempts to get info for given force num
                    bool getPlayerForce(u8 playerNum, u8 &force); // Attempts to get the force a player belongs to
                    bool setPlayerForce(u8 player, u8 newForce); // Sets a new force for a player
                    bool setForceAllied(u8 forceNum, bool allied); // Specifies whether a force's players are allied
                    bool setForceVision(u8 forceNum, bool sharedVision); // Specifies whether a force's players have shared vision
                    bool setForceRandom(u8 forceNum, bool randomStartLocs); // Specifies whether a force has randomized start locations
                    bool setForceAv(u8 forceNum, bool alliedVictory); // Specifies whether a force's players have allied victory
                    bool setForceName(u8 forceNum, ChkdString &newName);


/*  Dimensions  */  u16 getWidth(); // Get width of map
                    u16 getHeight(); // Get height of map
                    bool setDimensions(u16 newWidth, u16 newHeight); // Sets new dimensions for the map


/*   Tileset    */  u16 getTileset(); // Gets the maps tileset
                    bool setTileset(u16 newTileset); // Sets a new value for the maps tileset


/*    Tiles     */  u16 getTile(u16 xc, u16 yc);
                    u16 getTileSectionTile(u16 xc, u16 yc);
                    bool getTile(u16 xc, u16 yc, u16 &tileValue);
                    bool getTileSectionTile(u16 xc, u16 yc, u16 &tileValue);
                    bool setTile(u16 xc, u16 yc, u16 value);


/*    Units     */  u16 numUnits(); // Returns number of units in UNIT section
                    //bool getUnit(Chk::Unit* &unitRef, u16 index); // Gets unit at index
                    bool addUnit(u16 unitID, u8 owner, u16 xc, u16 yc, u16 stateFlags); // Attempts to create a unit
                    bool getUnitName(RawString &dest, u16 unitID);
                    bool getUnitName(ChkdString &dest, u16 unitID);
                    bool deleteUnit(u16 index);
                    u32 GetUnitFieldData(u16 unitIndex, Chk::Unit::Field field);
                    bool SetUnitFieldData(u16 unitIndex, Chk::Unit::Field field, u32 data);
                    bool SetUnitHitpoints(u16 unitIndex, u8 newHitpoints);
                    bool SetUnitEnergy(u16 unitIndex, u8 newEnergy);
                    bool SetUnitShields(u16 unitIndex, u8 newShields);
                    bool SetUnitResources(u16 unitIndex, u32 newResources);
                    bool SetUnitHanger(u16 unitIndex, u16 newHanger);
                    bool SetUnitTypeId(u16 unitIndex, u16 newTypeId);
                    bool SetUnitXc(u16 unitIndex, u16 newXc);
                    bool SetUnitYc(u16 unitIndex, u16 newYc);


/*   Sprites    */  u16 SpriteSectionCapacity();
                    bool GetSpriteRef(Chk::Sprite* &spriteRef, u16 index);
                    

/*  Locations   */  bool HasLocationSection();
                    u32 GetLocationFieldData(u16 locationIndex, Chk::Location::Field field);
                    bool SetLocationFieldData(u16 locationIndex, Chk::Location::Field field, u32 data);
                    bool insertLocation(u16 index, Chk::Location &location, RawString &name);
                    bool insertLocation(u16 index, Chk::Location &location, ChkdString &name);
                    u8 numLocations(); // Gets the current amount of used locations in the map
                    u16 locationCapacity(); // Gets the map's current location capacity
                    bool locationIsUsed(u16 locationIndex);
                    bool createLocation(s32 xc1, s32 yc1, s32 xc2, s32 yc2, u16& locationIndex); // Create a location
                    void deleteLocation(u16 locationIndex); // Unconditionally deletes a location
                    bool stringUsedWithLocs(u32 stringNum); // Returns whether the string is used for a location


/*   Triggers   */  bool HasTrigSection();
                    bool AddOrReplaceTrigSection(buffer & newTrigSection);
                    u32 numTriggers(); // Returns the number of triggers in this scenario
                    bool getActiveComment(Chk::Trigger* trigger, RawString &comment) { return false; }
                    bool addTrigger(Chk::Trigger &trigger) { return false; }
                    bool insertTrigger(u32 triggerId, Chk::Trigger &trigger) { return false; }
                    bool deleteTrigger(u32 triggerId);
                    bool copyTrigger(u32 triggerId);
                    bool moveTriggerUp(u32 triggerId); // Moves the given trigger up one absolute index
                    bool moveTriggerDown(u32 triggerId); // Moves the given trigger down one absolute index
                    bool moveTrigger(u32 triggerId, u32 destId); // Moves the given trigger to the destination index
                    TrigSectionPtr GetTrigSection();
                    

/*     CUWPs    */  bool GetCuwp(u8 cuwpIndex, Chk::Cuwp &outPropStruct);
                    bool AddCuwp(Chk::Cuwp &propStruct, u8 &outCuwpIndex);
                    bool ReplaceCuwp(Chk::Cuwp &propStruct, u8 cuwpIndex);
                    bool IsCuwpUsed(u8 cuwpIndex);
                    bool SetCuwpUsed(u8 cuwpIndex, bool isUsed);
                    bool FindCuwp(const Chk::Cuwp &cuwpToFind, u8 &outCuwpIndex);
                    int CuwpCapacity();
                    int NumUsedCuwps();

/*     WAVs     */  u32 WavSectionCapacity();
                    virtual bool GetWav(u16 wavIndex, u32 &outStringId);
                    bool GetWavString(u16 wavIndex, RawString &outString);
                    bool AddWav(u32 stringId);
                    bool AddWav(const RawString &wavMpqPath);
                    virtual bool RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed);
                    virtual bool RemoveWavByStringId(u32 stringId, bool removeIfUsed);
                    bool WavHasString(u16 wavIndex);
                    bool IsStringUsedWithWavs(u32 stringId);
                    bool GetWavs(std::map<u32/*stringId*/, u16/*wavIndex*/> &wavMap, bool includePureStringWavs);
                    bool MapUsesWavString(u32 wavStringId);
                    void ZeroWavUsers(u32 wavStringId);

/*   Switches   */  u32 NumNameableSwitches();
                    bool getSwitchStrId(u8 switchId, u32 &stringNum);
                    bool getSwitchName(ChkdString &dest, u8 switchID);
                    bool setSwitchName(ChkdString &newName, u8 switchId, bool extended);
                    bool hasSwitchSection();
                    bool switchHasName(u8 switchId);
                    void removeSwitchName(u8 switchId);
                    bool SwitchUsesExtendedName(u8 switchId);
                    bool ToggleUseExtendedSwitchName(u8 switchId);

/*   Briefing   */  bool getBriefingTrigger(Chk::Trigger* &trigRef, u32 index); // Gets the briefing trigger at index


/* UnitSettings */  bool ReplaceUNISSection(buffer &newUNISSection);
                    bool ReplaceUNIxSection(buffer &newUNIxSection);
                    bool ReplacePUNISection(buffer &newPUNISection);
                    bool getUnitStringNum(u16 unitId, u16 &stringNum);
                    bool unitUsesDefaultSettings(u16 unitId);
                    bool unitIsEnabled(u16 unitId);
                    //UnitEnabledState getUnitEnabledState(u16 unitId, u8 player);
                    bool hasUnitSettingsSection();


                    bool getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints);
                    bool getUnitSettingsHitpointByte(u16 unitId, u8 &hitpointByte);
                    bool getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints);
                    bool getUnitSettingsArmor(u16 unitId, u8 &armor);
                    bool getUnitSettingsBuildTime(u16 unitId, u16 &buildTime);
                    bool getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost);
                    bool getUnitSettingsGasCost(u16 unitId, u16 &gasCost);
                    bool getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage);
                    bool getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage);

                    bool getUnisStringId(u16 unitId, u16 &stringId);
                    bool getUnixStringId(u16 unitId, u16 &stringId);
                    bool setUnisStringId(u16 unitId, u16 newStringId);
                    bool setUnixStringId(u16 unitId, u16 newStringId);

                    bool setUnitUseDefaults(u16 unitID, bool useDefaults); // Specifies whether a unit uses default settings
                    bool setUnitEnabled(u16 unitId, bool enabled);
                    //bool setUnitEnabledState(u16 unitId, u8 player, UnitEnabledState enabledState);
                    bool setUnitSettingsCompleteHitpoints(u16 unitId, u32 completeHitpoints);
                    bool setUnitSettingsHitpoints(u16 unitId, u32 hitpoints);
                    bool setUnitSettingsHitpointByte(u16 unitId, u8 hitpointByte);
                    bool setUnitSettingsShieldPoints(u16 unitId, u16 shieldPoints);
                    bool setUnitSettingsArmor(u16 unitId, u8 armor);
                    bool setUnitSettingsBuildTime(u16 unitId, u16 buildTime);
                    bool setUnitSettingsMineralCost(u16 unitId, u16 mineralCost);
                    bool setUnitSettingsGasCost(u16 unitId, u16 gasCost);
                    bool setUnitSettingsBaseWeapon(u32 weaponId, u16 baseDamage);
                    bool setUnitSettingsBonusWeapon(u32 weaponId, u16 bonusDamage);
                    bool setUnitName(u16 unitId, RawString &newName);
                    bool setUnitName(u16 unitId, ChkdString &newName);


/* UpgrSettings */  bool ReplaceUPGSSection(buffer &newUPGSSection);
                    bool ReplaceUPGxSection(buffer &newUPGxSection);
                    bool ReplaceUPGRSection(buffer &newUPGRSection);
                    bool ReplacePUPxSection(buffer &newPUPxSection);
                    bool upgradeUsesDefaultCosts(u8 upgradeId);
                    bool getUpgradeMineralCost(u8 upgradeId, u16 &mineralCost);
                    bool getUpgradeMineralFactor(u8 upgradeId, u16 &mineralFactor);
                    bool getUpgradeGasCost(u8 upgradeId, u16 &gasCost);
                    bool getUpgradeGasFactor(u8 upgradeId, u16 &gasFactor);
                    bool getUpgradeTimeCost(u8 upgradeId, u16 &timeCost);
                    bool getUpgradeTimeFactor(u8 upgradeId, u16 &timeFactor);
                    bool getUpgradeDefaultStartLevel(u8 upgradeId, u8 &startLevel);
                    bool getUpgradeDefaultMaxLevel(u8 upgradeId, u8 &maxLevel);
                    bool playerUsesDefaultUpgradeLevels(u8 upgradeId, u8 player);
                    bool getUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 &startLevel);
                    bool getUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 &maxLevel);
                    bool setUpgradeUseDefaults(u8 upgradeNum, bool useDefaults); // Specifies whether an upgrade uses default costs
                    bool setUpgradeMineralCost(u8 upgradeId, u16 mineralCost);
                    bool setUpgradeMineralFactor(u8 upgradeId, u16 mineralFactor);
                    bool setUpgradeGasCost(u8 upgradeId, u16 gasCost);
                    bool setUpgradeGasFactor(u8 upgradeId, u16 gasFactor);
                    bool setUpgradeTimeCost(u8 upgradeId, u16 timeCost);
                    bool setUpgradeTimeFactor(u8 upgradeId, u16 timeFactor);
                    bool setUpgradePlayerDefaults(u8 upgradeId, u8 player, bool usesDefaultLevels);
                    bool setUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 newStartLevel);
                    bool setUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 newMaxLevel);
                    bool setUpgradeDefaultStartLevel(u8 upgradeId, u8 newStartLevel);
                    bool setUpgradeDefaultMaxLevel(u8 upgradeId, u8 newMaxLevel);


/* TechSettings */  bool ReplaceTECSSection(buffer &newTECSSection);
                    bool ReplaceTECxSection(buffer &newTECxSection);
                    bool ReplacePTECSection(buffer &newPTECSection);
                    bool ReplacePTExSection(buffer &newPTExSection);
                    bool techUsesDefaultCosts(u8 techId);
                    bool getTechMineralCost(u8 techId, u16 &mineralCost);
                    bool getTechGasCost(u8 techId, u16 &gasCost);
                    bool getTechTimeCost(u8 techId, u16 &timeCost);
                    bool getTechEnergyCost(u8 techId, u16 &energyCost);
                    bool techAvailableForPlayer(u8 techId, u8 player);
                    bool techResearchedForPlayer(u8 techId, u8 player);
                    bool techAvailableByDefault(u8 techId);
                    bool techResearchedByDefault(u8 techId);
                    bool playerUsesDefaultTechSettings(u8 techId, u8 player);
                    bool setTechUseDefaults(u8 techNum, bool useDefaultCosts); // Specifies whether a tech uses default costs
                    bool setTechMineralCost(u8 techId, u16 newMineralCost);
                    bool setTechGasCost(u8 techId, u16 newGasCost);
                    bool setTechTimeCost(u8 techId, u16 newTimeCost);
                    bool setTechEnergyCost(u8 techId, u16 newEnergyCost);
                    bool setTechAvailableForPlayer(u8 techId, u8 player, bool availableForPlayer);
                    bool setTechResearchedForPlayer(u8 techId, u8 player, bool researchedForPlayer);
                    bool setTechDefaultAvailability(u8 techId, bool availableByDefault);
                    bool setTechDefaultResearched(u8 techId, bool researchedByDefault);
                    bool setPlayerUsesDefaultTechSettings(u8 techId, u8 player, bool playerUsesDefaultSettings);


/*   Strings    */  u16 strSectionCapacity(); // The current capacity of the STR section
                    u32 kstrSectionCapacity(); // The current capacity of the KSTR section
                    u32 stringCapacity(); // The sum of the capcities of the STR and KSTR section
                    u32 stringCapacity(bool extended); // Returns the capcity of the STR or KSTR section
                    bool hasStrSection(bool extended); // Checks if the STR or KSTR section exists
                    u32 strBytesUsed(); // Returns the amount of bytes used in the STR section

                    bool stringIsUsed(u32 stringNum); // Returns whether the string is used in the map
                    bool isExtendedString(u32 stringNum); // Returns whether the string is an extended string
                    bool stringExists(u32 stringNum); // Checks if a string is associated with this stringNum
                    bool stringExists(const RawString &str, u32& stringNum); // Checks if string exists, returns true and stringNum if so
                    bool stringExists(const ChkdString &str, u32 &stringNum);
                    bool stringExists(const RawString &str, u32& stringNum, bool extended); // Checks if string exists in the same table
                    //bool stringExists(const RawString &str, u32& stringNum, std::vector<StringTableNode> &strList);
                    bool FindDifference(ChkdString &str, u32& stringNum); // Checks if there's a difference between str and string at stringNum
                    u32 extendedToRegularStr(u32 stringNum); // Returns string number for extended section

                    bool GetString(RawString &dest, u32 stringNum); // No special formatting
                    bool GetString(EscString &dest, u32 stringNum); // C++ formatting, see MakeEscString
                    bool GetString(ChkdString &dest, u32 stringNum); // Chkd and C++ formatting, see MakeChkdString

                    bool GetExtendedString(RawString &dest, u32 extendedStringSectionIndex); // No special formatting
                    bool GetExtendedString(EscString &dest, u32 extendedStringSectionIndex); // C++ formatting, see MakeEscString
                    bool GetExtendedString(ChkdString &dest, u32 extendedStringSectionIndex); // Chkd and C++ formatting, see MakeChkdString

                    inline void IncrementIfEqual(u32 lhs, u32 rhs, u32 &counter);
                    u32 amountStringUsed(u32 stringNum); // Returns the number of times a string is used
                    void getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches);

                    bool removeUnusedString(u32 stringNum); // Delete string if it is no longer used anywhere in the map
                    void forceDeleteString(u32 stringNum); // Unconditionally deletes a string

                    /** Attempt to associate a stringNum with a string
                        Returns true and stringNum if string exists or was created successfully */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool addString(const RawString &str, numType &stringNum, bool extended);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool addString(const ChkdString &str, numType &stringNum, bool extended);

                    /** Attempts to assocate a stringNum with a string that has
                        already been confirmed to not exist in the string section
                        Returns true and stringNum if the string was created successfully */
                    //bool addNonExistentString(RawString &str, u32 &stringNum, bool extended, std::vector<StringTableNode> &strList);

                    /** Attempt to replace a string (specific instance of string), if unsuccessful the old string will remain
                            (if it is not used elsewhere) based on safeReplace and memory conditions
                        stringNum: if not 0 and only used once, the map calls editString, otherwise stringNum is set if successful
                        safeReplace: if true the function will only attempt to make room for the new string if the old one will not be
                            lost; if false the fuction will always attempt to make room, and may lose the old string in low-mem
                            conditions
                        Returns true if the string exists already or was successfully created */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool replaceString(RawString &newString, numType &stringNum, bool extended, bool safeReplace);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool replaceString(ChkdString &newString, numType &stringNum, bool extended, bool safeReplace);

                    /** Attempts to edit the contents of a string (universal change to string), if unsucessesful the old string will
                            remain (if it is not used elsewhere) based on safeEdit and memory conditions
                        safeEdit: if true the function will only attempt to make room for the new string if the old one will not be lost
                            if false the function will always attempt to make room, and may lose the old string in low-mem conditions
                        Returns true if the string contents were successfully edited, always false if stringNum is 0 */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool editString(RawString &newString, numType stringNum, bool extended, bool safeEdit);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool editString(ChkdString &newString, numType stringNum, bool extended, bool safeEdit);

                    void replaceStringNum(u32 toReplace, u32 replacement);

                    void removeMetaStrings(); // Attemps to remove all strings not used in-game
                    bool cleanStringTable(bool extendedTable); // Rebuilds string table with current indexs, removes all unused strings
                    //bool cleanStringTable(bool extendedTable, std::vector<StringTableNode> &strList); // Cleans string table with given list
                    bool removeDuplicateStrings();
                    bool compressStringTable(bool extendedTable, bool recycleSubStrings); // Rebuilds string table with minimum indexs and smallest size possible
                    bool repairStringTable(bool extendedTable); // Removes what Scmdraft considers string corruption
                    /*bool addAllUsedStrings(std::vector<StringTableNode>& strList, bool includeStandard, bool includeExtended);
                    bool rebuildStringTable(std::vector<StringTableNode> &strList, bool extendedTable); // Rebuilds string table using the provided list
                    bool buildStringTables(std::vector<StringTableNode> &strList, bool extendedTable,
                        StrSection &offsets, buffer &strPortion, s64 strPortionOffset, u32 numStrs, bool recycleSubStrings) { return false; }*/


/*  Validation  */  bool GoodVCOD(); // Check if VCOD is valid


/*   Security   */  bool isProtected(); // Checks if map is protected
                    bool hasPassword(); // Checks if the map has a password
                    bool isPassword(std::string &password); // Checks if this is the password the map has
                    bool SetPassword(std::string &oldPass, std::string &newPass); // Attempts to password-protect the 
                    bool Login(std::string &password); // Attempts to login to the map


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*   File IO    */  bool ParseScenario(buffer &chk); // Parses supplied scenario file data
                    bool CreateNew(u16 width, u16 height, Sc::Terrain::Tileset tileset, u32 terrain, u32 triggers);
                    void WriteFile(FILE* pFile); // Writes all sections to the supplied file
                    std::shared_ptr<void> Serialize(); /** Writes all sections to a buffer in memory as it would to a .chk file
                                                     includes a 4 byte "CHK " tag followed by a 4-byte size, followed by data */
                    bool Deserialize(const void* data); // "Opens" a serialized Scenario.chk file, data must be 8+ bytes
                    
                    /** Makes whatever changes are required for saving in the given ChkVersion
                        and writes the scenario to outputStream
                        
                        Returns true upon success, returns false and rolls back any changes made during save on failure */
                    bool Save(ChkVersion chkVersion, std::ostream &outputStream, ScenarioSaverPtr scenarioSaver = nullptr);
                    bool PrepareSaveSectionChanges(ChkVersion chkVersion, std::unordered_map<Section, Section> &sectionsToReplace,
                        std::vector<Section> sectionsToAdd, std::vector<SectionName> sectionsToRemove, ScenarioSaverPtr scenarioSaver = nullptr) { return false; }

/*  Rebuilders  */  void correctMTXM(); // Corrects MTXM of protected maps for view-only purposes

/* Section mgmt */  Section FindLast(SectionName sectionName) { return nullptr; } // Finds the last instance of a section
                    bool RemoveSection(SectionName sectionName) { return false; } // Removes section containing this buffer
                    bool RemoveSection(Section sectionToRemove);
                    bool AddSection(Section sectionToAdd); // Adds a buffer as a new map section
                    Section AddSection(SectionName sectionName) { return nullptr; } // Adds a section with the given id to the map
                    bool AddOrReplaceSection(Section section);
                    void Flush(); // Deletes all sections and clears map protection

/*   Defaults   */  static ScenarioSaverPtr getDefaultScenarioSaver() { return nullptr; }


    private:
                    
                    /** Pointers to sections used by Chkdraft. These are never null, but unless they're in allSections they aren't saved with the map */
/*   Sections   */  TypeSectionPtr TYPE; VerSectionPtr VER; IverSectionPtr IVER; Ive2SectionPtr IVE2; VcodSectionPtr VCOD; IownSectionPtr IOWN;
                    OwnrSectionPtr OWNR; EraSectionPtr ERA; DimSectionPtr DIM; SideSectionPtr SIDE; MtxmSectionPtr MTXM; PuniSectionPtr PUNI;
                    UpgrSectionPtr UPGR; PtecSectionPtr PTEC; UnitSectionPtr UNIT; IsomSectionPtr ISOM; TileSectionPtr TILE; Dd2SectionPtr DD2;
                    Thg2SectionPtr THG2; MaskSectionPtr MASK; StrSectionPtr STR; UprpSectionPtr UPRP; UpusSectionPtr UPUS; MrgnSectionPtr MRGN;
                    TrigSectionPtr TRIG; MbrfSectionPtr MBRF; SprpSectionPtr SPRP; ForcSectionPtr FORC; WavSectionPtr WAV; UnisSectionPtr UNIS;
                    UpgsSectionPtr UPGS; TecsSectionPtr TECS; SwnmSectionPtr SWNM; ColrSectionPtr COLR; PupxSectionPtr PUPx; PtexSectionPtr PTEx;
                    UnixSectionPtr UNIx; UpgxSectionPtr UPGx; TecxSectionPtr TECx;

                    KstrSectionPtr KSTR;

    protected:

        bool ToSingleBuffer(buffer &chk); // Writes the chk to a buffer
        bool ParseSection(buffer &chk, s64 position, s64 &nextPosition, std::vector<Section> &sections);
        void CacheSections(); // Caches all section references for fast access

        bool GetStrInfo(char* &ptr, size_t &length, u32 stringNum); // Gets a pointer to the string and its length if successful
        bool ZeroOutString(u32 stringNum); // returns false if stringNum is not a string in any sense
        bool RepairString(u32& stringNum, bool extended); // Ensures that the string would not be considered corrupted by Scmdraft


    private:

        std::vector<Section> allSections; // Holds all the sections of a map
        std::array<u8, 7> tailData; // The 0-7 bytes just before the Scenario file ends, after the last valid section
        u8 tailLength; // 0 for no tail data, must be less than 8
        bool mapIsProtected; // Flagged if map is protected
        bool caching; // if true, section add/remove/moves currently result in re-caching
        bool jumpCompress; // If true, the map will attempt to compress using jump sections when saving

        /*// Cached regular section pointers
        Section type, ver, iver, ive2, vcod, iown, ownr, era,
            dim, side, mtxm, puni, upgr, ptec, unit, isom,
            tile, dd2, thg2, mask, str, uprp, upus, mrgn,
            trig, mbrf, sprp, forc, wav, unis, upgs, tecs,
            swnm, colr, pupx, ptex, unix, upgx, tecx;

        // Cached extended section pointers
        Section kstr;*/
};

class ScenarioSaver
{
    public:
        virtual bool confirmRemoveLocations(Scenario &chk) = 0;
};

enum class ChkVersion : u32
{
    StarCraft = 0,
    Hybrid = 1,
    BroodWar = 2,
    Unchanged = 3
};

#endif