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

extern const COLORREF stringColors[];
#define NUM_STRING_COLORS 28
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

#define UNIT_NAME_COLUMN  0
#define UNIT_OWNER_COLUMN 1
#define UNIT_XC_COLUMN	  2
#define UNIT_YC_COLUMN	  3
#define UNIT_INDEX_COLUMN 4

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

#define ID_CHKD 41000 /* Should be higher than any command ids defined in Resource.h
						 Using this constant directly in definitions seems to cause
						 errors, kept here for book-keeping sake. */

// New Map Window
#define IDD_COMBO_TRIGS					41001

// Main Window Items
#define IDR_MAIN_TOOLBAR                41002
#define IDR_MAIN_STATUS                 41003
#define IDR_MAIN_MDI					41004
#define IDR_MAIN_PLOT                   41005
#define IDR_MAIN_TREE                   41006
#define IDR_LEFT_BAR                    41007
#define IDR_MINIMAP                     41008
#define ID_COMBOBOX_LAYER               41009
#define ID_COMBOBOX_PLAYER              41010
#define ID_COMBOBOX_ZOOM                41011
#define ID_COMBOBOX_TERRAIN				41012

// Unit Properties Window
#define IDC_UNITLIST                    41013
#define IDC_HP_BUDDY                    41014
#define IDC_MP_BUDDY                    41015
#define IDC_SHIELD_BUDDY                41016
#define IDC_RESOURCES_BUDDY             41017
#define IDC_HANGER_BUDDY                41018
#define IDC_ID_BUDDY                    41019

// Map Settings Window

#define ID_MAPPROPERTIES				41020
#define ID_FORCES						41021
#define ID_UNITSETTINGS					41022
#define ID_UPGRADESETTINGS				41023
#define ID_TECHSETTINGS					41024
#define ID_STRINGEDITOR					41025
#define ID_WAVEDITOR					41026

#define ID_TAB_MAPPROPERTIES			0
#define ID_TAB_FORCES					1
#define ID_TAB_UNITSETTINGS				2
#define ID_TAB_UPGRADESETTINGS			3
#define ID_TAB_TECHSETTINGS				4
#define ID_TAB_STRINGEDITOR				5
#define ID_TAB_WAVEDITOR				6

// Map Properties Sub-Window
#define ID_EDIT_MAPTITLE				41027
#define ID_EDIT_MAPDESCRIPTION			41028
#define ID_CB_MAPTILESET				41029
#define ID_CB_NEWMAPTERRAIN				41030
#define ID_EDIT_NEWMAPWIDTH				41031
#define ID_EDIT_NEWMAPHEIGHT			41032
#define ID_BUTTON_APPLY					41033
#define ID_CB_P1OWNER					41034
#define ID_CB_P2OWNER					41035
#define ID_CB_P3OWNER					41036
#define ID_CB_P4OWNER					41037
#define ID_CB_P5OWNER					41038
#define ID_CB_P6OWNER					41039
#define ID_CB_P7OWNER					41040
#define ID_CB_P8OWNER					41041
#define ID_CB_P9OWNER					41042
#define ID_CB_P10OWNER					41043
#define ID_CB_P11OWNER					41044
#define ID_CB_P12OWNER					41045
#define ID_CB_P1RACE					41046
#define ID_CB_P2RACE					41047
#define ID_CB_P3RACE					41048
#define ID_CB_P4RACE					41049
#define ID_CB_P5RACE					41050
#define ID_CB_P6RACE					41051
#define ID_CB_P7RACE					41052
#define ID_CB_P8RACE					41053
#define ID_CB_P9RACE					41054
#define ID_CB_P10RACE					41055
#define ID_CB_P11RACE					41056
#define ID_CB_P12RACE					41057
#define ID_CB_P1COLOR					41058
#define ID_CB_P2COLOR					41059
#define ID_CB_P3COLOR					41060
#define ID_CB_P4COLOR					41061
#define ID_CB_P5COLOR					41062
#define ID_CB_P6COLOR					41063
#define ID_CB_P7COLOR					41064
#define ID_CB_P8COLOR					41065

