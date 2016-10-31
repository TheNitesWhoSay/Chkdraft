#include "ScData.h"
#include <string>

bool Tiles::LoadSets(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt)
{
    return    LoadSet(hStarDat, hBrooDat, hPatchRt, "badlands", 0)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "platform", 1)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "install" , 2)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "ashworld", 3)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "jungle"  , 4)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "Desert"  , 5)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "Ice"     , 6)
           && LoadSet(hStarDat, hBrooDat, hPatchRt, "Twilight", 7);
}

void CorrectPaletteForWindows(buffer &palette)
{
    u32 numColors = palette.size() / 4;
    for ( u32 i = 0; i < numColors; i++ )
        palette.swap<u8>(i * 4, i * 4 + 2);
}

bool Tiles::LoadSet(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* name, u8 num)
{
    const std::string path("tileset\\");
    const size_t extensionSize = 4;

    if ( (path.length() + std::strlen(name) + extensionSize) < MAX_PATH &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string(path + std::string(name) + ".cv5").c_str(), set[num].cv5) &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string(path + std::string(name) + ".vf4").c_str(), set[num].vf4) &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string(path + std::string(name) + ".vr4").c_str(), set[num].vr4) &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string(path + std::string(name) + ".vx4").c_str(), set[num].vx4) &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string(path + std::string(name) + ".wpe").c_str(), set[num].wpe) )
    {
        CorrectPaletteForWindows(set[num].wpe);
        return true;
    }
    return false;
}

bool GRP::LoadGrp(const char* fileName, MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt)
{
    const std::string path("unit\\");

    return
        path.length() + std::strlen(fileName) < MAX_PATH &&
        FileToBuffer(hStarDat, hBrooDat, hPatchRt, std::string("unit\\" + std::string(fileName)).c_str() , imageDat);
}

u8* GRP::data(u32 frame, u32 line)
{
    if ( frame < numFrames() )
    {
        if ( line < imageDat.get<u8>(0x9+frame*8) )
        {
            u32 globalOffset = imageDat.get<u32>(0xA+frame*8);
            return (u8*)imageDat.getPtr(globalOffset+(u32)imageDat.get<u16>(globalOffset+line*2));
        }
    }
    return nullptr;
}

Upgrades::Upgrades()
{
    for ( u8 i=0; i<61; i++ )
    {
        upgrade[i].MineralCost = 0;
        upgrade[i].MineralFactor = 0;
        upgrade[i].VespeneCost = 0;
        upgrade[i].VespeneFactor = 0;
        upgrade[i].TimeCost = 0;
        upgrade[i].TimeFactor = 0;
        upgrade[i].Unknown = 0;
        upgrade[i].Icon = 0;
        upgrade[i].Label = 0;
        upgrade[i].Race = 0;
        upgrade[i].MaxRepeats = 0;
        upgrade[i].BroodWarSpecific = 0;
    }
}

bool Upgrades::LoadUpgrades(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt)
{
    buffer upgradeDat("dDat");

    if (    !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\upgrades.dat", upgradeDat)
         || !( upgradeDat.size() == 1281 || upgradeDat.size() == 920 ) )
        return false;

    u32 pos, i, numUpgrades = 46;
    bool isExpansion = (upgradeDat.size() == 1281);
    if ( isExpansion )
        numUpgrades = 61;

    if ( isExpansion )
    {
        pos = 0x000; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MineralCost      = upgradeDat.get<u16>(pos+2*i);
        pos = 0x07A; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MineralFactor    = upgradeDat.get<u16>(pos+2*i);
        pos = 0x0F4; for ( i=0; i<numUpgrades; i++ ) upgrade[i].VespeneCost      = upgradeDat.get<u16>(pos+2*i);
        pos = 0x16E; for ( i=0; i<numUpgrades; i++ ) upgrade[i].VespeneFactor    = upgradeDat.get<u16>(pos+2*i);
        pos = 0x1E8; for ( i=0; i<numUpgrades; i++ ) upgrade[i].TimeCost         = upgradeDat.get<u16>(pos+2*i);
        pos = 0x262; for ( i=0; i<numUpgrades; i++ ) upgrade[i].TimeFactor       = upgradeDat.get<u16>(pos+2*i);
        pos = 0x2DC; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Unknown          = upgradeDat.get<u16>(pos+2*i);
        pos = 0x356; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Icon             = upgradeDat.get<u16>(pos+2*i);
        pos = 0x3D0; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Label            = upgradeDat.get<u16>(pos+2*i);
        pos = 0x44A; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Race             = upgradeDat.get< u8>(pos+  i);
        pos = 0x487; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MaxRepeats       = upgradeDat.get< u8>(pos+  i);
        pos = 0x4C4; for ( i=0; i<numUpgrades; i++ ) upgrade[i].BroodWarSpecific = upgradeDat.get< u8>(pos+  i);
    }
    else // Original
    {
        pos = 0x000; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MineralCost   = upgradeDat.get<u16>(pos+2*i);
        pos = 0x05C; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MineralFactor = upgradeDat.get<u16>(pos+2*i);
        pos = 0x0B8; for ( i=0; i<numUpgrades; i++ ) upgrade[i].VespeneCost   = upgradeDat.get<u16>(pos+2*i);
        pos = 0x114; for ( i=0; i<numUpgrades; i++ ) upgrade[i].VespeneFactor = upgradeDat.get<u16>(pos+2*i);
        pos = 0x170; for ( i=0; i<numUpgrades; i++ ) upgrade[i].TimeCost      = upgradeDat.get<u16>(pos+2*i);
        pos = 0x1CC; for ( i=0; i<numUpgrades; i++ ) upgrade[i].TimeFactor    = upgradeDat.get<u16>(pos+2*i);
        pos = 0x228; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Unknown       = upgradeDat.get<u16>(pos+2*i);
        pos = 0x284; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Icon          = upgradeDat.get<u16>(pos+2*i);
        pos = 0x2E0; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Label         = upgradeDat.get<u16>(pos+2*i);
        pos = 0x33C; for ( i=0; i<numUpgrades; i++ ) upgrade[i].Race          = upgradeDat.get< u8>(pos+  i);
        pos = 0x36A; for ( i=0; i<numUpgrades; i++ ) upgrade[i].MaxRepeats    = upgradeDat.get< u8>(pos+  i);
    }

    return true;
}

