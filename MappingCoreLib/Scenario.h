#ifndef SCENARIO_H
#define SCENARIO_H
#include "Basics.h"
#include "EscapeStrings.h"
#include "Sections.h"
#include <memory>
#include <string>
#include <array>
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

        Versions(bool useDefault = false);

        bool empty();
        
        Chk::Version getVersion();
        bool is(Chk::Version version);
        bool isOriginal();
        bool isHybrid();
        bool isExpansion();
        bool isHybridOrAbove();
        bool changeTo(Chk::Version version, bool lockAnywhere = true, bool autoDefragmentLocations = true);

        bool hasDefaultValidation();
        void setToDefaultValidation();

    private:
        Layers* layers; // For updating location capacity as necessary
        friend class Scenario;

        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
};

class Strings : public StrSynchronizer
{
    public:
        SprpSectionPtr sprp; // Scenario name and description
        StrSectionPtr str; // StarCraft string data
        OstrSectionPtr ostr; // Overrides for all but trigger and briefing strings
        KstrSectionPtr kstr; // Editor only string data

        struct StringBackup
        {
            StrSectionPtr strBackup;
        };

        Strings(bool useDefault = false);

        bool empty();

        enum_t(RescopeFlag, u32, {
            RescopeSwitchNames = BIT_0,
            RescopeComments = BIT_1,
            Rescope
        });

        size_t getCapacity(Chk::Scope storageScope = Chk::Scope::Game);