// Map forces subwindow
#define ID_EDIT_F1NAME					41066
#define ID_EDIT_F2NAME					41067
#define ID_EDIT_F3NAME					41068
#define ID_EDIT_F4NAME					41069
#define ID_LB_F1PLAYERS					41070
#define ID_LB_F2PLAYERS					41071
#define ID_LB_F3PLAYERS					41072
#define ID_LB_F4PLAYERS					41073
#define ID_CHECK_F1ALLIED				41074
#define ID_CHECK_F2ALLIED				41075
#define ID_CHECK_F3ALLIED				41076
#define ID_CHECK_F4ALLIED				41077
#define ID_CHECK_F1RANDOM				41078
#define ID_CHECK_F2RANDOM				41079
#define ID_CHECK_F3RANDOM				41080
#define ID_CHECK_F4RANDOM				41081
#define ID_CHECK_F1VISION				41082
#define ID_CHECK_F2VISION				41083
#define ID_CHECK_F3VISION				41084
#define ID_CHECK_F4VISION				41085
#define ID_CHECK_F1AV					41086
#define ID_CHECK_F2AV					41087
#define ID_CHECK_F3AV					41088
#define ID_CHECK_F4AV					41089

// Unit Settings Subwindow
#define ID_UNITSETTINGSTREE				41090
#define ID_BUTTON_RESETALLUNITDEFAULTS	41091
#define ID_CHECK_USEUNITDEFAULTS		41092
#define ID_CHECK_ENABLEDBYDEFAULT		41093
#define ID_GROUP_UNITPROPERTIES			41094
#define ID_GROUP_GROUNDWEAPON			41095
#define ID_GROUP_AIRWEAPON				41096
#define ID_GROUP_UNITNAME				41097
#define ID_GROUP_UNITAVAILABILITY		41098
#define ID_TEXT_UNITHITPOINTS			41099
#define ID_EDIT_UNITHITPOINTS			41100
#define ID_EDIT_UNITHITPOINTSBYTE		41101
#define ID_TEXT_UNITSHIELDPOINTS		41102
#define ID_EDIT_UNITSHIELDPOINTS		41103
#define ID_TEXT_UNITARMOR				41104
#define ID_EDIT_UNITARMOR				41105
#define ID_TEXT_UNITBUILDTIME			41106
#define ID_EDIT_UNITBUILDTIME			41107
#define ID_TEXT_UNITMINERALCOST			41108
#define ID_EDIT_UNITMINERALCOST			41109
#define ID_TEXT_UNITGASCOST				41110
#define ID_EDIT_UNITGASCOST				41111
#define ID_TEXT_UNITGROUNDDAMAGE		41112
#define ID_EDIT_UNITGROUNDDAMAGE		41113
#define ID_TEXT_UNITGROUNDBONUS			41114
#define ID_EDIT_UNITGROUNDBONUS			41115
#define ID_TEXT_UNITAIRDAMAGE			41116
#define ID_EDIT_UNITAIRDAMAGE			41117
#define ID_TEXT_UNITAIRBONUS			41118
#define ID_EDIT_UNITAIRBONUS			41119
#define ID_CHECK_USEDEFAULTUNITNAME		41120
#define ID_EDIT_UNITNAME				41121

#define ID_TEXT_P1UNITAVAILABILITY		41122
#define ID_TEXT_P2UNITAVAILABILITY		41123
#define ID_TEXT_P3UNITAVAILABILITY		41124
#define ID_TEXT_P4UNITAVAILABILITY		41125
#define ID_TEXT_P5UNITAVAILABILITY		41126
#define ID_TEXT_P6UNITAVAILABILITY		41127
#define ID_TEXT_P7UNITAVAILABILITY		41128
#define ID_TEXT_P8UNITAVAILABILITY		41129
#define ID_TEXT_P9UNITAVAILABILITY		41130
#define ID_TEXT_P10UNITAVAILABILITY		41131
#define ID_TEXT_P11UNITAVAILABILITY		41132
#define ID_TEXT_P12UNITAVAILABILITY		41133