UPGRADEDAT* Upgrades::UpgradeDat(u8 id)
{
    if ( id < 61 )
        return &upgrade[id];
    else
        return &upgrade[0];
}

Techs::Techs()
{
    for ( u8 i=0; i<44; i++ )
    {
        tech[i].MineralCost = 0;
        tech[i].VespeneCost = 0;
        tech[i].ResearchTime = 0;
        tech[i].EnergyCost = 0;
        tech[i].Unknown = 0;
        tech[i].Icon = 0;
        tech[i].Label = 0;
        tech[i].Race = 0;
        tech[i].Unused = 0;
        tech[i].BroodWar = 0;
    }
}

bool Techs::LoadTechs(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt)
{
    buffer techDat("tDat");

    if (    !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\techdata.dat", techDat)
         || !( techDat.size() == 836 || techDat.size() == 432 ) )
        return false;

    u32 pos, i, numTechs = 24;
    bool isExpansion = (techDat.size() == 836);
    if ( isExpansion )
        numTechs = 44;

    if ( isExpansion )
    {
        pos = 0x000; for ( i=0; i<numTechs; i++ ) tech[i].MineralCost  = techDat.get<u16>(pos+2*i);
        pos = 0x058; for ( i=0; i<numTechs; i++ ) tech[i].VespeneCost  = techDat.get<u16>(pos+2*i);
        pos = 0x0B0; for ( i=0; i<numTechs; i++ ) tech[i].ResearchTime = techDat.get<u16>(pos+2*i);
        pos = 0x108; for ( i=0; i<numTechs; i++ ) tech[i].EnergyCost   = techDat.get<u16>(pos+2*i);
        pos = 0x160; for ( i=0; i<numTechs; i++ ) tech[i].Unknown      = techDat.get<u32>(pos+4*i);
        pos = 0x210; for ( i=0; i<numTechs; i++ ) tech[i].Icon         = techDat.get<u16>(pos+2*i);
        pos = 0x268; for ( i=0; i<numTechs; i++ ) tech[i].Label        = techDat.get<u16>(pos+2*i);
        pos = 0x2C0; for ( i=0; i<numTechs; i++ ) tech[i].Race         = techDat.get< u8>(pos+  i);
        pos = 0x2EC; for ( i=0; i<numTechs; i++ ) tech[i].Unused       = techDat.get< u8>(pos+  i);
        pos = 0x318; for ( i=0; i<numTechs; i++ ) tech[i].BroodWar     = techDat.get< u8>(pos+  i);
    }
    else // Original
    {
        pos = 0x000; for ( i=0; i<numTechs; i++ ) tech[i].MineralCost  = techDat.get<u16>(pos+2*i);
        pos = 0x030; for ( i=0; i<numTechs; i++ ) tech[i].VespeneCost  = techDat.get<u16>(pos+2*i);
        pos = 0x060; for ( i=0; i<numTechs; i++ ) tech[i].ResearchTime = techDat.get<u16>(pos+2*i);
        pos = 0x090; for ( i=0; i<numTechs; i++ ) tech[i].EnergyCost   = techDat.get<u16>(pos+2*i);
        pos = 0x0C0; for ( i=0; i<numTechs; i++ ) tech[i].Unknown      = techDat.get<u32>(pos+4*i);
        pos = 0x120; for ( i=0; i<numTechs; i++ ) tech[i].Icon         = techDat.get<u16>(pos+2*i);
        pos = 0x150; for ( i=0; i<numTechs; i++ ) tech[i].Label        = techDat.get<u16>(pos+2*i);
        pos = 0x180; for ( i=0; i<numTechs; i++ ) tech[i].Race         = techDat.get< u8>(pos+  i);
        pos = 0x198; for ( i=0; i<numTechs; i++ ) tech[i].Unused       = techDat.get< u8>(pos+  i);
    }

    return true;
}

TECHDAT* Techs::TechDat(u8 id)
{
    if ( id < 44 )
        return &tech[id];
    else
        return &tech[0];
}

