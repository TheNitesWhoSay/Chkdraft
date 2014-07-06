#ifndef CONSTANTS_H
#define CONSTANTS_H
#include <Windows.h>

#ifndef u64
#define u64 unsigned long long
#endif
#ifndef s64
#define s64 signed long long
#endif
#ifndef u32
#define u32 unsigned long
#endif
#ifndef s32
#define s32 signed long
#endif
#ifndef u16
#define u16 unsigned short
#endif
#ifndef s16
#define s16 signed short
#endif
#ifndef u8
#define u8 unsigned char
#endif
#ifndef s8
#define s8 signed char
#endif

// "CHKD" = 67|72|75|68 = 0x43484B44 = 1128811332
#define ASCII_CHKD 1128811332

/**
	Contains constant variables used throughout the program,
	such as identifiers of GUI elements.
*/

#define NO_LOCATION 0xFFFF

extern HFONT defaultFont;

extern const double zooms[];
#define NUM_ZOOMS 10

extern const u32 onOffMenuItems[];
extern const int numOnOffMenuItems;

#define DEFAULT_HOVER_TIME 10

extern const char* tilesetNames[];
#define NUM_TILESETS 8

extern const char* playerOwners[];
extern const int numPlayerOwners;

extern const char* playerRaces[];
extern const int numPlayerRaces;

extern const char* playerColors[];
extern const int numPlayerColors;

extern const COLORREF stringColors[];
extern const int numStringColors;
extern const char* stringColorStrings[];
extern const char* stringColorPrefixes[];

extern const char* badlandsInitTerrain[];
extern const char* spaceInitTerrain[];
extern const char* installInitTerrain[];
extern const char* ashInitTerrain[];
extern const char* jungInitTerrain[];
extern const char* desertInitTerrain[];
extern const char* iceInitTerrain[];
extern const char* twilightInitTerrain[];
#define TERRAIN_BADLANDS	 0
#define TERRAIN_SPACE		 1
#define TERRAIN_INSTALLATION 2
#define TERRAIN_ASH			 3
#define TERRAIN_JUNGLE		 4
#define TERRAIN_DESERT		 5
#define TERRAIN_ICE			 6
#define TERRAIN_TWILIGHT	 7
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

/** values for extracting data types */

#define TYPE_SIZE_1 u8
#define TYPE_SIZE_2 u16
#define TYPE_SIZE_4 u32


/** Used for identifying layers, should be equivilant
	to the zero-based position in the layer combobox */
#define LAYER_TERRAIN                   0
#define LAYER_DOODADS                   1
#define LAYER_FOG                       2
#define LAYER_LOCATIONS                 3
#define LAYER_UNITS                     4
#define LAYER_SPRITES                   5
#define LAYER_VIEW_FOG                  6
#define LAYER_COPY_PASTE                7



/*###############################*\
## CHKDraft's Window Identifiers ##
## ID_CHKD through 65535		 ##
\*###############################*/

#define ID_CHKD 41000 /* Should be higher than any command ids defined in Resource.h
						 Using this constant directly in definitions seems to cause
						 errors, kept here for book-keeping sake. */

// New Map Window
#define IDD_COMBO_TRIGS					41001

// Main Window Items
#define IDR_MAIN_TOOLBAR                41002
#define IDR_MAIN_STATUS                 41003
#define IDR_MAIN_PLOT                   41004
#define IDR_MAIN_TREE                   41005
#define IDR_LEFT_BAR                    41006
#define IDR_MINIMAP                     41007
#define ID_COMBOBOX_LAYER               41008
#define ID_COMBOBOX_PLAYER              41009
#define ID_COMBOBOX_ZOOM                41010
#define ID_COMBOBOX_TERRAIN				41011

// Unit Properties Window
#define IDC_UNITLIST                    41011
#define IDC_HP_BUDDY                    41012
#define IDC_MP_BUDDY                    41013
#define IDC_SHIELD_BUDDY                41014
#define IDC_RESOURCES_BUDDY             41015
#define IDC_HANGER_BUDDY                41016
#define IDC_ID_BUDDY                    41017

// Map Settings Window

#define ID_MAPPROPERTIES				41018
#define ID_FORCES						41019
#define ID_UNITSETTINGS					41020
#define ID_UPGRADESETTINGS				41021
#define ID_TECHSETTINGS					41022
#define ID_STRINGEDITOR					41023
#define ID_WAVEDITOR					41024

#define ID_TAB_MAPPROPERTIES			0
#define ID_TAB_FORCES					1
#define ID_TAB_UNITSETTINGS				2
#define ID_TAB_UPGRADESETTINGS			3
#define ID_TAB_TECHSETTINGS				4
#define ID_TAB_STRINGEDITOR				5
#define ID_TAB_WAVEDITOR				6