#define ID_DROP_P1UNITAVAILABILITY		41134
#define ID_DROP_P2UNITAVAILABILITY		41135
#define ID_DROP_P3UNITAVAILABILITY		41136
#define ID_DROP_P4UNITAVAILABILITY		41137
#define ID_DROP_P5UNITAVAILABILITY		41138
#define ID_DROP_P6UNITAVAILABILITY		41139
#define ID_DROP_P7UNITAVAILABILITY		41140
#define ID_DROP_P8UNITAVAILABILITY		41141
#define ID_DROP_P9UNITAVAILABILITY		41142
#define ID_DROP_P10UNITAVAILABILITY		41143
#define ID_DROP_P11UNITAVAILABILITY		41144
#define ID_DROP_P12UNITAVAILABILITY		41145

// Upgrade Settings Subwindow
#define ID_TREE_UPGRADES				41146
#define ID_CHECK_USEDEFAULTCOSTS		41147
#define ID_BUTTON_RESETUPGRADEDEFAULTS	41148
#define ID_GROUP_MINERALCOSTS			41149
#define ID_TEXT_MINERALBASECOST			41150
#define ID_EDIT_MINERALBASECOST			41151
#define ID_TEXT_MINERALUPGRADEFACTOR	41152
#define ID_EDIT_MINERALUPGRADEFACTOR	41153
#define ID_GROUP_GASCOSTS				41154
#define ID_TEXT_GASBASECOST				41155
#define ID_EDIT_GASBASECOST				41156
#define ID_TEXT_GASUPGRADEFACTOR		41157
#define ID_EDIT_GASUPGRADEFACTOR		41158
#define ID_GROUP_TIMECOSTS				41159
#define ID_TEXT_TIMEBASECOST			41160
#define ID_EDIT_TIMEBASECOST			41161
#define ID_TEXT_TIMEUPGRADEFACTOR		41162
#define ID_EDIT_TIMEUPGRADEFACTOR		41163
#define ID_GROUP_PLAYERSETTINGS			41164
#define ID_GROUP_DEFAULTSETTINGS		41165
#define ID_TEXT_DEFAULTSTARTLEVEL		41166
#define ID_EDIT_DEFAULTSTARTLEVEL		41167
#define ID_TEXT_DEFAULTMAXLEVEL			41168
#define ID_EDIT_DEFAULTMAXLEVEL			41169