Units::Units()
{
    for ( u16 i=0; i<228; i++ )
    {
        unit[i].Graphics = 0;
        unit[i].Subunit1 = 0;
        unit[i].Subunit2 = 0;
        unit[i].Infestation = 0;
        unit[i].ConstructionAnimation = 0;
        unit[i].UnitDirection = 0;
        unit[i].ShieldEnable = 0;
        unit[i].ShieldAmount = 0;
        unit[i].HitPoints = 0;
        unit[i].ElevationLevel = 0;
        unit[i].Unknown = 0;
        unit[i].Sublabel = 0;
        unit[i].CompAIIdle = 0;
        unit[i].HumanAIIdle = 0;
        unit[i].ReturntoIdle = 0;
        unit[i].AttackUnit = 0;
        unit[i].AttackMove = 0;
        unit[i].GroundWeapon = 0;
        unit[i].MaxGroundHits = 0;
        unit[i].AirWeapon = 0;
        unit[i].MaxAirHits = 0;
        unit[i].AIInternal = 0;
        unit[i].SpecialAbilityFlags = 0;
        unit[i].TargetAcquisitionRange = 0;
        unit[i].SightRange = 0;
        unit[i].ArmorUpgrade = 0;
        unit[i].UnitSize = 0;
        unit[i].Armor = 0;
        unit[i].RightClickAction = 0;
        unit[i].ReadySound = 0;
        unit[i].WhatSoundStart = 0;
        unit[i].WhatSoundEnd = 0;
        unit[i].PissSoundStart = 0;
        unit[i].PissSoundEnd = 0;
        unit[i].YesSoundStart = 0;
        unit[i].YesSoundEnd = 0;
        unit[i].StarEditPlacementBoxWidth = 0;
        unit[i].StarEditPlacementBoxHeight = 0;
        unit[i].AddonHorizontal = 0;
        unit[i].AddonVertical = 0;
        unit[i].UnitSizeLeft = 0;
        unit[i].UnitSizeUp = 0;
        unit[i].UnitSizeRight = 0;
        unit[i].UnitSizeDown = 0;
        unit[i].Portrait = 0;
        unit[i].MineralCost = 0;
        unit[i].VespeneCost = 0;
        unit[i].BuildTime = 0;
        unit[i].Unknown1 = 0;
        unit[i].StarEditGroupFlags = 0;
        unit[i].SupplyProvided = 0;
        unit[i].SupplyRequired = 0;
        unit[i].SpaceRequired = 0;
        unit[i].SpaceProvided = 0;
        unit[i].BuildScore = 0;
        unit[i].DestroyScore = 0;
        unit[i].UnitMapString = 0;
        unit[i].BroodwarUnitFlag = 0;
        unit[i].StarEditAvailabilityFlags = 0;
    }
}