        bool stringStored(size_t stringId, Chk::Scope storageScope = Chk::Scope::Either);
        virtual bool stringUsed(size_t stringId, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All, bool ensureStored = false);
        virtual void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either, u32 userMask = Chk::StringUserFlag::All);
        virtual void markValidUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either, u32 userMask = Chk::StringUserFlag::All);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t stringId, Chk::Scope storageScope = Chk::Scope::EditorOverGame); // Gets the string at stringId with formatting based on StringType

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t findString(const StringType & str, Chk::Scope storageScope = Chk::Scope::Game);

        void setCapacity(size_t stringCapacity, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        size_t addString(const StringType & str, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void replaceString(size_t stringId, const StringType & str, Chk::Scope storageScope = Chk::Scope::Game);

        void deleteUnusedStrings(Chk::Scope storageScope = Chk::Scope::Both);
        void deleteString(size_t stringId, Chk::Scope storageScope = Chk::Scope::Both, bool deleteOnlyIfUnused = true);
        void moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope = Chk::Scope::Game);
        size_t rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo = Chk::Scope::Editor, bool autoDefragment = true);

        std::vector<u8> & getTailData(); // Gets the data appended after the STR section
        size_t getTailDataOffset(); // Gets the offset tail data would be at within the STR section were it written right now
        size_t getInitialTailDataOffset(); // Gets the offset tail data was at when it was initially read in
        size_t getBytePaddedTo(); // Gets the current byte alignment setting for tailData (usually 4 for new StrSections, 0/none if existing tailData was read in)
        void setBytePaddedTo(size_t bytePaddedTo); // Sets the current byte alignment setting for tailData (only 2 and 4 are aligned, other values are ignored/treat tailData as unpadded)

        size_t getScenarioNameStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getScenarioDescriptionStringId(Chk::Scope storageScope = Chk::Scope::Game);
        size_t getForceNameStringId(Chk::Force force, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::Game);
        size_t getLocationNameStringId(size_t locationId, Chk::Scope storageScope = Chk::Scope::Game);

        void setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
        void setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        void setLocationNameStringId(size_t locationId, size_t locationNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioName(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getScenarioDescription(Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getForceName(Chk::Force force, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getUnitName(Sc::Unit::Type unitType, bool defaultIfNull = false, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSoundPath(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getSwitchName(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        std::shared_ptr<StringType> getLocationName(size_t locationId, Chk::Scope storageScope = Chk::Scope::EditorOverGame);

        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioName(const StringType & scenarioNameString, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setScenarioDescription(const StringType & scenarioDescription, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setForceName(Chk::Force force, const StringType & forceName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setUnitName(Sc::Unit::Type unitType, const StringType &unitName, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSoundPath(size_t soundIndex, const StringType & soundPath, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setSwitchName(size_t switchIndex, const StringType & switchName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
        void setLocationName(size_t locationId, const StringType & locationName, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

        // Creates a viable internal data buffer for the string section using the methods in requestedCompressionFlags
        // If no configuration among requestedCompressionFlags is viable, additional methods through allowedCompressionFlags are added as neccessary
        // allowedCompressionFlags may be increased as neccessary if elevator.elevate() returns true
        
        virtual void syncStringsToBytes(std::deque<ScStrPtr> & strings, std::vector<u8> & stringBytes,
            StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = StrCompressFlag::Unchanged, u32 allowedCompressionFlags = StrCompressFlag::Unchanged);

        virtual void syncKstringsToBytes(std::deque<ScStrPtr> & strings, std::vector<u8> & stringBytes,
            StrCompressionElevatorPtr compressionElevator = StrCompressionElevator::NeverElevate(),
            u32 requestedCompressionFlags = StrCompressFlag::Unchanged, u32 allowedCompressionFlags = StrCompressFlag::Unchanged);

        StringBackup backup();
        void restore(StringBackup & backup); // A backup instance can only be restored once

    protected:
        virtual void remapStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope);

    private:
        Versions* versions; // For auto-determining the section for regular or expansion units
        Players* players; // For finding force string usage
        Layers* layers; // For finding location string usage
        Properties* properties; // For finding unit name string usage
        Triggers* triggers; // For finding trigger and briefing string usage
        friend class Scenario;

        static const std::vector<u32> compressionFlagsProgression;

        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
};

class Players
{
    public:
        SideSectionPtr side; // Races
        ColrSectionPtr colr; // Player colors
        ForcSectionPtr forc; // Forces
        OwnrSectionPtr ownr; // Slot owners
        IownSectionPtr iown; // Redundant slot owners

        Players(bool useDefault = false);

        bool empty();

        Sc::Player::SlotType getSlotType(size_t slotIndex, Chk::Scope scope = Chk::Scope::Game);
        void setSlotType(size_t slotIndex, Sc::Player::SlotType slotType, Chk::Scope scope = Chk::Scope::Both);

        Chk::Race getPlayerRace(size_t playerIndex);
        void setPlayerRace(size_t playerIndex, Chk::Race race);

        Chk::PlayerColor getPlayerColor(size_t slotIndex);
        void setPlayerColor(size_t slotIndex, Chk::PlayerColor color);

        Chk::Force getPlayerForce(size_t slotIndex);
        size_t getForceStringId(Chk::Force force);
        u8 getForceFlags(Chk::Force force);
        void setPlayerForce(size_t slotIndex, Chk::Force force);
        void setForceStringId(Chk::Force force, u16 forceStringId);
        void setForceFlags(Chk::Force force, u8 forceFlags);
        bool stringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All);
        void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        Strings* strings; // For reading and updating force strings
        friend class Scenario;
        
        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
};

class Terrain
{
    public:
        EraSectionPtr era; // Tileset
        DimSectionPtr dim; // Dimensions
        MtxmSectionPtr mtxm; // Real terrain data
        TileSectionPtr tile; // Intermediate terrain data
        IsomSectionPtr isom; // Isometric terrain data

        Terrain();
        Terrain(Sc::Terrain::Tileset tileset, u16 width, u16 height);

        bool empty();

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
        
        Chk::IsomEntry & getIsomEntry(size_t isomIndex);

    protected:
        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();

    private:
        friend class Scenario;
};

class Layers : public Terrain
{
    public:
        MaskSectionPtr mask; // Fog of war
        Thg2SectionPtr thg2; // Sprites
        Dd2SectionPtr dd2; // Doodads
        UnitSectionPtr unit; // Units
        MrgnSectionPtr mrgn; // Locations

        Layers();
        Layers(Sc::Terrain::Tileset tileset, u16 width, u16 height);

        bool empty();

        enum_t(SizeValidationFlag, u16, {
            UpdateAnywhere = BIT_0,
            UpdateAnywhereIfAlreadyStandard = BIT_1,
            UpdateOutOfBoundsLocations = BIT_2, // Not standard, locations larger than the map are useful
            RemoveOutOfBoundsDoodads = BIT_3,
            UpdateOutOfBoundsUnits = BIT_4,
            RemoveOutOfBoundsUnits = BIT_5,
            UpdateOutOfBoundsSprites = BIT_6,
            RemoveOutOfBoundsSprites = BIT_7,
            Default = UpdateAnywhereIfAlreadyStandard | RemoveOutOfBoundsDoodads | UpdateOutOfBoundsUnits | UpdateOutOfBoundsSprites
        });

        virtual void setTileWidth(u16 tileWidth, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 leftEdge = 0);
        virtual void setTileHeight(u16 tileHeight, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 topEdge = 0);
        virtual void setDimensions(u16 tileWidth, u16 tileHeight, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 leftEdge = 0, s32 topEdge = 0);
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
        std::shared_ptr<Chk::Location> getLocation(size_t locationId);
        size_t addLocation(std::shared_ptr<Chk::Location> location);
        void replaceLocation(size_t locationId, std::shared_ptr<Chk::Location> location);
        void deleteLocation(size_t locationId, bool deleteOnlyIfUnused = true);
        bool moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere = true);
        bool isBlank(size_t locationId);
        void downsizeOutOfBoundsLocations();

        bool locationsFitOriginal(bool lockAnywhere = true, bool autoDefragment = true); // Checks if all locations fit in indexes < Chk::TotalOriginalLocations
        bool trimLocationsToOriginal(bool lockAnywhere = true, bool autoDefragment = true); // If possible, trims locations to indexes < Chk::TotalOriginalLocations
        void expandToScHybridOrExpansion();
        
        bool anywhereIsStandardDimensions();
        void matchAnywhereToDimensions();

        bool stringUsed(size_t stringId, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All);
        void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        Strings* strings; // For reading and updating location names
        Triggers* triggers; // For reading and updating locationIds
        friend class Scenario;
        
        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
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

        Properties(bool useDefault = false);

        bool empty();

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
        bool playerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex);
        void setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable);
        void setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable);
        void setPlayerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault);
        void setUnitsToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);
        
        bool useExpansionUpgradeCosts(Chk::UseExpSection useExp);
        bool upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        u16 getUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        void setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

        bool useExpansionUpgradeLeveling(Chk::UseExpSection useExp);
        size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        bool playerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setUpgradesToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);
        
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
        bool playerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setPlayerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        void setTechsToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);

        bool stringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All);
        void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteString(size_t stringId);

    private:
        Versions* versions; // For auto-determining the section for regular or expansion units
        Strings* strings; // For reading and updating unit names
        friend class Scenario;
        
        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
};

class Triggers : public LocationSynchronizer
{
    public:
        UprpSectionPtr uprp; // CUWP - Create unit with properties properties
        UpusSectionPtr upus; // CUWP usage
        TrigSectionPtr trig; // Triggers
        MbrfSectionPtr mbrf; // Mission briefing triggers
        SwnmSectionPtr swnm; // Switch names
        WavSectionPtr wav; // Sound names

        Triggers(bool useDefault = false);

        bool empty();

        Chk::Cuwp getCuwp(size_t cuwpIndex);
        void setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp);
        size_t addCuwp(const Chk::Cuwp & cuwp, bool fixUsageBeforeAdding = true, size_t excludedTriggerIndex = Chk::MaximumTriggers, size_t excludedTriggerActionIndex = Chk::Trigger::MaxActions);
        
        void fixCuwpUsage(size_t excludedTriggerIndex = Chk::MaximumTriggers, size_t excludedTriggerActionIndex = Chk::Trigger::MaxActions);
        bool cuwpUsed(size_t cuwpIndex);
        void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);

        size_t numTriggers();
        std::shared_ptr<Chk::Trigger> getTrigger(size_t triggerIndex);
        size_t addTrigger(std::shared_ptr<Chk::Trigger> trigger);
        void insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger);
        void deleteTrigger(size_t triggerIndex);
        void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);
        std::deque<Chk::TriggerPtr> replaceRange(size_t beginIndex, size_t endIndex, std::deque<Chk::TriggerPtr> & triggers);

        size_t numBriefingTriggers();
        std::shared_ptr<Chk::Trigger> getBriefingTrigger(size_t briefingTriggerIndex);
        size_t addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger);
        void insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger);
        void deleteBriefingTrigger(size_t briefingTriggerIndex);
        void moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo);

        size_t getSwitchNameStringId(size_t switchIndex);
        void setSwitchNameStringId(size_t switchIndex, size_t stringId);

        size_t addSound(size_t stringId);
        bool stringIsSound(size_t stringId);
        size_t getSoundStringId(size_t soundIndex);
        void setSoundStringId(size_t soundIndex, size_t soundStringId);

        bool locationUsed(size_t locationId);
        bool stringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All);
        bool gameStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All);
        bool editorStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All);
        void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed);
        void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void markUsedEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All);
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteLocation(size_t locationId);
        void deleteString(size_t stringId);

    private:
        Strings* strings; // For reading and updating sound paths, next scenario paths, text messages, leader board text, comments, and switch names
        Layers* layers; // For reading locations
        friend class Scenario;
        
        void set(std::unordered_map<SectionName, Section> & sections);
        void clear();
};