#define ID_TEXT_UPGRADEP1				41170
#define ID_TEXT_UPGRADEP2				41171
#define ID_TEXT_UPGRADEP3				41172
#define ID_TEXT_UPGRADEP4				41173
#define ID_TEXT_UPGRADEP5				41174
#define ID_TEXT_UPGRADEP6				41175
#define ID_TEXT_UPGRADEP7				41176
#define ID_TEXT_UPGRADEP8				41177
#define ID_TEXT_UPGRADEP9				41178
#define ID_TEXT_UPGRADEP10				41179
#define ID_TEXT_UPGRADEP11				41180
#define ID_TEXT_UPGRADEP12				41181
#define ID_CHECK_DEFAULTUPGRADEP1		41182
#define ID_CHECK_DEFAULTUPGRADEP2		41183
#define ID_CHECK_DEFAULTUPGRADEP3		41184
#define ID_CHECK_DEFAULTUPGRADEP4		41185
#define ID_CHECK_DEFAULTUPGRADEP5		41186
#define ID_CHECK_DEFAULTUPGRADEP6		41187
#define ID_CHECK_DEFAULTUPGRADEP7		41188
#define ID_CHECK_DEFAULTUPGRADEP8		41189
#define ID_CHECK_DEFAULTUPGRADEP9		41190
#define ID_CHECK_DEFAULTUPGRADEP10		41191
#define ID_CHECK_DEFAULTUPGRADEP11		41192
#define ID_CHECK_DEFAULTUPGRADEP12		41193
#define ID_TEXT_P1STARTLEVEL			41194
#define ID_TEXT_P2STARTLEVEL			41195
#define ID_TEXT_P3STARTLEVEL			41196
#define ID_TEXT_P4STARTLEVEL			41197
#define ID_TEXT_P5STARTLEVEL			41198
#define ID_TEXT_P6STARTLEVEL			41199
#define ID_TEXT_P7STARTLEVEL			41200
#define ID_TEXT_P8STARTLEVEL			41201
#define ID_TEXT_P9STARTLEVEL			41202
#define ID_TEXT_P10STARTLEVEL			41203
#define ID_TEXT_P11STARTLEVEL			41204
#define ID_TEXT_P12STARTLEVEL			41205
#define ID_EDIT_P1STARTLEVEL			41206
#define ID_EDIT_P2STARTLEVEL			41207
#define ID_EDIT_P3STARTLEVEL			41208
#define ID_EDIT_P4STARTLEVEL			41209
#define ID_EDIT_P5STARTLEVEL			41210
#define ID_EDIT_P6STARTLEVEL			41211
#define ID_EDIT_P7STARTLEVEL			41212
#define ID_EDIT_P8STARTLEVEL			41213
#define ID_EDIT_P9STARTLEVEL			41214
#define ID_EDIT_P10STARTLEVEL			41215
#define ID_EDIT_P11STARTLEVEL			41216
#define ID_EDIT_P12STARTLEVEL			41217
#define ID_TEXT_P1MAXLEVEL				41218
#define ID_TEXT_P2MAXLEVEL				41219
#define ID_TEXT_P3MAXLEVEL				41220
#define ID_TEXT_P4MAXLEVEL				41221
#define ID_TEXT_P5MAXLEVEL				41222
#define ID_TEXT_P6MAXLEVEL				41223
#define ID_TEXT_P7MAXLEVEL				41224
#define ID_TEXT_P8MAXLEVEL				41225
#define ID_TEXT_P9MAXLEVEL				41226
#define ID_TEXT_P10MAXLEVEL				41227
#define ID_TEXT_P11MAXLEVEL				41228
#define ID_TEXT_P12MAXLEVEL				41229
#define ID_EDIT_P1MAXLEVEL				41230
#define ID_EDIT_P2MAXLEVEL				41231
#define ID_EDIT_P3MAXLEVEL				41232
#define ID_EDIT_P4MAXLEVEL				41233
#define ID_EDIT_P5MAXLEVEL				41234
#define ID_EDIT_P6MAXLEVEL				41235
#define ID_EDIT_P7MAXLEVEL				41236
#define ID_EDIT_P8MAXLEVEL				41237
#define ID_EDIT_P9MAXLEVEL				41238
#define ID_EDIT_P10MAXLEVEL				41239
#define ID_EDIT_P11MAXLEVEL				41240
#define ID_EDIT_P12MAXLEVEL				41241

// Tech Settings Subwindow
#define ID_TREE_TECHS					41241
#define ID_CHECK_DEFAULTTECHCOSTS		41242
#define ID_BUTTON_RESETTECHDEFAULTS		41243

#define ID_GROUP_TECHCOSTS				41244
#define ID_TEXT_MINERALCOSTS			41245
#define ID_EDIT_MINERALCOSTS			41246
#define ID_TEXT_GASCOSTS				41247
#define ID_EDIT_GASCOSTS				41248
#define ID_TEXT_TIMECOSTS				41249
#define ID_EDIT_TIMECOSTS				41250
#define ID_TEXT_ENERGYCOSTS				41251
#define ID_EDIT_ENERGYCOSTS				41252

#define ID_GROUP_DEFAULTPLAYERSETTINGS	41253
#define ID_RADIO_DEFAULTDISABLED		41254
#define ID_RADIO_DEFAULTENABLED			41255
#define ID_RADIO_DEFAULTRESEARCHED		41256