bool Units::LoadUnits(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt)
{
    buffer unitDat("uDat");

    if (    !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\units.dat", unitDat)
         || unitDat.size() != 19876 )
        return false;

    u32 pos, i;

    pos = 0x0000; for ( i=  0; i<228; i++ ) unit[i].Graphics               = unitDat.get<u8>(pos+i);
    pos = 0x00E4; for ( i=  0; i<228; i++ ) unit[i].Subunit1               = unitDat.get<u16>(pos+i*2);
    pos = 0x02AC; for ( i=  0; i<228; i++ ) unit[i].Subunit2               = unitDat.get<u16>(pos+i*2);
    pos = 0x0474; for ( i=106; i<202; i++ ) unit[i].Infestation            = unitDat.get<u16>(pos+i*2);
    pos = 0x0534; for ( i=  0; i<228; i++ ) unit[i].ConstructionAnimation  = unitDat.get<u32>(pos+i*4);
    pos = 0x08C4; for ( i=  0; i<228; i++ ) unit[i].UnitDirection          = unitDat.get<u8>(pos+i);
    pos = 0x09A8; for ( i=  0; i<228; i++ ) unit[i].ShieldEnable           = unitDat.get<u8>(pos+i);
    pos = 0x0A8C; for ( i=  0; i<228; i++ ) unit[i].ShieldAmount           = unitDat.get<u16>(pos+i*2);
    pos = 0x0C54; for ( i=  0; i<228; i++ ) unit[i].HitPoints              = unitDat.get<u32>(pos+i*4);
    pos = 0x0FE4; for ( i=  0; i<228; i++ ) unit[i].ElevationLevel         = unitDat.get<u8>(pos+i);
    pos = 0x10C8; for ( i=  0; i<228; i++ ) unit[i].Unknown                = unitDat.get<u8>(pos+i);
    pos = 0x11AC; for ( i=  0; i<228; i++ ) unit[i].Sublabel               = unitDat.get<u8>(pos+i);
    pos = 0x1290; for ( i=  0; i<228; i++ ) unit[i].CompAIIdle             = unitDat.get<u8>(pos+i);
    pos = 0x1374; for ( i=  0; i<228; i++ ) unit[i].HumanAIIdle            = unitDat.get<u8>(pos+i);
    pos = 0x1458; for ( i=  0; i<228; i++ ) unit[i].ReturntoIdle           = unitDat.get<u8>(pos+i);
    pos = 0x153C; for ( i=  0; i<228; i++ ) unit[i].AttackUnit             = unitDat.get<u8>(pos+i);
    pos = 0x1620; for ( i=  0; i<228; i++ ) unit[i].AttackMove             = unitDat.get<u8>(pos+i);
    pos = 0x1704; for ( i=  0; i<228; i++ ) unit[i].GroundWeapon           = unitDat.get<u8>(pos+i);
    pos = 0x17E8; for ( i=  0; i<228; i++ ) unit[i].MaxGroundHits          = unitDat.get<u8>(pos+i);
    pos = 0x18CC; for ( i=  0; i<228; i++ ) unit[i].AirWeapon              = unitDat.get<u8>(pos+i);
    pos = 0x19B0; for ( i=  0; i<228; i++ ) unit[i].MaxAirHits             = unitDat.get<u8>(pos+i);
    pos = 0x1A94; for ( i=  0; i<228; i++ ) unit[i].AIInternal             = unitDat.get<u8>(pos+i);
    pos = 0x1B78; for ( i=  0; i<228; i++ ) unit[i].SpecialAbilityFlags    = unitDat.get<u32>(pos+i*4);
    pos = 0x1F08; for ( i=  0; i<228; i++ ) unit[i].TargetAcquisitionRange = unitDat.get<u8>(pos+i);
    pos = 0x1FEC; for ( i=  0; i<228; i++ ) unit[i].SightRange             = unitDat.get<u8>(pos+i);
    pos = 0x20D0; for ( i=  0; i<228; i++ ) unit[i].ArmorUpgrade           = unitDat.get<u8>(pos+i);
    pos = 0x21B4; for ( i=  0; i<228; i++ ) unit[i].UnitSize               = unitDat.get<u8>(pos+i);
    pos = 0x2298; for ( i=  0; i<228; i++ ) unit[i].Armor                  = unitDat.get<u8>(pos+i);
    pos = 0x237C; for ( i=  0; i<228; i++ ) unit[i].RightClickAction       = unitDat.get<u8>(pos+i);
    pos = 0x2460; for ( i=  0; i<106; i++ ) unit[i].ReadySound             = unitDat.get<u16>(pos+i*2);
    pos = 0x2534; for ( i=  0; i<228; i++ ) unit[i].WhatSoundStart         = unitDat.get<u16>(pos+i*2);
    pos = 0x26FC; for ( i=  0; i<228; i++ ) unit[i].WhatSoundEnd           = unitDat.get<u16>(pos+i*2);
    pos = 0x28C4; for ( i=  0; i<106; i++ ) unit[i].PissSoundStart         = unitDat.get<u16>(pos+i*2);
    pos = 0x2998; for ( i=  0; i<106; i++ ) unit[i].PissSoundEnd           = unitDat.get<u16>(pos+i*2);
    pos = 0x2A6C; for ( i=  0; i<106; i++ ) unit[i].YesSoundStart          = unitDat.get<u16>(pos+i*2);
    pos = 0x2B40; for ( i=  0; i<106; i++ ) unit[i].YesSoundEnd            = unitDat.get<u16>(pos+i*2);

    pos = 0x2C14;
    for ( i=  0; i<228; i++ )
    {
        unit[i].StarEditPlacementBoxWidth  = unitDat.get<u16>(pos+i*4);
        unit[i].StarEditPlacementBoxHeight = unitDat.get<u16>(pos+i*4+2);
    }

    pos = 0x2FA4; for ( i=106; i<202; i++ ) unit[i].AddonHorizontal = unitDat.get<u16>(pos+(i-106)*2);
    pos = 0x3064; for ( i=106; i<202; i++ ) unit[i].AddonVertical   = unitDat.get<u16>(pos+(i-106)*2);

    pos = 0x3124;
    for ( i=  0; i<228; i++ )
    {
        unit[i].UnitSizeLeft  = unitDat.get<u16>(pos+i*8);
        unit[i].UnitSizeUp    = unitDat.get<u16>(pos+i*8+2);
        unit[i].UnitSizeRight = unitDat.get<u16>(pos+i*8+4);
        unit[i].UnitSizeDown  = unitDat.get<u16>(pos+i*8+6);
    }

    pos = 0x3844; for ( i=  0; i<228; i++ ) unit[i].Portrait                   = unitDat.get<u16>(pos+i*2);
    pos = 0x3A0C; for ( i=  0; i<228; i++ ) unit[i].MineralCost                = unitDat.get<u16>(pos+i*2);
    pos = 0x3BD4; for ( i=  0; i<228; i++ ) unit[i].VespeneCost                = unitDat.get<u16>(pos+i*2);
    pos = 0x3D9C; for ( i=  0; i<228; i++ ) unit[i].BuildTime                  = unitDat.get<u16>(pos+i*2);
    pos = 0x3F64; for ( i=  0; i<228; i++ ) unit[i].Unknown1                   = unitDat.get<u16>(pos+i*2);
    pos = 0x412C; for ( i=  0; i<228; i++ ) unit[i].StarEditGroupFlags         = unitDat.get<u8>(pos+i);
    pos = 0x4210; for ( i=  0; i<228; i++ ) unit[i].SupplyProvided             = unitDat.get<u8>(pos+i);
    pos = 0x42F4; for ( i=  0; i<228; i++ ) unit[i].SupplyRequired             = unitDat.get<u8>(pos+i);
    pos = 0x43D8; for ( i=  0; i<228; i++ ) unit[i].SpaceRequired              = unitDat.get<u8>(pos+i);
    pos = 0x44BC; for ( i=  0; i<228; i++ ) unit[i].SpaceProvided              = unitDat.get<u8>(pos+i);
    pos = 0x45A0; for ( i=  0; i<228; i++ ) unit[i].BuildScore                 = unitDat.get<u16>(pos+i*2);
    pos = 0x4768; for ( i=  0; i<228; i++ ) unit[i].DestroyScore               = unitDat.get<u16>(pos+i*2);
    pos = 0x4930; for ( i=  0; i<228; i++ ) unit[i].UnitMapString              = unitDat.get<u16>(pos+i*2);
    pos = 0x4AF8; for ( i=  0; i<228; i++ ) unit[i].BroodwarUnitFlag           = unitDat.get<u8>(pos+i);
    pos = 0x4BDC; for ( i=  0; i<228; i++ ) unit[i].StarEditAvailabilityFlags  = unitDat.get<u16>(pos+i*2);

    return true;
}