// Map Properties Sub-Window
#define ID_EDIT_MAPTITLE				41025
#define ID_EDIT_MAPDESCRIPTION			41026
#define ID_CB_MAPTILESET				41027
#define ID_CB_NEWMAPTERRAIN				41028
#define ID_EDIT_NEWMAPWIDTH				41029
#define ID_EDIT_NEWMAPHEIGHT			41030
#define ID_BUTTON_APPLY					41031
#define ID_CB_P1OWNER					41032
#define ID_CB_P2OWNER					41033
#define ID_CB_P3OWNER					41034
#define ID_CB_P4OWNER					41035
#define ID_CB_P5OWNER					41036
#define ID_CB_P6OWNER					41037
#define ID_CB_P7OWNER					41038
#define ID_CB_P8OWNER					41039
#define ID_CB_P9OWNER					41040
#define ID_CB_P10OWNER					41041
#define ID_CB_P11OWNER					41042
#define ID_CB_P12OWNER					41043
#define ID_CB_P1RACE					41044
#define ID_CB_P2RACE					41045
#define ID_CB_P3RACE					41046
#define ID_CB_P4RACE					41047
#define ID_CB_P5RACE					41048
#define ID_CB_P6RACE					41049
#define ID_CB_P7RACE					41050
#define ID_CB_P8RACE					41051
#define ID_CB_P9RACE					41052
#define ID_CB_P10RACE					41053
#define ID_CB_P11RACE					41054
#define ID_CB_P12RACE					41055
#define ID_CB_P1COLOR					41056
#define ID_CB_P2COLOR					41057
#define ID_CB_P3COLOR					41058
#define ID_CB_P4COLOR					41059
#define ID_CB_P5COLOR					41060
#define ID_CB_P6COLOR					41061
#define ID_CB_P7COLOR					41062
#define ID_CB_P8COLOR					41063

// Map forces subwindow
#define ID_EDIT_F1NAME					41064
#define ID_EDIT_F2NAME					41065
#define ID_EDIT_F3NAME					41066
#define ID_EDIT_F4NAME					41067
#define ID_LB_F1PLAYERS					41068
#define ID_LB_F2PLAYERS					41069
#define ID_LB_F3PLAYERS					41070
#define ID_LB_F4PLAYERS					41071
#define ID_CHECK_F1ALLIED				41072
#define ID_CHECK_F2ALLIED				41073
#define ID_CHECK_F3ALLIED				41074
#define ID_CHECK_F4ALLIED				41075
#define ID_CHECK_F1RANDOM				41076
#define ID_CHECK_F2RANDOM				41077
#define ID_CHECK_F3RANDOM				41078
#define ID_CHECK_F4RANDOM				41079
#define ID_CHECK_F1VISION				41080
#define ID_CHECK_F2VISION				41081
#define ID_CHECK_F3VISION				41082
#define ID_CHECK_F4VISION				41083
#define ID_CHECK_F1AV					41084
#define ID_CHECK_F2AV					41085
#define ID_CHECK_F3AV					41086
#define ID_CHECK_F4AV					41087

// String Editor Subwindow
#define ID_DELETE_STRING				41088
#define ID_CHECK_EXTENDEDSTRING			41089
#define ID_COMPRESS_STRINGS				41090
#define ID_REPAIR_STRINGS				41091
#define ID_LB_STRINGS					41092
#define ID_STRING_GUIDE					41093
#define ID_TEXT_COLOR_FIRST				41094
#define ID_TEXT_COLOR_LAST				41122
#define ID_EDIT_STRING					41123
#define ID_PREVIEW_STRING				41124
#define ID_LB_STRINGUSE					41125


// MDI Window
#define ID_MDI_FIRSTCHILD               41200 // Should be higher than any other identifiers



/*###############################*\
## Application specific messages ##
## WM_APP through 0xBFFF		 ##
\*###############################*/

#define TILE_UPDATE                     (WM_APP+1)
#define DO_SCROLL						(WM_APP+2)
#define ADD_UNIT						(WM_APP+3)
#define SET_LIST_REDRAW					(WM_APP+4)
#define REPOPULATE_LIST					(WM_APP+5)
#define UPDATE_ENABLED_STATE			(WM_APP+6)
#define NOTIFY_BUFFER_CHANGE			(WM_APP+7)
#define NOTIFY_BUFFER_ADDITION			(WM_APP+8)
#define NOTIFY_BUFFER_DELETIONS			(WM_APP+9)
#define NOTIFY_BUFFER_MOVEMENT			(WM_APP+10)
#define NOTIFY_BUFFER_ACCESS			(WM_APP+11)
#define REFRESH_LOCATION				(WM_APP+12)
#define REFRESH_WINDOW					(WM_APP+13)
#define FLAG_UNDOABLE_CHANGE			(WM_APP+14) // wParam == mapId
#define UNFLAG_UNDOABLE_CHANGES			(WM_APP+15) // wParam == mapId
#define FLAG_LOCKED_CHANGE				(WM_APP+16) // wParam == mapId

#endif