#define ID_GROUP_TECHPLAYERSETTINGS		41257
#define ID_CHECK_P1TECHDEFAULT			41258
#define ID_CHECK_P2TECHDEFAULT			41259
#define ID_CHECK_P3TECHDEFAULT			41260
#define ID_CHECK_P4TECHDEFAULT			41261
#define ID_CHECK_P5TECHDEFAULT			41262
#define ID_CHECK_P6TECHDEFAULT			41263
#define ID_CHECK_P7TECHDEFAULT			41264
#define ID_CHECK_P8TECHDEFAULT			41265
#define ID_CHECK_P9TECHDEFAULT			41266
#define ID_CHECK_P10TECHDEFAULT			41267
#define ID_CHECK_P11TECHDEFAULT			41268
#define ID_CHECK_P12TECHDEFAULT			41269

#define ID_DROP_P1TECHSETTINGS			41270
#define ID_DROP_P2TECHSETTINGS			41271
#define ID_DROP_P3TECHSETTINGS			41272
#define ID_DROP_P4TECHSETTINGS			41273
#define ID_DROP_P5TECHSETTINGS			41274
#define ID_DROP_P6TECHSETTINGS			41275
#define ID_DROP_P7TECHSETTINGS			41276
#define ID_DROP_P8TECHSETTINGS			41277
#define ID_DROP_P9TECHSETTINGS			41278
#define ID_DROP_P10TECHSETTINGS			41279
#define ID_DROP_P11TECHSETTINGS			41280
#define ID_DROP_P12TECHSETTINGS			41281

// String Editor Subwindow
#define ID_DELETE_STRING				41282
#define ID_CHECK_EXTENDEDSTRING			41283
#define ID_COMPRESS_STRINGS				41284
#define ID_REPAIR_STRINGS				41285
#define ID_LB_STRINGS					41286
#define ID_STRING_GUIDE					41287
#define ID_TEXT_COLOR_FIRST				41288
#define ID_TEXT_COLOR_LAST				41315
#define ID_EDIT_STRING					41316
#define ID_PREVIEW_STRING				41317
#define ID_LB_STRINGUSE					41318

// Wav Editor Subwindow
#define ID_TEXT_MAPSOUNDFILES			41319
#define ID_BUTTON_STOPSOUNDS			41320
#define ID_BUTTON_DELETESOUND			41321
#define ID_BUTTON_EXTRACTSOUND			41322
#define ID_BUTTON_PLAYSOUND				41323
#define ID_LB_MAPSOUNDS					41324
#define ID_TEXT_VIRTUALSOUNDS			41325
#define ID_BUTTON_PLAYVIRTUALSOUND		41326
#define ID_LB_VIRTUALSOUNDS				41327
#define ID_TEXT_SOUNDFILENAME			41328
#define ID_EDIT_SOUNDFILENAME			41329
#define ID_BUTTON_BROWSEFORSOUND		41330
#define ID_TEXT_COMPRESSIONLEVEL		41331
#define ID_DROP_COMPRESSIONQUALITY		41332
#define ID_CHECK_VIRTUALFILE			41333
#define ID_BUTTON_ADDFILE				41334

// Trig Editor
#define ID_NEWTABTAB					41335
#define ID_TRIGGERS						41336
#define ID_TEMPLATES					41337
#define ID_COUNTERS						41338
#define ID_CUWPS						41339
#define ID_SWITCHES						41340

#define ID_TAB_TRIGGERS					0
#define ID_TAB_TEMPLATES				1
#define ID_TAB_COUNTERS					2
#define ID_TAB_CUWPS					3
#define ID_TAB_SWITCHES					4

// MDI Window
#define ID_MDI_FIRSTCHILD               41800 // Should be higher than any other identifiers



/*###############################*\
## Application specific messages ##
## WM_APP through 0xBFFF		 ##
\*###############################*/

// All seem adequetly replaced through ClassWindows

#endif