UNITDAT* Units::UnitDat(u16 id)
{
    if ( id < 228 )
        return &unit[id];
    else
        return &unit[0];
}

Weapons::Weapons()
{
    for ( u32 i=0; i<130; i++ )
    {
        weapons[i].Label = 0;
        weapons[i].Graphics = 0;
        weapons[i].Unused = 0;
        weapons[i].TargetFlags = 0;
        weapons[i].MinimumRange = 0;
        weapons[i].MaximumRange = 0;
        weapons[i].DamageUpgrade = 0;
        weapons[i].WeaponType = 0;
        weapons[i].WeaponBehavior = 0;
        weapons[i].RemoveAfter = 0;
        weapons[i].WeaponEffect = 0;
        weapons[i].InnerSplashRadius = 0;
        weapons[i].MediumSplashRadius = 0;
        weapons[i].OuterSplashRadius = 0;
        weapons[i].DamageAmount = 0;
        weapons[i].DamageBonus = 0;
        weapons[i].WeaponCooldown = 0;
        weapons[i].DamageFactor = 0;
        weapons[i].AttackAngle = 0;
        weapons[i].LaunchSpin = 0;
        weapons[i].ForwardOffset = 0;
        weapons[i].UpwardOffset = 0;
        weapons[i].TargetErrorMessage = 0;
        weapons[i].Icon = 0;
    }
}

bool Weapons::LoadWeapons(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt)
{
    buffer weaponDat("wDat");

    if (    !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\weapons.dat", weaponDat)
         || weaponDat.size() != 5460 )
        return false;

    u32 pos, i;

    pos = 0x0000; for ( i=0; i<130; i++ ) weapons[i].Label              = weaponDat.get<u16>(pos+i*2);
    pos = 0x0104; for ( i=0; i<130; i++ ) weapons[i].Graphics           = weaponDat.get<u32>(pos+i*4);
    pos = 0x030C; for ( i=0; i<130; i++ ) weapons[i].Unused             = weaponDat.get<u8 >(pos+i  );
    pos = 0x038E; for ( i=0; i<130; i++ ) weapons[i].TargetFlags        = weaponDat.get<u16>(pos+i*2);
    pos = 0x0492; for ( i=0; i<130; i++ ) weapons[i].MinimumRange       = weaponDat.get<u32>(pos+i*4);
    pos = 0x069A; for ( i=0; i<130; i++ ) weapons[i].MaximumRange       = weaponDat.get<u32>(pos+i*4);
    pos = 0x08A2; for ( i=0; i<130; i++ ) weapons[i].DamageUpgrade      = weaponDat.get<u8 >(pos+i  );
    pos = 0x0924; for ( i=0; i<130; i++ ) weapons[i].WeaponType         = weaponDat.get<u8 >(pos+i  );
    pos = 0x09A6; for ( i=0; i<130; i++ ) weapons[i].WeaponBehavior     = weaponDat.get<u8 >(pos+i  );
    pos = 0x0A28; for ( i=0; i<130; i++ ) weapons[i].RemoveAfter        = weaponDat.get<u8 >(pos+i  );
    pos = 0x0AAA; for ( i=0; i<130; i++ ) weapons[i].WeaponEffect       = weaponDat.get<u8 >(pos+i  );
    pos = 0x0B2C; for ( i=0; i<130; i++ ) weapons[i].InnerSplashRadius  = weaponDat.get<u16>(pos+i*2);
    pos = 0x0C30; for ( i=0; i<130; i++ ) weapons[i].MediumSplashRadius = weaponDat.get<u16>(pos+i*2);
    pos = 0x0D34; for ( i=0; i<130; i++ ) weapons[i].OuterSplashRadius  = weaponDat.get<u16>(pos+i*2);
    pos = 0x0E38; for ( i=0; i<130; i++ ) weapons[i].DamageAmount       = weaponDat.get<u16>(pos+i*2);
    pos = 0x0F3C; for ( i=0; i<130; i++ ) weapons[i].DamageBonus        = weaponDat.get<u16>(pos+i*2);
    pos = 0x1040; for ( i=0; i<130; i++ ) weapons[i].WeaponCooldown     = weaponDat.get<u8 >(pos+i  );
    pos = 0x10C2; for ( i=0; i<130; i++ ) weapons[i].DamageFactor       = weaponDat.get<u8 >(pos+i  );
    pos = 0x1144; for ( i=0; i<130; i++ ) weapons[i].AttackAngle        = weaponDat.get<u8 >(pos+i  );
    pos = 0x11C6; for ( i=0; i<130; i++ ) weapons[i].LaunchSpin         = weaponDat.get<u8 >(pos+i  );
    pos = 0x1248; for ( i=0; i<130; i++ ) weapons[i].ForwardOffset      = weaponDat.get<u8 >(pos+i  );
    pos = 0x12CA; for ( i=0; i<130; i++ ) weapons[i].UpwardOffset       = weaponDat.get<u8 >(pos+i  );
    pos = 0x134C; for ( i=0; i<130; i++ ) weapons[i].TargetErrorMessage = weaponDat.get<u16>(pos+i*2);
    pos = 0x1450; for ( i=0; i<130; i++ ) weapons[i].Icon               = weaponDat.get<u16>(pos+i*2);

    return true;
}

