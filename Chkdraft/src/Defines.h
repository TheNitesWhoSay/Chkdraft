// Where do I put defines? Should these be enums? I'm not good with computers :(

// Game Speeds
#define SLOWEST     167
#define SLOWER      111
#define SLOW        83
#define NORMAL      67
#define FAST        56
#define FASTER      48
#define FASTEST     42

// Tilesets
#define BADLANDS    0
#define PLATFORM    1
#define INSTALL     2
#define ASHWORLD    3
#define JUNGLE      4
#define DESERT      5
#define ICE         6
#define TWILIGHT    7

// Iscript Opcodes
#define PLAYFRAM          0x00
#define PLAYFRAMTILE      0x01
#define SETHORPOS         0x02
#define SETVERTPOS        0x03
#define SETPOS            0x04
#define WAIT              0x05
#define WAITRAND          0x06
#define GOTO              0x07
#define IMGOL             0x08
#define IMGUL             0x09
#define IMGOLORIG         0x0A
#define SWITCHUL          0x0B
//#define __0C              0x0C
#define IMGOLUSELO        0x0D
#define IMGULUSELO        0x0E
#define SPROL             0x0F
#define HIGHSPROL         0x10
#define LOWSPRUL          0x11
#define UFLUNSTABLE       0x12
#define SPRULUSELO        0x13
#define SPRUL             0x14
#define SPROLUSELO        0x15
#define END               0x16
#define SETFLIPSTATE      0x17
#define PLAYSND           0x18
#define PLAYSNDRAND       0x19
#define PLAYSNDBTWN       0x1A
#define DOMISSILEDMG      0x1B
#define ATTACKMELEE       0x1C
#define FOLLOWMAINGRAPHIC 0x1D
#define RANDCONDJMP       0x1E
#define TURNCCWISE        0x1F
#define TURNCWISE         0x20
#define TURN1CWISE        0x21
#define TURNRAND          0x22
#define SETSPAWNFRAME     0x23
#define SIGORDER          0x24
#define ATTACKWITH        0x25
#define ATTACK            0x26
#define CASTSPELL         0x27
#define USEWEAPON         0x28
#define MOVE              0x29
#define GOTOREPEATATTK    0x2A
#define ENGFRAME          0x2B
#define ENGSET            0x2C
#define __2D              0x2D
#define NOBRKCODESTART    0x2E
#define NOBRKCODEEND      0x2F
#define IGNOREREST        0x30
#define ATTKSHIFTPROJ     0x31
#define TMPRMGRAPHICSTART 0x32
#define TMPRMGRAPHICEND   0x33
#define SETFLDIRECT       0x34
#define CALL              0x35
#define RETURN            0x36
#define SETFLSPEED        0x37
#define CREATEGASOVERLAYS 0x38
#define PWRUPCONDJMP      0x39
#define TRGTRANGECONDJMP  0x3A
#define TRGTARCCONDJMP    0x3B
#define CURDIRECTCONDJMP  0x3C
#define IMGULNEXTID       0x3D
//#define __3E              0x3E
#define LIFTOFFCONDJMP    0x3F
#define WARPOVERLAY       0x40
#define ORDERDONE         0x41
#define GRDSPROL          0x42
//#define __43              0x43
#define DOGRDDAMAGE       0x44

// Animations
#define ANIM_INIT             0
#define ANIM_DEATH            1
#define ANIM_GNDATTKINIT      2
#define ANIM_AIRATTKINIT      3
#define ANIM_UNUSED1          4
#define ANIM_GNDATTKRPT       5
#define ANIM_AIRATTKRPT       6
#define ANIM_CASTSPELL        7
#define ANIM_GNDATTKTOIDLE    8
#define ANIM_AIRATTKTOIDLE    9
#define ANIM_UNUSED2          10
#define ANIM_WALKING          11
#define ANIM_WALKINGTOIDLE    12
#define ANIM_SPECIALSTATE1    13
#define ANIM_SPECIALSTATE2    14
#define ANIM_ALMOSTBUILT      15
#define ANIM_BUILT            16
#define ANIM_LANDING          17
#define ANIM_LIFTOFF          18
#define ANIM_ISWORKING        19
#define ANIM_WORKINGTOIDLE    20
#define ANIM_WARPIN           21
#define ANIM_UNUSED3          22
#define ANIM_INITTURRET       23
#define ANIM_DISABLE          24
#define ANIM_BURROW           25
#define ANIM_UNBURROW         26
#define ANIM_ENABLE           27

// Draw Functions
#define DRAW_NORMAL              0
#define  DRAW_UNKNOWN1           1
#define  DRAW_ENEMY_CLOAK        2
#define  DRAW_OWN_CLOAK          3
#define  DRAW_ALLY_CLOAK         4
#define DRAW_CLOAK               5
#define DRAW_CLOAKED             6
#define DRAW_DECLOAK             7
#define  DRAW_EMP                8
#define DRAW_EFFECT              9
#define DRAW_SHADOW              10
#define DRAW_HPFLOATDRAW         11
#define DRAW_WARP_IN             12
#define DRAW_SELECTION           13
#define DRAW_PLAYER_SIDE         14
#define  DRAW_SIZE_RECT          15
#define DRAW_HALLUCINATION       16
#define DRAW_WARP_FLASH          17 // DRAW_FIRE ?

// Remap
#define REMAP_NONE              0
#define REMAP_OFIRE             1
#define REMAP_GFIRE             2
#define REMAP_BFIRE             3
#define REMAP_BEXPL             4
#define REMAP_CLOAK             5 // Trans50
#define REMAP_RED               6
#define REMAP_GREEN             7