class Scenario;
using ScenarioPtr = std::shared_ptr<Scenario>;

class ScenarioSaver;
using ScenarioSaverPtr = std::shared_ptr<ScenarioSaver>;

class Scenario : ScenarioSaver
{
    public:
        Versions versions; // All version and validation related information
        Strings strings; // All string related information: map title and description, strings used by other sections
        Players players; // All player related information: slot owners, races, player colors, forces
        Layers layers; // All layers (stuff with coordinates): terrain, units, fog of war, sprites, doodads, locations
        Properties properties; // All property sheets: unit settings, unit availability, upgrade costs, upgrade leveling, technology costs, technology leveling
        Triggers triggers; // All scenario triggers and mission briefing triggers and their metadata: CUWPs, switches, and sounds
        
        Scenario(); // Construct empty map
        Scenario(Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64); // Construct new map
        
        virtual ~Scenario();

        bool empty();
        
        bool isProtected(); // Checks if map is protected
        bool hasPassword(); // Checks if the map has a password
        bool isPassword(const std::string & password); // Checks if this is the password the map has
        bool setPassword(const std::string & oldPass, const std::string & newPass); // Attempts to password-protect the map
        bool login(const std::string & password); // Attempts to login to the map

        bool read(std::istream & is); // Parses supplied scenario file data
        void write(std::ostream & os); // Writes all sections to the supplied stream