WEAPONDAT* Weapons::WeaponDat(u32 weaponId)
{
    if ( weaponId < 130 )
        return &weapons[weaponId];
    else
        return nullptr;
}

Sprites::Sprites()
{
    for ( u32 i=0; i<209; i++ )
    {
        flingy[i].Sprite = 0;
        flingy[i].TopSpeed = 0;
        flingy[i].Acceleration = 0;
        flingy[i].HaltDistance = 0;
        flingy[i].TurnRadius = 0;
        flingy[i].Unused = 0;
        flingy[i].MoveControl = 0;
    }

    for ( u32 i=0; i<517; i++ )
    {
        sprite[i].ImageFile = 0;
        sprite[i].HealthBar = 0;
        sprite[i].Unknown = 0;
        sprite[i].IsVisible = 0;
        sprite[i].SelectionCircleImage = 0;
        sprite[i].SelectionCircleOffset = 0;
    }
}

bool Sprites::LoadSprites(MPQHANDLE hStarDat, MPQHANDLE hBrooDat, MPQHANDLE hPatchRt)
{
    buffer flingyDat("fDat"), spriteDat("sDat"), imageDat("iDat");
    
    if (    !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\flingy.dat", flingyDat) 
         || !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\sprites.dat", spriteDat)
         || !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\images.dat", imageDat)
         || flingyDat.size() != 3135
         || spriteDat.size() != 3229
         || imageDat.size() != 37962 )
    {
        return false;
    }

    u32 pos, i;

    pos = 0x000; for ( i=0; i<209; i++ ) flingy[i].Sprite       = flingyDat.get<u16>(pos+i*2);
    pos = 0x1A2; for ( i=0; i<209; i++ ) flingy[i].TopSpeed     = flingyDat.get<u32>(pos+i*4);
    pos = 0x4E6; for ( i=0; i<209; i++ ) flingy[i].Acceleration = flingyDat.get<u16>(pos+i*2);
    pos = 0x688; for ( i=0; i<209; i++ ) flingy[i].HaltDistance = flingyDat.get<u32>(pos+i*4);
    pos = 0x9CC; for ( i=0; i<209; i++ ) flingy[i].TurnRadius   = flingyDat.get<u8>(pos+i);
    pos = 0xA96; for ( i=0; i<209; i++ ) flingy[i].Unused       = flingyDat.get<u8>(pos+i);
    pos = 0xB6E; for ( i=0; i<209; i++ ) flingy[i].MoveControl  = flingyDat.get<u8>(pos+i);

    pos = 0x000; for ( i=  0; i<517; i++ ) sprite[i].ImageFile             = spriteDat.get<u16>(pos+i*2);
    pos = 0x40A; for ( i=130; i<517; i++ ) sprite[i].HealthBar             = spriteDat.get<u8>(pos+i-130);
    pos = 0x58D; for ( i=  0; i<517; i++ ) sprite[i].Unknown               = spriteDat.get<u8>(pos+i);
    pos = 0x792; for ( i=  0; i<517; i++ ) sprite[i].IsVisible             = spriteDat.get<u8>(pos+i);
    pos = 0x997; for ( i=130; i<517; i++ ) sprite[i].SelectionCircleImage  = spriteDat.get<u8>(pos+i-130);
    pos = 0xB1A; for ( i=130; i<517; i++ ) sprite[i].SelectionCircleOffset = spriteDat.get<u8>(pos+i-130);

    pos = 0x0000; for ( i=0; i<999; i++ ) image[i].GRPFile            = imageDat.get<u32>(pos+i*4);
    pos = 0x0F9C; for ( i=0; i<999; i++ ) image[i].GraphicTurns       = imageDat.get<u8>(pos+i);
    pos = 0x1383; for ( i=0; i<999; i++ ) image[i].Clickable          = imageDat.get<u8>(pos+i);
    pos = 0x176A; for ( i=0; i<999; i++ ) image[i].UseFullIscript     = imageDat.get<u8>(pos+i);
    pos = 0x1B51; for ( i=0; i<999; i++ ) image[i].DrawIfCloaked      = imageDat.get<u8>(pos+i);
    pos = 0x1F38; for ( i=0; i<999; i++ ) image[i].DrawFunction       = imageDat.get<u8>(pos+i);
    pos = 0x231F; for ( i=0; i<999; i++ ) image[i].Remapping          = imageDat.get<u8>(pos+i);
    pos = 0x2706; for ( i=0; i<999; i++ ) image[i].IscriptID          = imageDat.get<u32>(pos+i*4);
    pos = 0x36A2; for ( i=0; i<999; i++ ) image[i].ShieldOverlay      = imageDat.get<u32>(pos+i*4);
    pos = 0x463E; for ( i=0; i<999; i++ ) image[i].AttackOverlay      = imageDat.get<u32>(pos+i*4);
    pos = 0x55DA; for ( i=0; i<999; i++ ) image[i].DamageOverlay      = imageDat.get<u32>(pos+i*4);
    pos = 0x6576; for ( i=0; i<999; i++ ) image[i].SpecialOverlay     = imageDat.get<u32>(pos+i*4);
    pos = 0x7512; for ( i=0; i<999; i++ ) image[i].LandingDustOverlay = imageDat.get<u32>(pos+i*4);
    pos = 0x84AE; for ( i=0; i<999; i++ ) image[i].LiftOffOverlay     = imageDat.get<u32>(pos+i*4);

    return true;
}

