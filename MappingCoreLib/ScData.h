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
    Weapons weapons;
    TblFiles tblFiles;
    AiScripts aiScripts;

    Sc::Unit units;
    Sc::Sprite sprites;
    Sc::Terrain terrain;
    Sc::Pcx tunit;
    Sc::Pcx tselect;
    Sc::Pcx tminimap;

    WEAPONDAT* WeaponDat(u32 id) { return weapons.WeaponDat(id); }

    bool Load(Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
        const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
        const std::string & expectedStarCraftDirectory = GetDefaultScPath(),
        FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
};

#endif