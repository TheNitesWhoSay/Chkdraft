#ifndef SCDATA_H
#define SCDATA_H
#include "Basics.h"
#include "Buffer.h"
#include "MpqFile.h"
#include "SystemIO.h"
#include "Sc.h"
#include <unordered_map>
#include <string>
#include <tuple>
#include <map>

// TOOD: Finish Sc.h and delete this file

struct UPGRADEDAT
{
    u16 MineralCost;
    u16 MineralFactor;
    u16 VespeneCost;
    u16 VespeneFactor;
    u16 TimeCost;
    u16 TimeFactor;
    u16 Unknown;
    u16 Icon;
    u16 Label;
    u8 Race;
    u8 MaxRepeats;
    u8 BroodWarSpecific;
};

struct TECHDAT
{
    u16 MineralCost;
    u16 VespeneCost;
    u16 ResearchTime;
    u16 EnergyCost;
    u32 Unknown;
    u16 Icon;
    u16 Label;
    u8 Race;
    u8 Unused;
    u8 BroodWar;
};

struct UNITDAT
{
    u8 Graphics;
    u16 Subunit1;
    u16 Subunit2;
    u16 Infestation; // ID 106-201 only
    u32 ConstructionAnimation;
    u8 UnitDirection;
    u8 ShieldEnable;
    u16 ShieldAmount;
    u32 HitPoints;
    u8 ElevationLevel;
    u8 Unknown;
    u8 Sublabel;
    u8 CompAIIdle;
    u8 HumanAIIdle;
    u8 ReturntoIdle;
    u8 AttackUnit;
    u8 AttackMove;
    u8 GroundWeapon;
    u8 MaxGroundHits;
    u8 AirWeapon;
    u8 MaxAirHits;
    u8 AIInternal;
    u32 SpecialAbilityFlags;
    u8 TargetAcquisitionRange;
    u8 SightRange;
    u8 ArmorUpgrade;
    u8 UnitSize;
    u8 Armor;
    u8 RightClickAction;
    u16 ReadySound; // ID 0-105 only
    u16 WhatSoundStart;
    u16 WhatSoundEnd;
    u16 PissSoundStart; // ID 0-105 only
    u16 PissSoundEnd; // ID 0-105 only
    u16 YesSoundStart; // ID 0-105 only
    u16 YesSoundEnd; // ID 0-105 only
    u16 StarEditPlacementBoxWidth;
    u16 StarEditPlacementBoxHeight;
    u16 AddonHorizontal; // ID 106-201 only
    u16 AddonVertical; // ID 106-201 only
    u16 UnitSizeLeft;
    u16 UnitSizeUp;
    u16 UnitSizeRight;
    u16 UnitSizeDown;
    u16 Portrait;
    u16 MineralCost;
    u16 VespeneCost;
    u16 BuildTime;
    u16 Unknown1;
    u8 StarEditGroupFlags;
    u8 SupplyProvided;
    u8 SupplyRequired;
    u8 SpaceRequired;
    u8 SpaceProvided;
    u16 BuildScore;
    u16 DestroyScore;
    u16 UnitMapString;
    u8 BroodWarUnitFlag;
    u16 StarEditAvailabilityFlags;
};

struct WEAPONDAT
{
    u16 Label;
    u32 Graphics;
    u8 Unused;
    u16 TargetFlags;
    u32 MinimumRange;
    u32 MaximumRange;
    u8 DamageUpgrade;
    u8 WeaponType;
    u8 WeaponBehavior;
    u8 RemoveAfter;
    u8 WeaponEffect;
    u16 InnerSplashRadius;
    u16 MediumSplashRadius;
    u16 OuterSplashRadius;
    u16 DamageAmount;
    u16 DamageBonus;
    u8 WeaponCooldown;
    u8 DamageFactor;
    u8 AttackAngle;
    u8 LaunchSpin;
    u8 ForwardOffset;
    u8 UpwardOffset;
    u16 TargetErrorMessage;
    u16 Icon;
};

struct FLINGYDAT
{
    u16 Sprite;
    u32 TopSpeed;
    u16 Acceleration;
    u32 HaltDistance;
    u8 TurnRadius;
    u8 Unused;
    u8 MoveControl;
};

struct SPRITEDAT
{
    u16 ImageFile;
    u8 HealthBar;
    u8 Unknown;
    u8 IsVisible;
    u8 SelectionCircleImage;
    u8 SelectionCircleOffset;
};

class Upgrades
{
public:
    Upgrades();
    virtual ~Upgrades();
    bool LoadUpgrades(const std::vector<MpqFilePtr> & orderedSourceFiles);
    UPGRADEDAT* UpgradeDat(u8 id);

private:
    UPGRADEDAT upgrade[61];
};

