#ifndef SCENARIO_H
#define SCENARIO_H
#include "basics.h"
#include "escape_strings.h"
#include "chk.h"
#include <optional>
#include <string>
#include <array>
#include <map>
#include <list>

/*
    Versions [VER, TYPE, IVER, IVE2, VCOD] - versioning and validation
    Strings [SPRP, STR, OSTR, KSTR] - map title and description, game and editor string data
    Players [SIDE, COLR, FORC, OWNR, IOWN] - players, color, force, and slot information
    Terrain [ERA, DIM, MTXM, TILE, ISOM] - tilesets, dimensions, and terrain
    Layers:Terrain [MASK, THG2, DD2, UNIT, MRGN] - fog of war, sprites, doodads, units, and locations
    Properties [UNIS, UNIx, PUNI, UPGS, UPGx, UPGR, PUPx, TECS, TECx, PTEC, PTEx] - unit, upgrade, and technology properties
    Triggers [UPRP, UPUS, TRIG, MBRF, SWNM, WAV] - triggers, briefing, and trigger/briefing specific data
*/

struct Scenario
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

    REFLECT(Scenario, version, type, iVersion, i2Version, validation, strings, editorStrings, editorStringOverrides, scenarioProperties,
        playerRaces, playerColors, customColors, forces, slotTypes, iownSlotTypes, sprites, doodads, units, locations,
        dimensions, tileset, tileFog, tiles, editorTiles, isomRects,
        unitAvailability, unitSettings, upgradeCosts, upgradeLeveling, techCosts, techAvailability,
        origUnitSettings, origUpgradeCosts, origUpgradeLeveling, origTechnologyCosts, origTechnologyAvailability,
        createUnitProperties, createUnitPropertiesUsed, triggers, briefingTriggers, switchNames, soundPaths, triggerExtensions, triggerGroupings)
    
    Scenario(); // Construct empty map
    Scenario(Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64); // Construct new map

    // Versioning API
    Chk::Version getVersion() const;
    bool isVersion(Chk::Version version) const;
    bool isOriginal() const;
    bool isHybrid() const;
    bool isExpansion() const;
    bool isHybridOrAbove() const;
    bool isRemastered() const;

    bool hasDefaultValidation() const;
    void setToDefaultValidation();

    // Strings API
    bool hasExtendedStrings() const;

    size_t getCapacity(Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getBytesUsed(Chk::Scope storageScope = Chk::Scope::Game);

    bool stringStored(size_t stringId, Chk::Scope storageScope = Chk::Scope::Either) const;
    void appendUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All) const;
    bool stringUsed(size_t stringId, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All, bool ensureStored = false) const;
    void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either, u32 userMask = Chk::StringUserFlag::All) const;
    void markValidUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope = Chk::Scope::Either, Chk::Scope storageScope = Chk::Scope::Either, u32 userMask = Chk::StringUserFlag::All) const;

    StrProp getProperties(size_t editorStringId) const;
    void setProperties(size_t editorStringId, const StrProp & strProp);
        
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
    std::optional<StringType> getString(size_t stringId, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const; // Gets the string at stringId with formatting based on StringType

    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    size_t findString(const StringType & str, Chk::Scope storageScope = Chk::Scope::Game) const;

    bool setCapacity(size_t stringCapacity, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);
        
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    size_t addString(const StringType & str, Chk::Scope storageScope = Chk::Scope::Game, bool autoDefragment = true);

    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    void replaceString(size_t stringId, const StringType & str, Chk::Scope storageScope = Chk::Scope::Game);

    void deleteUnusedStrings(Chk::Scope storageScope = Chk::Scope::Both);
    void deleteString(size_t stringId, Chk::Scope storageScope = Chk::Scope::Both, bool deleteOnlyIfUnused = true);
    void moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope = Chk::Scope::Game);
    size_t rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo = Chk::Scope::Editor, bool autoDefragment = true);

    std::vector<u8> & getStrTailData(); // Gets the data appended after the STR section
    size_t getStrTailDataOffset(); // Gets the offset tail data would be at within the STR section were it written right now
    size_t getInitialStrTailDataOffset() const; // Gets the offset tail data was at when it was initially read in
    size_t getStrBytePaddedTo() const; // Gets the current byte alignment setting for tailData (usually 4 for new StrSections, 0/none if existing tailData was read in)
    void setStrBytePaddedTo(size_t bytePaddedTo); // Sets the current byte alignment setting for tailData (only 2 and 4 are aligned, other values are ignored/treat tailData as unpadded)

    size_t getScenarioNameStringId(Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getScenarioDescriptionStringId(Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getForceNameStringId(Chk::Force force, Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::Game) const;
    size_t getLocationNameStringId(size_t locationId, Chk::Scope storageScope = Chk::Scope::Game) const;

    void setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
    void setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope = Chk::Scope::Game);
    void setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
    void setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::Game);
    void setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope = Chk::Scope::Game);
    void setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
    void setLocationNameStringId(size_t locationId, size_t locationNameStringId, Chk::Scope storageScope = Chk::Scope::Game);
        
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getScenarioName(Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getScenarioDescription(Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getForceName(Chk::Force force, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getUnitName(Sc::Unit::Type unitType, bool defaultIfNull = false, Chk::UseExpSection useExp = Chk::UseExpSection::Auto, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getSoundPath(size_t soundIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getSwitchName(size_t switchIndex, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
    std::optional<StringType> getLocationName(size_t locationId, Chk::Scope storageScope = Chk::Scope::EditorOverGame) const;
    template <typename StringType>
    std::optional<StringType> getComment(size_t triggerIndex) const;
    template <typename StringType>
    std::optional<StringType> getExtendedComment(size_t triggerIndex) const;
    template <typename StringType>
    std::optional<StringType> getExtendedNotes(size_t triggerIndex) const;

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
    template <typename StringType>
    void setExtendedComment(size_t triggerIndex, const StringType & comment, bool autoDefragment = true);
    template <typename StringType>
    void setExtendedNotes(size_t triggerIndex, const StringType & notes, bool autoDefragment = true);

    // Creates a viable internal data buffer for the string section using the methods in requestedCompressionFlags
    // If no configuration among requestedCompressionFlags is viable, additional methods through allowedCompressionFlags are added as neccessary
    // allowedCompressionFlags may be increased as neccessary if elevator.elevate() returns true
    void syncStringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags = Chk::StrCompressFlag::Unchanged, u32 allowedCompressionFlags = Chk::StrCompressFlag::Unchanged);
    void syncRemasteredStringsToBytes(std::vector<u8> & stringBytes);
    void syncKstringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags = Chk::StrCompressFlag::Unchanged, u32 allowedCompressionFlags = Chk::StrCompressFlag::Unchanged);
    
    void syncBytesToStrings(const std::vector<u8> & stringBytes);
    void syncRemasteredBytesToStrings(const std::vector<u8> & stringBytes);
    void syncBytesToKstrings(const std::vector<u8> & stringBytes);

    size_t loadString(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize);
    void loadKstring(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize);
    void upgradeKstrToCurrent();

    std::vector<std::optional<ScStr>> copyStrings(Chk::Scope storageScope = Chk::Scope::Game) const;
    void swapStrings(std::vector<std::optional<ScStr>> & strings, Chk::Scope storageScope = Chk::Scope::Game);

    bool defragment(Chk::Scope storageScope = Chk::Scope::Both, bool matchCapacityToUsage = true);
    void remapStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope);

    // Players API
    Sc::Player::SlotType getSlotType(size_t slotIndex, Chk::Scope scope = Chk::Scope::Game) const;
    void setSlotType(size_t slotIndex, Sc::Player::SlotType slotType, Chk::Scope scope = Chk::Scope::Both);

    Chk::Race getPlayerRace(size_t playerIndex) const;
    void setPlayerRace(size_t playerIndex, Chk::Race race);

    Chk::PlayerColor getPlayerColor(size_t slotIndex) const;
    void setPlayerColor(size_t slotIndex, Chk::PlayerColor color);

    Chk::Force getPlayerForce(size_t slotIndex) const;
    size_t getForceStringId(Chk::Force force) const;
    u8 getForceFlags(Chk::Force force) const;
    void setPlayerForce(size_t slotIndex, Chk::Force force);
    void setForceStringId(Chk::Force force, u16 forceStringId);
    void setForceFlags(Chk::Force force, u8 forceFlags);
    void appendForceStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask = Chk::StringUserFlag::All) const;
    bool forceStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedForceStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const;
    void remapForceStringIds(const std::map<u32, u32> & stringIdRemappings);
    void deleteForceString(size_t stringId);

    bool isUsingRemasteredColors();
    void upgradeToRemasteredColors();
    Chk::PlayerColorSetting getPlayerColorSetting(size_t playerIndex);
    void setPlayerColorSetting(size_t playerIndex, Chk::PlayerColorSetting setting);
    Chk::Rgb getPlayerCustomColor(size_t playerIndex);
    void setPlayerCustomColor(size_t playerIndex, Chk::Rgb rgb);
        
    // Terrain API
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

    Sc::Terrain::Tileset getTileset() const;
    void setTileset(Sc::Terrain::Tileset tileset);
    
    constexpr size_t getTileWidth() const { return dimensions.tileWidth; }
    constexpr size_t getTileHeight() const { return dimensions.tileHeight; }
    constexpr size_t getIsomWidth() const { return size_t(dimensions.tileWidth)/2 + 1; }
    constexpr size_t getIsomHeight() const { return size_t(dimensions.tileHeight) + 1; }
    size_t getPixelWidth() const;
    size_t getPixelHeight() const;
    void setTileWidth(u16 newTileWidth, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 leftEdge = 0);
    void setTileHeight(u16 newTileHeight, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 topEdge = 0);
    void setDimensions(u16 newTileWidth, u16 newTileHeight, u16 sizeValidationFlags = SizeValidationFlag::Default, s32 leftEdge = 0, s32 topEdge = 0);

    u16 getTile(size_t tileXc, size_t tileYc, Chk::Scope scope = Chk::Scope::Game) const;
    u16 getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope = Chk::Scope::Game) const;
    void setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Game);
    void setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope = Chk::Scope::Game);
        
    Chk::IsomRect & getIsomRect(size_t isomRectIndex);
    const Chk::IsomRect & getIsomRect(size_t isomRectIndex) const;
    inline const Chk::IsomRect & getIsomRect(Chk::IsomRect::Point point) const { return isomRects[point.y*getIsomWidth() + point.x]; }
    inline Chk::IsomRect & isomRectAt(Chk::IsomRect::Point point) { return isomRects[point.y*getIsomWidth() + point.x]; }
    
    bool placeIsomTerrain(Chk::IsomDiamond isomDiamond, size_t terrainType, size_t brushExtent, Chk::IsomCache & cache);
    void copyIsomFrom(const Scenario & sourceMap, int32_t xTileOffset, int32_t yTileOffset, bool undoable, Chk::IsomCache & destCache);
    void updateTilesFromIsom(Chk::IsomCache & cache);
    bool resizeIsom(int32_t xTileOffset, int32_t yTileOffset, size_t oldMapWidth, size_t oldMapHeight, bool fixBorders, Chk::IsomCache & cache);
    Chk::TileOccupationCache getTileOccupationCache(const Sc::Terrain::Tiles & tileset, const Sc::Unit & unitData) const;

    // Layers API
    void validateSizes(u16 sizeValidationFlags, u16 prevWidth, u16 prevHeight);
    void fixTerrainToDimensions();

    u8 getFog(size_t tileXc, size_t tileYc) const;
    inline u8 getFogPx(size_t pixelXc, size_t pixelYc) const;
    void setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers);
    inline void setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers);

    size_t numSprites() const;
    Chk::Sprite & getSprite(size_t spriteIndex);
    const Chk::Sprite & getSprite(size_t spriteIndex) const;
    size_t addSprite(const Chk::Sprite & sprite);
    void insertSprite(size_t spriteIndex, const Chk::Sprite & sprite);
    void deleteSprite(size_t spriteIndex);
    void moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo);
    void updateOutOfBoundsSprites();
    void removeOutOfBoundsSprites();

    size_t numDoodads() const;
    Chk::Doodad & getDoodad(size_t doodadIndex);
    const Chk::Doodad & getDoodad(size_t doodadIndex) const;
    size_t addDoodad(const Chk::Doodad & doodad);
    void insertDoodad(size_t doodadIndex, const Chk::Doodad & doodad);
    void deleteDoodad(size_t doodadIndex);
    void moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo);
    void removeOutOfBoundsDoodads();

    size_t numUnits() const;
    Chk::Unit & getUnit(size_t unitIndex);
    const Chk::Unit & getUnit(size_t unitIndex) const;
    size_t addUnit(const Chk::Unit & unit);
    void insertUnit(size_t unitIndex, const Chk::Unit & unit);
    void deleteUnit(size_t unitIndex);
    void moveUnit(size_t unitIndexFrom, size_t unitIndexTo);
    void updateOutOfBoundsUnits();
    void removeOutOfBoundsUnits();
        
    size_t numLocations() const;
    Chk::Location & getLocation(size_t locationId);
    const Chk::Location & getLocation(size_t locationId) const;
    size_t addLocation(const Chk::Location & location);
    void replaceLocation(size_t locationId, const Chk::Location & location);
    void deleteLocation(size_t locationId, bool deleteOnlyIfUnused = true);
    bool moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere = true);
    bool isBlank(size_t locationId) const;
    void downsizeOutOfBoundsLocations();

    void markNonZeroLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const;
    bool locationsFitOriginal(bool lockAnywhere = true, bool autoDefragment = true); // Checks if all locations fit in indexes < Chk::TotalOriginalLocations
    bool trimLocationsToOriginal(bool lockAnywhere = true, bool autoDefragment = true); // If possible, trims locations to indexes < Chk::TotalOriginalLocations
    void expandToScHybridOrExpansion();
        
    bool anywhereIsStandardDimensions(u16 prevWidth = 0, u16 prevHeight = 0) const;
    void matchAnywhereToDimensions();
        
    void appendLocationStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask = Chk::StringUserFlag::All) const;
    bool locationStringUsed(size_t stringId, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedLocationStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const;
    void remapLocationStringIds(const std::map<u32, u32> & stringIdRemappings);
    void deleteLocationString(size_t stringId);

    // Properties API
    bool useExpansionUnitSettings(Chk::UseExpSection useExp, Sc::Weapon::Type weaponType = Sc::Weapon::Type::GaussRifle_Normal) const;
    bool unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u32 getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u8 getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;

    void setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

    bool isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex) const;
    bool isUnitDefaultBuildable(Sc::Unit::Type unitType) const;
    bool playerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex) const;
    void setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable);
    void setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable);
    void setPlayerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault);
    void setUnitsToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);
        
    bool useExpansionUpgradeCosts(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType = Sc::Upgrade::Type::TerranInfantryArmor) const;
    bool upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;

    void setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);

    bool useExpansionUpgradeLeveling(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType = Sc::Upgrade::Type::TerranInfantryArmor) const;
    size_t getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    size_t getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    size_t getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    size_t getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    bool playerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    void setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setUpgradesToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);
        
    bool useExpansionTechCosts(Chk::UseExpSection useExp, Sc::Tech::Type techType = Sc::Tech::Type::StimPacks) const;
    bool techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    u16 getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;

    void setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
        
    bool useExpansionTechAvailability(Chk::UseExpSection useExp, Sc::Tech::Type techType = Sc::Tech::Type::StimPacks) const;
    bool techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    bool techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    bool techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    bool techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;
    bool playerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp = Chk::UseExpSection::Auto) const;

    void setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setPlayerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp = Chk::UseExpSection::Auto);
    void setTechsToDefault(Chk::UseExpSection useExp = Chk::UseExpSection::Both);
        
    void appendUnitStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask = Chk::StringUserFlag::All) const;
    bool unitStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedUnitStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const;
    void remapUnitStringIds(const std::map<u32, u32> & stringIdRemappings);
    void deleteUnitString(size_t stringId);

    // Triggers API
    Chk::Cuwp getCuwp(size_t cuwpIndex) const;
    void setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp);
    size_t addCuwp(const Chk::Cuwp & cuwp, bool fixUsageBeforeAdding = true, size_t excludedTriggerIndex = Chk::MaximumTriggers, size_t excludedTriggerActionIndex = Chk::Trigger::MaxActions);
        
    void fixCuwpUsage(size_t excludedTriggerIndex = Chk::MaximumTriggers, size_t excludedTriggerActionIndex = Chk::Trigger::MaxActions);
    bool cuwpUsed(size_t cuwpIndex) const;
    void setCuwpUsed(size_t cuwpIndex, bool cuwpUsed);

    size_t numTriggers() const;
    Chk::Trigger & getTrigger(size_t triggerIndex);
    const Chk::Trigger & getTrigger(size_t triggerIndex) const;
    size_t addTrigger(const Chk::Trigger & trigger);
    void insertTrigger(size_t triggerIndex, const Chk::Trigger & trigger);
    void deleteTrigger(size_t triggerIndex);
    void moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo);
    std::vector<Chk::Trigger> replaceTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & triggers);
        
    bool hasTriggerExtension(size_t triggerIndex) const;
    Chk::ExtendedTrigData & getTriggerExtension(size_t triggerIndex, bool addIfNotFound = false);
    const Chk::ExtendedTrigData & getTriggerExtension(size_t triggerIndex) const;
    void removeTriggersExtension(size_t triggerIndex);
    void deleteTriggerExtension(size_t triggerExtensionIndex);
    void fixTriggerExtensions();

    size_t getCommentStringId(size_t triggerIndex) const;
    size_t getExtendedCommentStringId(size_t triggerIndex) const;
    void setExtendedCommentStringId(size_t triggerIndex, size_t stringId);
    size_t getExtendedNotesStringId(size_t triggerIndex) const;
    void setExtendedNotesStringId(size_t triggerIndex, size_t stringId);

    size_t numBriefingTriggers() const;
    Chk::Trigger & getBriefingTrigger(size_t briefingTriggerIndex);
    const Chk::Trigger & getBriefingTrigger(size_t briefingTriggerIndex) const;
    size_t addBriefingTrigger(const Chk::Trigger & briefingTrigger);
    void insertBriefingTrigger(size_t briefingTriggerIndex, const Chk::Trigger & briefingTrigger);
    void deleteBriefingTrigger(size_t briefingTriggerIndex);
    void moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo);
    std::vector<Chk::Trigger> replaceBriefingTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & triggers);

    size_t addSound(size_t stringId);
    bool stringIsSound(size_t stringId) const;
    size_t getSoundStringId(size_t soundIndex) const;
    void setSoundStringId(size_t soundIndex, size_t soundStringId);

    bool triggerSwitchUsed(size_t switchId) const;
    bool triggerLocationUsed(size_t locationId) const;
    void appendTriggerStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope = Chk::Scope::Game, u32 userMask = Chk::StringUserFlag::All) const;
    bool triggerStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const;
    bool triggerGameStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::All) const;
    bool triggerEditorStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedTriggerLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const;
    void markUsedTriggerStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedTriggerGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::All) const;
    void markUsedTriggerEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask = Chk::StringUserFlag::All) const;
    void remapTriggerLocationIds(const std::map<u32, u32> & locationIdRemappings);
    void remapTriggerStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope);
    void deleteTriggerLocation(size_t locationId);
    void deleteTriggerString(size_t stringId, Chk::Scope storageScope);

    // Misc API
    bool empty() const;
        
    bool isProtected() const; // Checks if map is protected
    bool hasPassword() const; // Checks if the map has a password
    bool isPassword(const std::string & password) const; // Checks if this is the password the map has
    bool setPassword(const std::string & oldPass, const std::string & newPass); // Attempts to password-protect the map
    bool login(const std::string & password) const; // Attempts to login to the map
        
    void read(std::istream & is, Chk::SectionName sectionName, Chk::SectionSize sectionSize); // Parse the given section
    bool read(std::istream & is); // Parses supplied scenario file data
    
    struct Section {
        Chk::SectionName sectionName;
        std::optional<std::vector<u8>> sectionData {}; // If not present, section data is found in the fields of Scenario
    };
    
    void writeSection(std::ostream & os, const Section & section, bool includeHeader = true);
    void writeSection(std::ostream & os, Chk::SectionName sectionName, bool includeHeader = true);

    void write(std::ostream & os); // Writes all sections to the supplied stream

    std::vector<u8> serialize(); /** Writes all sections to a buffer in memory as it would to a .chk file
                                     includes a 4 byte "CHK " tag followed by a 4-byte size, followed by data */
    bool deserialize(Chk::SerializedChk* data); // "Opens" a serialized Scenario.chk file, data must be 8+ bytes

    bool hasSection(SectionName sectionName) const;
    Section & addSaveSection(Section section); // Adds a section to save sections if not already present
    void addSaveSection(Chk::SectionName sectionName); // Adds a section to save sections if not already present
    void removeSaveSection(Chk::SectionName sectionName); // Removes a section from save sections if present
    void updateSaveSections();
    bool changeVersionTo(Chk::Version version, bool lockAnywhere = true, bool autoDefragmentLocations = true);

