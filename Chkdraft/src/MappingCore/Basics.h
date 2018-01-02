#ifndef BASICS_H
#define BASICS_H
#include <string>
#include <cstring>
#include <cstdint>
#include <vector>

#ifdef CHKDRAFT // Globally defined in project properties if this is used in CHKDraft
    void Debug();
    void DebugIf(bool condition);
    void NoDebug();
    void mb(const char* text); // Basic message box message
    void mb(int i, const char* text);
    void Error(const char* ErrorMessage); // Basic error message box
    bool RetryError(const char* text); // Error box with yes/no confirm
    extern void PrintError(const char* file, unsigned int line, const char* msg, ...); // Prints to LastError and LastErrorLoc
    void ShoutError(const char* file, unsigned int line, const char* msg, ...);
    #if defined(_MSC_VER) && !defined(__clang__)
    #define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, __VA_ARGS__) // Prints a detailed error
    #define CHKD_SHOUT(msg, ...) ShoutError(__FILE__, __LINE__, msg, __VA_ARGS__) // Shouts an error message
    #elif defined(__GNUC__) || defined(__clang__)
    #define CHKD_ERR(msg, ...) PrintError(__FILE__, __LINE__, msg, ##__VA_ARGS__) // Prints a detailed error
    #define CHKD_SHOUT(msg, ...) ShoutError(__FILE__, __LINE__, msg, ##__VA_ARGS__) // Shouts an error message
    #else
    #error Other compiler!
    #endif 
#else
    void IgnoreErr(const char* file, unsigned int line, const char* msg, ...); // Ignores an error message
    #if defined(_MSC_VER)
    #define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__)
    #define CHKD_SHOUT(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, __VA_ARGS__)
    #elif defined(__GNUC__)
    #define CHKD_ERR(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, ##__VA_ARGS__)
    #define CHKD_SHOUT(msg, ...) IgnoreErr(__FILE__, __LINE__, msg, ##__VA_ARGS__)
    #else
    #error Other compiler!
    #endif
#endif

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

#define BIT_0  0x1
#define BIT_1  0x2
#define BIT_2  0x4
#define BIT_3  0x8
#define BIT_4  0x10
#define BIT_5  0x20
#define BIT_6  0x40
#define BIT_7  0x80

#define BIT_8  0x100
#define BIT_9  0x200
#define BIT_10 0x400
#define BIT_11 0x800
#define BIT_12 0x1000
#define BIT_13 0x2000
#define BIT_14 0x4000
#define BIT_15 0x8000

#define BIT_16 0x10000
#define BIT_17 0x20000
#define BIT_18 0x40000
#define BIT_19 0x80000
#define BIT_20 0x100000
#define BIT_21 0x200000
#define BIT_22 0x400000
#define BIT_23 0x800000

#define BIT_24 0x1000000
#define BIT_25 0x2000000
#define BIT_26 0x4000000
#define BIT_27 0x8000000
#define BIT_28 0x10000000
#define BIT_29 0x20000000
#define BIT_30 0x40000000
#define BIT_31 0x80000000

#define x8BIT_0 0xFE
#define x8BIT_1 0xFD
#define x8BIT_2 0xFB
#define x8BIT_3 0xF7
#define x8BIT_4 0xEF
#define x8BIT_5 0xDF
#define x8BIT_6 0xBF
#define x8BIT_7 0x7F

#define x16BIT_0 0xFFFE
#define x16BIT_1 0xFFFD
#define x16BIT_2 0xFFFB
#define x16BIT_3 0xFFF7
#define x16BIT_4 0xFFEF
#define x16BIT_5 0xFFDF
#define x16BIT_6 0xFFBF
#define x16BIT_7 0xFF7F

#define x16BIT_8 0xFEFF
#define x16BIT_9 0xFDFF
#define x16BIT_10 0xFBFF
#define x16BIT_11 0xF7FF
#define x16BIT_12 0xEFFF
#define x16BIT_13 0xDFFF
#define x16BIT_14 0xBFFF
#define x16BIT_15 0x7FFF


#define x32BIT_0 0xFFFFFFFE
#define x32BIT_1 0xFFFFFFFD
#define x32BIT_2 0xFFFFFFFB
#define x32BIT_3 0xFFFFFFF7
#define x32BIT_4 0xFFFFFFEF
#define x32BIT_5 0xFFFFFFDF
#define x32BIT_6 0xFFFFFFBF
#define x32BIT_7 0xFFFFFF7F

#define x32BIT_8 0xFFFFFEFF
#define x32BIT_9 0xFFFFFDFF
#define x32BIT_10 0xFFFFFBFF
#define x32BIT_11 0xFFFFF7FF
#define x32BIT_12 0xFFFFEFFF
#define x32BIT_13 0xFFFFDFFF
#define x32BIT_14 0xFFFFBFFF
#define x32BIT_15 0xFFFF7FFF

#define x32BIT_16 0xFFFEFFFF
#define x32BIT_17 0xFFFDFFFF
#define x32BIT_18 0xFFFBFFFF
#define x32BIT_19 0xFFF7FFFF
#define x32BIT_20 0xFFEFFFFF
#define x32BIT_21 0xFFDFFFFF
#define x32BIT_22 0xFFBFFFFF
#define x32BIT_23 0xFF7FFFFF

#define x32BIT_24 0xFEFFFFFF
#define x32BIT_25 0xFDFFFFFF
#define x32BIT_26 0xFBFFFFFF
#define x32BIT_27 0xF7FFFFFF
#define x32BIT_28 0xEFFFFFFF
#define x32BIT_29 0xDFFFFFFF
#define x32BIT_30 0xBFFFFFFF
#define x32BIT_31 0x7FFFFFFF