FLINGYDAT* Sprites::FlingyDat(u32 id)
{
    if ( id < 209 )
        return &flingy[id];
    else
        return &flingy[0];
}

SPRITEDAT* Sprites::SpriteDat(u32 id)
{
    if ( id < 517 )
        return &sprite[id];
    else
        return &sprite[0];
}

IMAGEDAT* Sprites::ImageDat(u32 id)
{
    if ( id < 999 )
        return &image[id];
    else
        return &image[0];
}

bool PCX::load(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt, const char* filename)
{
    buffer rawDat;
    if ( FileToBuffer(hStarDat, hBrooDat, hPatchRt, filename, rawDat) )
    {
        if ( rawDat.get<u8>((u32)PCX::DataLocs::BitCount) != 8 )
        {
            Error("Unsupported PCX Format");
            return false;
        }

        u8* pal;
        u8 ncp = rawDat.get<u8>((u32)PCX::DataLocs::Ncp);
        u16 nbs = rawDat.get<u16>((u32)PCX::DataLocs::Nbs);
        u32 palStart = rawDat.size()-768;

        if ( !rawDat.getPtr(pal, palStart, 768) )
        {
            Error("Bad Pallete");
            return false;
        }

        u32 pos = (u32)PCX::DataLocs::Data,
            pixel = 0;

        while ( pixel < ((u32)ncp)*((u32)nbs) )
        {
            u8 compSect = rawDat.get<u8>(pos);
            pos ++;

            if ( compSect < 0xC0 )
            {
                for ( u32 i=0; i<3; i++ )
                    pcxDat.add<u8>(pal[compSect*3+i]);

                pcxDat.add<u8>(0);
                pixel ++;
            }
            else
            {
                u8 repeatColor = rawDat.get<u8>(pos);
                pos ++;

                for ( u32 repeat=0; repeat<((u32)compSect)-0xC0; repeat++ )
                {
                    for ( u32 i=0; i<3; i++ )
                        pcxDat.add<u8>(pal[repeatColor*3+i]);

                    pcxDat.add<u8>(0);
                    pixel ++;
                }
            }
        }
        CorrectPaletteForWindows(pcxDat);
        return true;
    }
    return false;
}

bool TblFiles::Load(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt)
{
    return FileToBuffer(hStarDat, hBrooDat, hPatchRt, "Rez\\stat_txt.tbl", stat_txtTbl);
}

bool TblFiles::GetStatTblString(u16 stringNum, std::string &outString)
{
    return GetTblString(stat_txtTbl, stringNum, outString);
}

bool TblFiles::GetTblString(buffer &tbl, u16 stringNum, std::string &outString)
{
    outString.clear();
    u16 numStrings = tbl.get<u16>(0);

    if ( tbl.exists() && stringNum != 0 )
    {
        u32 start = tbl.get<u16>(2 * stringNum);
        u32 end = 0;
        if ( !tbl.getNext('\0', start, end) ) // Next NUL char marks the end of the string
            end = tbl.size(); // Just end it where buffer ends

        size_t length = (size_t)(end - start);
        char* cString = (char*)tbl.getPtr(start);
        try {
            outString = std::string(cString, length);
            return true;
        }
        catch ( std::bad_alloc ) {}
    }
    return false;
}

