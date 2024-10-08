#ifndef CONSTANTS_H
#define CONSTANTS_H

#ifndef enum_t
/**
    enum_t "enum type (scoped)" assumes the property of enum classes that encloses the enum values within a particular scope
    e.g. MyClass::MyEnum::Value cannot be accessed via MyClass::Value (as it could with regular enums) and potentially cause redefinition errors
    while avoiding the property of enum classes that removes the one-to-one relationship with the underlying type (which forces excessive type-casting)

    Usage:
    enum_t(name, type, {
        enumerator = constexpr,
        enumerator = constexpr,
        ...
    });
*/
#pragma warning(disable: 26812) // In the context of using enum_t, enum class is definitely not preferred, disable the warning in visual studios

/** enum_t "enum type (scoped)" documentation minimized for expansion visibility, see definition for description and usage */
#define enum_t(name, type, ...) struct name ## _ { enum type ## _ : type __VA_ARGS__; }; using name = name ## _::type ## _;
#endif

#include <cstdint>
#include <string>
#include <tuple>
#include <vector>

/**
    Contains constants used throughout the program,
    such as identifiers, strings, and fonts
*/

using u64 = std::uint64_t;
using s64 = std::int64_t;
using u32 = std::uint32_t;
using s32 = std::int32_t;
using u16 = std::uint16_t;
using s16 = std::int16_t;
using u8 = std::uint8_t;
using s8 = std::int8_t;

//constexpr u8 u8_max = UINT8_MAX;
//constexpr u16 u16_max = UINT16_MAX;
//constexpr u32 u32_max = UINT32_MAX;

// "CHKD" = 67|72|75|68 = 0x43484B44 = 1128811332
constexpr int ascii_chkd = 1128811332;

constexpr u32 defaultHoverTime = 50;

constexpr u16 NO_LOCATION = 0xFFFF;

extern const std::vector<double> defaultZooms;

extern const std::vector<u32> onOffMenuItems;

extern const std::vector<std::string> weaponNames;

extern const std::vector<std::string> upgradeNames;

extern const std::vector<std::string> techNames;

extern const std::vector<std::string> playerOwners;

extern const std::vector<std::string> playerRaces;

extern const std::vector<std::string> specialColors;

extern const std::vector<std::string> playerColors;

extern const std::vector<std::string> triggerPlayers;

extern const std::vector<std::string> briefingTriggerSlots;

extern const std::vector<std::string> triggerConditions;

extern const std::vector<std::string> triggerActions;

extern const std::vector<std::string> briefingTriggerActions;

extern const std::vector<std::string> triggerScores;

extern const std::vector<std::string> tilesetNames;


/** Used for identifying layers, should be equivilant
    to the zero-based position in the layer combobox */
enum class Layer {
    Terrain,
    Doodads,
    FogEdit,
    Locations,
    Units,
    Sprites,
    FogView,
    CutCopyPaste
};
enum class TerrainSubLayer {
    Isom,
    Rectangular
};

// Generally for trees...
//  - LPARAM First 4 bits: defines the item type
//  - LPARAM Last 28 bits: defines data specific to the type of item
// 'AND' with the following to retrieve either from the LPARAM
constexpr u32 TreeDataPortion = 0x0FFFFFFF;
constexpr u32 TreeTypePortion = 0xF0000000;

constexpr u32 TreeTypeRoot = (0x00000000); // Roots are AND'd with an associated layer (found in TREE_ITEM_DATA), must be 0
constexpr u32 TreeTypeCategory = (0x10000000); // Categories are AND'd with an associated layer (found in TREE_ITEM_DATA)
constexpr u32 TreeTypeIsom = (0x20000000);
constexpr u32 TreeTypeUnit = (0x30000000);
constexpr u32 TreeTypeLocation = (0x40000000);
constexpr u32 TreeTypeSprite = (0x50000000);
constexpr u32 TreeTypeDoodad = (0x60000000);
constexpr u32 TreeTypeUpgrade = (0x70000000);
constexpr u32 TreeTypeTech = (0x80000000);


/*###############################*\
## CHKDraft's Window Identifiers ##
## ID_CHKD through 65535         ##
\*###############################*/

/*###############################*\
## Application specific messages ##
## WM_APP through 0xBFFF         ##
\*###############################*/

#endif