const u32 u32Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
                        BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15,
                        BIT_16, BIT_17, BIT_18, BIT_19, BIT_20, BIT_21, BIT_22, BIT_23,
                        BIT_24, BIT_25, BIT_26, BIT_27, BIT_28, BIT_29, BIT_30, BIT_31 };

const u16 u16Bits[] = { BIT_0, BIT_1, BIT_2, BIT_3, BIT_4, BIT_5, BIT_6, BIT_7,
                        BIT_8, BIT_9, BIT_10, BIT_11, BIT_12, BIT_13, BIT_14, BIT_15 };

enum class SectionId : uint32_t {
    TYPE = 1162893652, VER = 542262614, IVER = 1380275785, IVE2 = 843404873,
    VCOD = 1146045270, IOWN = 1314344777, OWNR = 1380865871, ERA = 541151813,
    DIM = 541935940, SIDE = 1162103123, MTXM = 1297634381, PUNI = 1229870416,
    UPGR = 1380405333, PTEC = 1128617040, UNIT = 1414090325, ISOM = 1297044297,
    TILE = 1162627412, DD2 = 540165188, THG2 = 843532372, MASK = 1263747405,
    STR = 542266451, UPRP = 1347571797, UPUS = 1398100053, MRGN = 1313296973,
    TRIG = 1195987540, MBRF = 1179796045, SPRP = 1347571795, FORC = 1129467718,
    WAV = 542523735, UNIS = 1397313109, UPGS = 1397182549, TECS = 1396917588,
    SWNM = 1296979795, COLR = 1380732739, PUPx = 2018530640, PTEx = 2017809488,
    UNIx = 2018070101, UPGx = 2017939541, TECx = 2017674580,

    KSTR = 1381258059
};

enum class ForceFlags : uint8_t {
    None = 0,
    RandomizeStartLocation = BIT_0,
    Allied = BIT_1,
    AlliedVictory = BIT_2,
    SharedVision = BIT_3
};

enum class UnitEnabledState : uint8_t
{
    Default = 0,
    Enabled = 1,
    Disabled = 2
};

enum class NumericComparison : uint8_t
{
    AtLeast = 0,
    AtMost = 1,
    Exactly = 10
};

enum class NumericModifier : uint8_t
{
    Add = 8,
    Subtract = 9,
    SetTo = 7
};

enum class SwitchState : uint8_t
{
    Cleared = 3,
    Set = 2
};

enum class SwitchModifier : uint8_t
{
    Set = 4,
    Clear = 5,
    Toggle = 6,
    Randomize = 11
};

enum class UnitSettingsDataLoc {
    HitPoints = 228, ShieldPoints = 1140, Armor = 1596, BuildTime = 1824,
    MineralCost = 2280, GasCost = 2736, StringIds = 3192, BaseWeapon = 3648,
};
#define UnitSettingsDataLocBonusWeapon(isExpansion) (isExpansion?3908:3848)
enum class PlayerUnitSettingsDataLoc {
    GlobalAvailability = 2736, PlayerUsesDefault = 2964
};

#define UpgradeSettingsDataLocMineralCost(isExpansion) (isExpansion?62:46)
#define UpgradeSettingsDataLocMineralFactor(isExpansion) (isExpansion?184:138)
#define UpgradeSettingsDataLocGasCost(isExpansion) (isExpansion?306:230)
#define UpgradeSettingsDataLocGasFactor(isExpansion) (isExpansion?428:322)
#define UpgradeSettingsDataLocTimeCost(isExpansion) (isExpansion?550:414)
#define UpgradeSettingsDataLocTimeFactor(isExpansion) (isExpansion?672:506)

#define UpgradeSettingsDataLocPlayerMaxLevel(isExpansion, player) (isExpansion?(61*(u32)player):(46*(u32)player))
#define UpgradeSettingsDataLocPlayerStartLevel(isExpansion, player) (isExpansion?(732+61*(u32)player):(552+46*(u32)player))
#define UpgradeSettingsDataLocDefaultMaxLevel(isExpansion) (isExpansion?1464:1104)
#define UpgradeSettingsDataLocDefaultStartLevel(isExpansion) (isExpansion?1525:1150)
#define UpgradeSettingsDataLocPlayerUsesDefault(isExpansion, player) (isExpansion?(1586+61*(u32)player):(1196+46*(u32)player))

#define TechSettingsDataLocMineralCost(isExpansion) (isExpansion?44:24)
#define TechSettingsDataLocGasCost(isExpansion) (isExpansion?132:72)
#define TechSettingsDataLocTimeCost(isExpansion) (isExpansion?220:120)
#define TechSettingsDataLocEnergyCost(isExpansion) (isExpansion?308:168)

#define PlayerTechSettingsDataLocAvailableForPlayer(isExpansion, player) (isExpansion?(44*(u32)player):(24*(u32)player))
#define PlayerTechSettingsDataLocResearchedForPlayer(isExpansion, player) (isExpansion?(528+44*(u32)player):(288+24*(u32)player))
#define PlayerTechSettingsDataLocDefaultAvailability(isExpansion) (isExpansion?1056:576)
#define PlayerTechSettingsDataLocDefaultReserached(isExpansion) (isExpansion?1100:600)
#define PlayerTechSettingsDataLocPlayerUsesDefault(isExpansion, player) (isExpansion?(1144+44*(u32)player):(624+24*(u32)player))

extern const char* DefaultUnitDisplayName[233];

extern const char* LegacyTextTrigDisplayName[233];

extern const char* VirtualSoundFiles[];

constexpr s32 NumUnitNames = 233;
constexpr s32 NumRealUnits = 227;
constexpr s32 NumVirtualSounds = 1143;

#endif