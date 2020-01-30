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

class ScData
{
public:

    ScData();
    virtual ~ScData();

    Techs techs;
    Weapons weapons;

    Sc::Unit units;
    Sc::Sprite sprites;
    Sc::Terrain terrain;
    Sc::Pcx tunit;
    Sc::Pcx tselect;
    Sc::Pcx tminimap;
    Sc::Ai ai;
    Sc::Upgrade upgrades;

    WEAPONDAT* WeaponDat(u32 id) { return weapons.WeaponDat(id); }

    bool Load(Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
        const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
        const std::string & expectedStarCraftDirectory = GetDefaultScPath(),
        FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
};

#endif