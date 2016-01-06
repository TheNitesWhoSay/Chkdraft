#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Windows.h>

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

constexpr u8 u8_max = UINT8_MAX;
constexpr u16 u16_max = UINT16_MAX;
constexpr u32 u32_max = UINT32_MAX;

// "CHKD" = 67|72|75|68 = 0x43484B44 = 1128811332
constexpr int ascii_chkd = 67 | 72 | 75 | 68; 

#define DEFAULT_HOVER_TIME 10

#define NO_LOCATION 0xFFFF

extern const HFONT defaultFont;

extern const double zooms[];
#define NUM_ZOOMS 10

extern const std::vector<u32> onOffMenuItems;

extern const std::vector<std::string> weaponNames;

extern const std::vector<std::string> upgradeNames;

extern const std::vector<std::string> techNames;

extern const std::vector<std::string> playerOwners;

extern const std::vector<std::string> playerRaces;

extern const std::vector<std::string> playerColors;

extern const std::vector<std::string> triggerPlayers;

extern const std::vector<std::string> triggerConditions;

extern const std::vector<std::string> triggerActions;

extern const std::vector<std::string> triggerScores;

extern const std::vector<std::tuple<COLORREF, std::string, std::string>> strColors;
//bad design.....
#define STRCOLORS_SIZE 28

enum Tileset
{
	TERRAIN_BADLANDS,
	TERRAIN_SPACE,
	TERRAIN_INSTALLATION,
	TERRAIN_ASH,
	TERRAIN_JUNGLE,
	TERRAIN_DESERT,
	TERRAIN_ICE,
	TERRAIN_TWILIGHT
};

extern const std::vector<std::string> tilesetNames;

extern const std::vector<std::string> badlandsInitTerrain;
extern const std::vector<std::string> spaceInitTerrain;
extern const std::vector<std::string> installInitTerrain;
extern const std::vector<std::string> ashInitTerrain;
extern const std::vector<std::string> jungInitTerrain;
extern const std::vector<std::string> desertInitTerrain;
extern const std::vector<std::string> iceInitTerrain;
extern const std::vector<std::string> twilightInitTerrain;

extern const std::vector<std::vector<std::string>> initTerrains;


/** Values for extracting specific data types from raw bytes */

#define TYPE_SIZE_1 u8
#define TYPE_SIZE_2 u16
#define TYPE_SIZE_4 u32


/** Used for identifying layers, should be equivilant
	to the zero-based position in the layer combobox */
enum Layers {
	LAYER_TERRAIN,
	LAYER_DOODADS,
	LAYER_FOG,
	LAYER_LOCATIONS,
	LAYER_UNITS,
	LAYER_SPRITES,
	LAYER_VIEW_FOG,
	LAYER_COPY_PASTE
};

// Generally for trees...
//	- LPARAM First 4 bits: defines the item type
//  - LPARAM Last 28 bits: defines data specific to the type of item
// 'AND' with the following to retrieve either from the LPARAM
#define TREE_ITEM_DATA (0x0FFFFFFF)
#define TREE_ITEM_TYPE (0xF0000000)

#define TREE_TYPE_ROOT			(0x00000000) // Roots are AND'd with an associated layer (found in TREE_ITEM_DATA), must be 0
#define TREE_TYPE_CATEGORY		(0x10000000) // Categories are AND'd with an associated layer (found in TREE_ITEM_DATA)
#define TREE_TYPE_ISOM			(0x20000000)
#define TREE_TYPE_UNIT			(0x30000000)
#define TREE_TYPE_LOCATION		(0x40000000)
#define TREE_TYPE_SPRITE		(0x50000000)
#define TREE_TYPE_DOODAD		(0x60000000)
#define TREE_TYPE_UPGRADE		(0x70000000)
#define TREE_TYPE_TECH			(0x80000000)


/*###############################*\
## CHKDraft's Window Identifiers ##
## ID_CHKD through 65535		 ##
\*###############################*/

enum {
	ID_CHKD = 41000, // Should be higher than any window ids defined in Resource.h
	ID_FIRST		 // First identifier used by a set of sub-windows in Chkdraft
};

// Main Window Identifiers
enum MainWindow {
	ID_COMBOBOX_LAYER = ID_FIRST,
	ID_COMBOBOX_PLAYER,
	ID_COMBOBOX_ZOOM,
	ID_COMBOBOX_TERRAIN,
	IDR_MAIN_TOOLBAR,
	IDR_MAIN_STATUS,
	IDR_MAIN_MDI,
	IDR_MAIN_PLOT,
	IDR_MAIN_TREE,
	IDR_LEFT_BAR,
	IDR_MINIMAP,
	ID_MDI_FIRSTCHILD // Keep this higher than all other main window identifiers
};


/*###############################*\
## Application specific messages ##
## WM_APP through 0xBFFF		 ##
\*###############################*/

// In ListBoxControl.h
// LBN_DBLCLKITEM = (WM_APP+1)

#endif