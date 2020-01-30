#include "ScData.h"
#include "Sc.h"
#include "../CommanderLib/Logger.h"

extern Logger logger;

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

    if ( !techs.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load techs");

    if ( !units.load(orderedSourceFiles) )
        CHKD_ERR("Failed to load unit dat");

    if ( !weapons.load(orderedSourceFiles) )
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
