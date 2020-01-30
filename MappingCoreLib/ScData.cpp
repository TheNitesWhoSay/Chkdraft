#include "ScData.h"
#include "Sc.h"
#include "../CommanderLib/Logger.h"

extern Logger logger;

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

Techs::~Techs()
{

}

bool Techs::LoadTechs(const std::vector<MpqFilePtr> & orderedSourceFiles)
{
    buffer techDat("tDat");

    if ( !Sc::Data::GetAsset(orderedSourceFiles, "arr\\techdata.dat", techDat) ||
        !( techDat.size() == 836 || techDat.size() == 432 ) )
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

Weapons::~Weapons()
{

}

bool Weapons::LoadWeapons(const std::vector<MpqFilePtr> & orderedSourceFiles)
{
    buffer weaponDat("wDat");

    if ( !Sc::Data::GetAsset(orderedSourceFiles, "arr\\weapons.dat", weaponDat) ||
        weaponDat.size() != 5460 )
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

ScData::ScData() //: aiScripts(tblFiles)
{

}

ScData::~ScData()
{

}

bool ScData::Load(Sc::DataFile::BrowserPtr dataFileBrowser, const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles, const std::string & expectedStarCraftDirectory, FileBrowserPtr<u32> starCraftBrowser)
{
    if ( dataFileBrowser == nullptr )
        return false;

    const std::vector<MpqFilePtr> orderedSourceFiles = dataFileBrowser->openScDataFiles(dataFiles, expectedStarCraftDirectory, starCraftBrowser);
    if ( orderedSourceFiles.empty() )
    {
        CHKD_ERR("No archives selected, many features will not work without the game files.\n\nInstall or locate StarCraft for the best experience.");
        return false;
    }
    
    if ( !terrain.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load terrain");

    if ( !upgrades.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load upgrades");

    if ( !techs.LoadTechs(orderedSourceFiles) )
        CHKD_ERR("Failed to load techs");

    if ( !units.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load unit dat");

    if ( !weapons.LoadWeapons(orderedSourceFiles) )
        CHKD_ERR("Failed to load Weapons.dat");

    if ( !sprites.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load sprites!");

    if ( !tunit.load(orderedSourceFiles, "game\\tunit.pcx") )
        CHKD_ERR("Failed to load tunit.pcx");

    if ( !tminimap.load(orderedSourceFiles, "game\\tminimap.pcx") )
        CHKD_ERR("Failed to load tminimap.pcx");

    if ( !tselect.load(orderedSourceFiles, "game\\tselect.pcx") )
        CHKD_ERR("Failed to load tselect.pcx");

    Sc::TblFilePtr statTxt = Sc::TblFilePtr(new Sc::TblFile());
    if ( !statTxt->load(orderedSourceFiles, "Rez\\stat_txt.tbl") )
        CHKD_ERR("Failed to load stat_txt.tbl");

    if ( !ai.load(orderedSourceFiles, statTxt) )
        CHKD_ERR("Failed to load AiScripts");

    return true;
}