bool AiScripts::Load(MPQHANDLE &hStarDat, MPQHANDLE &hBrooDat, MPQHANDLE &hPatchRt)
{
    return FileToBuffer(hStarDat, hBrooDat, hPatchRt, "scripts\\AISCRIPT.BIN", aiScriptBin);
}

bool AiScripts::GetAiEntry(int aiNum, AIEntry &outAiEntry)
{
    return aiScriptBin.get<AIEntry>(outAiEntry, aiNum*sizeof(AIEntry) + aiScriptBin.get<u32>(0));
}

int AiScripts::GetNumAiScripts()
{
    return (aiScriptBin.size() - aiScriptBin.get<u32>(0)) / sizeof(AIEntry);
}

bool AiScripts::GetAiIdentifier(int aiNum, u32 &outAiId)
{
    AIEntry aiEntry = {};
    if ( GetAiEntry(aiNum, aiEntry) )
    {
        outAiId = aiEntry.identifier;
        return true;
    }
    return false;
}

bool AiScripts::GetAiIdentifier(std::string &inAiName, u32 &outAiId)
{
    std::string scAiName;
    int numAiScripts = GetNumAiScripts();
    for ( int i = 0; i < numAiScripts; i++ )
    {
        if ( GetAiName(i, scAiName) && scAiName.compare(inAiName) == 0 )
            return true;
    }
    return false;
}

bool AiScripts::GetAiName(int aiNum, std::string &outAiName)
{
    AIEntry aiEntry = {};
    return GetAiEntry(aiNum, aiEntry) && tblFiles.GetStatTblString(aiEntry.statStrIndex, outAiName);
}

bool AiScripts::GetAiIdAndName(int aiNum, u32 &outId, std::string &outAiName)
{
    AIEntry aiEntry = {};
    if ( GetAiEntry(aiNum, aiEntry) )
    {
        outId = aiEntry.identifier;
        if ( tblFiles.GetStatTblString(aiEntry.statStrIndex, outAiName) )
            return true;
    }
    return false;
}

void ScData::Load()
{
    MPQHANDLE hStarDat = nullptr,
              hBrooDat = nullptr,
              hPatchRt = nullptr;

    char scPath[MAX_PATH] = { 0 };
    GetRegScPath(scPath, MAX_PATH);

    bool openedAnArchive = OpenArchive(scPath, "StarDat.mpq" , hStarDat) ||
        OpenArchive(scPath, "BrooDat.mpq" , hBrooDat) || OpenArchive(scPath, "patch_rt.mpq", hPatchRt);

    if ( openedAnArchive )
    {
        if ( !tilesets.LoadSets(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load tilesets");

        if ( !upgrades.LoadUpgrades(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load upgrades");

        if ( !techs.LoadTechs(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load techs");

        if ( !units.LoadUnits(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load Units.dat");

        if ( !weapons.LoadWeapons(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load Weapons.dat");

        if ( sprites.LoadSprites(hStarDat, hBrooDat, hPatchRt) )
            LoadGrps(hPatchRt, hBrooDat, hStarDat);
        else
            Error("Failed to load sprites");

        if ( !tunit.load(hStarDat, hBrooDat, hPatchRt, "game\\tunit.pcx") )
            Error("Failed to load tunit.pcx");

        if ( !tminimap.load(hStarDat, hBrooDat, hPatchRt, "game\\tminimap.pcx") )
            Error("Failed to load tminimap.pcx");

        if ( !tselect.load(hStarDat, hBrooDat, hPatchRt, "game\\tselect.pcx") )
            Error("Failed to load tselect.pcx");

        if ( !aiScripts.Load(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load AIScripts");

        if ( !tblFiles.Load(hStarDat, hBrooDat, hPatchRt) )
            Error("Failed to load tbl files");

        CloseArchive(hStarDat);
        CloseArchive(hBrooDat);
        CloseArchive(hPatchRt);
    }
    else
    {
        Error("No archives selected, many features will not work without the game files.\n\nInstall or locate StarCraft for the best experience.");
    }
}

bool ScData::LoadGrps(MPQHANDLE &hPatchRt, MPQHANDLE &hBrooDat, MPQHANDLE &hStarDat)
{
    buffer imageTbl;

    if ( !FileToBuffer(hStarDat, hBrooDat, hPatchRt, "arr\\images.tbl", imageTbl) )
        Error("Could not load images.tbl");

    char* fileName;
    numGrps = imageTbl.get<u16>(0);

    try {
        grps = new GRP[numGrps];
    }
    catch ( std::bad_alloc ) {
        return false;
    }

    for ( u32 i = 0; i<numGrps; i++ )
    {
        fileName = (char*)imageTbl.getPtr(imageTbl.get<u16>((i + 1) * 2));
        grps[i].LoadGrp(fileName, hPatchRt, hBrooDat, hStarDat);
    }

    return true;
}

bool GetCV5References(TileSet* tiles, u32 &cv5Index, u16 TileValue)
{
    cv5Index = ((TileValue>>4)*26+(TileValue&0xF)+0xA)*2;
    /** simplified from:
        cv5Index = group*52 + tile*2 + 0x14
        where:  group = TileValue / 16  = high 12 bits
                tile  = TileValue & 0xF = low 4 bits   */

    return cv5Index < tiles->cv5.size();
}