class Techs
{
public:
    Techs();
    virtual ~Techs();
    bool LoadTechs(const std::vector<MpqFilePtr> & orderedSourceFiles);
    TECHDAT* TechDat(u8 id);

private:
    TECHDAT tech[44];
};

class Units
{
public:
    Units();
    virtual ~Units();
    bool LoadUnits(const std::vector<MpqFilePtr> & orderedSourceFiles);
    UNITDAT* UnitDat(u16 id);

private:
    UNITDAT unit[228];
};

class Weapons
{
public:
    Weapons();
    virtual ~Weapons();
    bool LoadWeapons(const std::vector<MpqFilePtr> & orderedSourceFiles);
    WEAPONDAT* WeaponDat(u32 weaponId);

private:
    WEAPONDAT weapons[130];
};

class Sprites
{
public:
    Sprites();
    virtual ~Sprites();
    bool LoadSprites(const std::vector<MpqFilePtr> & orderedSourceFiles);
    FLINGYDAT* FlingyDat(u32 id);
    SPRITEDAT* SpriteDat(u32 id);

private:
    FLINGYDAT flingy[209];
    SPRITEDAT sprite[517];
};

class PCX
{
public:
    buffer pcxDat;
    virtual ~PCX();
    bool load(const std::vector<MpqFilePtr> & orderedSourceFiles, const std::string & fileName);

    class PcxFile
    {
    public:
        u8 manufacturer;
        u8 verInfo;
        u8 encoding;
        u8 bitCount;
        u16 leftMargin;
        u16 upperMargin;
        u16 rightMargin;
        u16 lowerMargin;
        u16 hozDpi;
        u16 vertDpi;
        u8 palette[48];
        u8 reserved;
        u8 ncp;
        u16 nbs;
        u16 palInfo;
        u16 hozScreenSize;
        u16 vertScreenSize;
        u8 reserved2[54];
        u8 data[1];
    };
};

struct AIEntry
{
    u32 identifier; // 4-byte text, stored in TRIG section
    u32 filePtr; // File pointer in this file, 0 means it's in BWScript.bin
    u32 statStrIndex; // stat_txt.tbl string index for name
    u32 flags;
    // 01 - Ran in Location (Run AI Script at Location)
    // 02 - StarEdit Invisible
    // 04 - BW Only
};

class TblFiles
{
public:
    TblFiles() {}
    virtual ~TblFiles();
    bool Load(const std::vector<MpqFilePtr> & orderedSourceFiles);
    bool GetStatTblString(u16 stringNum, std::string & outString);

protected:
    bool GetTblString(buffer & buf, u16 stringNum, std::string & outString);

private:
    buffer stat_txtTbl;
};

class AiScripts
{
public:
    AiScripts(TblFiles & tblFiles) : tblFiles(tblFiles) {}
    virtual ~AiScripts();
    bool Load(const std::vector<MpqFilePtr> & orderedSourceFiles);
    bool GetAiEntry(int aiNum, AIEntry & outAiEntry);
    int GetNumAiScripts();
    bool GetAiIdentifier(int aiNum, u32 & outAiId);
    bool GetAiIdentifier(const std::string & inAiName, u32 & outAiId);
    bool GetAiName(int aiNum, std::string & outAiName);
    bool GetAiName(u32 aiId, std::string & outAiName);
    bool GetAiIdAndName(int aiNum, u32 & outId, std::string & outAiName);

private:
    buffer aiScriptBin;
    TblFiles & tblFiles;
};

class ScData
{
public:

    ScData();
    virtual ~ScData();

    Upgrades upgrades;
    Techs techs;
    Units units;
    Weapons weapons;
    Sprites oldSprites;
    TblFiles tblFiles;
    AiScripts aiScripts;
    Sc::Sprite sprites;
    Sc::Terrain terrain;
    Sc::Pcx tunit;
    Sc::Pcx tselect;
    Sc::Pcx tminimap;

    UNITDAT*   UnitDat  (u16 id) { return units.UnitDat    (id); }
    WEAPONDAT* WeaponDat(u32 id) { return weapons.WeaponDat(id); }
    FLINGYDAT* FlingyDat(u32 id) { return oldSprites.FlingyDat(id); }
    SPRITEDAT* SpriteDat(u32 id) { return oldSprites.SpriteDat(id); }

    bool Load(Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
        const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
        const std::string & expectedStarCraftDirectory = GetDefaultScPath(),
        FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
};

#endif