        std::vector<u8> serialize(); /** Writes all sections to a buffer in memory as it would to a .chk file
                                         includes a 4 byte "CHK " tag followed by a 4-byte size, followed by data */
        bool deserialize(Chk::SerializedChk* data); // "Opens" a serialized Scenario.chk file, data must be 8+ bytes
        
        bool changeVersionTo(Chk::Version version, bool lockAnywhere = true, bool autoDefragmentLocations = true);
        virtual void setTileset(Sc::Terrain::Tileset tileset);

    protected:
        virtual StrSynchronizerPtr getStrSynchronizer(); // Returns strings, may be overidden

        void addSection(Section section);
        void removeSection(const SectionName & sectionName);

        bool parsingFailed(const std::string & error);
        void clear();

    private:
        std::vector<Section> allSections; // Holds all the sections of a map
        std::array<u8, 7> tailData; // The 0-7 bytes just before the Scenario file ends, after the last valid section
        u8 tailLength; // 0 for no tail data, must be less than 8
        bool mapIsProtected; // Flagged if map is protected
        bool jumpCompress; // If true, the map will attempt to compress using jump sections when saving
};

class ScenarioAllocationFailure : std::bad_alloc
{
public:
    ScenarioAllocationFailure(const std::string & sectionName) : sectionName(sectionName) {}
    virtual const char* what() { return sectionName.c_str(); }

private:
    std::string sectionName;
    ScenarioAllocationFailure();
};

#endif