protected:
    bool parsingFailed(const std::string & error);
    void clear();

private:
    std::list<Section> saveSections {}; // Maintains the order of sections in the map and stores data for any sections that are not parsed
    std::array<u8, 7> tailData {}; // The 0-7 bytes just before the Scenario file ends, after the last valid section
    u8 tailLength {0}; // 0 for no tail data, must be less than 8
    mutable bool mapIsProtected {false}; // Flagged if map is protected
    bool jumpCompress {false}; // If true, the map will attempt to compress using jump sections when saving

    size_t strBytePaddedTo {0}; // If 2, or 4, it's padded to the nearest 2 or 4 byte boundary; no other value has any effect; 4 by default, 0 if "read" is called and any tailData is found
    size_t initialStrTailDataOffset {0}; // The offset at which strTailData started when the STR section was read, "0" if "read" is never called or there was no tailData
    std::vector<u8> strTailData {}; // Any data that comes after the regular STR section data, and after any padding
        
    static const std::vector<u32> compressionFlagsProgression;


    // ISOM helpers
    inline uint16_t getTileValue(size_t tileX, size_t tileY) const { return editorTiles[tileY*getTileWidth() + tileX]; }
    inline uint16_t getCentralIsomValue(Chk::IsomRect::Point point) const { return isomRects[point.y*getIsomWidth() + point.x].left >> 4; }
    inline bool centralIsomValueModified(Chk::IsomRect::Point point) const { return isomRects[point.y*getIsomWidth() + point.x].isLeftModified(); }
    constexpr bool isInBounds(Chk::IsomRect::Point point) const { return point.x < getIsomWidth() && point.y < getIsomHeight(); }

    void addIsomUndo(Chk::IsomRect::Point point, Chk::IsomCache & cache);
    bool diamondNeedsUpdate(Chk::IsomDiamond isomDiamond) const;
    void setIsomValue(Chk::IsomRect::Point isomDiamond, Sc::Isom::Quadrant shapeQuadrant, uint16_t isomValue, bool undoable, Chk::IsomCache & cache);
    void setDiamondIsomValues(Chk::IsomDiamond isomDiamond, uint16_t isomValue, bool undoable, Chk::IsomCache & cache);

    struct IsomNeighbors
    {
        struct BestMatch
        {
            uint16_t isomValue = 0;
            uint16_t matchCount = 0;
        };

        struct NeighborQuadrant
        {
            Sc::Isom::LinkId linkId = Sc::Isom::LinkId::None;
            uint16_t isomValue = 0;
            bool modified = false;
        };
        
        NeighborQuadrant upperLeft {};
        NeighborQuadrant upperRight {};
        NeighborQuadrant lowerRight {};
        NeighborQuadrant lowerLeft {};

        uint8_t maxModifiedOfFour = 0;
        BestMatch bestMatch {};

        constexpr NeighborQuadrant & operator[](Sc::Isom::Quadrant i) {
            switch ( i ) {
                case Sc::Isom::Quadrant::TopLeft: return upperLeft;
                case Sc::Isom::Quadrant::TopRight: return upperRight;
                case Sc::Isom::Quadrant::BottomRight: return lowerRight;
                default: /*Quadrant::BottomLeft*/ return lowerLeft;
            }
        }

        constexpr NeighborQuadrant & operator[](size_t i) { return (*this)[Sc::Isom::Quadrant(i)]; }
    };
    void loadNeighborInfo(Chk::IsomDiamond isomDiamond, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const;
    uint16_t countNeighborMatches(const Sc::Isom::ShapeLinks & shapeLinks, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const;
    void searchForBestMatch(uint16_t startingTerrainType, IsomNeighbors & neighbors, Chk::IsomCache & cache) const;
    std::optional<uint16_t> findBestMatchIsomValue(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache) const;
    void radiallyUpdateTerrain(bool undoable, std::deque<Chk::IsomDiamond> & diamondsToUpdate, Chk::IsomCache & cache);

    void updateTileFromIsom(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache);
};

#endif