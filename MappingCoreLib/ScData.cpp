#include "ScData.h"
#include "Sc.h"
#include "../CommanderLib/Logger.h"

extern Logger logger;

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

Upgrades::~Upgrades()
{

}

bool Upgrades::LoadUpgrades(const std::vector<MpqFilePtr> & orderedSourceFiles)
{
    buffer upgradeDat("dDat");

    if ( !Sc::Data::GetAsset(orderedSourceFiles, "arr\\upgrades.dat", upgradeDat) ||
        !( upgradeDat.size() == 1281 || upgradeDat.size() == 920 ) )
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

TblFiles::~TblFiles()
{

}

bool TblFiles::Load(const std::vector<MpqFilePtr> & orderedSourceFiles)
{
    return Sc::Data::GetAsset(orderedSourceFiles, "Rez\\stat_txt.tbl", stat_txtTbl);
}

bool TblFiles::GetStatTblString(u16 stringNum, std::string & outString)
{
    return GetTblString(stat_txtTbl, stringNum, outString);
}

bool TblFiles::GetTblString(buffer & tbl, u16 stringNum, std::string & outString)
{
    outString.clear();
    u16 numStrings = tbl.get<u16>(0);

    if ( tbl.exists() && stringNum != 0 )
    {
        s64 start = tbl.get<u16>(2 * stringNum);
        s64 end = 0;
        if ( !tbl.getNext('\0', start, end) ) // Next NUL char marks the end of the string
            end = tbl.size(); // Just end it where buffer ends

        size_t length = (size_t)(end - start);
        outString = (const char*)tbl.getPtr(start);
        return true;
    }
    return false;
}

AiScripts::~AiScripts()
{

}

bool AiScripts::Load(const std::vector<MpqFilePtr> & orderedSourceFiles)
{
    return Sc::Data::GetAsset(orderedSourceFiles, "scripts\\AISCRIPT.BIN", aiScriptBin);
}

bool AiScripts::GetAiEntry(int aiNum, AIEntry & outAiEntry)
{
    return aiScriptBin.get<AIEntry>(outAiEntry, aiNum*sizeof(AIEntry) + aiScriptBin.get<u32>(0));
}

int AiScripts::GetNumAiScripts()
{
    return (u32(aiScriptBin.size()) - aiScriptBin.get<u32>(0)) / sizeof(AIEntry);
}

bool AiScripts::GetAiIdentifier(int aiNum, u32 & outAiId)
{
    AIEntry aiEntry = {};
    if ( GetAiEntry(aiNum, aiEntry) )
    {
        outAiId = aiEntry.identifier;
        return true;
    }
    return false;
}

bool AiScripts::GetAiIdentifier(const std::string & inAiName, u32 & outAiId)
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

bool AiScripts::GetAiName(int aiNum, std::string & outAiName)
{
    AIEntry aiEntry = {};
    return GetAiEntry(aiNum, aiEntry) && tblFiles.GetStatTblString(aiEntry.statStrIndex, outAiName);
}

bool AiScripts::GetAiName(u32 aiId, std::string & outAiName)
{
    int numAiScripts = GetNumAiScripts();
    for ( int i = 0; i < numAiScripts; i++  )
    {
        AIEntry aiEntry = {};
        if ( GetAiEntry(i, aiEntry) )
        {
            if ( aiId == aiEntry.identifier )
                return GetAiName(i, outAiName);
        }
    }
    return false;
}

bool AiScripts::GetAiIdAndName(int aiNum, u32 & outId, std::string & outAiName)
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

ScData::ScData() : aiScripts(tblFiles)
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

    if ( !upgrades.LoadUpgrades(orderedSourceFiles) )
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

    if ( !aiScripts.Load(orderedSourceFiles) )
        CHKD_ERR("Failed to load AIScripts");

    if ( !tblFiles.Load(orderedSourceFiles) )
        CHKD_ERR("Failed to load tbl files");

    return true;
}
