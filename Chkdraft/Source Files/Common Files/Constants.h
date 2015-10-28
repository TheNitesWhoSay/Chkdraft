#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Windows.h>
#include <cstdint>

/**
	Contains constants used throughout the program,
	such as identifiers, strings, and fonts
*/

typedef uint64_t u64;
typedef int64_t s64;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint16_t u16;
typedef int16_t s16;
typedef uint8_t u8;
typedef int8_t s8;

#define u8_max 255
#define u32_max 4294967295

#define ASCII_CHKD 1128811332 // "CHKD" = 67|72|75|68 = 0x43484B44 = 1128811332

#define NO_LOCATION 0xFFFF

extern const HFONT defaultFont;

extern const double zooms[];
#define NUM_ZOOMS 10

extern const u32 onOffMenuItems[];
extern const int numOnOffMenuItems;

#define DEFAULT_HOVER_TIME 10

extern const char* weaponNames[];
extern const int numWeaponNames;

extern const char* upgradeNames[];
extern const int numUpgradeNames;

extern const char* techNames[];
extern const int numTechNames;

extern const char* tilesetNames[];
#define NUM_TILESETS 8

extern const char* playerOwners[];
extern const int numPlayerOwners;

extern const char* playerRaces[];
extern const int numPlayerRaces;

extern const char* playerColors[];
extern const int numPlayerColors;

extern const char* triggerPlayers[];
extern const int numTriggerPlayers;

extern const char* triggerConditions[];
extern const int numTriggerConditions;

extern const char* triggerScores[];
extern const int numTriggerScores;

extern const COLORREF stringColors[];
extern const char* stringColorStrings[];
extern const char* stringColorPrefixes[];
#define NUM_STRING_COLORS 28

enum Tileset {
	TERRAIN_BADLANDS,
	TERRAIN_SPACE,
	TERRAIN_INSTALLATION,
	TERRAIN_ASH,
	TERRAIN_JUNGLE,
	TERRAIN_DESERT,
	TERRAIN_ICE,
	TERRAIN_TWILIGHT
};
extern const char* badlandsInitTerrain[];
extern const char* spaceInitTerrain[];
extern const char* installInitTerrain[];
extern const char* ashInitTerrain[];
extern const char* jungInitTerrain[];
extern const char* desertInitTerrain[];
extern const char* iceInitTerrain[];
extern const char* twilightInitTerrain[];
extern const int numBadlandsInitTerrain;
extern const int numSpaceInitTerrain;
extern const int numInstallationInitTerrain;
extern const int numAshInitTerrain;
extern const int numJungInitTerrain;
extern const int numDesertInitTerrain;
extern const int numIceInitTerrain;
extern const int numTwilightInitTerrain;

extern const char** initTerrains[];
extern const int numTilesetInitTerrains[];


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