// Overlay Types
#define IMAGES_ATTACK_OVERLAY   0
#define IMAGES_DAMAGE_OVERLAY   1
#define IMAGES_SPECIAL_OVERLAY  2
#define IMAGES_LANDING_OVERLAY  3
#define IMAGES_LIFTING_OVERLAY  4


#define CONTROL_FLINGY          0
#define CONTROL_WEAPON          1
#define CONTROL_ISCRIPT         2

#define PROPERTY_FLYINGBUILDING 0x00000020
#define PROPERTY_CLOAKABLE      0x00000200
#define PROPERTY_RESOURCE       0x00002000
#define PROPERTY_BURROWABLE     0x00100000
#define PROPERTY_SPELLCASTER    0x00200000
#define PROPERTY_INVINCIBLE     0x20000000
#define PROPERTY_MECHANICAL     0x40000000

#define UPROP_BUILDING          0x00000001
#define UPROP_ADDON             0x00000002
#define UPROP_FLYER             0x00000004
#define UPROP_RESOURCEMINER     0x00000008
#define UPROP_SUBUNIT           0x00000010
#define UPROP_FLYINGBUILDING    0x00000020
#define UPROP_HERO              0x00000040
#define UPROP_REGENERATE        0x00000080
#define UPROP_ANIMIDLE          0x00000100
#define UPROP_CLOAKABLE         0x00000200
#define UPROP_2UNIT1EGG         0x00000400
#define UPROP_SINGLEENTITY      0x00000800
#define UPROP_RESOURCEDEPOT     0x00001000
#define UPROP_RESOURCECONTAINER 0x00002000
#define UPROP_ROBOTIC           0x00004000
#define UPROP_DETECTOR          0x00008000
#define UPROP_ORGANIC           0x00010000
#define UPROP_REQUIRESCREEP     0x00020000
#define UPROP_UNUSED            0x00040000
#define UPROP_REQUIRESPSI       0x00080000
#define UPROP_BURROWABLE        0x00100000
#define UPROP_SPELLCASTER       0x00200000
#define UPROP_CLOAKED           0x00400000
#define UPROP_PICKUPITEM        0x00800000
#define UPROP_IGNORESUPPLYCHECK 0x01000000
#define UPROP_MEDIUMOVERLAY     0x02000000
#define UPROP_LARGEOVERLAY      0x04000000
#define UPROP_BATTLEREACTIONS   0x08000000
#define UPROP_AUTOATTACK        0x10000000
#define UPROP_INVINCIBLE        0x20000000
#define UPROP_MECHANICAL        0x40000000
#define UPROP_PRODUCESUNITS     0x80000000

#define UAVAIL_NONNEUTRAL           0x0001
#define UAVAIL_UNITLISTING          0x0002
#define UAVAIL_MISSIONBRIEFING      0x0004
#define UAVAIL_PLAYERSETTINGS       0x0008
#define UAVAIL_ALLRACES             0x0010
#define UAVAIL_SETDOODADSTATE       0x0020
#define UAVAIL_NONLOCATIONTRIGGERS  0x0040
#define UAVAIL_UNITANDHEROSETTINGS  0x0080
#define UAVAIL_LOCATIONTRIGGERS     0x0100
#define UAVAIL_BROODWAR             0x0200

// unitStatus flags
#define USTATUS_COMPLETED            0x00000001
#define USTATUS_GROUNDBUILDING       0x00000002 // a building that is on the ground
#define USTATUS_INAIR                0x00000004
#define USTATUS_DISABLED             0x00000008 /**< Protoss Unpowered */
#define USTATUS_BURROWED             0x00000010
#define USTATUS_INBUILDING           0x00000020
#define USTATUS_INTRANSPORT          0x00000040
#define USTATUS_UNKNOWN7             0x00000080 /**< @todo Unknown */ // EDIT: found in target acquisition
#define USTATUS_REQUIRESDETECTION    0x00000100
#define USTATUS_CLOAKED              0x00000200
#define USTATUS_DOODADSTATESTHING    0x00000400
#define USTATUS_CLOAKINGFORFREE      0x00000800 /**< Requires no energy to cloak */
#define USTATUS_CANNOTRECEIVEORDERS  0x00001000
#define USTATUS_NOBRKCODESTART       0x00002000
#define USTATUS_UNKNOWN14            0x00004000
#define USTATUS_CANNOTATTACK         0x00008000
#define USTATUS_ISAUNIT              0x00010000 // canAttack? /**< @todo Unknown */
#define USTATUS_ISABUILDING          0x00020000
#define USTATUS_IGNORETILECOLLISION  0x00040000
#define USTATUS_UNKNOWN19            0x00080000
#define USTATUS_ISNORMAL             0x00100000 /**< 1 for "normal" units, 0 for hallucinated units */
#define USTATUS_NOCOLLIDE            0x00200000
#define USTATUS_UNKNOWN20            0x00400000
#define USTATUS_ISGATHERING          0x00800000
#define USTATUS_UNKNOWN21            0x01000000
#define USTATUS_UNKNOWN22            0x02000000 // Turret related
#define USTATUS_INVINCIBLE           0x04000000
#define USTATUS_HOLDINGPOSITION      0x08000000 // Set if the unit is currently holding position
#define USTATUS_SPEEDUPGRADE         0x10000000
#define USTATUS_COOLDOWNUPGRADE      0x20000000
#define USTATUS_ISHALLUCINATION      0x40000000 /**< 1 for hallucinated units, 0 for "normal" units */
#define USTATUS_ISSELFDESTRUCTING    0x80000000 // Set for when the unit is self-destructing (scarab, scourge, infested terran)
