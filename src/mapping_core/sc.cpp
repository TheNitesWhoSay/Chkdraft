#include "sc.h"
#include <rarecpp/json.h>
#include "cross_cut/logger.h"
#include "mpq_file.h"
#include "casc_archive.h"
#include <algorithm>
#include <chrono>
#include <list>
#include <set>
#include <stdexcept>
#include <tuple>

extern Logger logger;

const std::string Sc::DataFile::starCraftFileName = "StarCraft.exe";
const std::string Sc::DataFile::buildInfoFileName = ".build.info";
const std::string Sc::DataFile::starDatFileName = "StarDat.mpq";
const std::string Sc::DataFile::brooDatFileName = "BrooDat.mpq";
const std::string Sc::DataFile::patchRtFileName = "patch_rt.mpq";

std::vector<FilterEntry<u32>> Sc::DataFile::getStarDatFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>(starDatFileName, starDatFileName + " Archive") };
}
std::vector<FilterEntry<u32>> Sc::DataFile::getBrooDatFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>(brooDatFileName, brooDatFileName + " Archive") };
}
std::vector<FilterEntry<u32>> Sc::DataFile::getPatchRtFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>(patchRtFileName, patchRtFileName + " Archive") };
}
std::vector<FilterEntry<u32>> Sc::DataFile::getStarCraftExeFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>(starCraftFileName, "StarCraft Executable") };
}
std::vector<FilterEntry<u32>> Sc::DataFile::getCascBuildInfoFilter()
{
    return std::vector<FilterEntry<u32>> { FilterEntry<u32>(buildInfoFileName, "Casc Build Info") };
}

Sc::DataFile::Descriptor::Descriptor(Priority priority, bool isCasc, bool isOptionalIfCascFound, const std::string & fileName, const std::string & expectedFilePath, FileBrowserPtr<u32> browser, bool expectedInScDirectory)
    : priority(priority), fileName(fileName), expectedFilePath(expectedFilePath), browser(browser), expectedInScDirectory(expectedInScDirectory), isCascDataFile(isCasc), optionalIfCascFound(isOptionalIfCascFound)
{

}

Sc::DataFile::Priority Sc::DataFile::Descriptor::getPriority() const
{
    return priority;
}

const std::string & Sc::DataFile::Descriptor::getFileName() const
{
    return fileName;
}

const std::string & Sc::DataFile::Descriptor::getExpectedFilePath() const
{
    return expectedFilePath;
}

FileBrowserPtr<u32> Sc::DataFile::Descriptor::getBrowser() const
{
    return browser;
}

bool Sc::DataFile::Descriptor::isExpectedInScDirectory() const
{
    return expectedInScDirectory;
}

bool Sc::DataFile::Descriptor::isCasc() const
{
    return isCascDataFile;
}

bool Sc::DataFile::Descriptor::isOptionalIfCascFound() const
{
    return optionalIfCascFound;
}

void Sc::DataFile::Descriptor::setExpectedFilePath(const std::string & expectedFilePath)
{
    this->expectedFilePath = expectedFilePath;
}

std::vector<Sc::DataFile::Descriptor> Sc::DataFile::getDefaultDataFiles(RemasteredDescriptor remasteredDescriptor)
{
    switch ( remasteredDescriptor )
    {
    case RemasteredDescriptor::No:
        return std::vector<Descriptor> {
            Descriptor(Priority::PatchRt, false, true, patchRtFileName, "", std::make_shared<FileBrowser<u32>>(getPatchRtFilter(), "")),
            Descriptor(Priority::BrooDat, false, true, brooDatFileName, "", std::make_shared<FileBrowser<u32>>(getBrooDatFilter(), "")),
            Descriptor(Priority::StarDat, false, true, starDatFileName, "", std::make_shared<FileBrowser<u32>>(getStarDatFilter(), ""))
        };
    case RemasteredDescriptor::Yes:
        return std::vector<Descriptor> {
            Descriptor(Priority::RemasteredCasc, true, false, "Data", "", nullptr)
        };
    case RemasteredDescriptor::Either:
    default:
        return std::vector<Descriptor> {
            Descriptor(Priority::RemasteredCasc, true, false, "Data", "", nullptr),
            Descriptor(Priority::PatchRt, false, true, patchRtFileName, "", std::make_shared<FileBrowser<u32>>(getPatchRtFilter(), "")),
            Descriptor(Priority::BrooDat, false, true, brooDatFileName, "", std::make_shared<FileBrowser<u32>>(getBrooDatFilter(), "")),
            Descriptor(Priority::StarDat, false, true, starDatFileName, "", std::make_shared<FileBrowser<u32>>(getStarDatFilter(), ""))
        };
    }
}

ArchiveClusterPtr Sc::DataFile::Browser::openScDataFiles(
    bool & includesRemastered,
    const std::vector<Descriptor> & constDataFileDescriptors,
    const std::string & expectedStarCraftDirectory,
    FileBrowserPtr<u32> starCraftBrowser)
{
    std::vector<Descriptor> dataFileDescriptors = constDataFileDescriptors;
    std::sort(dataFileDescriptors.begin(), dataFileDescriptors.end(),
        [](const Descriptor & l, const Descriptor & r) { return l.getPriority() < r.getPriority(); });

    std::unordered_map<Priority, ArchiveFilePtr> openedDataFiles;
    std::string scDirectory = "";
    bool foundStarCraftDirectory = false;
    bool declinedScBrowser = false;
    bool openedCasc = false;

    for ( auto dataFileDescriptor : dataFileDescriptors )
    {
        Priority dataFilePriority = dataFileDescriptor.getPriority();
        if ( dataFilePriority != dataFileDescriptor.getPriority() )
            throw std::runtime_error("The dataFilePriority provided in the dataFile key must match the dataFilePriority in the associated descriptor.");

        const std::string & fileName = dataFileDescriptor.getFileName();
        const std::string & expectedFilePath = dataFileDescriptor.getExpectedFilePath();
        const FileBrowserPtr<u32> browser = dataFileDescriptor.getBrowser();
        const bool expectedInScDirectory = dataFileDescriptor.isExpectedInScDirectory();
        bool isOptional = openedCasc && dataFileDescriptor.isOptionalIfCascFound();
        bool skipStarCraftBrowse = isOptional || (expectedInScDirectory && declinedScBrowser);
        bool foundRemastered = false;

        bool opened = false;
        u32 filterIndex = 0;
        std::string browsedFilePath = "";
        ArchiveFilePtr dataFile = nullptr;

        if ( !expectedFilePath.empty() && findFile(expectedFilePath) )
        {
            dataFile = openDataFile(expectedFilePath, dataFileDescriptor);
            if ( dataFile && dataFileDescriptor.isCasc() )
                includesRemastered = true;
        }
        else if ( !skipStarCraftBrowse && expectedInScDirectory &&
            (foundStarCraftDirectory || (foundStarCraftDirectory = findStarCraftDirectory(scDirectory, foundRemastered, declinedScBrowser, expectedStarCraftDirectory, starCraftBrowser)))
            && (foundRemastered || findFile(makeSystemFilePath(scDirectory, fileName))) )
        {
            if ( foundRemastered )
                includesRemastered = true;

            foundStarCraftDirectory = true;
            dataFile = openDataFile(makeSystemFilePath(scDirectory, fileName), dataFileDescriptor);
        }
        else if ( !isOptional && browser != nullptr && browser->promptTryBrowse("Failed to find " + fileName + " would you like to locate it manually?") && browser->browseForOpenPath(browsedFilePath, filterIndex) )
            dataFile = openDataFile(browsedFilePath, dataFileDescriptor);

        if ( dataFile != nullptr )
        {
            if ( dataFileDescriptor.isCasc() )
                openedCasc = true;

            openedDataFiles.insert({dataFilePriority, dataFile});
        }
    }

    std::vector<ArchiveFilePtr> orderedDataFiles;
    std::list<Priority> dataFilePriorities;
    for ( auto openedDataFile : openedDataFiles )
        dataFilePriorities.push_back(openedDataFile.first);
    
    dataFilePriorities.sort();
    for ( auto dataFilePriority = dataFilePriorities.begin(); dataFilePriority != dataFilePriorities.end(); ++dataFilePriority )
        orderedDataFiles.push_back(openedDataFiles.find(*dataFilePriority)->second);

    return std::make_shared<ArchiveCluster>(orderedDataFiles);
}

ArchiveClusterPtr Sc::DataFile::Browser::openScDataFiles(
    const std::vector<Descriptor> & constDataFileDescriptors,
    const std::string & expectedStarCraftDirectory,
    FileBrowserPtr<u32> starCraftBrowser)
{
    bool discarded = false;
    return openScDataFiles(discarded, constDataFileDescriptors, expectedStarCraftDirectory, starCraftBrowser);
}

bool Sc::DataFile::Browser::findStarCraftDirectory(std::string & starCraftDirectory, bool & isRemastered, bool & declinedStarCraftBrowse,
    const std::string & expectedStarCraftDirectory, FileBrowserPtr<u32> starCraftBrowser)
{
    u32 filterIndex = 0;
    if ( !expectedStarCraftDirectory.empty() && findFile(makeSystemFilePath(expectedStarCraftDirectory, starCraftFileName)) &&
        !findFile(makeSystemFilePath(expectedStarCraftDirectory, buildInfoFileName)) )
    {
        starCraftDirectory = expectedStarCraftDirectory;
        isRemastered = false;
        return true;
    }
    else if ( isDirectory(makeSystemFilePath(expectedStarCraftDirectory, "Data")) )
    {
        starCraftDirectory = expectedStarCraftDirectory;
        isRemastered = true;
        return true;
    }
    else if ( starCraftBrowser != nullptr )
    {
        std::string starCraftFilePath = "";
        if ( starCraftBrowser->promptTryBrowse("Failed to find " + starCraftFileName + " would you like to locate it manually?") &&
            starCraftBrowser->browseForOpenPath(starCraftFilePath, filterIndex) )
        {
            isRemastered = false;
            starCraftDirectory = getSystemFileDirectory(starCraftFilePath, false);
            auto directoryName = getSystemFileName(starCraftDirectory);
            if ( directoryName == "x86" || directoryName == "x86_64" )
            {
                auto remasteredDirectory = getSystemFileDirectory(starCraftDirectory, true);
                auto dataDirectory = remasteredDirectory + "Data";
                if ( ::isDirectory(dataDirectory) )
                {
                    isRemastered = true;
                    starCraftDirectory = remasteredDirectory;
                }
            }
            return true;
        }
        else
            declinedStarCraftBrowse = true;
    }
    return false;
}

ArchiveFilePtr Sc::DataFile::Browser::openDataFile(const std::string & dataFilePath, const Descriptor & dataFileDescriptor)
{
    ArchiveFilePtr archiveFile = dataFileDescriptor.isCasc() ?
        std::static_pointer_cast<ArchiveFile>(std::make_shared<CascArchive>()) :
        std::static_pointer_cast<ArchiveFile>(std::make_shared<MpqFile>());

    do
    {
        if ( archiveFile->open(dataFilePath, true, false) )
            return archiveFile;
    }
    while ( dataFileDescriptor.getBrowser() != nullptr &&
            dataFileDescriptor.getBrowser()->promptOpenRetry("Failed to open " + dataFilePath + "! The file may be in use.\n\nWould you like to try again?") );
    
    return nullptr;
}

FileBrowserPtr<u32> Sc::DataFile::Browser::getDefaultStarCraftBrowser()
{
    return FileBrowserPtr<u32>(new FileBrowser<u32>(getStarCraftExeFilter(), getDefaultScPath()));
}

FileBrowserPtr<u32> Sc::DataFile::Browser::getNoPromptNoDefaultStarCraftBrowser()
{
    return FileBrowserPtr<u32>(new NoPromptFileBrowser<u32>(getStarCraftExeFilter(), ""));
}

bool Sc::Unit::load(ArchiveCluster & archiveCluster)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto unitData = Sc::Data::GetAsset(archiveCluster, "arr\\units.dat");
    if ( !unitData )
    {
        logger.error() << "Failed to load arr\\units.dat" << std::endl;
        return false;
    }
    else if ( unitData->size() != sizeof(Sc::Unit::DatFile) )
    {
        logger.error() << "Unrecognized UnitDat format!" << std::endl;
        return false;
    }

    DatFile & dat = (DatFile &)unitData.value()[0];
    size_t i=0;
    for ( ; i<DatFile::IdRange::From0To105; i++ )
    {
        units.push_back(Sc::Unit::DatEntry {
            dat.graphics[i], dat.subunit1[i], dat.subunit2[i], u16(0), dat.constructionAnimation[i],
            dat.unitDirection[i], dat.shieldEnable[i], dat.shieldAmount[i], dat.hitPoints[i], dat.elevationLevel[i],
            dat.unknown[i], dat.sublabel[i], dat.compAIIdle[i], dat.humanAIIdle[i], dat.returntoIdle[i], dat.attackUnit[i],
            dat.attackMove[i], dat.groundWeapon[i], dat.maxGroundHits[i], dat.airWeapon[i], dat.maxAirHits[i], dat.aiInternal[i],
            dat.flags[i], dat.targetAcquisitionRange[i], dat.sightRange[i], dat.armorUpgrade[i], dat.unitSize[i],
            dat.armor[i], dat.rightClickAction[i], dat.readySound[i], dat.whatSoundStart[i], dat.whatSoundEnd[i],
            dat.pissSoundStart[i], dat.pissSoundEnd[i], dat.yesSoundStart[i], dat.yesSoundEnd[i],
            dat.starEditPlacementBox[i].width, dat.starEditPlacementBox[i].height, u16(0), u16(0),
            dat.unitExtent[i].left, dat.unitExtent[i].up, dat.unitExtent[i].right, dat.unitExtent[i].down, dat.portrait[i], dat.mineralCost[i],
            dat.vespeneCost[i], dat.buildTime[i], dat.unknown1[i], dat.starEditGroupFlags[i], dat.supplyProvided[i], dat.supplyRequired[i],
            dat.spaceRequired[i], dat.spaceProvided[i], dat.buildScore[i], dat.destroyScore[i], dat.unitMapString[i], dat.broodWarUnitFlag[i],
            dat.starEditAvailabilityFlags[i]
        });
    }
    for ( ; i<DatFile::IdRange::From0To105+DatFile::IdRange::From106To201; i++ )
    {
        units.push_back(Sc::Unit::DatEntry {
            dat.graphics[i], dat.subunit1[i], dat.subunit2[i], dat.infestation[i-106], dat.constructionAnimation[i],
            dat.unitDirection[i], dat.shieldEnable[i], dat.shieldAmount[i], dat.hitPoints[i], dat.elevationLevel[i],
            dat.unknown[i], dat.sublabel[i], dat.compAIIdle[i], dat.humanAIIdle[i], dat.returntoIdle[i], dat.attackUnit[i],
            dat.attackMove[i], dat.groundWeapon[i], dat.maxGroundHits[i], dat.airWeapon[i], dat.maxAirHits[i], dat.aiInternal[i],
            dat.flags[i], dat.targetAcquisitionRange[i], dat.sightRange[i], dat.armorUpgrade[i], dat.unitSize[i],
            dat.armor[i], dat.rightClickAction[i], u16(0), dat.whatSoundStart[i], dat.whatSoundEnd[i],
            u16(0), u16(0), u16(0), u16(0),
            dat.starEditPlacementBox[i].width, dat.starEditPlacementBox[i].height, dat.addonOffset[i-106].width, dat.addonOffset[i-106].height,
            dat.unitExtent[i].left, dat.unitExtent[i].up, dat.unitExtent[i].right, dat.unitExtent[i].down, dat.portrait[i], dat.mineralCost[i],
            dat.vespeneCost[i], dat.buildTime[i], dat.unknown1[i], dat.starEditGroupFlags[i], dat.supplyProvided[i], dat.supplyRequired[i],
            dat.spaceRequired[i], dat.spaceProvided[i], dat.buildScore[i], dat.destroyScore[i], dat.unitMapString[i], dat.broodWarUnitFlag[i],
            dat.starEditAvailabilityFlags[i]
        });
    }
    for ( ; i<TotalTypes; i++ )
    {
        units.push_back(Sc::Unit::DatEntry {
            dat.graphics[i], dat.subunit1[i], dat.subunit2[i], u16(0), dat.constructionAnimation[i],
            dat.unitDirection[i], dat.shieldEnable[i], dat.shieldAmount[i], dat.hitPoints[i], dat.elevationLevel[i],
            dat.unknown[i], dat.sublabel[i], dat.compAIIdle[i], dat.humanAIIdle[i], dat.returntoIdle[i], dat.attackUnit[i],
            dat.attackMove[i], dat.groundWeapon[i], dat.maxGroundHits[i], dat.airWeapon[i], dat.maxAirHits[i], dat.aiInternal[i],
            dat.flags[i], dat.targetAcquisitionRange[i], dat.sightRange[i], dat.armorUpgrade[i], dat.unitSize[i],
            dat.armor[i], dat.rightClickAction[i], u16(0), dat.whatSoundStart[i], dat.whatSoundEnd[i],
            u16(0), u16(0), u16(0), u16(0),
            dat.starEditPlacementBox[i].width, dat.starEditPlacementBox[i].height, u16(0), u16(0),
            dat.unitExtent[i].left, dat.unitExtent[i].up, dat.unitExtent[i].right, dat.unitExtent[i].down, dat.portrait[i], dat.mineralCost[i],
            dat.vespeneCost[i], dat.buildTime[i], dat.unknown1[i], dat.starEditGroupFlags[i], dat.supplyProvided[i], dat.supplyRequired[i],
            dat.spaceRequired[i], dat.spaceProvided[i], dat.buildScore[i], dat.destroyScore[i], dat.unitMapString[i], dat.broodWarUnitFlag[i],
            dat.starEditAvailabilityFlags[i]
        });
    }

    auto flingyData = Sc::Data::GetAsset(archiveCluster, "arr\\flingy.dat");
    if ( !flingyData )
    {
        logger.error() << "Failed to load arr\\flingy.dat" << std::endl;
        return false;
    }
    else if ( flingyData->size() != sizeof(Sc::Unit::FlingyDatFile) )
    {
        logger.error() << "Unrecognized UnitDat format!" << std::endl;
        return false;
    }

    FlingyDatFile & flingyDat = (FlingyDatFile &)flingyData.value()[0];
    for ( i=0; i<TotalFlingies; i++ )
    {
        flingies.push_back(Sc::Unit::FlingyDatEntry { flingyDat.sprite[i], flingyDat.topSpeed[i], flingyDat.acceleration[i],
            flingyDat.haltDistance[i], flingyDat.turnRadius[i], flingyDat.unused[i], flingyDat.moveControl[i] });
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "Unit loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

const Sc::Unit::DatEntry & Sc::Unit::getUnit(Type unitType) const
{
    if ( unitType < units.size() )
        return units[unitType];
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for units vector of size " + std::to_string(units.size()));
}

const Sc::Unit::FlingyDatEntry & Sc::Unit::getFlingy(size_t flingyIndex) const
{
    if ( flingyIndex < flingies.size() )
        return flingies[flingyIndex];
    else
        throw std::out_of_range(std::string("FlingyIndex: ") + std::to_string(flingyIndex) + " is out of range for flingies vector of size " + std::to_string(flingies.size()));
}


const std::unordered_map<Sc::Ai::ScriptId, std::string> Sc::Ai::scriptNames = {
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter3, "Terran 3 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter5, "Terran 5 - Terran Main Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te5H, "Terran 5 - Terran Harvest Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter6, "Terran 6 - Air Attack Zerg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te6b, "Terran 6 - Ground Attack Zerg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te6c, "Terran 6 - Zerg Support Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter7, "Terran 7 - Bottom Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te7s, "Terran 7 - Right Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te7m, "Terran 7 - Middle Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter8, "Terran 8 - Confederate Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Tr9L, "Terran 9 - Light Attack" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Tr9H, "Terran 9 - Heavy Attack" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te10, "Terran 10 - Confederate Towns" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11z, "Terran 11 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11a, "Terran 11 - Lower Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11b, "Terran 11 - Upper Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12N, "Terran 12 - Nuke Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12P, "Terran 12 - Phoenix Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12T, "Terran 12 - Tank Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED1, "Terran 1 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED2, "Terran 2 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED3, "Terran 3 - Electronic Distribution" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW1, "Terran 1 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW2, "Terran 2 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW3, "Terran 3 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW4, "Terran 4 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW5, "Terran 5 - Shareware" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer1, "Zerg 1 - Terran Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer2, "Zerg 2 - Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer3, "Zerg 3 - Terran Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer4, "Zerg 4 - Right Terran Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ze4S, "Zerg 4 - Lower Terran Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer6, "Zerg 6 - Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7a, "Zerg 7 - Air Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7g, "Zerg 7 - Ground Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7s, "Zerg 7 - Support Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer8, "Zerg 8 - Scout Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ze8T, "Zerg 8 - Templar Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer9, "Zerg 9 - Teal Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ly, "Zerg 9 - Left Yellow Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ry, "Zerg 9 - Right Yellow Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9lo, "Zerg 9 - Left Orange Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ro, "Zerg 9 - Right Orange Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10a, "Zerg 10 - Left Teal (Attack)," ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10b, "Zerg 10 - Right Teal (Support)," ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10c, "Zerg 10 - Left Yellow (Support)," ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10d, "Zerg 10 - Right Yellow (Attack)," ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10e, "Zerg 10 - Red Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro1, "Protoss 1 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro2, "Protoss 2 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr3R, "Protoss 3 - Air Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr3G, "Protoss 3 - Ground Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro4, "Protoss 4 - Zerg Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr5I, "Protoss 5 - Zerg Town Island" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr5B, "Protoss 5 - Zerg Town Base" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro7, "Protoss 7 - Left Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr7B, "Protoss 7 - Right Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr7S, "Protoss 7 - Shrine Protoss" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro8, "Protoss 8 - Left Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr8B, "Protoss 8 - Right Protoss Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr8D, "Protoss 8 - Protoss Defenders" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro9, "Protoss 9 - Ground Zerg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr9W, "Protoss 9 - Air Zerg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr9Y, "Protoss 9 - Spell Zerg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr10, "Protoss 10 - Mini-Towns" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::P10C, "Protoss 10 - Mini-Town Master" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::P10o, "Protoss 10 - Overmind Defenders" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1A, "Brood Wars Protoss 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1B, "Brood Wars Protoss 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1C, "Brood Wars Protoss 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1D, "Brood Wars Protoss 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1E, "Brood Wars Protoss 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1F, "Brood Wars Protoss 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2A, "Brood Wars Protoss 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2B, "Brood Wars Protoss 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2C, "Brood Wars Protoss 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2D, "Brood Wars Protoss 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2E, "Brood Wars Protoss 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2F, "Brood Wars Protoss 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3A, "Brood Wars Protoss 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3B, "Brood Wars Protoss 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3C, "Brood Wars Protoss 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3D, "Brood Wars Protoss 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3E, "Brood Wars Protoss 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3F, "Brood Wars Protoss 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4A, "Brood Wars Protoss 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4B, "Brood Wars Protoss 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4C, "Brood Wars Protoss 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4D, "Brood Wars Protoss 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4E, "Brood Wars Protoss 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4F, "Brood Wars Protoss 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5A, "Brood Wars Protoss 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5B, "Brood Wars Protoss 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5C, "Brood Wars Protoss 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5D, "Brood Wars Protoss 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5E, "Brood Wars Protoss 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5F, "Brood Wars Protoss 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6A, "Brood Wars Protoss 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6B, "Brood Wars Protoss 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6C, "Brood Wars Protoss 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6D, "Brood Wars Protoss 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6E, "Brood Wars Protoss 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6F, "Brood Wars Protoss 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7A, "Brood Wars Protoss 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7B, "Brood Wars Protoss 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7C, "Brood Wars Protoss 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7D, "Brood Wars Protoss 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7E, "Brood Wars Protoss 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7F, "Brood Wars Protoss 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8A, "Brood Wars Protoss 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8B, "Brood Wars Protoss 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8C, "Brood Wars Protoss 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8D, "Brood Wars Protoss 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8E, "Brood Wars Protoss 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8F, "Brood Wars Protoss 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1A, "Brood Wars Terran 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1B, "Brood Wars Terran 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1C, "Brood Wars Terran 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1D, "Brood Wars Terran 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1E, "Brood Wars Terran 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1F, "Brood Wars Terran 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2A, "Brood Wars Terran 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2B, "Brood Wars Terran 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2C, "Brood Wars Terran 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2D, "Brood Wars Terran 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2E, "Brood Wars Terran 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2F, "Brood Wars Terran 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3A, "Brood Wars Terran 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3B, "Brood Wars Terran 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3C, "Brood Wars Terran 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3D, "Brood Wars Terran 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3E, "Brood Wars Terran 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3F, "Brood Wars Terran 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4A, "Brood Wars Terran 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4B, "Brood Wars Terran 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4C, "Brood Wars Terran 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4D, "Brood Wars Terran 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4E, "Brood Wars Terran 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4F, "Brood Wars Terran 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5A, "Brood Wars Terran 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5B, "Brood Wars Terran 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5C, "Brood Wars Terran 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5D, "Brood Wars Terran 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5E, "Brood Wars Terran 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5F, "Brood Wars Terran 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6A, "Brood Wars Terran 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6B, "Brood Wars Terran 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6C, "Brood Wars Terran 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6D, "Brood Wars Terran 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6E, "Brood Wars Terran 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6F, "Brood Wars Terran 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7A, "Brood Wars Terran 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7B, "Brood Wars Terran 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7C, "Brood Wars Terran 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7D, "Brood Wars Terran 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7E, "Brood Wars Terran 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7F, "Brood Wars Terran 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8A, "Brood Wars Terran 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8B, "Brood Wars Terran 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8C, "Brood Wars Terran 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8D, "Brood Wars Terran 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8E, "Brood Wars Terran 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8F, "Brood Wars Terran 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1A, "Brood Wars Zerg 1 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1B, "Brood Wars Zerg 1 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1C, "Brood Wars Zerg 1 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1D, "Brood Wars Zerg 1 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1E, "Brood Wars Zerg 1 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1F, "Brood Wars Zerg 1 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2A, "Brood Wars Zerg 2 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2B, "Brood Wars Zerg 2 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2C, "Brood Wars Zerg 2 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2D, "Brood Wars Zerg 2 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2E, "Brood Wars Zerg 2 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2F, "Brood Wars Zerg 2 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3A, "Brood Wars Zerg 3 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3B, "Brood Wars Zerg 3 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3C, "Brood Wars Zerg 3 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3D, "Brood Wars Zerg 3 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3E, "Brood Wars Zerg 3 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3F, "Brood Wars Zerg 3 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4A, "Brood Wars Zerg 4 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4B, "Brood Wars Zerg 4 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4C, "Brood Wars Zerg 4 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4D, "Brood Wars Zerg 4 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4E, "Brood Wars Zerg 4 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4F, "Brood Wars Zerg 4 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5A, "Brood Wars Zerg 5 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5B, "Brood Wars Zerg 5 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5C, "Brood Wars Zerg 5 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5D, "Brood Wars Zerg 5 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5E, "Brood Wars Zerg 5 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5F, "Brood Wars Zerg 5 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6A, "Brood Wars Zerg 6 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6B, "Brood Wars Zerg 6 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6C, "Brood Wars Zerg 6 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6D, "Brood Wars Zerg 6 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6E, "Brood Wars Zerg 6 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6F, "Brood Wars Zerg 6 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7A, "Brood Wars Zerg 7 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7B, "Brood Wars Zerg 7 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7C, "Brood Wars Zerg 7 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7D, "Brood Wars Zerg 7 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7E, "Brood Wars Zerg 7 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7F, "Brood Wars Zerg 7 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8A, "Brood Wars Zerg 8 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8B, "Brood Wars Zerg 8 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8C, "Brood Wars Zerg 8 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8D, "Brood Wars Zerg 8 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8E, "Brood Wars Zerg 8 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8F, "Brood Wars Zerg 8 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9A, "Brood Wars Zerg 9 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9B, "Brood Wars Zerg 9 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9C, "Brood Wars Zerg 9 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9D, "Brood Wars Zerg 9 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9E, "Brood Wars Zerg 9 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9F, "Brood Wars Zerg 9 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0A, "Brood Wars Zerg 10 - Town A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0B, "Brood Wars Zerg 10 - Town B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0C, "Brood Wars Zerg 10 - Town C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0D, "Brood Wars Zerg 10 - Town D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0E, "Brood Wars Zerg 10 - Town E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0F, "Brood Wars Zerg 10 - Town F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMCu, "Terran Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMCu, "Zerg Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMCu, "Protoss Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMCx, "Terran Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMCx, "Zerg Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMCx, "Protoss Expansion Custom Level" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TLOf, "Terran Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMED, "Terran Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::THIf, "Terran Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSUP, "Terran Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TARE, "Terran Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZLOf, "Zerg Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMED, "Zerg Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZHIf, "Zerg Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZSUP, "Zerg Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZARE, "Zerg Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PLOf, "Protoss Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMED, "Protoss Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PHIf, "Protoss Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PSUP, "Protoss Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PARE, "Protoss Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TLOx, "Expansion Terran Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMEx, "Expansion Terran Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::THIx, "Expansion Terran Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSUx, "Expansion Terran Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TARx, "Expansion Terran Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZLOx, "Expansion Zerg Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMEx, "Expansion Zerg Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZHIx, "Expansion Zerg Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZSUx, "Expansion Zerg Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZARx, "Expansion Zerg Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PLOx, "Expansion Protoss Campaign Easy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMEx, "Expansion Protoss Campaign Medium" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PHIx, "Expansion Protoss Campaign Difficult" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PSUx, "Expansion Protoss Campaign Insane" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PARx, "Expansion Protoss Campaign Area Town" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Suic, "Send All Units on Strategic Suicide Missions" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::SuiR, "Send All Units on Random Suicide Missions" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Rscu, "Switch Computer Player to Rescue Passive" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi0, "Turn ON Shared Vision for Player 1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi1, "Turn ON Shared Vision for Player 2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi2, "Turn ON Shared Vision for Player 3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi3, "Turn ON Shared Vision for Player 4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi4, "Turn ON Shared Vision for Player 5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi5, "Turn ON Shared Vision for Player 6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi6, "Turn ON Shared Vision for Player 7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi7, "Turn ON Shared Vision for Player 8" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi0, "Turn OFF Shared Vision for Player 1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi1, "Turn OFF Shared Vision for Player 2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi2, "Turn OFF Shared Vision for Player 3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi3, "Turn OFF Shared Vision for Player 4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi4, "Turn OFF Shared Vision for Player 5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi5, "Turn OFF Shared Vision for Player 6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi6, "Turn OFF Shared Vision for Player 7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi7, "Turn OFF Shared Vision for Player 8" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MvTe, "Move Dark Templars to Region" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ClrC, "Clear Previous Combat Data" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Enmy, "Set Player to Enemy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ally, "Set Player to Ally" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::VluA, "Value This Area Higher" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::EnBk, "Enter Closest Bunker" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::StTg, "Set Generic Command Target" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::StPt, "Make These Units Patrol" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::EnTr, "Enter Transport" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ExTr, "Exit Transport" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::NuHe, "AI Nuke Here" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::HaHe, "AI Harass Here" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::JYDg, "Set Unit Order To: Junk Yard Dog" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::DWHe, "AI Nuke Here" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ReHe, "AI Nuke Here" )
};

const std::unordered_map<Sc::Ai::ScriptId, std::string> scriptIdStr = {
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter3, "Ter3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter5, "Ter5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te5H, "Te5H" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter6, "Ter6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te6b, "Te6b" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te6c, "Te6c" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter7, "Ter7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te7s, "Te7s" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te7m, "Te7m" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ter8, "Ter8" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Tr9L, "Tr9L" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Tr9H, "Tr9H" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Te10, "Te10" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11z, "T11z" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11a, "T11a" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T11b, "T11b" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12N, "T12N" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12P, "T12P" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::T12T, "T12T" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED1, "TED1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED2, "TED2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TED3, "TED3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW1, "TSW1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW2, "TSW2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW3, "TSW3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW4, "TSW4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSW5, "TSW5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer1, "Zer1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer2, "Zer2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer3, "Zer3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer4, "Zer4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ze4S, "Ze4S" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer6, "Zer6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7a, "Zr7a" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7g, "Zr7g" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zr7s, "Zr7s" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer8, "Zer8" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ze8T, "Ze8T" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Zer9, "Zer9" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ly, "Z9ly" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ry, "Z9ry" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9lo, "Z9lo" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z9ro, "Z9ro" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10a, "Z10a" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10b, "Z10b" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10c, "Z10c" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10d, "Z10d" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Z10e, "Z10e" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro1, "Pro1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro2, "Pro2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr3R, "Pr3R" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr3G, "Pr3G" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro4, "Pro4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr5I, "Pr5I" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr5B, "Pr5B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro7, "Pro7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr7B, "Pr7B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr7S, "Pr7S" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro8, "Pro8" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr8B, "Pr8B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr8D, "Pr8D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pro9, "Pro9" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr9W, "Pr9W" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr9Y, "Pr9Y" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Pr10, "Pr10" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::P10C, "P10C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::P10o, "P10o" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1A, "PB1A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1B, "PB1B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1C, "PB1C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1D, "PB1D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1E, "PB1E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB1F, "PB1F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2A, "PB2A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2B, "PB2B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2C, "PB2C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2D, "PB2D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2E, "PB2E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB2F, "PB2F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3A, "PB3A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3B, "PB3B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3C, "PB3C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3D, "PB3D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3E, "PB3E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB3F, "PB3F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4A, "PB4A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4B, "PB4B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4C, "PB4C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4D, "PB4D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4E, "PB4E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB4F, "PB4F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5A, "PB5A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5B, "PB5B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5C, "PB5C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5D, "PB5D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5E, "PB5E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB5F, "PB5F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6A, "PB6A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6B, "PB6B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6C, "PB6C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6D, "PB6D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6E, "PB6E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB6F, "PB6F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7A, "PB7A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7B, "PB7B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7C, "PB7C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7D, "PB7D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7E, "PB7E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB7F, "PB7F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8A, "PB8A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8B, "PB8B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8C, "PB8C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8D, "PB8D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8E, "PB8E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PB8F, "PB8F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1A, "TB1A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1B, "TB1B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1C, "TB1C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1D, "TB1D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1E, "TB1E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB1F, "TB1F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2A, "TB2A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2B, "TB2B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2C, "TB2C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2D, "TB2D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2E, "TB2E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB2F, "TB2F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3A, "TB3A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3B, "TB3B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3C, "TB3C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3D, "TB3D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3E, "TB3E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB3F, "TB3F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4A, "TB4A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4B, "TB4B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4C, "TB4C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4D, "TB4D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4E, "TB4E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB4F, "TB4F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5A, "TB5A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5B, "TB5B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5C, "TB5C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5D, "TB5D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5E, "TB5E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB5F, "TB5F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6A, "TB6A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6B, "TB6B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6C, "TB6C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6D, "TB6D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6E, "TB6E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB6F, "TB6F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7A, "TB7A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7B, "TB7B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7C, "TB7C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7D, "TB7D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7E, "TB7E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB7F, "TB7F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8A, "TB8A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8B, "TB8B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8C, "TB8C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8D, "TB8D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8E, "TB8E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TB8F, "TB8F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1A, "ZB1A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1B, "ZB1B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1C, "ZB1C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1D, "ZB1D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1E, "ZB1E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB1F, "ZB1F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2A, "ZB2A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2B, "ZB2B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2C, "ZB2C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2D, "ZB2D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2E, "ZB2E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB2F, "ZB2F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3A, "ZB3A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3B, "ZB3B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3C, "ZB3C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3D, "ZB3D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3E, "ZB3E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB3F, "ZB3F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4A, "ZB4A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4B, "ZB4B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4C, "ZB4C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4D, "ZB4D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4E, "ZB4E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB4F, "ZB4F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5A, "ZB5A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5B, "ZB5B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5C, "ZB5C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5D, "ZB5D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5E, "ZB5E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB5F, "ZB5F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6A, "ZB6A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6B, "ZB6B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6C, "ZB6C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6D, "ZB6D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6E, "ZB6E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB6F, "ZB6F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7A, "ZB7A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7B, "ZB7B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7C, "ZB7C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7D, "ZB7D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7E, "ZB7E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB7F, "ZB7F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8A, "ZB8A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8B, "ZB8B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8C, "ZB8C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8D, "ZB8D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8E, "ZB8E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB8F, "ZB8F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9A, "ZB9A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9B, "ZB9B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9C, "ZB9C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9D, "ZB9D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9E, "ZB9E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB9F, "ZB9F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0A, "ZB0A" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0B, "ZB0B" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0C, "ZB0C" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0D, "ZB0D" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0E, "ZB0E" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZB0F, "ZB0F" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMCu, "TMCu" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMCu, "ZMCu" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMCu, "PMCu" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMCx, "TMCx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMCx, "ZMCx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMCx, "PMCx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TLOf, "TLOf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMED, "TMED" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::THIf, "THIf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSUP, "TSUP" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TARE, "TARE" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZLOf, "ZLOf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMED, "ZMED" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZHIf, "ZHIf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZSUP, "ZSUP" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZARE, "ZARE" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PLOf, "PLOf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMED, "PMED" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PHIf, "PHIf" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PSUP, "PSUP" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PARE, "PARE" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TLOx, "TLOx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TMEx, "TMEx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::THIx, "THIx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TSUx, "TSUx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::TARx, "TARx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZLOx, "ZLOx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZMEx, "ZMEx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZHIx, "ZHIx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZSUx, "ZSUx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ZARx, "ZARx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PLOx, "PLOx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PMEx, "PMEx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PHIx, "PHIx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PSUx, "PSUx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PARx, "PARx" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Suic, "Suic" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::SuiR, "SuiR" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Rscu, "Rscu" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi0, "+Vi0" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi1, "+Vi1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi2, "+Vi2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi3, "+Vi3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi4, "+Vi4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi5, "+Vi5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi6, "+Vi6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::PlusVi7, "+Vi7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi0, "-Vi0" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi1, "-Vi1" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi2, "-Vi2" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi3, "-Vi3" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi4, "-Vi4" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi5, "-Vi5" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi6, "-Vi6" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MinusVi7, "-Vi7" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::MvTe, "MvTe" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ClrC, "ClrC" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Enmy, "Enmy" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::Ally, "Ally" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::VluA, "VluA" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::EnBk, "EnBk" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::StTg, "StTg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::StPt, "StPt" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::EnTr, "EnTr" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ExTr, "ExTr" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::NuHe, "NuHe" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::HaHe, "HaHe" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::JYDg, "JYDg" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::DWHe, "DWHe" ),
    std::pair<Sc::Ai::ScriptId, const std::string>( Sc::Ai::ScriptId::ReHe, "ReHe" )
};

const std::vector<std::string> Sc::Unit::defaultDisplayNames = {
    /*  0*/ "Terran Marine",
    /*  1*/ "Terran Ghost", 
    /*  2*/ "Terran Vulture",
    /*  3*/ "Terran Goliath", 
    /*  4*/ "Goliath Turret", 
    /*  5*/ "Terran Siege Tank (Tank Mode)", 
    /*  6*/ "Siege Tank Turret (Tank Mode)",
    /*  7*/ "Terran SCV", 
    /*  8*/ "Terran Wraith",
    /*  9*/ "Terran Science Vessel",
    /* 10*/ "Gui Montag (Firebat)",
    /* 11*/ "Terran Dropship",
    /* 12*/ "Terran Battlecruiser",
    /* 13*/ "Spider Mine",
    /* 14*/ "Nuclear Missile",
    /* 15*/ "Terran Civilian",
    /* 16*/ "Sarah Kerrigan (Ghost)",
    /* 17*/ "Alan Schezar (Goliath)",
    /* 18*/ "Alan Schezar Turret",
    /* 19*/ "Jim Raynor (Vulture)",
    /* 20*/ "Jim Raynor (Marine)",
    /* 21*/ "Tom Kazansky (Wraith)",
    /* 22*/ "Magellan (Science Vessel)",
    /* 23*/ "Edmund Duke (Tank Mode)",
    /* 24*/ "Edmund Duke Turret (Tank Mode)",
    /* 25*/ "Edmund Duke (Siege Mode)",
    /* 26*/ "Edmund Duke Turret (Siege Mode)",
    /* 27*/ "Arcturus Mengsk (Battlecruiser)",
    /* 28*/ "Hyperion (Battlecruiser)",
    /* 29*/ "Norad II (Battlecruiser)",
    /* 30*/ "Terran Siege Tank (Siege Mode)",
    /* 31*/ "Siege Tank Turret (Siege Mode)",
    /* 32*/ "Terran Firebat",
    /* 33*/ "Scanner Sweep",
    /* 34*/ "Terran Medic",
    /* 35*/ "Zerg Larva",
    /* 36*/ "Zerg Egg",
    /* 37*/ "Zerg Zergling",
    /* 38*/ "Zerg Hydralisk",
    /* 39*/ "Zerg Ultralisk",
    /* 40*/ "Zerg Broodling",
    /* 41*/ "Zerg Drone",
    /* 42*/ "Zerg Overlord",
    /* 43*/ "Zerg Mutalisk",
    /* 44*/ "Zerg Guardian",
    /* 45*/ "Zerg Queen",
    /* 46*/ "Zerg Defiler",
    /* 47*/ "Zerg Scourge",
    /* 48*/ "Torrasque (Ultralisk)",
    /* 49*/ "Matriarch (Queen)",
    /* 50*/ "Infested Terran",
    /* 51*/ "Infested Kerrigan (Infested Terran)",
    /* 52*/ "Unclean One (Defiler)",
    /* 53*/ "Hunter Killer (Hydralisk)",
    /* 54*/ "Devouring One (Zergling)",
    /* 55*/ "Kukulza (Mutalisk)",
    /* 56*/ "Kukulza (Guardian)",
    /* 57*/ "Yggdrasill (Overlord)",
    /* 58*/ "Terran Valkyrie",
    /* 59*/ "Mutalisk Cocoon",
    /* 60*/ "Protoss Corsair",
    /* 61*/ "Protoss Dark Templar",
    /* 62*/ "Zerg Devourer",
    /* 63*/ "Protoss Dark Archon",
    /* 64*/ "Protoss Probe",
    /* 65*/ "Protoss Zealot",
    /* 66*/ "Protoss Dragoon",
    /* 67*/ "Protoss High Templar",
    /* 68*/ "Protoss Archon",
    /* 69*/ "Protoss Shuttle",
    /* 70*/ "Protoss Scout",
    /* 71*/ "Protoss Arbiter",
    /* 72*/ "Protoss Carrier",
    /* 73*/ "Protoss Interceptor",
    /* 74*/ "Protoss Dark Templar (Hero)",
    /* 75*/ "Zeratul (Dark Templar)",
    /* 76*/ "Tassadar/Zeratul (Archon)",
    /* 77*/ "Fenix (Zealot)",
    /* 78*/ "Fenix (Dragoon)",
    /* 79*/ "Tassadar (Templar)",
    /* 80*/ "Mojo (Scout)",
    /* 81*/ "Warbringer (Reaver)",
    /* 82*/ "Gantrithor (Carrier)",
    /* 83*/ "Protoss Reaver",
    /* 84*/ "Protoss Observer",
    /* 85*/ "Protoss Scarab",
    /* 86*/ "Danimoth (Arbiter)",
    /* 87*/ "Aldaris (Templar)",
    /* 88*/ "Artanis (Scout)",
    /* 89*/ "Rhynadon (Badlands Critter)",
    /* 90*/ "Bengalaas (Jungle Critter)",
    /* 91*/ "Cargo Ship (Unused)",
    /* 92*/ "Mercenary Gunship (Unused)",
    /* 93*/ "Scantid (Desert Critter)",
    /* 94*/ "Kakaru (Twilight Critter)",
    /* 95*/ "Ragnasaur (Ashworld Critter)",
    /* 96*/ "Ursadon (Ice World Critter)",
    /* 97*/ "Lurker Egg",
    /* 98*/ "Raszagal (Corsair)",
    /* 99*/ "Samir Duran (Ghost)",
    /*100*/ "Alexei Stukov (Ghost)",
    /*101*/ "Map Revealer",
    /*102*/ "Gerard DuGalle (BattleCruiser)",
    /*103*/ "Zerg Lurker",
    /*104*/ "Infested Duran",
    /*105*/ "Disruption Web",
    /*106*/ "Terran Command Center",
    /*107*/ "Terran Comsat Station",
    /*108*/ "Terran Nuclear Silo",
    /*109*/ "Terran Supply Depot",
    /*110*/ "Terran Refinery",
    /*111*/ "Terran Barracks",
    /*112*/ "Terran Academy",
    /*113*/ "Terran Factory",
    /*114*/ "Terran Starport",
    /*115*/ "Terran Control Tower",
    /*116*/ "Terran Science Facility",
    /*117*/ "Terran Covert Ops",
    /*118*/ "Terran Physics Lab",
    /*119*/ "Starbase (Unused)",
    /*120*/ "Terran Machine Shop",
    /*121*/ "Repair Bay (Unused)",
    /*122*/ "Terran Engineering Bay",
    /*123*/ "Terran Armory",
    /*124*/ "Terran Missile Turret",
    /*125*/ "Terran Bunker",
    /*126*/ "Norad II (Crashed)",
    /*127*/ "Ion Cannon",
    /*128*/ "Uraj Crystal",
    /*129*/ "Khalis Crystal",
    /*130*/ "Infested Command Center",
    /*131*/ "Zerg Hatchery",
    /*132*/ "Zerg Lair",
    /*133*/ "Zerg Hive",
    /*134*/ "Zerg Nydus Canal",
    /*135*/ "Zerg Hydralisk Den",
    /*136*/ "Zerg Defiler Mound",
    /*137*/ "Zerg Greater Spire",
    /*138*/ "Zerg Queen's Nest",
    /*139*/ "Zerg Evolution Chamber",
    /*140*/ "Zerg Ultralisk Cavern",
    /*141*/ "Zerg Spire",
    /*142*/ "Zerg Spawning Pool",
    /*143*/ "Zerg Creep Colony",
    /*144*/ "Zerg Spore Colony",
    /*145*/ "Unused Zerg Building 1",
    /*146*/ "Zerg Sunken Colony",
    /*147*/ "Zerg Overmind (With Shell)",
    /*148*/ "Zerg Overmind",
    /*149*/ "Zerg Extractor",
    /*150*/ "Mature Crysalis",
    /*151*/ "Zerg Cerebrate",
    /*152*/ "Zerg Cerebrate Daggoth",
    /*153*/ "Unused Zerg Building 2",
    /*154*/ "Protoss Nexus",
    /*155*/ "Protoss Robotics Facility",
    /*156*/ "Protoss Pylon",
    /*157*/ "Protoss Assimilator",
    /*158*/ "Unused Protoss Building 1",
    /*159*/ "Protoss Observatory",
    /*160*/ "Protoss Gateway",
    /*161*/ "Unused Protoss Building 2",
    /*162*/ "Protoss Photon Cannon",
    /*163*/ "Protoss Citadel of Adun",
    /*164*/ "Protoss Cybernetics Core",
    /*165*/ "Protoss Templar Archives",
    /*166*/ "Protoss Forge",
    /*167*/ "Protoss Stargate",
    /*168*/ "Stasis Cell/Prison",
    /*169*/ "Protoss Fleet Beacon",
    /*170*/ "Protoss Arbiter Tribunal",
    /*171*/ "Protoss Robotics Support Bay",
    /*172*/ "Protoss Shield Battery",
    /*173*/ "Khaydarin Crystal Formation",
    /*174*/ "Protoss Temple",
    /*175*/ "Xel'Naga Temple",
    /*176*/ "Mineral Field (Type 1)",
    /*177*/ "Mineral Field (Type 2)",
    /*178*/ "Mineral Field (Type 3)",
    /*179*/ "Cave (Unused)",
    /*180*/ "Cave-in (Unused)",
    /*181*/ "Cantina (Unused)",
    /*182*/ "Mining Platform (Unused)",
    /*183*/ "Independent Command Center (Unused)",
    /*184*/ "Independent Starport (Unused)",
    /*185*/ "Independent Jump Gate (Unused)",
    /*186*/ "Ruins (Unused)",
    /*187*/ "Khadarin Crystal Formation (Unused)",
    /*188*/ "Vespene Geyser",
    /*189*/ "Warp Gate",
    /*190*/ "Psi Disrupter",
    /*191*/ "Zerg Marker",
    /*192*/ "Terran Marker",
    /*193*/ "Protoss Marker",
    /*194*/ "Zerg Beacon",
    /*195*/ "Terran Beacon",
    /*196*/ "Protoss Beacon",
    /*197*/ "Zerg Flag Beacon",
    /*198*/ "Terran Flag Beacon",
    /*199*/ "Protoss Flag Beacon",
    /*200*/ "Power Generator",
    /*201*/ "Overmind Cocoon",
    /*202*/ "Dark Swarm",
    /*203*/ "Floor Missile Trap",
    /*204*/ "Floor Hatch (Unused)",
    /*205*/ "Left Upper Level Door",
    /*206*/ "Right Upper Level Door",
    /*207*/ "Left Pit Door",
    /*208*/ "Right Pit Door",
    /*209*/ "Floor Gun Trap",
    /*210*/ "Left Wall Missile Trap",
    /*211*/ "Left Wall Flame Trap",
    /*212*/ "Right Wall Missile Trap",
    /*213*/ "Right Wall Flame Trap",
    /*214*/ "Start Location",
    /*215*/ "Flag",
    /*216*/ "Young Chrysalis",
    /*217*/ "Psi Emitter",
    /*218*/ "Data Disc",
    /*219*/ "Khaydarin Crystal",
    /*220*/ "Mineral Cluster Type 1",
    /*221*/ "Mineral Cluster Type 2",
    /*222*/ "Protoss Vespene Gas Orb Type 1",
    /*223*/ "Protoss Vespene Gas Orb Type 2",
    /*224*/ "Zerg Vespene Gas Sac Type 1",
    /*225*/ "Zerg Vespene Gas Sac Type 2",
    /*226*/ "Terran Vespene Gas Tank Type 1",
    /*227*/ "Terran Vespene Gas Tank Type 2",
    /*228*/ "ID:228",
    /*229*/ "[any unit]",
    /*230*/ "[men]",
    /*231*/ "[buildings]",
    /*232*/ "[factories]"
};

const std::vector<std::string> Sc::Unit::legacyTextTrigDisplayNames = {
    /*  0*/ "Terran Marine",
    /*  1*/ "Terran Ghost", 
    /*  2*/ "Terran Vulture",
    /*  3*/ "Terran Goliath", 
    /*  4*/ "Goliath Turret", 
    /*  5*/ "Terran Siege Tank (Tank Mode)", 
    /*  6*/ "Tank Turret type 1",
    /*  7*/ "Terran SCV", 
    /*  8*/ "Terran Wraith",
    /*  9*/ "Terran Science Vessel",
    /* 10*/ "Gui Montag (Firebat)",
    /* 11*/ "Terran Dropship",
    /* 12*/ "Terran Battlecruiser",
    /* 13*/ "Vulture Spider Mine",
    /* 14*/ "Nuclear Missile",
    /* 15*/ "Terran Civilian",
    /* 16*/ "Sarah Kerrigan (Ghost)",
    /* 17*/ "Alan Schezar (Goliath)",
    /* 18*/ "Alan Turret",
    /* 19*/ "Jim Raynor (Vulture)",
    /* 20*/ "Jim Raynor (Marine)",
    /* 21*/ "Tom Kazansky (Wraith)",
    /* 22*/ "Magellan (Science Vessel)",
    /* 23*/ "Edmund Duke (Siege Tank)",
    /* 24*/ "Duke Turret type 1",
    /* 25*/ "Edmund Duke (Siege Mode)",
    /* 26*/ "Duke Turret type 2",
    /* 27*/ "Arcturus Mengsk (Battlecruiser)",
    /* 28*/ "Hyperion (Battlecruiser)",
    /* 29*/ "Norad II (Battlecruiser)",
    /* 30*/ "Terran Siege Tank (Siege Mode)",
    /* 31*/ "Tank Turret type 2",
    /* 32*/ "Terran Firebat",
    /* 33*/ "Scanner Sweep",
    /* 34*/ "Terran Medic",
    /* 35*/ "Zerg Larva",
    /* 36*/ "Zerg Egg",
    /* 37*/ "Zerg Zergling",
    /* 38*/ "Zerg Hydralisk",
    /* 39*/ "Zerg Ultralisk",
    /* 40*/ "Zerg Broodling",
    /* 41*/ "Zerg Drone",
    /* 42*/ "Zerg Overlord",
    /* 43*/ "Zerg Mutalisk",
    /* 44*/ "Zerg Guardian",
    /* 45*/ "Zerg Queen",
    /* 46*/ "Zerg Defiler",
    /* 47*/ "Zerg Scourge",
    /* 48*/ "Torrasque (Ultralisk)",
    /* 49*/ "Matriarch (Queen)",
    /* 50*/ "Infested Terran",
    /* 51*/ "Infested Kerrigan (Infested Terran)",
    /* 52*/ "Unclean One (Defiler)",
    /* 53*/ "Hunter Killer (Hydralisk)",
    /* 54*/ "Devouring One (Zergling)",
    /* 55*/ "Kukulza (Mutalisk)",
    /* 56*/ "Kukulza (Guardian)",
    /* 57*/ "Yggdrasill (Overlord)",
    /* 58*/ "Terran Valkyrie",
    /* 59*/ "Cocoon",
    /* 60*/ "Protoss Corsair",
    /* 61*/ "Protoss Dark Templar",
    /* 62*/ "Zerg Devourer",
    /* 63*/ "Protoss Dark Archon",
    /* 64*/ "Protoss Probe",
    /* 65*/ "Protoss Zealot",
    /* 66*/ "Protoss Dragoon",
    /* 67*/ "Protoss High Templar",
    /* 68*/ "Protoss Archon",
    /* 69*/ "Protoss Shuttle",
    /* 70*/ "Protoss Scout",
    /* 71*/ "Protoss Arbiter",
    /* 72*/ "Protoss Carrier",
    /* 73*/ "Protoss Interceptor",
    /* 74*/ "Dark Templar (Hero)",
    /* 75*/ "Zeratul (Dark Templar)",
    /* 76*/ "Tassadar/Zeratul (Archon)",
    /* 77*/ "Fenix (Zealot)",
    /* 78*/ "Fenix (Dragoon)",
    /* 79*/ "Tassadar (Templar)",
    /* 80*/ "Mojo (Scout)",
    /* 81*/ "Warbringer (Reaver)",
    /* 82*/ "Gantrithor (Carrier)",
    /* 83*/ "Protoss Reaver",
    /* 84*/ "Protoss Observer",
    /* 85*/ "Protoss Scarab",
    /* 86*/ "Danimoth (Arbiter)",
    /* 87*/ "Aldaris (Templar)",
    /* 88*/ "Artanis (Scout)",
    /* 89*/ "Rhynadon (Badlands)",
    /* 90*/ "Bengalaas (Jungle)",
    /* 91*/ "Unused type 1",
    /* 92*/ "Unused type 2",
    /* 93*/ "Scantid (Desert)",
    /* 94*/ "Kakaru (Twilight)",
    /* 95*/ "Ragnasaur (Ash World)",
    /* 96*/ "Ursadon (Ice World)",
    /* 97*/ "Zerg Lurker Egg",
    /* 98*/ "Raszagal (Dark Templar)",
    /* 99*/ "Samir Duran (Ghost)",
    /*100*/ "Alexei Stukov (Ghost)",
    /*101*/ "Map Revealer",
    /*102*/ "Gerard DuGalle (Ghost)",
    /*103*/ "Zerg Lurker",
    /*104*/ "Infested Duran",
    /*105*/ "Disruption Field",
    /*106*/ "Terran Command Center",
    /*107*/ "Terran Comsat Station",
    /*108*/ "Terran Nuclear Silo",
    /*109*/ "Terran Supply Depot",
    /*110*/ "Terran Refinery",
    /*111*/ "Terran Barracks",
    /*112*/ "Terran Academy",
    /*113*/ "Terran Factory",
    /*114*/ "Terran Starport",
    /*115*/ "Terran Control Tower",
    /*116*/ "Terran Science Facility",
    /*117*/ "Terran Covert Ops",
    /*118*/ "Terran Physics Lab",
    /*119*/ "Unused Terran Bldg type 1",
    /*120*/ "Terran Machine Shop",
    /*121*/ "Unused Terran Bldg type 2",
    /*122*/ "Terran Engineering Bay",
    /*123*/ "Terran Armory",
    /*124*/ "Terran Missile Turret",
    /*125*/ "Terran Bunker",
    /*126*/ "Norad II (Crashed Battlecruiser)",
    /*127*/ "Ion Cannon",
    /*128*/ "Uraj Crystal",
    /*129*/ "Khalis Crystal",
    /*130*/ "Infested Command Center",
    /*131*/ "Zerg Hatchery",
    /*132*/ "Zerg Lair",
    /*133*/ "Zerg Hive",
    /*134*/ "Zerg Nydus Canal",
    /*135*/ "Zerg Hydralisk Den",
    /*136*/ "Zerg Defiler Mound",
    /*137*/ "Zerg Greater Spire",
    /*138*/ "Zerg Queen's Nest",
    /*139*/ "Zerg Evolution Chamber",
    /*140*/ "Zerg Ultralisk Cavern",
    /*141*/ "Zerg Spire",
    /*142*/ "Zerg Spawning Pool",
    /*143*/ "Zerg Creep Colony",
    /*144*/ "Zerg Spore Colony",
    /*145*/ "Unused Zerg Bldg",
    /*146*/ "Zerg Sunken Colony",
    /*147*/ "Zerg Overmind (With Shell)",
    /*148*/ "Zerg Overmind",
    /*149*/ "Zerg Extractor",
    /*150*/ "Mature Crysalis",
    /*151*/ "Zerg Cerebrate",
    /*152*/ "Zerg Cerebrate Daggoth",
    /*153*/ "Unused Zerg Bldg 5",
    /*154*/ "Protoss Nexus",
    /*155*/ "Protoss Robotics Facility",
    /*156*/ "Protoss Pylon",
    /*157*/ "Protoss Assimilator",
    /*158*/ "Protoss Unused type 1",
    /*159*/ "Protoss Observatory",
    /*160*/ "Protoss Gateway",
    /*161*/ "Protoss Unused type 2",
    /*162*/ "Protoss Photon Cannon",
    /*163*/ "Protoss Citadel of Adun",
    /*164*/ "Protoss Cybernetics Core",
    /*165*/ "Protoss Templar Archives",
    /*166*/ "Protoss Forge",
    /*167*/ "Protoss Stargate",
    /*168*/ "Stasis Cell/Prison",
    /*169*/ "Protoss Fleet Beacon",
    /*170*/ "Protoss Arbiter Tribunal",
    /*171*/ "Protoss Robotics Support Bay",
    /*172*/ "Protoss Shield Battery",
    /*173*/ "Khaydarin Crystal Formation",
    /*174*/ "Protoss Temple",
    /*175*/ "Xel'Naga Temple",
    /*176*/ "Mineral Field (Type 1)",
    /*177*/ "Mineral Field (Type 2)",
    /*178*/ "Mineral Field (Type 3)",
    /*179*/ "Cave",
    /*180*/ "Cave-in",
    /*181*/ "Cantina",
    /*182*/ "Mining Platform",
    /*183*/ "Independent Command Center",
    /*184*/ "Independent Starport",
    /*185*/ "Jump Gate",
    /*186*/ "Ruins",
    /*187*/ "Kyadarin Crystal Formation",
    /*188*/ "Vespene Geyser",
    /*189*/ "Warp Gate",
    /*190*/ "Psi Disrupter",
    /*191*/ "Zerg Marker",
    /*192*/ "Terran Marker",
    /*193*/ "Protoss Marker",
    /*194*/ "Zerg Beacon",
    /*195*/ "Terran Beacon",
    /*196*/ "Protoss Beacon",
    /*197*/ "Zerg Flag Beacon",
    /*198*/ "Terran Flag Beacon",
    /*199*/ "Protoss Flag Beacon",
    /*200*/ "Power Generator",
    /*201*/ "Overmind Cocoon",
    /*202*/ "Dark Swarm",
    /*203*/ "Floor Missile Trap",
    /*204*/ "Floor Hatch (UNUSED)",
    /*205*/ "Left Upper Level Door",
    /*206*/ "Right Upper Level Door",
    /*207*/ "Left Pit Door",
    /*208*/ "Right Pit Door",
    /*209*/ "Floor Gun Trap",
    /*210*/ "Left Wall Missile Trap",
    /*211*/ "Left Wall Flame Trap",
    /*212*/ "Right Wall Missile Trap",
    /*213*/ "Right Wall Flame Trap",
    /*214*/ "Start Location",
    /*215*/ "Flag",
    /*216*/ "Young Chrysalis",
    /*217*/ "Psi Emitter",
    /*218*/ "Data Disc",
    /*219*/ "Khaydarin Crystal",
    /*220*/ "Mineral Chunk (Type 1)",
    /*221*/ "Mineral Chunk (Type 2)",
    /*222*/ "Vespene Orb (Protoss Type 1)",
    /*223*/ "Vespene Orb (Protoss Type 2)",
    /*224*/ "Vespene Sac (Zerg Type 1)",
    /*225*/ "Vespene Sac (Zerg Type 2)",
    /*226*/ "Vespene Tank (Terran Type 1)",
    /*227*/ "Vespene Tank (Terran Type 2)",
    /*228*/ "Invalid Unit",
    /*229*/ "Any unit",
    /*230*/ "Men",
    /*231*/ "Buildings",
    /*232*/ "Factories"
};

const std::vector<std::string> Sc::Upgrade::names = {
    "Terran Infantry Armor",
    "Terran Vehicle Plating",
    "Terran Ship Plating",
    "Zerg Carapace",
    "Zerg Flyer Carapace",
    "Protoss Armor",
    "Protoss Plating",
    "Terran Infantry Weapons",
    "Terran Vehicle Weapons",
    "Terran Ship Weapons",
    "Zerg Melee Attacks",
    "Zerg Missile Attacks",
    "Zerg Flyer Attacks",
    "Protoss Ground Weapons",
    "Protoss Air Weapons",
    "Protoss Plasma Shields",
    "U-238 Shells",
    "Ion Thrusters",
    "Burst Lasers (Unused)",
    "Titan Reactor (SV +50)",
    "Ocular Implants",
    "Moebius Reactor (Ghost +50)",
    "Apollo Reactor (Wraith +50)",
    "Colossus Reactor (BC +50)",
    "Ventral Sacs",
    "Antennae",
    "Pneumatized Carapace",
    "Metabolic Boost",
    "Adrenal Glands",
    "Muscular Augments",
    "Grooved Spines",
    "Gamete Meiosis (Queen +50)",
    "Metasynaptic Node (Defiler +50)",
    "Singularity Charge",
    "Leg Enhancements",
    "Scarab Damage",
    "Reaver Capacity",
    "Gravitic Drive",
    "Sensor Array",
    "Gravitic Boosters",
    "Khaydarin Amulet (HT +50)",
    "Apial Sensors",
    "Gravitic Thrusters",
    "Carrier Capacity",
    "Khaydarin Core (Arbiter +50)",
    "Unused Upgrade (45)",
    "Unused Upgrade (46)",
    "Argus Jewel (Corsair +50)",
    "Unused Upgrade (48)",
    "Argus Talisman (DA +50)",
    "Unused Upgrade (50)",
    "Caduceus Reactor (Medic +50)",
    "Chitinous Plating",
    "Anabolic Synthesis",
    "Charon Booster",
    "Unused Upgrade (55)",
    "Unused Upgrade (56)",
    "Unused Upgrade (57)",
    "Unused Upgrade (58)",
    "Unused Upgrade (59)",
    "Special Upgrade (60)"
};

const std::vector<std::string> Sc::Tech::names = {
    "Stim Packs",
    "Lockdown",
    "EMP Shockwave",
    "Spider Mines",
    "Scanner Sweep",
    "Tank Siege Mode",
    "Defensive Matrix",
    "Irradiate",
    "Yamato Gun",
    "Cloaking Field",
    "Personnel Cloaking",
    "Burrowing",
    "Infestation",
    "Spawn Broodlings",
    "Dark Swarm",
    "Plague",
    "Consume",
    "Ensnare",
    "Parasite",
    "Psionic Storm",
    "Hallucination",
    "Recall",
    "Stasis Field",
    "Archon Warp",
    "Restoration",
    "Disruption Web",
    "Unused Tech (26)",
    "Mind Control",
    "Dark Archon Meld",
    "Feedback",
    "Optical Flare",
    "Maelstrom",
    "Lurker Aspect",
    "Unused Tech (33)",
    "Healing",
    "Unused Tech (35)",
    "Unused Tech (36)",
    "Unused Tech (37)",
    "Unused Tech (38)",
    "Unused Tech (39)",
    "Unused Tech (40)",
    "Unused Tech (41)",
    "Unused Tech (42)",
    "Unused Tech (43)",
};

const std::string Sc::Ai::aiScriptBinPath = makeExtArchiveFilePath(makeArchiveFilePath("scripts", "AISCRIPT"), "BIN");

const std::string & Sc::Ai::Entry::getName(const TblFile & tblFile) const
{
    return tblFile.getString(statStrIndex);
}

bool Sc::Ai::Entry::getName(const TblFile & statTxt, std::string & outAiName) const
{
    return statTxt.getString(statStrIndex, outAiName);
}

Sc::Ai::~Ai()
{

}

bool Sc::Ai::load(ArchiveCluster & archiveCluster, TblFilePtr statTxt)
{
    this->statTxt = statTxt;
    if ( auto rawData = Sc::Data::GetAsset(archiveCluster, aiScriptBinPath) )
    {
        if ( rawData->size() >= 4 )
        {
            u32 aiEntriesOffset = (u32 &)rawData.value()[0];
            u32 numAiEntries = u32((rawData->size() - (size_t)aiEntriesOffset) / sizeof(Entry));
            if ( numAiEntries > 0 )
            {
                for ( u32 i=0; i<numAiEntries; i++ )
                    entries.push_back((Entry &)rawData.value()[aiEntriesOffset+i*sizeof(Entry)]);
            }
            else
                logger.warn() << "Zero AI entries in " << aiScriptBinPath << std::endl;

            return true;
        }
        else
            logger.error() << "Unrecognized " << aiScriptBinPath << " format!" << std::endl;
    }
    return false;
}

const Sc::Ai::Entry & Sc::Ai::getEntry(size_t aiIndex) const
{
    if ( aiIndex < entries.size() )
        return entries[aiIndex];
    else
        throw std::out_of_range("AiIndex " + std::to_string(aiIndex) + " is out of range for the AISCRIPT.BIN file containing " + std::to_string(entries.size()) + " entries");
}

const std::string & Sc::Ai::getName(size_t aiIndex) const
{
    if ( aiIndex < entries.size() )
    {
        if ( statTxt != nullptr )
            return entries[aiIndex].getName(*statTxt);
        else
            throw std::logic_error("statTxt cannot be null during a call to getName!");
    }
    else
        throw std::out_of_range("AiIndex " + std::to_string(aiIndex) + " is out of range for the AISCRIPT.BIN file containing " + std::to_string(entries.size()) + " entries");
}

bool Sc::Ai::getName(size_t aiIndex, std::string & outAiName) const
{
    if ( aiIndex < entries.size() )
    {
        if ( statTxt != nullptr )
            return entries[aiIndex].getName(*statTxt, outAiName);
    }
    return false;
}

bool Sc::Ai::getNameById(u32 aiId, std::string & outAiName) const
{
    for ( size_t i=0; i<entries.size(); i++ )
    {
        const Sc::Ai::Entry & entry = entries[i];
        if ( entry.identifier == aiId )
        {
            if ( statTxt != nullptr )
                return entries[i].getName(*statTxt, outAiName);
        }
    }
    return false;
}

size_t Sc::Ai::numEntries() const
{
    return entries.size();
}

const std::vector<std::string> Sc::Terrain::TilesetNames = {
    "badlands",
    "platform",
    "install",
    "ashworld",
    "jungle",
    "Desert",
    "Ice",
    "Twilight"
};

void Sc::Terrain::Tiles::populateTerrainTypeMap(size_t tilesetIndex)
{
    const auto & compressedTerrainTypeMap = Isom::compressedTerrainTypeMaps[tilesetIndex];

    size_t totalTerrainTypes = terrainTypes.size();
    terrainTypeMap.assign(totalTerrainTypes*totalTerrainTypes, uint16_t(0));
    std::vector<uint16_t> tempTypeMap(totalTerrainTypes*totalTerrainTypes, 0);
    std::vector<uint16_t> rowData {};

    // The compressedTerrainTypeMap maps terrain types to terrain types that isom searches start at, separated by zeroes
    for ( size_t i=0; compressedTerrainTypeMap[i] != 0; ++i )
    {
        for ( size_t j=totalTerrainTypes*size_t(compressedTerrainTypeMap[i++]); compressedTerrainTypeMap[i] != 0; ++i,++j )
            tempTypeMap[j] = compressedTerrainTypeMap[i];
    }

    // This expand the compressed type map to a square letting you use two types as x and y coordinates to get search start terrain types
    for ( int i=int(totalTerrainTypes)-1; i>=0; --i )
    {
        rowData.assign(totalTerrainTypes, 0);
        std::deque<uint16_t> terrainTypeStack { uint16_t(i) };
        terrainTypeMap[totalTerrainTypes*i+terrainTypeStack[0]] = i;

        while ( !terrainTypeStack.empty() )
        {
            uint16_t destRow = terrainTypeStack.front();
            terrainTypeStack.pop_front();

            size_t start = i*totalTerrainTypes;
            for ( size_t j=destRow*totalTerrainTypes; tempTypeMap[j] != 0; ++j )
            {
                auto tempPath = tempTypeMap[j];
                if ( terrainTypeMap[start+tempPath] == 0 )
                {
                    uint16_t nextValue = rowData[destRow] == 0 ? tempPath : rowData[destRow];
                    terrainTypeStack.push_back(tempPath);
                    terrainTypeMap[start+tempPath] = nextValue;
                    rowData[tempPath] = nextValue;
                }
            }
        }
    }
}

void Sc::Terrain::Tiles::generateIsomLinks()
{
    size_t totalTileGroups = std::min(size_t(1024), tileGroups.size());
    Span<TileGroup> tilesetCv5s((TileGroup*)&tileGroups[0], totalTileGroups);

    std::vector<std::vector<uint16_t>> terrainTypeTileGroups(terrainTypes.size(), std::vector<uint16_t>{});
    for ( uint16_t i=0; i<totalTileGroups; i +=2 )
    {
        if ( tilesetCv5s[i].terrainType > 0 )
            terrainTypeTileGroups[tilesetCv5s[i].terrainType].push_back(i);
    }

    std::vector<Isom::TerrainTypeInfo> solidBrushes {};
    std::vector<Isom::TerrainTypeInfo> otherTerrainTypes {};
    size_t i = 1;
    for ( ; i<=terrainTypes.size()/2; ++i )
    {
        if ( terrainTypes[i].isomValue != 0 )
            solidBrushes.push_back(terrainTypes[i]);
    }
    for ( ; i<terrainTypes.size(); ++i )
    {
        if ( terrainTypes[i].isomValue != 0 )
            otherTerrainTypes.push_back({uint16_t(i), terrainTypes[i].isomValue});
    }
    std::sort(solidBrushes.begin(), solidBrushes.end(), [](const Isom::TerrainTypeInfo & l, const Isom::TerrainTypeInfo & r) {
        return l.isomValue < r.isomValue;
    });
    std::sort(otherTerrainTypes.begin(), otherTerrainTypes.end(), [](const Isom::TerrainTypeInfo & l, const Isom::TerrainTypeInfo & r) {
        return l.isomValue < r.isomValue;
    });

    for ( const auto & solidBrush : solidBrushes )
    {
        while ( isomLinks.size() < size_t(solidBrush.isomValue) )
            isomLinks.push_back(Isom::ShapeLinks{});

        auto tileGroup = terrainTypeTileGroups[solidBrush.index][0];
        const auto & links = tilesetCv5s[tileGroup].links;
        isomLinks.push_back(
            Isom::ShapeLinks{uint8_t(solidBrush.index),
                {links.right, links.bottom, solidBrush.linkId},
                {links.left, links.bottom, solidBrush.linkId},
                {links.left, links.top, solidBrush.linkId},
                {links.top, links.right, solidBrush.linkId}
            }
        );
    }

    size_t totalSolidBrushEntries = isomLinks.size();
    while ( isomLinks.size() < otherTerrainTypes[0].isomValue )
        isomLinks.push_back(Isom::ShapeLinks{});

    for ( const auto & otherTerrainType : otherTerrainTypes )
    {
        // In the isomLink table there are 14 shapes/entries per terrain types that are not solid brushes
        size_t terrainTypeIsomLinkStart = isomLinks.size();
        for ( size_t i=0; i<14; ++i )
            isomLinks.push_back(Isom::ShapeLinks{uint8_t(otherTerrainType.index)});

        const auto & tileGroupIndexes = terrainTypeTileGroups[otherTerrainType.index]; // All tile group indexes that belong to this terrain type
        Isom::TerrainTypeShapes & shapes = (Isom::TerrainTypeShapes &)isomLinks[terrainTypeIsomLinkStart]; // Treat these 14 entries as a shapes struct
        Isom::ShapeTileGroup shapeTileGroups[14] {}; // Record all tile group indexes that get used as shape quadrants

        for ( auto tileGroupIndex : tileGroupIndexes )
        {
            const auto & tileGroup = tilesetCv5s[tileGroupIndex];

            bool noStackAbove = (tileGroup.stackConnections.top == 0);
            for ( size_t shapeIndex=0; shapeIndex<Isom::shapes.size(); ++shapeIndex )
            {
                if ( !tileGroup.links.isShapeQuadrant() )
                    continue; // Tile groups that have all hard links or no hard links do not refer to shape quadrants

                // If this tile group matches any quadrants of this shape, update shape links & shapeTileGroups
                const auto & checkShape = Isom::shapes[shapeIndex];
                if ( checkShape.matches(Isom::Quadrant::TopLeft, tileGroup.links, noStackAbove) )
                {
                    shapes[shapeIndex].topLeft.right = tileGroup.links.right;
                    shapes[shapeIndex].topLeft.bottom = tileGroup.links.bottom;
                    shapeTileGroups[shapeIndex].topLeft = tileGroupIndex;
                }
                if ( checkShape.matches(Isom::Quadrant::TopRight, tileGroup.links, noStackAbove) )
                {
                    shapes[shapeIndex].topRight.left = tileGroup.links.left;
                    shapes[shapeIndex].topRight.bottom = tileGroup.links.bottom;
                    shapeTileGroups[shapeIndex].topRight = tileGroupIndex;
                }
                if ( checkShape.matches(Isom::Quadrant::BottomRight, tileGroup.links, noStackAbove) )
                {
                    shapes[shapeIndex].bottomRight.left = tileGroup.links.left;
                    shapes[shapeIndex].bottomRight.top = tileGroup.links.top;
                    shapeTileGroups[shapeIndex].bottomRight = tileGroupIndex;
                }
                if ( checkShape.matches(Isom::Quadrant::BottomLeft, tileGroup.links, noStackAbove) )
                {
                    shapes[shapeIndex].bottomLeft.top = tileGroup.links.top;
                    shapes[shapeIndex].bottomLeft.right = tileGroup.links.right;
                    shapeTileGroups[shapeIndex].bottomLeft = tileGroupIndex;
                }
            }
        }

        shapes.populateJutInEastWest(tilesetCv5s, shapeTileGroups);
        shapes.populateEmptyQuadrantLinks();
        shapes.populateHardcodedLinkIds();
        shapes.populateLinkIdsToSolidBrushes(tilesetCv5s, shapeTileGroups, totalSolidBrushEntries, isomLinks);
    }
}

void Sc::Terrain::Tiles::loadIsom(size_t tilesetIndex)
{
    Span<Isom::TerrainTypeInfo> terrainTypeInfo = Isom::tilesetTerrainTypes[tilesetIndex];
    this->terrainTypes = terrainTypeInfo;
    populateTerrainTypeMap(tilesetIndex);

    for ( size_t i=0; i<tileGroups.size(); i+=2 )
    {
        const auto & groupLinks = tileGroups[i].links;
        uint32_t left = uint32_t(groupLinks.left);
        uint32_t top = uint32_t(groupLinks.top);
        uint32_t right = uint32_t(groupLinks.right);
        uint32_t bottom = uint32_t(groupLinks.bottom);

        uint32_t tileGroupHash = (((left << 6 | top) << 6 | right) << 6 | bottom) << 6;
        if ( left >= 48 || top >= 48 || right >= 48 || bottom >= 48 )
            tileGroupHash |= tileGroups[i].terrainType;

        auto existing = hashToTileGroup.find(tileGroupHash);
        if ( existing != hashToTileGroup.end() )
            existing->second.push_back(uint16_t(i));
        else
            hashToTileGroup.insert(std::make_pair(tileGroupHash, std::vector<uint16_t>{uint16_t(i)}));
    }

    generateIsomLinks();

    for ( const auto & terrainType : terrainTypeInfo )
    {
        if ( terrainType.brushSortOrder >= 0 )
            brushes.push_back(terrainType);
    }
    std::sort(brushes.begin(), brushes.end(), [&](const Isom::TerrainTypeInfo & l, const Isom::TerrainTypeInfo & r) {
        return l.brushSortOrder < r.brushSortOrder;
    });
    defaultBrush = terrainTypeInfo[Isom::defaultBrushIndex[tilesetIndex]];
}

bool Sc::Terrain::Tiles::load(size_t tilesetIndex, ArchiveCluster & archiveCluster, const std::string & tilesetName, Sc::TblFilePtr statTxt,
    std::array<u16, Sprite::TotalSprites> & doodadSpriteFlags, std::array<u16, Unit::TotalTypes> & doodadUnitFlags)
{
    const std::string tilesetMpqDirectory = "tileset";
    const std::string mpqFilePath = makeArchiveFilePath(tilesetMpqDirectory, tilesetName);
    const std::string cv5FilePath = makeExtArchiveFilePath(mpqFilePath, "cv5");
    const std::string vf4FilePath = makeExtArchiveFilePath(mpqFilePath, "vf4");
    const std::string vr4FilePath = makeExtArchiveFilePath(mpqFilePath, "vr4");
    const std::string vx4FilePath = makeExtArchiveFilePath(mpqFilePath, "vx4");
    const std::string vx4exFilePath = makeExtArchiveFilePath(mpqFilePath, "vx4ex");
    const std::string wpeFilePath = makeExtArchiveFilePath(mpqFilePath, "wpe");
    const std::string ddDataFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "dddata"), "bin");
    
    const std::string darkFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "dark"), "pcx");
    const std::string shiftFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "shift"), "pcx");
    const std::string ofireFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "ofire"), "pcx");
    const std::string gfireFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "gfire"), "pcx");
    const std::string bfireFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "bfire"), "pcx");
    const std::string bexplFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "bexpl"), "pcx");
    const std::string trans50FilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "trans50"), "pcx");
    const std::string redFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "red"), "pcx");
    const std::string greenFilePath = makeExtArchiveFilePath(makeArchiveFilePath(mpqFilePath, "green"), "pcx");
    bool darkLoaded = dark.load(archiveCluster, darkFilePath);
    bool shiftLoaded = shift.load(archiveCluster, shiftFilePath);
    bool ofireLoaded = remap[0].load(archiveCluster, ofireFilePath);
    bool gfireLoaded = remap[1].load(archiveCluster, gfireFilePath);
    bool bfireLoaded = remap[2].load(archiveCluster, bfireFilePath);
    bool bexplLoaded = remap[3].load(archiveCluster, bexplFilePath);
    bool trans50Loaded = remap[4].load(archiveCluster, trans50FilePath);
    bool redLoaded = remap[5].load(archiveCluster, redFilePath);
    bool greenLoaded = remap[6].load(archiveCluster, greenFilePath);

    bool remappingFilesLoaded = darkLoaded && shiftLoaded &&
        ofireLoaded && gfireLoaded && bfireLoaded && bexplLoaded && trans50Loaded && redLoaded && greenLoaded;

    if ( !remappingFilesLoaded )
        logger.error() << "Failed to get one or more remapping files for tileset " << mpqFilePath << std::endl;
    
    auto cv5Data = Sc::Data::GetAsset(archiveCluster, cv5FilePath);
    auto vf4Data = Sc::Data::GetAsset(archiveCluster, vf4FilePath);
    auto vr4Data = Sc::Data::GetAsset(archiveCluster, vr4FilePath);
    bool isVx4ex = false;
    auto vx4Data = Sc::Data::GetAsset(archiveCluster, isVx4ex, vx4exFilePath, vx4FilePath);
    auto wpeData = Sc::Data::GetAsset(archiveCluster, wpeFilePath);
    auto ddData = Sc::Data::GetAsset(archiveCluster, ddDataFilePath);

    if ( cv5Data && vf4Data && vr4Data && vx4Data && wpeData && ddData )
    {
        if ( cv5Data->size() % sizeof(Sc::Terrain::TileGroup) == 0 &&
            vf4Data->size() % sizeof(Sc::Terrain::TileFlags) == 0 &&
            vr4Data->size() % sizeof(Sc::Terrain::MiniTilePixels) == 0 &&
            vx4Data->size() % sizeof(Sc::Terrain::TileGraphics) == 0 &&
            wpeData->size() == sizeof(Sc::Terrain::WpeDat) )
        {
            size_t numTileGroups = Cv5Dat::tileGroupsSize(cv5Data->size());
            size_t numTileFlags = Vf4Dat::size(vf4Data->size());
            size_t numMiniTilePixels = Vr4Dat::size(vr4Data->size());
            size_t numTileGraphics = Vx4Dat::size(isVx4ex, vx4Data->size());
            size_t totalDoodadDats = DdData::size(ddData->size());

            if ( numTileGroups > 0 )
            {
                TileGroup* rawTileGroups = (TileGroup*)&cv5Data.value()[0];
                tileGroups.assign(&rawTileGroups[0], &rawTileGroups[numTileGroups]);
            }
            else
                tileGroups.clear();

            if ( numTileFlags > 0 )
            {
                TileFlags* rawTileFlags = (TileFlags*)&vf4Data.value()[0];
                tileFlags.assign(&rawTileFlags[0], &rawTileFlags[numTileFlags]);
            }
            else
                tileFlags.clear();

            if ( numMiniTilePixels > 0 )
            {
                MiniTilePixels* rawMiniTilePixels = (MiniTilePixels*)&vr4Data.value()[0];
                miniTilePixels.assign(&rawMiniTilePixels[0], &rawMiniTilePixels[numMiniTilePixels]);
            }
            else
                miniTilePixels.clear();

            if ( numTileGraphics > 0 )
            {
                if ( isVx4ex )
                {
                    TileGraphicsEx* rawTileGraphics = (TileGraphicsEx*)&vx4Data.value()[0];
                    tileGraphics.assign(&rawTileGraphics[0], &rawTileGraphics[numTileGraphics]);
                }
                else
                {
                    TileGraphics* rawTileGraphics = (TileGraphics*)&vx4Data.value()[0];
                    tileGraphics.clear();
                    for ( size_t i=0; i<numTileGraphics; ++i )
                        tileGraphics.push_back(TileGraphicsEx(rawTileGraphics[i]));
                }
            }
            else
                tileGraphics.clear();

            WpeColor* wpeColors = (WpeColor*)&wpeData.value()[0];
            std::memcpy(&systemColorPalette[0], wpeColors, sizeof(Sc::Terrain::WpeDat));
            if ( offsetof(WpeColor, red) == offsetof(SystemColor, blue) &&
                offsetof(WpeColor, green) == offsetof(SystemColor, green) &&
                offsetof(WpeColor, blue) == offsetof(SystemColor, red) )
            {
                // Red-blue swap
                for ( size_t i=0; i<NumColors; i++ )
                    std::swap(systemColorPalette[i].red, systemColorPalette[i].blue);
            }
            else if ( offsetof(WpeColor, red) != offsetof(SystemColor, red) ||
                offsetof(WpeColor, green) != offsetof(SystemColor, green) ||
                offsetof(WpeColor, blue) != offsetof(SystemColor, blue) )
            {
                throw std::logic_error("Unrecognized color swap required to load system colors, please update the code!");
            }
            staticSystemColorPalette = systemColorPalette;

            if ( totalDoodadDats > 0 )
            {
                DoodadPlacibility* rawDoodadPlacibility = (DoodadPlacibility*)&ddData.value()[0];
                doodadPlacibility.assign(&rawDoodadPlacibility[0], &rawDoodadPlacibility[totalDoodadDats]);
            }
            else
                doodadPlacibility.clear();

            for ( size_t tileGroupIndex=0; tileGroupIndex<tileGroups.size(); ++tileGroupIndex )
            {
                const auto & doodad = (const DoodadCv5 &)tileGroups[tileGroupIndex];
                if ( doodad.index == 1 && doodad.doodadName != 0 )
                {
                    if ( doodad.overlayIndex != 0 )
                    {
                        if ( (doodad.flags & Doodad::Flags::DrawAsSprite) == Doodad::Flags::DrawAsSprite )
                            doodadSpriteFlags[doodad.overlayIndex] = doodad.flags;
                        else
                            doodadUnitFlags[doodad.overlayIndex] = doodad.flags;
                    }
                    bool newGroup = true;
                    for ( auto & existingGroup : doodadGroups )
                    {
                        if ( doodad.doodadName == existingGroup.nameIndex )
                        {
                            const auto & previousDoodad = (const DoodadCv5 &)tileGroups[existingGroup.doodadStartTileGroup.back()];
                            if ( doodad.ddDataIndex != previousDoodad.ddDataIndex )
                            {
                                doodadIdToTileGroup.try_emplace(doodad.ddDataIndex, u16(tileGroupIndex));
                                existingGroup.doodadStartTileGroup.push_back(u16(tileGroupIndex));
                            }

                            newGroup = false;
                            break;
                        }
                    }

                    if ( newGroup )
                    {
                        doodadIdToTileGroup.try_emplace(doodad.ddDataIndex, u16(tileGroupIndex));
                        doodadGroups.push_back({doodad.doodadName, statTxt == nullptr ? "" : statTxt->getString(doodad.doodadName), {u16(tileGroupIndex)}});
                    }
                }
            }
            std::sort(doodadGroups.begin(), doodadGroups.end(), [&](auto & l, auto & r) {
                return l.name < r.name;
            });

            loadIsom(tilesetIndex);

            return remappingFilesLoaded;
        }
        else
            logger.error() << "One or more files improperly sized for tileset " << mpqFilePath << std::endl;
    }
    else
        logger.error() << "Failed to get one or more files for tileset " << mpqFilePath << std::endl;

    return false;
}

std::optional<uint16_t> Sc::Terrain::Tiles::getDoodadGroupIndex(uint16_t doodadId) const
{
    auto found = doodadIdToTileGroup.find(doodadId);
    if ( found != doodadIdToTileGroup.end() )
        return found->second;
    else
        return std::nullopt;
}

const Sc::Terrain::Tiles & Sc::Terrain::get(const Tileset & tileset) const
{
    if ( tileset < NumTilesets )
        return tilesets[tileset];
    else
        return tilesets[tileset % NumTilesets];
}

bool Sc::Terrain::load(ArchiveCluster & archiveCluster, Sc::TblFilePtr statTxt)
{
    auto start = std::chrono::high_resolution_clock::now();
    bool success = true;
    this->doodadSpriteFlags.fill(0);
    this->doodadUnitFlags.fill(0);
    for ( size_t i=0; i<NumTilesets; i++ )
        success &= tilesets[i].load(i, archiveCluster, TilesetNames[i], statTxt, doodadSpriteFlags, doodadUnitFlags);
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "Terrain loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return success;
}

const std::array<Sc::SystemColor, Sc::NumColors> & Sc::Terrain::getColorPalette(Tileset tileset) const
{
    if ( tileset < Terrain::NumTilesets )
        return tilesets[tileset].systemColorPalette;
    else
        return tilesets[tileset % Terrain::NumTilesets].systemColorPalette;
}

const std::array<Sc::SystemColor, Sc::NumColors> & Sc::Terrain::getStaticColorPalette(Tileset tileset) const
{
    if ( tileset < Terrain::NumTilesets )
        return tilesets[tileset].staticSystemColorPalette;
    else
        return tilesets[tileset % Terrain::NumTilesets].staticSystemColorPalette;
}

void Sc::Terrain::mergeSpriteFlags(const Sc::Unit & unitData)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; ++i )
        doodadSpriteFlags[unitData.getFlingy(unitData.getUnit(Sc::Unit::Type(i)).graphics).sprite] = doodadUnitFlags[i];
}

bool Sc::Weapon::load(ArchiveCluster & archiveCluster)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto weaponData = Sc::Data::GetAsset(archiveCluster, "arr\\weapons.dat");
    if ( !weaponData )
    {
        logger.error() << "Failed to load arr\\weapons.dat" << std::endl;
        return false;
    }
    else if ( weaponData->size() != sizeof(Sc::Weapon::DatFile<true>) && weaponData->size() != sizeof(Sc::Weapon::DatFile<false>) )
    {
        logger.error() << "Unrecognized WeaponDat format!" << std::endl;
        return false;
    }
    
    bool isExpansion = (weaponData->size() == sizeof(Sc::Weapon::DatFile<true>));
    if ( isExpansion )
    {
        const DatFile<true> & weaponDat = (DatFile<true> &)weaponData.value()[0];
        for ( size_t i=0; i<Total; i++ )
        {
            weapons.push_back(Sc::Weapon::DatEntry { weaponDat.label[i], weaponDat.graphics[i], weaponDat.unused[i], weaponDat.targetFlags[i], weaponDat.minimumRange[i],
                weaponDat.maximumRange[i], weaponDat.damageUpgrade[i], weaponDat.weaponType[i], weaponDat.weaponBehavior[i], weaponDat.removeAfter[i], weaponDat.weaponEffect[i],
                weaponDat.innerSplashRadius[i], weaponDat.mediumSplashRadius[i], weaponDat.outerSplashRadius[i], weaponDat.damageAmount[i], weaponDat.damageBonus[i],
                weaponDat.weaponCooldown[i], weaponDat.damageFactor[i], weaponDat.attackAngle[i], weaponDat.launchSpin[i], weaponDat.forwardOffset[i], weaponDat.upwardOffset[i],
                weaponDat.targetErrorMessage[i], weaponDat.icon[i] });
        }
    }
    else
    {
        const DatFile<false> & weaponDat = (DatFile<false> &)weaponData.value()[0];
        for ( size_t i=0; i<TotalOriginal; i++ )
        {
            weapons.push_back(Sc::Weapon::DatEntry { weaponDat.label[i], weaponDat.graphics[i], weaponDat.unused[i], weaponDat.targetFlags[i], weaponDat.minimumRange[i],
                weaponDat.maximumRange[i], weaponDat.damageUpgrade[i], weaponDat.weaponType[i], weaponDat.weaponBehavior[i], weaponDat.removeAfter[i], weaponDat.weaponEffect[i],
                weaponDat.innerSplashRadius[i], weaponDat.mediumSplashRadius[i], weaponDat.outerSplashRadius[i], weaponDat.damageAmount[i], weaponDat.damageBonus[i],
                weaponDat.weaponCooldown[i], weaponDat.damageFactor[i], weaponDat.attackAngle[i], weaponDat.launchSpin[i], weaponDat.forwardOffset[i], weaponDat.upwardOffset[i],
                weaponDat.targetErrorMessage[i], weaponDat.icon[i] });
        }
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "Weapon loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

const Sc::Weapon::DatEntry & Sc::Weapon::get(Type weaponType) const
{
    if ( weaponType < weapons.size() )
        return weapons[weaponType];
    else
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string(weaponType) + " is out of range for weapons vector of size " + std::to_string(weapons.size()));
}

bool Sc::Sprite::Grp::load(ArchiveCluster & archiveCluster, const std::string & assetArchivePath)
{
    auto grpAsset = Sc::Data::GetAsset(archiveCluster, assetArchivePath);
    if ( grpAsset )
    {
        this->grpData.swap(*grpAsset);
        return isValid(assetArchivePath);
    }
    else
        logger.error() << "Failed to load GRP " << assetArchivePath << std::endl;

    return false;
}

void Sc::Sprite::Grp::makeBlank()
{
    grpData.assign(GrpFile::FileHeaderSize, u8(0));
}

const Sc::Sprite::GrpFile & Sc::Sprite::Grp::get() const
{
    return (const GrpFile &)grpData[0];
}

bool Sc::Sprite::Grp::isValid(const std::string & assetArchivePath) const
{
    return fileHeaderIsValid(assetArchivePath) &&
        frameHeadersAreValid(assetArchivePath) &&
        framesAreValid(assetArchivePath);
}

bool Sc::Sprite::Grp::fileHeaderIsValid(const std::string & assetArchivePath) const
{
    if ( grpData.size() < Sc::Sprite::GrpFile::FileHeaderSize )
    {
        logger.error() << "GRP file \"" << assetArchivePath << "\" must be at least " << Sc::Sprite::GrpFile::FileHeaderSize << " bytes" << std::endl;
        return false;
    }
    return true;
}

bool Sc::Sprite::Grp::frameHeadersAreValid(const std::string & assetArchivePath) const
{
    GrpFile & grpFile = (GrpFile &)grpData[0];
    size_t minimumFileSize = Sc::Sprite::GrpFile::FileHeaderSize + size_t(grpFile.numFrames)*sizeof(GrpFrameHeader);
    if ( grpData.size() < minimumFileSize )
    {
        logger.error() << "GRP file \"" << assetArchivePath << "\" with " << grpFile.numFrames << " frames must be at least " << minimumFileSize << " bytes" << std::endl;
        return false;
    }
    return true;
}

bool Sc::Sprite::Grp::framesAreValid(const std::string & assetArchivePath) const
{
    GrpFile & grpFile = (GrpFile &)grpData[0];
    u16 numFrames = grpFile.numFrames;
    for ( u16 frame=0; frame<grpFile.numFrames; frame++ )
    {
        GrpFrameHeader & grpFrameHeader = grpFile.frameHeaders[frame];
        if ( grpFrameHeader.frameHeight == 0 )
        {
            logger.warn() << "GRP file \"" << assetArchivePath << "\", frame " << frame << " has a height of 0 pixels and will be skipped! " << std::endl;
            continue;
        }
        else if ( grpFrameHeader.frameWidth == 0 )
        {
            logger.warn() << "GRP file \"" << assetArchivePath << "\", frame " << frame << " has a width of 0 pixels and will be skipped! " << std::endl;
            continue;
        }
        else if ( u16(grpFrameHeader.yOffset) + u16(grpFrameHeader.frameHeight) > grpFile.grpHeight )
        {
            logger.error() << "GRP file \"" << assetArchivePath << "\" with height " << grpFile.grpHeight << " has frame " << frame
                << " with yOffset " << u16(grpFrameHeader.yOffset) << " and frameHeight " << u16(grpFrameHeader.frameHeight)
                << " which overflows the grp height!" << std::endl;
            return false;
        }
        else if ( u16(grpFrameHeader.xOffset) + u16(grpFrameHeader.frameWidth) > grpFile.grpWidth )
        {
            logger.error() << "GRP file \"" << assetArchivePath << "\" with width " << grpFile.grpWidth << " has frame " << frame
                << " with xOffset " << u16(grpFrameHeader.xOffset) << " and frameWidth " << u16(grpFrameHeader.frameWidth)
                << " which overflows the grp width!" << std::endl;
            return false;
        }

        size_t frameMinimumFileSize = grpFrameHeader.frameOffset +
            sizeof(u16) * size_t(grpFrameHeader.frameHeight) +
            PixelRow::MinimumSize * size_t(grpFrameHeader.frameHeight);

        if ( grpData.size() < frameMinimumFileSize )
        {
            logger.error() << "GRP file \"" << assetArchivePath << "\", frame " << frame << " with frame offset " << grpFrameHeader.frameOffset << " and frame height " << grpFrameHeader.frameHeight
                << " requires a file size of at least " << frameMinimumFileSize << " bytes but the file is only " << grpData.size() << " bytes" << std::endl;
            return false;
        }

        GrpFrame & grpFrame = (GrpFrame &)grpData[grpFrameHeader.frameOffset];
        for ( size_t row = 0; row < size_t(grpFrameHeader.frameHeight); row++ )
        {
            u16 rowOffset = grpFrame.rowOffsets[row];
            size_t pixelRowOffset = size_t(grpFrameHeader.frameOffset) + size_t(rowOffset);

            size_t pixelRowMinimumFileSize = pixelRowOffset + PixelRow::MinimumSize;
            if ( grpData.size() < pixelRowMinimumFileSize )
            {
                logger.error() << "GRP file \"" << assetArchivePath << "\", frame " << frame << ", row " << row << " with frame offset " << grpFrameHeader.frameOffset << " and rowOffset " << rowOffset
                    << " requires a file size of at least " << pixelRowMinimumFileSize << " bytes but the file is only " << grpData.size() << " bytes" << std::endl;
                return false;
            }
            
            size_t column = 0;
            size_t pixelLineOffset = pixelRowOffset;

            // Loop until column == frameWidth or until error is found
            for ( size_t line = 0; column < grpFrameHeader.frameWidth; line++ )
            {
                if ( grpData.size() < pixelLineOffset + PixelLine::MinimumSize )
                {
                    logger.error() << "GRP file \"" << assetArchivePath << "\", frame " << frame << ", row " << row << ", line header " << line << " with frame offset " << grpFrameHeader.frameOffset << " and rowOffset " << rowOffset
                        << " requires a file size of at least " << pixelRowMinimumFileSize << " bytes but the file is only " << grpData.size() << " bytes" << std::endl;
                    return false;
                }

                PixelLine & pixelLine = (PixelLine &)grpData[pixelLineOffset];
                if ( grpData.size() < pixelLineOffset + size_t(pixelLine.sizeInBytes()) )
                {
                    logger.error() << "GRP file \"" << assetArchivePath << "\", frame " << frame << ", row " << row << ", line " << line << " with frame offset " << grpFrameHeader.frameOffset << " and rowOffset " << rowOffset
                        << " requires a file size of at least " << pixelRowMinimumFileSize << " bytes but the file is only " << grpData.size() << " bytes" << std::endl;
                    return false;
                }

                column += size_t(pixelLine.lineLength());
                pixelLineOffset += pixelLine.sizeInBytes();
            }
        }
    }
    return true;
}

std::vector<std::string_view> Sc::Sprite::OpName {
    "playfram",
    "playframtile",
    "sethorpos",
    "setvertpos",
    "setpos",
    "wait",
    "waitrand",
    "goto_",
    "imgol",
    "imgul",
    "imgolorig",
    "switchul",
    "unknown_0c",
    "imgoluselo",
    "imguluselo",
    "sprol",
    "highsprol",
    "lowsprul",
    "uflunstable",
    "spruluselo",
    "sprul",
    "sproluselo",
    "end",
    "setflipstate",
    "playsnd",
    "playsndrand",
    "playsndbtwn",
    "domissiledmg",
    "attackmelee",
    "followmaingraphic",
    "randcondjmp",
    "turnccwise",
    "turncwise",
    "turn1cwise",
    "turnrand",
    "setspawnframe",
    "sigorder",
    "attackwith",
    "attack",
    "castspell",
    "useweapon",
    "move",
    "gotorepeatattk",
    "engframe",
    "engset",
    "unknown_2d",
    "nobrkcodestart",
    "nobrkcodeend",
    "ignorerest",
    "attkshiftproj",
    "tmprmgraphicstart",
    "tmprmgraphicend",
    "setfldirect",
    "call",
    "return_",
    "setflspeed",
    "creategasoverlays",
    "pwrupcondjmp",
    "trgtrangecondjmp",
    "trgtarccondjmp",
    "curdirectcondjmp",
    "imgulnextid",
    "unknown_3e",
    "liftoffcondjmp",
    "warpoverlay",
    "orderdone",
    "grdsprol",
    "unknown_43",
    "dogrddamage"
};

std::vector<std::vector<Sc::Sprite::ParamType>> Sc::Sprite::OpParams {
    /*          playfram */ {ParamType::frame},
    /*      playframtile */ {ParamType::frame},
    /*         sethorpos */ {ParamType::sbyte},
    /*        setvertpos */ {ParamType::sbyte},
    /*            setpos */ {ParamType::sbyte, ParamType::sbyte},
    /*              wait */ {ParamType::byte},
    /*          waitrand */ {ParamType::byte, ParamType::byte},
    /*             goto_ */ {ParamType::label},
    /*             imgol */ {ParamType::imageid, ParamType::sbyte, ParamType::sbyte},
    /*             imgul */ {ParamType::imageid, ParamType::sbyte, ParamType::sbyte},
    /*         imgolorig */ {ParamType::imageid},
    /*          switchul */ {ParamType::imageid},
    /*        unknown_0c */ {},
    /*        imgoluselo */ {ParamType::imageid, ParamType::sbyte, ParamType::sbyte},
    /*        imguluselo */ {ParamType::imageid, ParamType::sbyte, ParamType::sbyte},
    /*             sprol */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*         highsprol */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*          lowsprul */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*       uflunstable */ {ParamType::flingyid},
    /*        spruluselo */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*             sprul */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*        sproluselo */ {ParamType::spriteid, ParamType::overlayid},
    /*               end */ {},
    /*      setflipstate */ {ParamType::flipstate},
    /*           playsnd */ {ParamType::soundid},
    /*       playsndrand */ {ParamType::sounds, ParamType::soundid},
    /*       playsndbtwn */ {ParamType::soundid, ParamType::soundid},
    /*      domissiledmg */ {},
    /*       attackmelee */ {ParamType::sounds, ParamType::soundid},
    /* followmaingraphic */ {},
    /*       randcondjmp */ {ParamType::byte, ParamType::label},
    /*        turnccwise */ {ParamType::byte},
    /*         turncwise */ {ParamType::byte},
    /*        turn1cwise */ {},
    /*          turnrand */ {ParamType::byte},
    /*     setspawnframe */ {ParamType::byte},
    /*          sigorder */ {ParamType::signalid},
    /*        attackwith */ {ParamType::weapon},
    /*            attack */ {},
    /*         castspell */ {},
    /*         useweapon */ {ParamType::weaponid},
    /*              move */ {ParamType::byte},
    /*    gotorepeatattk */ {},
    /*          engframe */ {ParamType::bframe},
    /*            engset */ {ParamType::frameset},
    /*        unknown_2d */ {},
    /*    nobrkcodestart */ {},
    /*      nobrkcodeend */ {},
    /*        ignorerest */ {},
    /*     attkshiftproj */ {ParamType::byte},
    /* tmprmgraphicstart */ {},
    /*   tmprmgraphicend */ {},
    /*       setfldirect */ {ParamType::byte},
    /*              call */ {ParamType::label},
    /*           return_ */ {},
    /*        setflspeed */ {ParamType::speed},
    /* creategasoverlays */ {ParamType::gasoverlay},
    /*      pwrupcondjmp */ {ParamType::label},
    /*  trgtrangecondjmp */ {ParamType::short_, ParamType::label},
    /*    trgtarccondjmp */ {ParamType::short_, ParamType::short_, ParamType::label},
    /*  curdirectcondjmp */ {ParamType::short_, ParamType::short_, ParamType::label},
    /*       imgulnextid */ {ParamType::sbyte, ParamType::sbyte},
    /*        unknown_3e */ {},
    /*    liftoffcondjmp */ {ParamType::label},
    /*       warpoverlay */ {ParamType::frame},
    /*         orderdone */ {ParamType::signalid},
    /*          grdsprol */ {ParamType::spriteid, ParamType::sbyte, ParamType::sbyte},
    /*        unknown_43 */ {},
    /*       dogrddamage */ {}
};

std::vector<size_t> Sc::Sprite::ParamSize {
    /* bframe */ 1,
    /* frame */ 2,
    /* frameset */ 1,
    /* byte */ 1,
    /* sbyte */ 1,
    /* label */ 2,
    /* imageid */ 2,
    /* spriteid */ 2,
    /* flingyid */ 2,
    /* overlayid */ 1,
    /* flipstate */ 1,
    /* soundid */ 2,
    /* sounds */ 1,
    /* signalid */ 1,
    /* weapon */ 1,
    /* weaponid */ 1,
    /* speed */ 2,
    /* gasoverlay */ 1,
    /* short_ */ 2
};

bool Sc::Sprite::LoFile::load(ArchiveCluster & archiveCluster, const std::string & archiveFileName)
{
    auto loFile = Sc::Data::GetAsset(archiveCluster, archiveFileName);
    if ( !loFile )
    {
        logger.error("Failed to find " + archiveFileName);
        return false;
    }

    this->loData = loFile.value();
    if ( this->loData.size() < 8 )
    {
        this->loData.assign(12, 0); // Create a header indicating an empty lo file
        logger.error("Invalid .lo* headerfor " + archiveFileName);
        return false;
    }
    return true;
}

bool Sc::Sprite::load(ArchiveCluster & archiveCluster, Sc::TblFilePtr imagesTbl)
{
    logger.debug("Loading Sprites...");
    auto start = std::chrono::high_resolution_clock::now();
    
    size_t numStrings = imagesTbl->numStrings();
    this->imagesTbl = imagesTbl;
    this->loFiles.assign(numStrings, LoFile{});

    Sc::Sprite::Grp blankGrp;
    blankGrp.makeBlank();
    
    grps.push_back(blankGrp);
    for ( size_t i=1; i<=numStrings; i++ )
    {
        const std::string & imageFilePath = imagesTbl->getString(i);
        Sc::Sprite::Grp grp;
        if ( getArchiveFileExtension(imageFilePath).compare(".grp") == 0 )
        {
            if ( grp.load(archiveCluster, "unit\\" + imageFilePath) )
                grps.push_back(grp);
            else
            {
                logger.error() << "Loading of one or more GRP files failed!" << std::endl;
                return false;
            }
        }
        else if ( getArchiveFileExtension(imageFilePath).starts_with(".lo") )
        {
            grps.push_back(blankGrp);
            loFiles[i].load(archiveCluster, "unit\\" + imageFilePath);
        }
        else
        {
            grps.push_back(blankGrp);
        }
    }

    if ( numStrings == 0 )
        logger.warn() << "images.tbl was empty, no grps were loaded!" << std::endl;

    auto imageData = Sc::Data::GetAsset(archiveCluster, "arr\\images.dat");
    if ( !imageData )
    {
        logger.error() << "Failed to load arr\\images.dat" << std::endl;
        return false;
    }
    else if ( imageData->size() != sizeof(Sc::Sprite::ImageDatFile) )
    {
        logger.error() << "Unrecognized ImageDat format!" << std::endl;
        return false;
    }

    ImageDatFile & datFile = (ImageDatFile &)imageData.value()[0];
    for ( size_t i=0; i<TotalImages; i++ )
    {
        images.push_back(ImageDatEntry { datFile.grpFile[i], datFile.graphicTurns[i], datFile.clickable[i], datFile.useFullIscript[i], datFile.drawIfCloaked[i],
            datFile.drawFunction[i], datFile.remapping[i], datFile.iScriptId[i], datFile.shieldOverlay[i], datFile.attackOverlay[i], datFile.damageOverlay[i],
            datFile.specialOverlay[i], datFile.landingDustOverlay[i], datFile.liftOffOverlay[i] });
    }

    auto spriteData = Sc::Data::GetAsset(archiveCluster, "arr\\sprites.dat");
    if ( !spriteData )
    {
        logger.error() << "Failed to load arr\\sprites.dat" << std::endl;
        return false;
    }
    else if ( spriteData->size() != sizeof(Sc::Sprite::DatFile) )
    {
        logger.error() << "Unrecognized SpriteDat format!" << std::endl;
        return false;
    }

    DatFile & spriteDatFile = (DatFile &)spriteData.value()[0];
    size_t i=0;
    for ( ; i<DatFile::IdRange::From0To129; i++ )
        sprites.push_back(DatEntry { spriteDatFile.imageFile[i], u8(0), spriteDatFile.unknown[i], spriteDatFile.isVisible[i], u8(0), u8(0) });

    for ( ; i<TotalSprites; i++ )
    {
        sprites.push_back(DatEntry { spriteDatFile.imageFile[i], spriteDatFile.healthBar[i-DatFile::IdRange::From0To129],
            spriteDatFile.unknown[i], spriteDatFile.isVisible[i], spriteDatFile.selectionCircleImage[i-DatFile::IdRange::From0To129],
            spriteDatFile.selectionCircleOffset[i-DatFile::IdRange::From0To129] });
    }
    
    auto finish = std::chrono::high_resolution_clock::now();

    auto iscriptFile = Sc::Data::GetAsset(archiveCluster, "scripts\\ISCRIPT.BIN");
    if ( !iscriptFile )
    {
        logger.error() << "Failed to load scripts\\ISCRIPT.BIN" << std::endl;
        return false;
    }
    this->iscript.swap(*iscriptFile);

    IScriptDatFileHeader* scriptHeader = (IScriptDatFileHeader*)&iscript[0];
    size_t isIdTableOffset = size_t(scriptHeader->isIdTableOffset);
    IScriptIdTableEntry* iScriptIdTable = (IScriptIdTableEntry*)&iscript[isIdTableOffset];
    for ( ; iScriptIdTable->id != 0xFFFF; ++iScriptIdTable )
    {
        if ( iScriptIdTable->id >= iscriptOffsets.size() )
            iscriptOffsets.resize(iScriptIdTable->id+1, 0);
        
        iscriptOffsets[iScriptIdTable->id] = iScriptIdTable->offset;

        // TODO: Long-term it's likely that only the iscriptOffsets code above is needed and the below code and the loadAnimation method can be removed
        u16 id = iScriptIdTable->id;
        bool idIncludesFlip = false;
        bool idIncludesUnflip = false;
        size_t animationsOffset = size_t(iScriptIdTable->offset);
        if ( animationsOffset >= iscript.size() )
        {
            logger.error() << "Failed to parse scripts\\ISCRIPT.BIN" << std::endl;
            return false;
        }
        IScriptAnimationHeader* iScriptAnimationHeader = (IScriptAnimationHeader*)&iscript[animationsOffset];
        size_t totalAnimations = size_t(iScriptAnimationHeader->animationCount & 0xFFFE) + 2;

        for ( size_t animationIndex=0; animationIndex < totalAnimations; ++animationIndex )
        {
            size_t animationOffset = iScriptAnimationHeader->animationsOffset[animationIndex];
            if ( animationOffset > 0 )
            {
                size_t currOffset = animationOffset;
                if ( currOffset >= iscript.size() )
                {
                    logger.error() << "Failed to parse scripts\\ISCRIPT.BIN" << std::endl;
                    return false;
                }

                if ( currOffset < iscript.size() )
                {
                    IScriptAnimation* animation = (IScriptAnimation*)&iscript[currOffset];
                    std::set<size_t> visitedOffsets {};
                    visitedOffsets.insert(currOffset);
                    if ( !loadAnimation(id, animation, currOffset, idIncludesFlip, idIncludesUnflip, visitedOffsets) )
                        return false;
                }
                else
                {
                    logger.error() << "Failed to parse scripts\\ISCRIPT.BIN" << std::endl;
                    return false;
                }
            }
        }
        if ( idIncludesFlip && !idIncludesUnflip )
            iscriptIdFlipsGrp.insert(id);
    }

    logger.debug() << "Sprite loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

bool Sc::Sprite::loadAnimation(u16 id, IScriptAnimation* animation, size_t currOffset, bool & idIncludesFlip, bool & idIncludesUnflip, std::set<size_t> & visitedOffsets)
{
    // id here is just a temporary stopgap so I can examine one script, flip and unflip temporary loaders
    // visitedOffset is an immediate-cache preventing infinite loops during initial load... during actual animation though you want the infinite loop behavior
    // iScript being just a byte vector..., currOffset an index into it, animation a pointer into it at currOffset
    for ( ; ; )
    {
        if ( currOffset >= iscript.size() )
            return true;

        Op code = Op(animation->code);
        ++currOffset; // 1-byte code
        if ( code < OpParams.size() )
        {
            auto & opCodeParams = OpParams[code];
            if ( code == Op::setflipstate && iscript[currOffset] == 1 ) 
                idIncludesFlip = true;
            else if ( code == Op::setflipstate && iscript[currOffset] == 0 )
                idIncludesUnflip = true;
            else if ( code == Op::end || code == Op::return_ )
                return true;
            else if ( code == Op::goto_ ) // TODO: There are more such jump codes
            {
                u16 dest = (u16 &)iscript[currOffset];
                if ( dest > iscript.size() )
                {
                    logger.error() << "Failed to parse scripts\\ISCRIPT.BIN" << std::endl;
                    return true;
                }
                else if ( visitedOffsets.find(dest) == visitedOffsets.end() )
                {
                    visitedOffsets.insert(dest);
                    IScriptAnimation* subAnimation = (IScriptAnimation*)&iscript[dest];
                    return loadAnimation(id, subAnimation, dest, idIncludesFlip, idIncludesUnflip, visitedOffsets);
                }
                else
                    return true;
            }
            
            // Move currOffset past all the parameters
            for ( int param=0; param<opCodeParams.size(); ++param )
            {
                auto currParam = opCodeParams[param];
                auto paramSize = ParamSize[size_t(currParam)];
                u16 paramValue = paramSize == 1 ? iscript[currOffset] : (u16 &)iscript[currOffset];
                currOffset += paramSize;
            }
            if ( currOffset < iscript.size() )
                animation = (IScriptAnimation*)&iscript[currOffset];
        }
    }
    return true;
}

const Sc::Sprite::IScriptAnimation* Sc::Sprite::getAnimationHeader(size_t iScriptId, AnimHeader animHeader) const
{
    IScriptDatFileHeader* scriptHeader = (IScriptDatFileHeader*)&iscript[0];
    size_t animationsOffset = iscriptOffsets[iScriptId];
    if ( animationsOffset >= iscript.size() )
    {
        logger.error() << "Invalid iscript\n";
        return nullptr;
    }
    IScriptAnimationHeader* iScriptAnimationHeader = (IScriptAnimationHeader*)&iscript[animationsOffset];
    size_t totalAnimations = size_t(iScriptAnimationHeader->animationCount & 0xFFFE) + 2;
    if ( std::size_t(animHeader) >= totalAnimations )
        return nullptr;

    size_t animationOffset = iScriptAnimationHeader->animationsOffset[std::size_t(animHeader)];
    if ( animationOffset > 0 )
    {
        size_t currOffset = animationOffset;
        if ( currOffset >= iscript.size() )
        {
            logger.error() << "Invalid iscript\n";
            return nullptr;
        }
        else
        {
            IScriptAnimation* animation = (IScriptAnimation*)&iscript[currOffset];
            return animation;
        }
    }
    return nullptr;
}

const Sc::Sprite::Grp & Sc::Sprite::getGrp(size_t grpIndex)
{
    if ( grpIndex < grps.size() )
        return grps[grpIndex];
    else
        throw std::out_of_range(std::string("GrpIndex: ") + std::to_string(grpIndex) + " is out of range for grps vector of size " + std::to_string(grps.size()));
}

const Sc::Sprite::ImageDatEntry & Sc::Sprite::getImage(size_t imageIndex) const
{
    if ( imageIndex < images.size() )
        return images[imageIndex];
    else
        throw std::out_of_range(std::string("ImageIndex: ") + std::to_string(imageIndex) + " is out of range for images vector of size " + std::to_string(images.size()));
}

const Sc::Sprite::DatEntry & Sc::Sprite::getSprite(size_t spriteIndex) const
{
    if ( spriteIndex < sprites.size() )
        return sprites[spriteIndex];
    else
        throw std::out_of_range(std::string("SpriteIndex: ") + std::to_string(spriteIndex) + " is out of range for sprites vector of size " + std::to_string(sprites.size()));
}

size_t Sc::Sprite::numGrps() const
{
    return grps.size();
}

size_t Sc::Sprite::numImages() const
{
    return images.size();
}

size_t Sc::Sprite::numSprites() const
{
    return sprites.size();
}

bool Sc::Sprite::imageFlipped(u16 imageId) const
{
    return this->iscriptIdFlipsGrp.find(this->images[imageId].iScriptId) != this->iscriptIdFlipsGrp.end();
}

bool Sc::Spk::load(ArchiveCluster & archiveCluster)
{
    try
    {
        auto starSpkFile = Sc::Data::GetAsset(archiveCluster, "SD\\TileSet\\platform.spk", true);
        if ( !starSpkFile )
            starSpkFile = Sc::Data::GetAsset(archiveCluster, "parallax\\star.spk");

        if ( !starSpkFile )
        {
            logger.error() << "Failed to load either \"parallax\\star.spk\" or \"SD\\TileSet\\platform.spk\"";
            return false;
        }
    
        if ( starSpkFile->size() < sizeof(u16) )
            throw std::runtime_error("star.spk must be 2-bytes at minimum");

        const Sc::Spk::SpkFile* starSpk = reinterpret_cast<Sc::Spk::SpkFile*>(starSpkFile->data());

        auto totalLayers = starSpk->totalLayers;
        if ( starSpkFile->size() < sizeof(u16) + totalLayers*sizeof(u16) )
        {
            throw std::runtime_error("A star.spk with " + std::to_string(totalLayers) +
                " must be at least " + std::to_string(sizeof(u16)+totalLayers*sizeof(u16)) + " bytes");
        }

        const Sc::Spk::SpkFile::StarPosition* baseStarPositions = reinterpret_cast<Sc::Spk::SpkFile::StarPosition*>(
            starSpkFile->data() + sizeof(u16) + totalLayers*sizeof(u16));

        size_t starPositionsVisited = 0;
        for ( size_t layer=0; layer<size_t(totalLayers); ++layer )
        {
            auto & stars = layerStars.emplace_back();
            auto totalLayerImages = starSpk->totalImagesInLayer[layer];
            if ( totalLayerImages > 0 )
            {
                stars.reserve(totalLayerImages);
                const Sc::Spk::SpkFile::StarPosition* layerStarPositions = &baseStarPositions[starPositionsVisited];
                for ( size_t i=0; i<totalLayerImages; ++i )
                {
                    if ( starSpkFile->size() < sizeof(u16) + totalLayers*sizeof(u16) + (starPositionsVisited+i)*sizeof(Sc::Spk::SpkFile::StarPosition) )
                        throw std::runtime_error("star.spk was not large enough to hold all of its star positions!");

                    const Sc::Spk::SpkFile::StarPosition & layerStarPosition = layerStarPositions[i];
                    auto xc = layerStarPosition.xc;
                    auto yc = layerStarPosition.yc;
                    size_t bitmapOffset = static_cast<size_t>(layerStarPosition.bitmapOffset);
                    if ( starSpkFile->size() < bitmapOffset+2*sizeof(u16) )
                        throw std::runtime_error("One or more bitmap positions in star.spk were invalid");

                    const Sc::Spk::SpkFile::StarBitmap* starBitmap = reinterpret_cast<Sc::Spk::SpkFile::StarBitmap*>(starSpkFile->data() + bitmapOffset);
                    auto width = starBitmap->width;
                    auto height = starBitmap->height;
                    if ( width > 0 && height > 0 )
                    {
                        if ( starSpkFile->size() < bitmapOffset+2*sizeof(u16) + size_t(width)*size_t(height) )
                            throw std::runtime_error("One or more bitmaps in star.spk were invalid");
                        
                        stars.emplace_back(xc, yc, starBitmap);
                    }
                    else
                        throw std::runtime_error("A star.spk bitmap had an invalid size");
                
                }
                starPositionsVisited += totalLayerImages;
            }
        }
        spkData.swap(*starSpkFile);
    }
    catch ( std::exception & e )
    {
        logger.error("Error loading classic star.spk file ", e);
        return false;
    }

    return true;
}

bool Sc::Upgrade::load(ArchiveCluster & archiveCluster)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto upgradeData = Sc::Data::GetAsset(archiveCluster, "arr\\upgrades.dat");
    if ( !upgradeData )
    {
        logger.error() << "Failed to load arr\\upgrades.dat" << std::endl;
        return false;
    }
    else if ( upgradeData->size() != sizeof(Sc::Upgrade::DatFile) && upgradeData->size() != sizeof(Sc::Upgrade::OriginalDatFile) )
    {
        logger.error() << "Unrecognized UpgradeDat format!" << std::endl;
        return false;
    }
    
    bool isExpansion = (upgradeData->size() == sizeof(Sc::Upgrade::DatFile));
    if ( isExpansion )
    {
        const DatFile & upgradeDat = (DatFile &)upgradeData.value()[0];
        for ( size_t i=0; i<TotalTypes; i++ )
        {
            upgrades.push_back(Sc::Upgrade::DatEntry { upgradeDat.mineralCost[i], upgradeDat.mineralFactor[i], upgradeDat.vespeneCost[i], upgradeDat.vespeneFactor[i],
                upgradeDat.timeCost[i], upgradeDat.timeFactor[i], upgradeDat.unknown[i], upgradeDat.icon[i], upgradeDat.label[i], upgradeDat.race[i],
                upgradeDat.maxRepeats[i], upgradeDat.broodWarSpecific[i] });
        }
    }
    else
    {
        const OriginalDatFile & upgradeDat = (OriginalDatFile &)upgradeData.value()[0];
        for ( size_t i=0; i<TotalOriginalTypes; i++ )
        {
            upgrades.push_back(Sc::Upgrade::DatEntry { upgradeDat.mineralCost[i], upgradeDat.mineralFactor[i], upgradeDat.vespeneCost[i], upgradeDat.vespeneFactor[i],
                upgradeDat.timeCost[i], upgradeDat.timeFactor[i], upgradeDat.unknown[i], upgradeDat.icon[i], upgradeDat.label[i], upgradeDat.race[i],
                upgradeDat.maxRepeats[i], u8(0) });
        }
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "Upgrade loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

const Sc::Upgrade::DatEntry & Sc::Upgrade::getUpgrade(Type upgradeType) const
{
    if ( upgradeType < upgrades.size() )
        return upgrades[upgradeType];
    else
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for upgrades vector of size " + std::to_string(upgrades.size()));
}

bool Sc::Tech::load(ArchiveCluster & archiveCluster)
{
    auto start = std::chrono::high_resolution_clock::now();
    auto techData = Sc::Data::GetAsset(archiveCluster, "arr\\techdata.dat");
    if ( !techData )
    {
        logger.error() << "Failed to load arr\\techdata.dat" << std::endl;
        return false;
    }
    else if ( techData->size() != sizeof(Sc::Tech::DatFile) && techData->size() != sizeof(Sc::Tech::OriginalDatFile) )
    {
        logger.error() << "Unrecognized TechDat format!" << std::endl;
        return false;
    }
    
    bool isExpansion = (techData->size() == sizeof(Sc::Tech::DatFile));
    if ( isExpansion )
    {
        const DatFile & techDat = (DatFile &)techData.value()[0];
        for ( size_t i=0; i<TotalTypes; i++ )
        {
            techs.push_back(Sc::Tech::DatEntry { techDat.mineralCost[i], techDat.vespeneCost[i], techDat.researchTime[i], techDat.energyCost[i],
                techDat.unknown[i], techDat.icon[i], techDat.label[i], techDat.race[i], techDat.unused[i], techDat.broodWar[i] });
        }
    }
    else
    {
        const OriginalDatFile & techDat = (OriginalDatFile &)techData.value()[0];
        for ( size_t i=0; i<TotalOriginalTypes; i++ )
        {
            techs.push_back(Sc::Tech::DatEntry { techDat.mineralCost[i], techDat.vespeneCost[i], techDat.researchTime[i], techDat.energyCost[i],
                techDat.unknown[i], techDat.icon[i], techDat.label[i], techDat.race[i], techDat.unused[i], u8(0) });
        }
    }
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "Tech loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

const Sc::Tech::DatEntry & Sc::Tech::getTech(Type techType) const
{
    if ( size_t(techType) < techs.size() )
        return techs[size_t(techType)];
    else
        throw std::out_of_range(std::string("TechType: ") + std::to_string(size_t(techType)) + " is out of range for tecs vector of size " + std::to_string(techs.size()));
}

const std::vector<std::string> Sc::Sound::virtualSoundPaths = {
    "sound\\Zerg\\Drone\\ZDrErr00.WAV",
    "sound\\Misc\\Buzz.wav",
    "sound\\Misc\\PError.WAV",
    "sound\\Misc\\ZBldgPlc.wav",
    "sound\\Misc\\TBldgPlc.wav",
    "sound\\Misc\\PBldgPlc.wav",
    "sound\\Misc\\ExploLrg.wav",
    "sound\\Misc\\ExploMed.wav",
    "sound\\Misc\\ExploSm.wav",
    "sound\\Misc\\Explo1.wav",
    "sound\\Misc\\Explo2.wav",
    "sound\\Misc\\Explo3.wav",
    "sound\\Misc\\Explo4.wav",
    "sound\\Misc\\Explo5.wav",
    "sound\\Misc\\Button.wav",
    "sound\\Misc\\BurrowDn.wav",
    "sound\\Misc\\BurrowUp.wav",
    "sound\\Misc\\OnFirSml.wav",
    "sound\\Misc\\IntoNydus.wav",
    "sound\\Misc\\OutOfGas.wav",
    "sound\\Misc\\youwin.wav",
    "sound\\Misc\\youlose.wav",
    "sound\\Misc\\Transmission.wav",
    "sound\\Misc\\Door\\Door1Opn.wav",
    "sound\\Misc\\Door\\Door1Cls.wav",
    "sound\\Misc\\Door\\Door2Opn.wav",
    "sound\\Misc\\Door\\Door2Cls.wav",
    "sound\\Misc\\Door\\Door3Opn.wav",
    "sound\\Misc\\Door\\Door3Cls.wav",
    "sound\\Misc\\Door\\Door4Opn.wav",
    "sound\\Misc\\Door\\Door4Cls.wav",
    "sound\\Misc\\ZRescue.wav",
    "sound\\Misc\\TRescue.wav",
    "sound\\Misc\\PRescue.wav",
    "sound\\terran\\SCV\\EDrRep00.wav",
    "sound\\terran\\SCV\\EDrRep01.wav",
    "sound\\terran\\SCV\\EDrRep02.wav",
    "sound\\terran\\SCV\\EDrRep03.wav",
    "sound\\terran\\SCV\\EDrRep04.wav",
    "sound\\Misc\\ZOvTra00.wav",
    "sound\\Misc\\TDrTra00.wav",
    "sound\\Misc\\PShTra00.wav",
    "sound\\Misc\\ZOvTra01.wav",
    "sound\\Misc\\TDrTra01.wav",
    "sound\\Misc\\PShTra01.wav",
    "sound\\Misc\\CRITTERS\\JCrWht00.wav",
    "sound\\Misc\\CRITTERS\\JCrWht01.wav",
    "sound\\Misc\\CRITTERS\\JCrWht02.wav",
    "sound\\Misc\\CRITTERS\\JCrDth00.wav",
    "sound\\Misc\\CRITTERS\\LCrWht00.wav",
    "sound\\Misc\\CRITTERS\\LCrWht01.wav",
    "sound\\Misc\\CRITTERS\\LCrWht02.wav",
    "sound\\Misc\\CRITTERS\\LCrDth00.wav",
    "sound\\Misc\\CRITTERS\\BCrWht00.wav",
    "sound\\Misc\\CRITTERS\\BCrWht01.wav",
    "sound\\Misc\\CRITTERS\\BCrWht02.wav",
    "sound\\Misc\\CRITTERS\\BCrDth00.wav",
    "sound\\Bullet\\PArFir00.wav",
    "sound\\Bullet\\ZHyFir00.wav",
    "sound\\Bullet\\ZHyHit00.wav",
    "sound\\Bullet\\ZDrHit00.wav",
    "sound\\Bullet\\LaserHit.wav",
    "sound\\Bullet\\pshield.wav",
    "sound\\Bullet\\SpoogHit.wav",
    "sound\\Bullet\\BlastCan.wav",
    "sound\\Bullet\\BlastGn2.wav",
    "sound\\Bullet\\ZBGHit00.wav",
    "sound\\Bullet\\TTaFir00.wav",
    "sound\\Bullet\\TMaFir00.wav",
    "sound\\Bullet\\TGoFir00.wav",
    "sound\\Bullet\\ZGuFir00.wav",
    "sound\\Bullet\\ZGuHit00.wav",
    "sound\\Bullet\\LASERB.wav",
    "sound\\Bullet\\TPhFi100.wav",
    "sound\\Bullet\\LASRHIT1.wav",
    "sound\\Bullet\\LASRHIT2.wav",
    "sound\\Bullet\\LASRHIT3.wav",
    "sound\\Bullet\\ZLuFir00.wav",
    "sound\\Bullet\\ZLuHit00.wav",
    "sound\\Bullet\\HKMISSLE.wav",
    "sound\\Bullet\\TGoFi200.wav",
    "sound\\Bullet\\TPhFi200.wav",
    "sound\\Bullet\\TNsFir00.wav",
    "sound\\Bullet\\TNsHit00.wav",
    "sound\\Bullet\\PhoHit00.wav",
    "sound\\Bullet\\PSIBLADE.wav",
    "sound\\Bullet\\PSIBOLT.wav",
    "sound\\Bullet\\ZQuFir00.wav",
    "sound\\Bullet\\ZQuHit00.wav",
    "sound\\Bullet\\ZQuHit01.wav",
    "sound\\Bullet\\ZQuHit02.wav",
    "sound\\Bullet\\tscFir00.wav",
    "sound\\Bullet\\SHOCKBMB.wav",
    "sound\\Bullet\\TTaFi200.wav",
    "sound\\Bullet\\SHCKLNCH.wav",
    "sound\\Bullet\\TGhFir00.wav",
    "sound\\Bullet\\DragBull.wav",
    "sound\\Bullet\\psaHit00.wav",
    "sound\\Bullet\\LaserB.wav",
    "sound\\Bullet\\pTrFir00.wav",
    "sound\\Bullet\\pTrFir01.wav",
    "sound\\Bullet\\tvuFir00.wav",
    "sound\\Bullet\\tvuHit00.wav",
    "sound\\Bullet\\tvuHit01.wav",
    "sound\\Bullet\\tvuHit02.wav",
    "sound\\Bullet\\zdeAtt00.wav",
    "sound\\Bullet\\UZeFir00.wav",
    "sound\\Bullet\\UKiFir00.wav",
    "sound\\Bullet\\zmuFir00.wav",
    "sound\\Misc\\ZPwrDown.wav",
    "sound\\Misc\\TPwrDown.wav",
    "sound\\Misc\\PPwrDown.wav",
    "sound\\Zerg\\Advisor\\ZAdUpd00.WAV",
    "sound\\Terran\\Advisor\\TAdUpd00.WAV",
    "sound\\Protoss\\Advisor\\PAdUpd00.WAV",
    "sound\\Zerg\\Advisor\\ZAdUpd01.WAV",
    "sound\\Terran\\Advisor\\TAdUpd01.WAV",
    "sound\\Protoss\\Advisor\\PAdUpd01.WAV",
    "sound\\Zerg\\Advisor\\ZAdUpd02.WAV",
    "sound\\Terran\\Advisor\\TAdUpd02.WAV",
    "sound\\Protoss\\Advisor\\PAdUpd02.WAV",
    "sound\\Terran\\Advisor\\TAdUpd03.WAV",
    "sound\\Zerg\\Advisor\\ZAdUpd04.WAV",
    "sound\\Terran\\Advisor\\TAdUpd04.WAV",
    "sound\\Protoss\\Advisor\\PAdUpd04.WAV",
    "sound\\Terran\\Advisor\\TAdUpd05.WAV",
    "sound\\Terran\\Advisor\\TAdUpd06.WAV",
    "sound\\Protoss\\Advisor\\PAdUpd06.WAV",
    "sound\\Terran\\Advisor\\TAdUpd07.WAV",
    "sound\\Zerg\\Bldg\\ZChRdy00.WAV",
    "sound\\Terran\\SCV\\TSCUpd00.WAV",
    "sound\\Zerg\\DRONE\\ZDrErr00.WAV",
    "sound\\Misc\\PError.wav",
    "sound\\Terran\\Advisor\\tAdErr04.WAV",
    "sound\\Terran\\Advisor\\tAdErr03.WAV",
    "sound\\Zerg\\Advisor\\ZAdErr00.WAV",
    "sound\\Terran\\Advisor\\tAdErr00.WAV",
    "sound\\Protoss\\Advisor\\PAdErr00.WAV",
    "sound\\Zerg\\Advisor\\ZAdErr01.WAV",
    "sound\\Terran\\Advisor\\tAdErr01.WAV",
    "sound\\Protoss\\Advisor\\PAdErr01.WAV",
    "sound\\Zerg\\Advisor\\ZAdErr02.WAV",
    "sound\\Terran\\Advisor\\tAdErr02.WAV",
    "sound\\Protoss\\Advisor\\PAdErr02.WAV",
    "sound\\Zerg\\Advisor\\ZAdErr06.WAV",
    "sound\\Terran\\Advisor\\tAdErr06.WAV",
    "sound\\Protoss\\Advisor\\PAdErr06.WAV",
    "sound\\Terran\\SCV\\TSCErr01.WAV",
    "sound\\Protoss\\PROBE\\PPrErr00.WAV",
    "sound\\Terran\\SCV\\TSCErr00.WAV",
    "sound\\Protoss\\PROBE\\PPrErr01.WAV",
    "sound\\glue\\mousedown2.wav",
    "sound\\glue\\mouseover.wav",
    "sound\\glue\\bnetclick.wav",
    "sound\\glue\\scorefill.wav",
    "sound\\glue\\scorefillend.wav",
    "sound\\glue\\countdown.wav",
    "sound\\glue\\swishlock.wav",
    "sound\\glue\\swishin.wav",
    "sound\\glue\\swishout.wav",
    "sound\\Terran\\SCV\\TSCMin00.wav",
    "sound\\Terran\\SCV\\TSCMin01.wav",
    "sound\\Terran\\BATTLE\\tbardy00.wav",
    "sound\\Terran\\BATTLE\\tbaDth00.wav",
    "sound\\Terran\\BATTLE\\tBaYam01.wav",
    "sound\\Terran\\BATTLE\\tBaYam02.wav",
    "sound\\Terran\\BATTLE\\tbapss00.wav",
    "sound\\Terran\\BATTLE\\tbapss01.wav",
    "sound\\Terran\\BATTLE\\tbapss02.wav",
    "sound\\Terran\\BATTLE\\tbapss03.wav",
    "sound\\Terran\\BATTLE\\tbapss04.wav",
    "sound\\Terran\\BATTLE\\tbawht00.wav",
    "sound\\Terran\\BATTLE\\tbawht01.wav",
    "sound\\Terran\\BATTLE\\tbawht02.wav",
    "sound\\Terran\\BATTLE\\tbawht03.wav",
    "sound\\Terran\\BATTLE\\tbayes00.wav",
    "sound\\Terran\\BATTLE\\tbayes01.wav",
    "sound\\Terran\\BATTLE\\tbayes02.wav",
    "sound\\Terran\\BATTLE\\tbayes03.wav",
    "sound\\Terran\\CIVILIAN\\TCvPss00.WAV",
    "sound\\Terran\\CIVILIAN\\TCvPss01.WAV",
    "sound\\Terran\\CIVILIAN\\TCvPss02.WAV",
    "sound\\Terran\\CIVILIAN\\TCvPss03.WAV",
    "sound\\Terran\\CIVILIAN\\TCvPss04.WAV",
    "sound\\Terran\\CIVILIAN\\TCvWht00.WAV",
    "sound\\Terran\\CIVILIAN\\TCvWht01.WAV",
    "sound\\Terran\\CIVILIAN\\TCvWht02.WAV",
    "sound\\Terran\\CIVILIAN\\TCvWht03.WAV",
    "sound\\Terran\\CIVILIAN\\TCvYes00.WAV",
    "sound\\Terran\\CIVILIAN\\TCvYes01.WAV",
    "sound\\Terran\\CIVILIAN\\TCvYes02.WAV",
    "sound\\Terran\\CIVILIAN\\TCvYes03.WAV",
    "sound\\Terran\\CIVILIAN\\TCvYes04.WAV",
    "sound\\Terran\\CIVILIAN\\TCvDth00.WAV",
    "sound\\Terran\\CIVILIAN\\TCvRdy00.WAV",
    "sound\\Terran\\DROPSHIP\\TDrRdy00.wav",
    "sound\\Terran\\DROPSHIP\\TDrDth00.wav",
    "sound\\Terran\\DROPSHIP\\TDrPss00.wav",
    "sound\\Terran\\DROPSHIP\\TDrPss01.wav",
    "sound\\Terran\\DROPSHIP\\TDrPss02.wav",
    "sound\\Terran\\DROPSHIP\\TDrPss03.wav",
    "sound\\Terran\\DROPSHIP\\TDrWht00.wav",
    "sound\\Terran\\DROPSHIP\\TDrWht01.wav",
    "sound\\Terran\\DROPSHIP\\TDrWht02.wav",
    "sound\\Terran\\DROPSHIP\\TDrWht03.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes00.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes01.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes02.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes03.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes04.wav",
    "sound\\Terran\\DROPSHIP\\TDrYes05.wav",
    "sound\\Terran\\GHOST\\TGhRdy00.wav",
    "sound\\Terran\\GHOST\\TGhPss00.wav",
    "sound\\Terran\\GHOST\\TGhPss01.wav",
    "sound\\Terran\\GHOST\\TGhPss02.wav",
    "sound\\Terran\\GHOST\\TGhPss03.wav",
    "sound\\Terran\\GHOST\\TGhWht00.wav",
    "sound\\Terran\\GHOST\\TGhWht01.wav",
    "sound\\Terran\\GHOST\\TGhWht02.wav",
    "sound\\Terran\\GHOST\\TGhWht03.wav",
    "sound\\Terran\\GHOST\\TGhYes00.wav",
    "sound\\Terran\\GHOST\\TGhYes01.wav",
    "sound\\Terran\\GHOST\\TGhYes02.wav",
    "sound\\Terran\\GHOST\\TGhYes03.wav",
    "sound\\Terran\\GHOST\\TGhDth00.wav",
    "sound\\Terran\\GHOST\\TGhLas00.wav",
    "sound\\Terran\\GHOST\\TGhLkd00.wav",
    "sound\\Terran\\GOLIATH\\TGoRdy00.wav",
    "sound\\Terran\\GOLIATH\\TGoPss00.wav",
    "sound\\Terran\\GOLIATH\\TGoPss01.WAV",
    "sound\\Terran\\GOLIATH\\TGoPss02.WAV",
    "sound\\Terran\\GOLIATH\\TGoPss03.WAV",
    "sound\\Terran\\GOLIATH\\TGoPss04.WAV",
    "sound\\Terran\\GOLIATH\\TGoPss05.WAV",
    "sound\\Terran\\GOLIATH\\TGoWht00.WAV",
    "sound\\Terran\\GOLIATH\\TGoWht01.WAV",
    "sound\\Terran\\GOLIATH\\TGoWht02.WAV",
    "sound\\Terran\\GOLIATH\\TGoWht03.WAV",
    "sound\\Terran\\GOLIATH\\TGoYes00.WAV",
    "sound\\Terran\\GOLIATH\\TGoYes01.WAV",
    "sound\\Terran\\GOLIATH\\TGoYes02.WAV",
    "sound\\Terran\\GOLIATH\\TGoYes03.WAV",
    "sound\\Terran\\PHOENIX\\TPhRdy00.WAV",
    "sound\\Terran\\PHOENIX\\TPhDth00.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss00.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss01.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss02.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss03.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss04.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss05.WAV",
    "sound\\Terran\\PHOENIX\\TPhPss06.WAV",
    "sound\\Terran\\PHOENIX\\TPhWht00.WAV",
    "sound\\Terran\\PHOENIX\\TPhWht01.WAV",
    "sound\\Terran\\PHOENIX\\TPhWht02.WAV",
    "sound\\Terran\\PHOENIX\\TPhWht03.WAV",
    "sound\\Terran\\PHOENIX\\TPhYes00.WAV",
    "sound\\Terran\\PHOENIX\\TPhYes01.WAV",
    "sound\\Terran\\PHOENIX\\TPhYes02.WAV",
    "sound\\Terran\\PHOENIX\\TPhYes03.WAV",
    "sound\\Terran\\PHOENIX\\TPhClo00.WAV",
    "sound\\Terran\\PHOENIX\\TPhClo01.WAV",
    "sound\\Terran\\MARINE\\TMaRdy00.WAV",
    "sound\\Terran\\MARINE\\TMaDth00.WAV",
    "sound\\Terran\\MARINE\\TMaDth01.WAV",
    "sound\\Terran\\MARINE\\TMaSti00.WAV",
    "sound\\Terran\\MARINE\\TMaSti01.WAV",
    "sound\\Terran\\MARINE\\TMaPss00.WAV",
    "sound\\Terran\\MARINE\\TMaPss01.WAV",
    "sound\\Terran\\MARINE\\TMaPss02.WAV",
    "sound\\Terran\\MARINE\\TMaPss03.WAV",
    "sound\\Terran\\MARINE\\TMaPss04.WAV",
    "sound\\Terran\\MARINE\\TMaPss05.WAV",
    "sound\\Terran\\MARINE\\TMaPss06.WAV",
    "sound\\Terran\\MARINE\\TMaWht00.WAV",
    "sound\\Terran\\MARINE\\TMaWht01.WAV",
    "sound\\Terran\\MARINE\\TMaWht02.WAV",
    "sound\\Terran\\MARINE\\TMaWht03.WAV",
    "sound\\Terran\\MARINE\\TMaYes00.WAV",
    "sound\\Terran\\MARINE\\TMaYes01.WAV",
    "sound\\Terran\\MARINE\\TMaYes02.WAV",
    "sound\\Terran\\MARINE\\TMaYes03.WAV",
    "sound\\Terran\\Firebat\\TFBRdy00.WAV",
    "sound\\Terran\\Firebat\\TFBDth00.WAV",
    "sound\\Terran\\Firebat\\TFBDth01.WAV",
    "sound\\Terran\\Firebat\\TFBDth02.WAV",
    "sound\\Terran\\Firebat\\TFBWht00.WAV",
    "sound\\Terran\\Firebat\\TFBWht01.WAV",
    "sound\\Terran\\Firebat\\TFBWht02.WAV",
    "sound\\Terran\\Firebat\\TFBWht03.WAV",
    "sound\\Terran\\Firebat\\TFBPss00.WAV",
    "sound\\Terran\\Firebat\\TFBPss01.WAV",
    "sound\\Terran\\Firebat\\TFBPss02.WAV",
    "sound\\Terran\\Firebat\\TFBPss03.WAV",
    "sound\\Terran\\Firebat\\TFBPss04.WAV",
    "sound\\Terran\\Firebat\\TFBPss05.WAV",
    "sound\\Terran\\Firebat\\TFBPss06.WAV",
    "sound\\Terran\\Firebat\\TFBYes00.WAV",
    "sound\\Terran\\Firebat\\TFBYes01.WAV",
    "sound\\Terran\\Firebat\\TFBYes02.WAV",
    "sound\\Terran\\Firebat\\TFBYes03.WAV",
    "sound\\Terran\\Firebat\\TFBFir00.WAV",
    "sound\\Terran\\Firebat\\TFBFir01.WAV",
    "sound\\Terran\\TANK\\TTaRdy00.WAV",
    "sound\\Terran\\TANK\\TTaDth00.WAV",
    "sound\\Terran\\TANK\\TTaTra00.WAV",
    "sound\\Terran\\TANK\\TTaTra01.WAV",
    "sound\\Terran\\TANK\\TTaPss00.WAV",
    "sound\\Terran\\TANK\\TTaPss01.WAV",
    "sound\\Terran\\TANK\\TTaPss02.WAV",
    "sound\\Terran\\TANK\\TTaPss03.WAV",
    "sound\\Terran\\TANK\\TTaWht00.WAV",
    "sound\\Terran\\TANK\\TTaWht01.WAV",
    "sound\\Terran\\TANK\\TTaWht02.WAV",
    "sound\\Terran\\TANK\\TTaWht03.WAV",
    "sound\\Terran\\TANK\\TTaYes00.WAV",
    "sound\\Terran\\TANK\\TTaYes01.WAV",
    "sound\\Terran\\TANK\\TTaYes02.WAV",
    "sound\\Terran\\TANK\\TTaYes03.WAV",
    "sound\\Terran\\VESSEL\\TVeRdy00.WAV",
    "sound\\Terran\\VESSEL\\TVePss00.WAV",
    "sound\\Terran\\VESSEL\\TVePss01.WAV",
    "sound\\Terran\\VESSEL\\TVePss02.WAV",
    "sound\\Terran\\VESSEL\\TVePss03.WAV",
    "sound\\Terran\\VESSEL\\TVePss04.WAV",
    "sound\\Terran\\VESSEL\\TVePss05.WAV",
    "sound\\Terran\\VESSEL\\TVePss06.WAV",
    "sound\\Terran\\VESSEL\\TVeWht00.WAV",
    "sound\\Terran\\VESSEL\\TVeWht01.WAV",
    "sound\\Terran\\VESSEL\\TVeWht02.WAV",
    "sound\\Terran\\VESSEL\\TVeWht03.WAV",
    "sound\\Terran\\VESSEL\\TVeYes00.WAV",
    "sound\\Terran\\VESSEL\\TVeYes01.WAV",
    "sound\\Terran\\VESSEL\\TVeYes02.WAV",
    "sound\\Terran\\VESSEL\\TVeYes03.WAV",
    "sound\\Terran\\VESSEL\\TVeDth00.WAV",
    "sound\\Terran\\VESSEL\\TVeDef00.WAV",
    "sound\\Terran\\VESSEL\\TVeEmp00.WAV",
    "sound\\Terran\\VESSEL\\TVeIrr00.WAV",
    "sound\\Terran\\VULTURE\\TVuRdy00.WAV",
    "sound\\Terran\\VULTURE\\TVuDth00.WAV",
    "sound\\Terran\\VULTURE\\TVuMin01.WAV",
    "sound\\Terran\\VULTURE\\TVuMin00.WAV",
    "sound\\Terran\\VULTURE\\TVuPss00.WAV",
    "sound\\Terran\\VULTURE\\TVuPss01.WAV",
    "sound\\Terran\\VULTURE\\TVuPss02.WAV",
    "sound\\Terran\\VULTURE\\TVuPss03.WAV",
    "sound\\Terran\\VULTURE\\TVuWht00.WAV",
    "sound\\Terran\\VULTURE\\TVuWht01.WAV",
    "sound\\Terran\\VULTURE\\TVuWht02.WAV",
    "sound\\Terran\\VULTURE\\TVuWht03.WAV",
    "sound\\Terran\\VULTURE\\TVuYes00.WAV",
    "sound\\Terran\\VULTURE\\TVuYes01.WAV",
    "sound\\Terran\\VULTURE\\TVuYes02.WAV",
    "sound\\Terran\\VULTURE\\TVuYes03.WAV",
    "sound\\Terran\\SCV\\TSCRdy00.WAV",
    "sound\\Terran\\SCV\\TSCDth00.WAV",
    "sound\\Terran\\SCV\\TSCPss00.WAV",
    "sound\\Terran\\SCV\\TSCPss01.WAV",
    "sound\\Terran\\SCV\\TSCPss02.WAV",
    "sound\\Terran\\SCV\\TSCPss03.WAV",
    "sound\\Terran\\SCV\\TSCPss04.WAV",
    "sound\\Terran\\SCV\\TSCPss05.WAV",
    "sound\\Terran\\SCV\\TSCPss06.WAV",
    "sound\\Terran\\SCV\\TSCWht00.WAV",
    "sound\\Terran\\SCV\\TSCWht01.WAV",
    "sound\\Terran\\SCV\\TSCWht02.WAV",
    "sound\\Terran\\SCV\\TSCWht03.WAV",
    "sound\\Terran\\SCV\\TSCYes00.WAV",
    "sound\\Terran\\SCV\\TSCYes01.WAV",
    "sound\\Terran\\SCV\\TSCYes02.WAV",
    "sound\\Terran\\SCV\\TSCYes03.WAV",
    "sound\\Terran\\bldg\\tacwht00.WAV",
    "sound\\Terran\\bldg\\tclwht00.WAV",
    "sound\\Terran\\bldg\\tcswht00.WAV",
    "sound\\Terran\\bldg\\tcsSca00.WAV",
    "sound\\Terran\\bldg\\tddwht00.WAV",
    "sound\\Terran\\bldg\\tglwht00.WAV",
    "sound\\Misc\\UICWht00.wav",
    "sound\\Terran\\bldg\\tmswht00.WAV",
    "sound\\Terran\\bldg\\tmtwht00.WAV",
    "sound\\Misc\\UNrWht00.wav",
    "sound\\Terran\\bldg\\tRlWht00.WAV",
    "sound\\Terran\\bldg\\tnswht00.WAV",
    "sound\\Terran\\bldg\\tpgwht00.WAV",
    "sound\\Terran\\bldg\\tplwht00.WAV",
    "sound\\Misc\\button.wav",
    "sound\\Terran\\bldg\\trewht00.WAV",
    "sound\\Terran\\bldg\\trfwht00.WAV",
    "sound\\Terran\\bldg\\twpwht00.WAV",
    "sound\\Terran\\bldg\\OnFirLrg.wav",
    "sound\\Terran\\SCV\\TScTra00.WAV",
    "sound\\Terran\\RAYNORM\\URaDth00.WAV",
    "sound\\Terran\\RAYNORM\\URaDth01.WAV",
    "sound\\Terran\\RAYNORM\\URaPss00.WAV",
    "sound\\Terran\\RAYNORM\\URaPss01.WAV",
    "sound\\Terran\\RAYNORM\\URaPss02.WAV",
    "sound\\Terran\\RAYNORM\\URaPss03.WAV",
    "sound\\Terran\\RAYNORM\\URaWht00.WAV",
    "sound\\Terran\\RAYNORM\\URaWht01.WAV",
    "sound\\Terran\\RAYNORM\\URaWht02.WAV",
    "sound\\Terran\\RAYNORM\\URaWht03.WAV",
    "sound\\Terran\\RAYNORM\\URaYes00.WAV",
    "sound\\Terran\\RAYNORM\\URaYes01.WAV",
    "sound\\Terran\\RAYNORM\\URaYes02.WAV",
    "sound\\Terran\\RAYNORM\\URaYes03.WAV",
    "sound\\Terran\\RAYNORV\\URVPss00.WAV",
    "sound\\Terran\\RAYNORV\\URVPss01.WAV",
    "sound\\Terran\\RAYNORV\\URVPss02.WAV",
    "sound\\Terran\\RAYNORV\\URVPss03.WAV",
    "sound\\Terran\\RAYNORV\\URVWht00.WAV",
    "sound\\Terran\\RAYNORV\\URVWht01.WAV",
    "sound\\Terran\\RAYNORV\\URVWht02.WAV",
    "sound\\Terran\\RAYNORV\\URVWht03.WAV",
    "sound\\Terran\\RAYNORV\\URVYes00.WAV",
    "sound\\Terran\\RAYNORV\\URVYes01.WAV",
    "sound\\Terran\\RAYNORV\\URVYes02.WAV",
    "sound\\Terran\\RAYNORV\\URVYes03.WAV",
    "sound\\Terran\\DUKET\\UDTPss00.WAV",
    "sound\\Terran\\DUKET\\UDTPss01.WAV",
    "sound\\Terran\\DUKET\\UDTPss02.WAV",
    "sound\\Terran\\DUKET\\UDTPss03.WAV",
    "sound\\Terran\\DUKET\\UDTPss04.WAV",
    "sound\\Terran\\DUKET\\UDTWht00.WAV",
    "sound\\Terran\\DUKET\\UDTWht01.WAV",
    "sound\\Terran\\DUKET\\UDTWht02.WAV",
    "sound\\Terran\\DUKET\\UDTWht03.WAV",
    "sound\\Terran\\DUKET\\UDTYes00.WAV",
    "sound\\Terran\\DUKET\\UDTYes01.WAV",
    "sound\\Terran\\DUKET\\UDTYes02.WAV",
    "sound\\Terran\\DUKET\\UDTYes03.WAV",
    "sound\\Terran\\DUKEB\\UDupss00.wav",
    "sound\\Terran\\DUKEB\\UDupss01.wav",
    "sound\\Terran\\DUKEB\\UDupss02.wav",
    "sound\\Terran\\DUKEB\\UDupss03.wav",
    "sound\\Terran\\DUKEB\\UDupss04.wav",
    "sound\\Terran\\DUKEB\\UDuwht00.wav",
    "sound\\Terran\\DUKEB\\UDuwht01.wav",
    "sound\\Terran\\DUKEB\\UDuwht02.wav",
    "sound\\Terran\\DUKEB\\UDuwht03.wav",
    "sound\\Terran\\DUKEB\\UDuyes00.wav",
    "sound\\Terran\\DUKEB\\UDuyes01.wav",
    "sound\\Terran\\DUKEB\\UDuyes02.wav",
    "sound\\Terran\\DUKEB\\UDuyes03.wav",
    "sound\\Terran\\KERRIGAN\\UKePss00.wav",
    "sound\\Terran\\KERRIGAN\\UKePss01.wav",
    "sound\\Terran\\KERRIGAN\\UKePss02.wav",
    "sound\\Terran\\KERRIGAN\\UKePss03.wav",
    "sound\\Terran\\KERRIGAN\\UKePss04.wav",
    "sound\\Terran\\KERRIGAN\\UKeWht00.wav",
    "sound\\Terran\\KERRIGAN\\UKeWht01.wav",
    "sound\\Terran\\KERRIGAN\\UKeWht02.wav",
    "sound\\Terran\\KERRIGAN\\UKeWht03.wav",
    "sound\\Terran\\KERRIGAN\\UKeYes00.wav",
    "sound\\Terran\\KERRIGAN\\UKeYes01.wav",
    "sound\\Terran\\KERRIGAN\\UKeYes02.wav",
    "sound\\Terran\\KERRIGAN\\UKeYes03.wav",
    "sound\\Terran\\KERRIGAN\\UKeDth00.wav",
    "sound\\Misc\\LiftOff.WAV",
    "sound\\Misc\\Land.WAV",
    "sound\\Protoss\\Bldg\\pAcWht00.WAV",
    "sound\\Protoss\\Bldg\\pAsWht00.WAV",
    "sound\\Protoss\\Bldg\\pBaWht00.WAV",
    "sound\\Protoss\\Bldg\\pBeWht00.WAV",
    "sound\\Protoss\\Bldg\\pCiWht00.WAV",
    "sound\\Protoss\\Bldg\\pFoWht00.WAV",
    "sound\\Protoss\\Bldg\\pGaWht00.WAV",
    "sound\\Protoss\\Bldg\\pGcWht00.WAV",
    "sound\\Protoss\\Bldg\\PbaAct00.wav",
    "sound\\Misc\\Button.WAV",
    "sound\\Protoss\\Bldg\\pNaWht00.WAV",
    "sound\\Protoss\\Bldg\\pNeWht00.WAV",
    "sound\\Protoss\\Bldg\\pPBWht00.WAV",
    "sound\\Misc\\UTmWht00.WAV",
    "sound\\Protoss\\Bldg\\pPyWht00.WAV",
    "sound\\Protoss\\Bldg\\pRoWht00.WAV",
    "sound\\Misc\\UPiWht00.WAV",
    "sound\\Protoss\\Bldg\\PTrWht00.WAV",
    "sound\\Protoss\\Bldg\\pWaWht00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrRdy00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrDth00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrPss00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrPss01.WAV",
    "sound\\Protoss\\DRAGOON\\PDrPss02.WAV",
    "sound\\Protoss\\DRAGOON\\PDrPss03.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht01.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht02.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht03.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht04.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht05.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht06.WAV",
    "sound\\Protoss\\DRAGOON\\PDrWht07.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes00.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes01.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes02.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes03.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes04.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes05.WAV",
    "sound\\Protoss\\DRAGOON\\PDrYes06.WAV",
    "sound\\Protoss\\SHUTTLE\\PShRdy00.WAV",
    "sound\\Protoss\\SHUTTLE\\PShDth00.WAV",
    "sound\\Protoss\\SHUTTLE\\PShPss00.WAV",
    "sound\\Protoss\\SHUTTLE\\PShPss01.WAV",
    "sound\\Protoss\\SHUTTLE\\PShPss02.WAV",
    "sound\\Protoss\\SHUTTLE\\PShPss03.WAV",
    "sound\\Protoss\\SHUTTLE\\PShPss04.WAV",
    "sound\\Protoss\\SHUTTLE\\PShWht00.WAV",
    "sound\\Protoss\\SHUTTLE\\PShWht01.WAV",
    "sound\\Protoss\\SHUTTLE\\PShWht02.WAV",
    "sound\\Protoss\\SHUTTLE\\PShWht03.WAV",
    "sound\\Protoss\\SHUTTLE\\PShYes00.WAV",
    "sound\\Protoss\\SHUTTLE\\PShYes01.WAV",
    "sound\\Protoss\\SHUTTLE\\PShYes02.WAV",
    "sound\\Protoss\\SHUTTLE\\PShYes03.WAV",
    "sound\\Protoss\\SHUTTLE\\pshBld00.wav",
    "sound\\Protoss\\SHUTTLE\\pshBld03.wav",
    "sound\\Protoss\\SHUTTLE\\pshBld01.wav",
    "sound\\Protoss\\SHUTTLE\\pshBld02.wav",
    "sound\\Protoss\\SHUTTLE\\pshBld04.wav",
    "sound\\Protoss\\Scout\\PScDth00.WAV",
    "sound\\Protoss\\SCOUT\\PScRdy00.WAV",
    "sound\\Protoss\\Scout\\PScPss00.WAV",
    "sound\\Protoss\\Scout\\PScPss01.WAV",
    "sound\\Protoss\\Scout\\PScPss02.WAV",
    "sound\\Protoss\\Scout\\PScPss03.WAV",
    "sound\\Protoss\\Scout\\PScPss04.WAV",
    "sound\\Protoss\\Scout\\PScWht00.WAV",
    "sound\\Protoss\\Scout\\PScWht01.WAV",
    "sound\\Protoss\\Scout\\PScWht02.WAV",
    "sound\\Protoss\\Scout\\PScWht03.WAV",
    "sound\\Protoss\\Scout\\PScYes00.WAV",
    "sound\\Protoss\\Scout\\PScYes01.WAV",
    "sound\\Protoss\\Scout\\PScYes02.WAV",
    "sound\\Protoss\\Scout\\PScYes03.WAV",
    "sound\\Protoss\\ARBITER\\PAbCag00.WAV",
    "sound\\Protoss\\ARBITER\\PAbRdy00.WAV",
    "sound\\Protoss\\ARBITER\\PAbFol00.WAV",
    "sound\\Protoss\\ARBITER\\PAbFol01.WAV",
    "sound\\Protoss\\ARBITER\\PAbFol02.WAV",
    "sound\\Protoss\\ARBITER\\PAbFol03.WAV",
    "sound\\Protoss\\ARBITER\\PAbPss00.WAV",
    "sound\\Protoss\\ARBITER\\PAbPss01.WAV",
    "sound\\Protoss\\ARBITER\\PAbPss02.WAV",
    "sound\\Protoss\\ARBITER\\PAbPss03.WAV",
    "sound\\Protoss\\ARBITER\\PAbPss04.WAV",
    "sound\\Protoss\\ARBITER\\PAbWht00.WAV",
    "sound\\Protoss\\ARBITER\\PAbWht01.WAV",
    "sound\\Protoss\\ARBITER\\PAbWht02.WAV",
    "sound\\Protoss\\ARBITER\\PAbWht03.WAV",
    "sound\\Protoss\\ARBITER\\PAbYes00.WAV",
    "sound\\Protoss\\ARBITER\\PAbYes01.WAV",
    "sound\\Protoss\\ARBITER\\PAbYes02.WAV",
    "sound\\Protoss\\ARBITER\\PAbDth00.WAV",
    "sound\\Protoss\\ARCHON\\PArRdy00.WAV",
    "sound\\Protoss\\ARCHON\\PArMin00.WAV",
    "sound\\Protoss\\ARCHON\\PArPss00.WAV",
    "sound\\Protoss\\ARCHON\\PArPss01.WAV",
    "sound\\Protoss\\ARCHON\\PArPss02.WAV",
    "sound\\Protoss\\ARCHON\\PArPss03.WAV",
    "sound\\Protoss\\ARCHON\\PArWht00.WAV",
    "sound\\Protoss\\ARCHON\\PArWht01.WAV",
    "sound\\Protoss\\ARCHON\\PArWht02.WAV",
    "sound\\Protoss\\ARCHON\\PArWht03.WAV",
    "sound\\Protoss\\ARCHON\\PArYes00.WAV",
    "sound\\Protoss\\ARCHON\\PArYes01.WAV",
    "sound\\Protoss\\ARCHON\\PArYes02.WAV",
    "sound\\Protoss\\ARCHON\\PArYes03.WAV",
    "sound\\Protoss\\ARCHON\\PArDth00.WAV",
    "sound\\Protoss\\CARRIER\\PCaRdy00.WAV",
    "sound\\Protoss\\Carrier\\PCaPss00.WAV",
    "sound\\Protoss\\Carrier\\PCaPss01.WAV",
    "sound\\Protoss\\Carrier\\PCaPss02.WAV",
    "sound\\Protoss\\Carrier\\PCaPss03.WAV",
    "sound\\Protoss\\Carrier\\PCaWht00.WAV",
    "sound\\Protoss\\Carrier\\PCaWht01.WAV",
    "sound\\Protoss\\Carrier\\PCaWht02.WAV",
    "sound\\Protoss\\Carrier\\PCaWht03.WAV",
    "sound\\Protoss\\Carrier\\PCaYes00.WAV",
    "sound\\Protoss\\Carrier\\PCaYes01.WAV",
    "sound\\Protoss\\Carrier\\PCaYes02.WAV",
    "sound\\Protoss\\Carrier\\PCaYes03.WAV",
    "sound\\Protoss\\Carrier\\PCaDth00.WAV",
    "sound\\Protoss\\Carrier\\PCaDth01.WAV",
    "sound\\Protoss\\PROBE\\PPrRdy00.WAV",
    "sound\\Protoss\\PROBE\\PPrDth00.WAV",
    "sound\\Protoss\\PROBE\\PPrAtt00.WAV",
    "sound\\Protoss\\PROBE\\PPrAtt01.WAV",
    "sound\\Protoss\\PROBE\\PPrMin00.WAV",
    "sound\\Protoss\\PROBE\\PPrPss00.WAV",
    "sound\\Protoss\\PROBE\\PPrPss01.WAV",
    "sound\\Protoss\\PROBE\\PPrPss02.WAV",
    "sound\\Protoss\\PROBE\\PPrPss03.WAV",
    "sound\\Protoss\\PROBE\\PPrWht00.WAV",
    "sound\\Protoss\\PROBE\\PPrWht01.WAV",
    "sound\\Protoss\\PROBE\\PPrWht02.WAV",
    "sound\\Protoss\\PROBE\\PPrWht03.WAV",
    "sound\\Protoss\\PROBE\\PPrYes00.WAV",
    "sound\\Protoss\\PROBE\\PPrYes01.WAV",
    "sound\\Protoss\\PROBE\\PPrYes02.WAV",
    "sound\\Protoss\\PROBE\\PPrYes03.WAV",
    "sound\\Protoss\\INTERCEP\\PInLau00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeSum00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeHal00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeHal01.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeSto00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeSto01.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeRdy00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTePss00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTePss01.WAV",
    "sound\\Protoss\\TEMPLAR\\PTePss02.WAV",
    "sound\\Protoss\\TEMPLAR\\PTePss03.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeWht00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeWht01.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeWht02.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeWht03.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeYes00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeYes01.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeYes02.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeYes03.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeDth00.WAV",
    "sound\\Protoss\\TEMPLAR\\PTeMov00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrRdy00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrDth00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrPss00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrPss01.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrPss02.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrWht00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrWht01.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrWht02.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrWht03.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrYes00.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrYes01.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrYes02.WAV",
    "sound\\Protoss\\TRILOBYTE\\PTrYes03.WAV",
    "sound\\Protoss\\Witness\\PWiRdy00.WAV",
    "sound\\Protoss\\Witness\\PWiDth00.WAV",
    "sound\\Protoss\\Witness\\PWiDth01.WAV",
    "sound\\Protoss\\Witness\\PWiPss00.WAV",
    "sound\\Protoss\\Witness\\PWiPss01.WAV",
    "sound\\Protoss\\Witness\\PWiPss02.WAV",
    "sound\\Protoss\\Witness\\PWiPss03.WAV",
    "sound\\Protoss\\Witness\\PWiPss04.WAV",
    "sound\\Protoss\\Witness\\PWiWht00.WAV",
    "sound\\Protoss\\Witness\\PWiWht01.WAV",
    "sound\\Protoss\\Witness\\PWiYes00.WAV",
    "sound\\Protoss\\Witness\\PWiYes01.WAV",
    "sound\\Protoss\\ZEALOT\\pzeAtt00.WAV",
    "sound\\Protoss\\ZEALOT\\pzeAtt01.WAV",
    "sound\\Protoss\\ZEALOT\\pzeHit00.WAV",
    "sound\\Protoss\\ZEALOT\\PZeRag00.wav",
    "sound\\Protoss\\ZEALOT\\PZeRdy00.WAV",
    "sound\\Protoss\\ZEALOT\\PZePss00.WAV",
    "sound\\Protoss\\ZEALOT\\PZePss01.WAV",
    "sound\\Protoss\\ZEALOT\\PZePss02.WAV",
    "sound\\Protoss\\ZEALOT\\PZeWht00.WAV",
    "sound\\Protoss\\ZEALOT\\PZeWht01.WAV",
    "sound\\Protoss\\ZEALOT\\PZeWht02.WAV",
    "sound\\Protoss\\ZEALOT\\PZeWht03.WAV",
    "sound\\Protoss\\ZEALOT\\PZeYes00.WAV",
    "sound\\Protoss\\ZEALOT\\PZeYes01.WAV",
    "sound\\Protoss\\ZEALOT\\PZeYes02.WAV",
    "sound\\Protoss\\ZEALOT\\PZeYes03.WAV",
    "sound\\Protoss\\ZEALOT\\PZeDth00.WAV",
    "sound\\Protoss\\FENIXD\\UFdPss00.WAV",
    "sound\\Protoss\\FENIXD\\UFdPss01.WAV",
    "sound\\Protoss\\FENIXD\\UFdPss02.WAV",
    "sound\\Protoss\\FENIXD\\UFdPss03.WAV",
    "sound\\Protoss\\FENIXD\\UFdWht00.WAV",
    "sound\\Protoss\\FENIXD\\UFdWht01.WAV",
    "sound\\Protoss\\FENIXD\\UFdWht02.WAV",
    "sound\\Protoss\\FENIXD\\UFdWht03.WAV",
    "sound\\Protoss\\FENIXD\\UFdYes00.WAV",
    "sound\\Protoss\\FENIXD\\UFdYes01.WAV",
    "sound\\Protoss\\FENIXD\\UFdYes02.WAV",
    "sound\\Protoss\\FENIXD\\UFdYes03.WAV",
    "sound\\Protoss\\FENIXZ\\UFePss00.WAV",
    "sound\\Protoss\\FENIXZ\\UFePss01.WAV",
    "sound\\Protoss\\FENIXZ\\UFePss02.WAV",
    "sound\\Protoss\\FENIXZ\\UFePss03.WAV",
    "sound\\Protoss\\FENIXZ\\UFeWht00.WAV",
    "sound\\Protoss\\FENIXZ\\UFeWht01.WAV",
    "sound\\Protoss\\FENIXZ\\UFeWht02.WAV",
    "sound\\Protoss\\FENIXZ\\UFeWht03.WAV",
    "sound\\Protoss\\FENIXZ\\UFeYes00.WAV",
    "sound\\Protoss\\FENIXZ\\UFeYes01.WAV",
    "sound\\Protoss\\FENIXZ\\UFeYes02.WAV",
    "sound\\Protoss\\FENIXZ\\UFeYes03.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCPss00.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCPss01.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCPss02.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCPss03.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCWht00.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCWht01.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCWht02.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCWht03.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCYes00.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCYes01.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCYes02.WAV",
    "sound\\Protoss\\GANTRITHOR\\UTCYes03.WAV",
    "sound\\Protoss\\TASSADAR\\UTaPss00.WAV",
    "sound\\Protoss\\TASSADAR\\UTaPss01.WAV",
    "sound\\Protoss\\TASSADAR\\UTaPss02.WAV",
    "sound\\Protoss\\TASSADAR\\UTaPss03.WAV",
    "sound\\Protoss\\TASSADAR\\UTaWht00.WAV",
    "sound\\Protoss\\TASSADAR\\UTaWht01.WAV",
    "sound\\Protoss\\TASSADAR\\UTaWht02.WAV",
    "sound\\Protoss\\TASSADAR\\UTaWht03.WAV",
    "sound\\Protoss\\TASSADAR\\UTaYes00.WAV",
    "sound\\Protoss\\TASSADAR\\UTaYes01.WAV",
    "sound\\Protoss\\TASSADAR\\UTaYes02.WAV",
    "sound\\Protoss\\TASSADAR\\UTaYes03.WAV",
    "sound\\Protoss\\TASSADAR\\UTaDth00.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTRdy00.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTPss00.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTPss01.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTPss02.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTPss03.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTWht00.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTWht01.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTWht02.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTWht03.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTYes00.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTYes01.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTYes02.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTYes03.WAV",
    "sound\\Protoss\\DARKTEMPLAR\\PDTDth00.WAV",
    "sound\\Protoss\\ZERATUL\\UZePss00.WAV",
    "sound\\Protoss\\ZERATUL\\UZePss01.WAV",
    "sound\\Protoss\\ZERATUL\\UZePss02.WAV",
    "sound\\Protoss\\ZERATUL\\UZePss03.WAV",
    "sound\\Protoss\\ZERATUL\\UZeWht00.WAV",
    "sound\\Protoss\\ZERATUL\\UZeWht01.WAV",
    "sound\\Protoss\\ZERATUL\\UZeWht02.WAV",
    "sound\\Protoss\\ZERATUL\\UZeWht03.WAV",
    "sound\\Protoss\\ZERATUL\\UZeYes00.WAV",
    "sound\\Protoss\\ZERATUL\\UZeYes01.WAV",
    "sound\\Protoss\\ZERATUL\\UZeYes02.WAV",
    "sound\\Protoss\\ZERATUL\\UZeYes03.WAV",
    "sound\\Protoss\\ZERATUL\\UZeDth00.WAV",
    "sound\\Zerg\\Bldg\\ZChWht00.WAV",
    "sound\\Zerg\\Bldg\\ZCbWht00.WAV",
    "sound\\Zerg\\Bldg\\ZEvWht00.WAV",
    "sound\\Zerg\\Bldg\\ZFcWht00.WAV",
    "sound\\Zerg\\Bldg\\ZHaWht00.WAV",
    "sound\\Zerg\\Bldg\\ZHiWht00.WAV",
    "sound\\Zerg\\Bldg\\ZIgWht00.WAV",
    "sound\\Zerg\\Bldg\\ZLrWht00.WAV",
    "sound\\Zerg\\Bldg\\ZLuWht00.WAV",
    "sound\\Zerg\\Bldg\\ZMcWht00.WAV",
    "sound\\Zerg\\Bldg\\ZMhWht00.WAV",
    "sound\\Zerg\\Bldg\\ZNeWht00.WAV",
    "sound\\Zerg\\Bldg\\ZNyWht00.WAV",
    "sound\\Zerg\\Bldg\\ZO1Wht00.WAV",
    "sound\\Zerg\\Bldg\\ZRcWht00.WAV",
    "sound\\Zerg\\Bldg\\ZSbWht00.WAV",
    "sound\\Zerg\\Bldg\\ZScWht00.WAV",
    "sound\\Zerg\\Bldg\\ZSpWht00.WAV",
    "sound\\Zerg\\Bldg\\ZBldgDth.WAV",
    "sound\\Zerg\\AVENGER\\ZAvRdy00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvDth00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvAtt00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvHit00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvPss00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvPss01.WAV",
    "sound\\Zerg\\AVENGER\\ZAvWht00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvWht01.WAV",
    "sound\\Zerg\\AVENGER\\ZAvYes00.WAV",
    "sound\\Zerg\\AVENGER\\ZAvYes01.WAV",
    "sound\\Zerg\\BROODLING\\ZBrDth00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrAtt00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrRdy00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrPss00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrPss01.WAV",
    "sound\\Zerg\\BROODLING\\ZBrPss02.WAV",
    "sound\\Zerg\\BROODLING\\ZBrPss03.WAV",
    "sound\\Zerg\\BROODLING\\ZBrWht00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrWht01.WAV",
    "sound\\Zerg\\BROODLING\\ZBrWht02.WAV",
    "sound\\Zerg\\BROODLING\\ZBrWht03.WAV",
    "sound\\Zerg\\BROODLING\\ZBrYes00.WAV",
    "sound\\Zerg\\BROODLING\\ZBrYes01.WAV",
    "sound\\Zerg\\BROODLING\\ZBrYes02.WAV",
    "sound\\Zerg\\BUGGUY\\ZBGRdy00.wav",
    "sound\\Zerg\\BUGGUY\\ZBGPss00.wav",
    "sound\\Zerg\\BUGGUY\\ZBGPss01.wav",
    "sound\\Zerg\\BUGGUY\\ZBGPss02.wav",
    "sound\\Zerg\\BUGGUY\\ZBGPss03.wav",
    "sound\\Zerg\\BUGGUY\\ZBGWht00.wav",
    "sound\\Zerg\\BUGGUY\\ZBGWht01.wav",
    "sound\\Zerg\\BUGGUY\\ZBGWht02.wav",
    "sound\\Zerg\\BUGGUY\\ZBGWht03.wav",
    "sound\\Zerg\\BUGGUY\\ZBGYes00.wav",
    "sound\\Zerg\\BUGGUY\\ZBGYes01.wav",
    "sound\\Zerg\\BUGGUY\\ZBGYes02.wav",
    "sound\\Zerg\\BUGGUY\\ZBGYes03.wav",
    "sound\\Zerg\\DEFILER\\ZDeCon00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeRdy00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeDth00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeBlo00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeBlo01.WAV",
    "sound\\Zerg\\DEFILER\\ZDePss00.WAV",
    "sound\\Zerg\\DEFILER\\ZDePss01.WAV",
    "sound\\Zerg\\DEFILER\\ZDePss02.WAV",
    "sound\\Zerg\\DEFILER\\ZDeWht00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeWht01.WAV",
    "sound\\Zerg\\DEFILER\\ZDeWht02.WAV",
    "sound\\Zerg\\DEFILER\\ZDeYes00.WAV",
    "sound\\Zerg\\DEFILER\\ZDeYes01.WAV",
    "sound\\Zerg\\DEFILER\\ZDeYes02.WAV",
    "sound\\Zerg\\Egg\\ZEgRdy00.WAV",
    "sound\\Zerg\\Egg\\ZEgWht00.WAV",
    "sound\\Zerg\\Egg\\ZEgWht01.WAV",
    "sound\\Zerg\\Egg\\ZEgDth00.WAV",
    "sound\\Zerg\\Egg\\ZEgPss00.WAV",
    "sound\\Zerg\\DRONE\\ZDrRdy00.WAV",
    "sound\\Zerg\\DRONE\\ZDrDth00.WAV",
    "sound\\Zerg\\DRONE\\ZDrPss00.WAV",
    "sound\\Zerg\\DRONE\\ZDrPss01.WAV",
    "sound\\Zerg\\DRONE\\ZDrPss02.WAV",
    "sound\\Zerg\\DRONE\\ZDrWht00.WAV",
    "sound\\Zerg\\DRONE\\ZDrWht01.WAV",
    "sound\\Zerg\\DRONE\\ZDrWht02.WAV",
    "sound\\Zerg\\DRONE\\ZDrWht03.WAV",
    "sound\\Zerg\\DRONE\\ZDrWht04.WAV",
    "sound\\Zerg\\DRONE\\ZDrYes00.WAV",
    "sound\\Zerg\\DRONE\\ZDrYes01.WAV",
    "sound\\Zerg\\DRONE\\ZDrYes02.WAV",
    "sound\\Zerg\\DRONE\\ZDrYes03.WAV",
    "sound\\Zerg\\DRONE\\ZDrYes04.WAV",
    "sound\\Zerg\\DRONE\\ZDrMin00.wav",
    "sound\\Zerg\\Larva\\ZLaDth00.WAV",
    "sound\\Zerg\\Larva\\ZLaPss00.WAV",
    "sound\\Zerg\\Larva\\ZLaWht00.WAV",
    "sound\\Zerg\\Guardian\\ZGuDth00.WAV",
    "sound\\Zerg\\Guardian\\ZGuPss00.WAV",
    "sound\\Zerg\\Guardian\\ZGuPss01.WAV",
    "sound\\Zerg\\Guardian\\ZGuPss02.WAV",
    "sound\\Zerg\\Guardian\\ZGuPss03.WAV",
    "sound\\Zerg\\Guardian\\ZGuRdy00.WAV",
    "sound\\Zerg\\Guardian\\ZGuWht00.WAV",
    "sound\\Zerg\\Guardian\\ZGuWht01.WAV",
    "sound\\Zerg\\Guardian\\ZGuWht02.WAV",
    "sound\\Zerg\\Guardian\\ZGuWht03.WAV",
    "sound\\Zerg\\Guardian\\ZGuYes00.WAV",
    "sound\\Zerg\\Guardian\\ZGuYes01.WAV",
    "sound\\Zerg\\Guardian\\ZGuYes02.WAV",
    "sound\\Zerg\\Guardian\\ZGuYes03.WAV",
    "sound\\Zerg\\Hydra\\ZHyRdy00.WAV",
    "sound\\Zerg\\Hydra\\ZHyDth00.WAV",
    "sound\\Zerg\\Hydra\\ZHyPss00.WAV",
    "sound\\Zerg\\Hydra\\ZHyPss01.WAV",
    "sound\\Zerg\\Hydra\\ZHyWht00.WAV",
    "sound\\Zerg\\Hydra\\ZHyWht01.WAV",
    "sound\\Zerg\\Hydra\\ZHyWht02.WAV",
    "sound\\Zerg\\Hydra\\ZHyYes00.WAV",
    "sound\\Zerg\\Hydra\\ZHyYes01.WAV",
    "sound\\Zerg\\Hydra\\ZHyYes02.WAV",
    "sound\\Zerg\\Hydra\\ZHyYes03.WAV",
    "sound\\Zerg\\Ultra\\ZUlRor00.WAV",
    "sound\\Zerg\\Ultra\\ZUlDth00.WAV",
    "sound\\Zerg\\Ultra\\ZUlPss00.WAV",
    "sound\\Zerg\\Ultra\\ZUlPss01.WAV",
    "sound\\Zerg\\Ultra\\ZUlPss02.WAV",
    "sound\\Zerg\\Ultra\\ZUlRdy00.WAV",
    "sound\\Zerg\\Ultra\\ZUlWht00.WAV",
    "sound\\Zerg\\Ultra\\ZUlWht01.WAV",
    "sound\\Zerg\\Ultra\\ZUlWht02.WAV",
    "sound\\Zerg\\Ultra\\ZUlWht03.WAV",
    "sound\\Zerg\\Ultra\\ZUlYes00.WAV",
    "sound\\Zerg\\Ultra\\ZUlYes01.WAV",
    "sound\\Zerg\\Ultra\\ZUlYes02.WAV",
    "sound\\Zerg\\Ultra\\ZUlYes03.WAV",
    "sound\\Zerg\\Ultra\\zulAtt00.WAV",
    "sound\\Zerg\\Ultra\\zulAtt01.WAV",
    "sound\\Zerg\\Ultra\\zulAtt02.WAV",
    "sound\\Zerg\\Ultra\\zulHit00.WAV",
    "sound\\Zerg\\Ultra\\zulHit01.WAV",
    "sound\\Zerg\\Zergling\\ZZeDth00.WAV",
    "sound\\Zerg\\Zergling\\ZZePss00.WAV",
    "sound\\Zerg\\Zergling\\ZZePss01.WAV",
    "sound\\Zerg\\Zergling\\ZZePss02.WAV",
    "sound\\Zerg\\Zergling\\ZZeRdy00.WAV",
    "sound\\Zerg\\Zergling\\ZZeWht00.WAV",
    "sound\\Zerg\\Zergling\\ZZeWht01.WAV",
    "sound\\Zerg\\Zergling\\ZZeWht02.WAV",
    "sound\\Zerg\\Zergling\\ZZeWht03.WAV",
    "sound\\Zerg\\Zergling\\ZZeYes00.WAV",
    "sound\\Zerg\\Zergling\\ZZeYes01.WAV",
    "sound\\Zerg\\Zergling\\ZZeYes02.WAV",
    "sound\\Zerg\\Zergling\\ZZeYes03.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvRdy00.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvDth00.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvPss00.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvWht00.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvWht01.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvWht02.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvWht03.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvYes00.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvYes01.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvYes02.WAV",
    "sound\\Zerg\\OVERLORD\\ZOvYes03.WAV",
    "sound\\Zerg\\Queen\\ZQuTag00.WAV",
    "sound\\Zerg\\Queen\\ZQuTag01.WAV",
    "sound\\Zerg\\Queen\\ZQuSwm00.WAV",
    "sound\\Zerg\\Queen\\ZQuSwm01.WAV",
    "sound\\Zerg\\Queen\\ZQuEns00.WAV",
    "sound\\Zerg\\Queen\\ZQuDth00.WAV",
    "sound\\Zerg\\Queen\\ZQuDth01.WAV",
    "sound\\Zerg\\Queen\\ZQuDth02.WAV",
    "sound\\Zerg\\Queen\\ZQuRdy00.WAV",
    "sound\\Zerg\\Queen\\ZQuPss00.WAV",
    "sound\\Zerg\\Queen\\ZQuPss01.WAV",
    "sound\\Zerg\\Queen\\ZQuPss02.WAV",
    "sound\\Zerg\\Queen\\ZQuPss03.WAV",
    "sound\\Zerg\\Queen\\ZQuWht00.WAV",
    "sound\\Zerg\\Queen\\ZQuWht01.WAV",
    "sound\\Zerg\\Queen\\ZQuWht02.WAV",
    "sound\\Zerg\\Queen\\ZQuWht03.WAV",
    "sound\\Zerg\\Queen\\ZQuYes00.WAV",
    "sound\\Zerg\\Queen\\ZQuYes01.WAV",
    "sound\\Zerg\\Queen\\ZQuYes02.WAV",
    "sound\\Zerg\\Queen\\ZQuYes03.WAV",
    "sound\\Zerg\\Mutalid\\ZMuRdy00.WAV",
    "sound\\Zerg\\Mutalid\\ZMuDth00.WAV",
    "sound\\Zerg\\Mutalid\\ZMuPss00.WAV",
    "sound\\Zerg\\Mutalid\\ZMuPss01.WAV",
    "sound\\Zerg\\Mutalid\\ZMuPss02.WAV",
    "sound\\Zerg\\Mutalid\\ZMuPss03.WAV",
    "sound\\Zerg\\Mutalid\\ZMuWht00.WAV",
    "sound\\Zerg\\Mutalid\\ZMuWht01.WAV",
    "sound\\Zerg\\Mutalid\\ZMuWht02.WAV",
    "sound\\Zerg\\Mutalid\\ZMuWht03.WAV",
    "sound\\Zerg\\Mutalid\\ZMuYes00.WAV",
    "sound\\Zerg\\Mutalid\\ZMuYes01.WAV",
    "sound\\Zerg\\Mutalid\\ZMuYes02.WAV",
    "sound\\Zerg\\Mutalid\\ZMuYes03.WAV",
    "sound\\Zerg\\ZERGKERRI\\UKiPss00.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiPss01.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiPss02.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiPss03.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiWht00.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiWht01.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiWht02.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiWht03.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiYes00.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiYes01.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiYes02.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiYes03.wav",
    "sound\\Zerg\\ZERGKERRI\\UKiDth00.wav",
    "sound\\Misc\\CRITTERS\\PbWht01.wav",
    "sound\\Misc\\CRITTERS\\PbWht02.wav",
    "sound\\Misc\\CRITTERS\\PbWht03.wav",
    "sound\\Misc\\CRITTERS\\PBDeath01.wav",
    "sound\\Misc\\CRITTERS\\ScWht01.wav",
    "sound\\Misc\\CRITTERS\\ScWht02.wav",
    "sound\\Misc\\CRITTERS\\ScWht03.wav",
    "sound\\Misc\\CRITTERS\\ScDeath01.wav",
    "sound\\Misc\\CRITTERS\\TerWht01.wav",
    "sound\\Misc\\CRITTERS\\TerWht02.wav",
    "sound\\Misc\\CRITTERS\\TerWht03.wav",
    "sound\\Misc\\CRITTERS\\TerDeath01.wav",
    "sound\\Terran\\Duran\\TDnPss00.wav",
    "sound\\Terran\\Duran\\TDnPss01.wav",
    "sound\\Terran\\Duran\\TDnPss02.wav",
    "sound\\Terran\\Duran\\TDnPss03.wav",
    "sound\\Terran\\Duran\\TDnPss04.wav",
    "sound\\Terran\\Duran\\TDnPss05.wav",
    "sound\\Terran\\Duran\\TDnPss06.wav",
    "sound\\Terran\\Duran\\TDnPss07.wav",
    "sound\\Terran\\Duran\\TDnPss08.wav",
    "sound\\Terran\\Duran\\TDnWht00.wav",
    "sound\\Terran\\Duran\\TDnWht01.wav",
    "sound\\Terran\\Duran\\TDnWht02.wav",
    "sound\\Terran\\Duran\\TDnWht03.wav",
    "sound\\Terran\\Duran\\TDnYes00.wav",
    "sound\\Terran\\Duran\\TDnYes01.wav",
    "sound\\Terran\\Duran\\TDnYes02.wav",
    "sound\\Terran\\Duran\\TDnYes03.wav",
    "sound\\Terran\\Duran\\TDnDth00.wav",
    "sound\\Terran\\Medic\\TmedCure.wav",
    "sound\\Terran\\Medic\\TMdRdy00.wav",
    "sound\\Terran\\Medic\\TMdDth00.wav",
    "sound\\Terran\\Medic\\TMdWht00.wav",
    "sound\\Terran\\Medic\\TMdWht01.WAV",
    "sound\\Terran\\Medic\\TMdWht02.WAV",
    "sound\\Terran\\Medic\\TMdWht03.wav",
    "sound\\Terran\\Medic\\TMdYes00.wav",
    "sound\\Terran\\Medic\\TMdYes01.wav",
    "sound\\Terran\\Medic\\TMdYes02.wav",
    "sound\\Terran\\Medic\\TMdYes03.wav",
    "sound\\Terran\\Medic\\TMdPss00.wav",
    "sound\\Terran\\Medic\\TMdPss01.WAV",
    "sound\\Terran\\Medic\\TMdPss02.WAV",
    "sound\\Terran\\Medic\\TMdPss03.WAV",
    "sound\\Terran\\Medic\\TMdPss04.WAV",
    "sound\\Terran\\Medic\\TMdPss05.WAV",
    "sound\\Terran\\Medic\\TMdPss06.wav",
    "sound\\Terran\\Medic\\TMedflsh.wav",
    "sound\\Terran\\Medic\\TMedRest.wav",
    "sound\\Terran\\Medic\\TMedHeal.wav",
    "sound\\Terran\\Medic\\Miopia1.wav",
    "sound\\Terran\\Frigate\\AfterOn.wav",
    "sound\\Terran\\Frigate\\AfterOff.wav",
    "sound\\Bullet\\Tfrhit.wav",
    "sound\\Bullet\\Tfrshoot.wav",
    "sound\\Terran\\FRIGATE\\TVkRdy00.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss00.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss01.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss02.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss03.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss04.WAV",
    "sound\\Terran\\FRIGATE\\TVkPss05.WAV",
    "sound\\Terran\\FRIGATE\\TVkWht00.WAV",
    "sound\\Terran\\FRIGATE\\TVkWht01.WAV",
    "sound\\Terran\\FRIGATE\\TVkWht02.WAV",
    "sound\\Terran\\FRIGATE\\TVkWht03.WAV",
    "sound\\Terran\\FRIGATE\\TVkYes00.WAV",
    "sound\\Terran\\FRIGATE\\TVkYes01.WAV",
    "sound\\Terran\\FRIGATE\\TVkYes02.WAV",
    "sound\\Terran\\FRIGATE\\TVkYes03.WAV",
    "sound\\Terran\\FRIGATE\\TVkYes04.WAV",
    "sound\\Terran\\FRIGATE\\TVkDth00.WAV",
    "sound\\Protoss\\Corsair\\PCoRdy00.wav",
    "sound\\Protoss\\Corsair\\PCorhit1.wav",
    "sound\\Protoss\\Corsair\\PCorhit2.wav",
    "sound\\Protoss\\Corsair\\PCoWht00.wav",
    "sound\\Protoss\\Corsair\\PCoWht01.WAV",
    "sound\\Protoss\\Corsair\\PCoWht02.WAV",
    "sound\\Protoss\\Corsair\\PCoWht03.wav",
    "sound\\Protoss\\Corsair\\PCoYes00.wav",
    "sound\\Protoss\\Corsair\\PCoYes01.WAV",
    "sound\\Protoss\\Corsair\\PCoYes02.WAV",
    "sound\\Protoss\\Corsair\\PCoYes03.wav",
    "sound\\Protoss\\Corsair\\PCoPss00.wav",
    "sound\\Protoss\\Corsair\\PCoPss01.WAV",
    "sound\\Protoss\\Corsair\\PCoPss02.WAV",
    "sound\\Protoss\\Corsair\\PCoPss03.WAV",
    "sound\\Protoss\\Corsair\\PCoPss04.WAV",
    "sound\\Protoss\\Corsair\\PCoPss05.WAV",
    "sound\\Protoss\\Corsair\\PCoPss06.wav",
    "sound\\Protoss\\Corsair\\PCorWeb1.wav",
    "sound\\Protoss\\Corsair\\PCorlasr2.wav",
    "sound\\Protoss\\Darchon\\feedback.wav",
    "sound\\Protoss\\Darchon\\mind.wav",
    "sound\\Protoss\\Darchon\\ParaAttk.wav",
    "sound\\Protoss\\Darchon\\Parahit.wav",
    "sound\\Protoss\\DARCHON\\PDaRdy00.WAV",
    "sound\\Protoss\\DARCHON\\PDaPss00.WAV",
    "sound\\Protoss\\DARCHON\\PDaPss01.WAV",
    "sound\\Protoss\\DARCHON\\PDaPss02.WAV",
    "sound\\Protoss\\DARCHON\\PDaPss03.WAV",
    "sound\\Protoss\\DARCHON\\PDaWht00.WAV",
    "sound\\Protoss\\DARCHON\\PDaWht01.WAV",
    "sound\\Protoss\\DARCHON\\PDaWht02.WAV",
    "sound\\Protoss\\DARCHON\\PDaWht03.WAV",
    "sound\\Protoss\\DARCHON\\PDaYes00.WAV",
    "sound\\Protoss\\DARCHON\\PDaYes01.WAV",
    "sound\\Protoss\\DARCHON\\PDaYes02.WAV",
    "sound\\Protoss\\DARCHON\\PDaYes03.WAV",
    "sound\\Zerg\\LURKER\\ZLuRdy00.WAV",
    "sound\\Zerg\\LURKER\\ZLuDth00.WAV",
    "sound\\Zerg\\LURKER\\ZLuPss00.WAV",
    "sound\\Zerg\\Lurker\\ZLuPss01.WAV",
    "sound\\Zerg\\Lurker\\ZLuPss02.WAV",
    "sound\\Zerg\\LURKER\\ZLuPss03.WAV",
    "sound\\Zerg\\LURKER\\ZLuWht00.WAV",
    "sound\\Zerg\\LURKER\\ZLuWht01.WAV",
    "sound\\Zerg\\LURKER\\ZLuWht02.WAV",
    "sound\\Zerg\\LURKER\\ZLuWht03.WAV",
    "sound\\Zerg\\LURKER\\ZLuYes00.WAV",
    "sound\\Zerg\\LURKER\\ZLuYes01.WAV",
    "sound\\Zerg\\LURKER\\ZLuYes02.WAV",
    "sound\\Zerg\\LURKER\\ZLuYes03.WAV",
    "sound\\Zerg\\LURKER\\ZLuBurrw.wav",
    "sound\\Zerg\\Devourer\\firesuck.wav",
    "sound\\Zerg\\Devourer\\goophit.wav",
    "sound\\Zerg\\Devourer\\ZDvRdy00.WAV",
    "sound\\Zerg\\Devourer\\ZDvDth00.WAV",
    "sound\\Zerg\\Devourer\\ZDvPss00.WAV",
    "sound\\Zerg\\Devourer\\ZDvPss01.WAV",
    "sound\\Zerg\\Devourer\\ZDvPss02.WAV",
    "sound\\Zerg\\Devourer\\ZDvWht00.WAV",
    "sound\\Zerg\\Devourer\\ZDvWht01.WAV",
    "sound\\Zerg\\Devourer\\ZDvWht02.WAV",
    "sound\\Zerg\\Devourer\\ZDvYes00.WAV",
    "sound\\Zerg\\Devourer\\ZDvYes01.WAV",
    "sound\\Zerg\\Devourer\\ZDvYes02.WAV",
    "sound\\Zerg\\Devourer\\ZDvYes03.WAV",
    "sound\\Bullet\\ZLrkFir1.wav",
    "sound\\Bullet\\ZLrkFir2.wav",
    "sound\\Bullet\\ZLrkHit1.wav",
    "sound\\Bullet\\ZLrkHit2.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss00.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss01.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss02.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss03.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss04.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss05.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss06.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss07.wav",
    "sound\\Zerg\\ZergDuran\\ZDnPss08.wav",
    "sound\\Zerg\\ZergDuran\\ZDnWht00.wav",
    "sound\\Zerg\\ZergDuran\\ZDnWht01.wav",
    "sound\\Zerg\\ZergDuran\\ZDnWht02.wav",
    "sound\\Zerg\\ZergDuran\\ZDnWht03.wav",
    "sound\\Zerg\\ZergDuran\\ZDnYes00.wav",
    "sound\\Zerg\\ZergDuran\\ZDnYes01.wav",
    "sound\\Zerg\\ZergDuran\\ZDnYes02.wav",
    "sound\\Zerg\\ZergDuran\\ZDnYes03.wav",
    "sound\\Zerg\\ZergDuran\\ZDnDth00.wav",
    "sound\\Protoss\\Artanis\\PAtPss00.wav",
    "sound\\Protoss\\Artanis\\PAtPss01.wav",
    "sound\\Protoss\\Artanis\\PAtPss02.wav",
    "sound\\Protoss\\Artanis\\PAtPss03.wav",
    "sound\\Protoss\\Artanis\\PAtPss04.wav",
    "sound\\Protoss\\Artanis\\PAtPss05.wav",
    "sound\\Protoss\\Artanis\\PAtWht00.wav",
    "sound\\Protoss\\Artanis\\PAtWht01.wav",
    "sound\\Protoss\\Artanis\\PAtWht02.wav",
    "sound\\Protoss\\Artanis\\PAtWht03.wav",
    "sound\\Protoss\\Artanis\\PAtYes00.wav",
    "sound\\Protoss\\Artanis\\PAtYes01.wav",
    "sound\\Protoss\\Artanis\\PAtYes02.wav",
    "sound\\Protoss\\Artanis\\PAtYes03.wav"
};

bool Sc::TblFile::load(ArchiveCluster & archiveCluster, const std::string & assetArchivePath)
{
    strings.clear();

    if ( auto rawData = Sc::Data::GetAsset(archiveCluster, assetArchivePath) )
    {
        s64 numStrings = rawData->size() >= 2 ? s64((u16 &)rawData.value()[0]) : 0;
        if ( numStrings > 0 )
        {
            if ( rawData->back() != u8('\0') )
                rawData->push_back('\0');

            strings.push_back(std::string());
            for ( s64 i=1; i<=numStrings && size_t(2*i) < rawData->size(); i++ )
            {
                size_t stringOffset = size_t((u16 &)rawData.value()[2*size_t(i)]);
                if ( stringOffset < rawData->size() )
                    strings.push_back((char*)&rawData.value()[stringOffset]);
                else
                    strings.push_back(std::string());
            }
        }
        else
            logger.warn() << assetArchivePath << " contained no strings" << std::endl;

        return true;
    }
    return false;
}

size_t Sc::TblFile::numStrings() const
{
    return strings.empty() ? 0 : strings.size()-1;
}

const std::string & Sc::TblFile::getString(size_t stringIndex) const
{
    if ( stringIndex < strings.size() )
        return strings[stringIndex];
    else
        throw std::out_of_range("StringIndex " + std::to_string(stringIndex) + " overflows tbl file of size " + std::to_string(strings.size()));
}

bool Sc::TblFile::getString(size_t stringIndex, std::string & outString) const
{
    if ( stringIndex == 0 )
    {
        outString = std::string();
        return true;
    }
    else if ( stringIndex < strings.size() )
    {
        outString = strings[stringIndex];
        return true;
    }
    return false;
}

bool Sc::Pcx::load(ArchiveCluster & archiveCluster, const std::string & assetArchivePath)
{
    if ( auto pcxData = Sc::Data::GetAsset(archiveCluster, assetArchivePath) )
    {
        if ( pcxData->size() < PcxFile::PcxHeaderSize )
        {
            logger.error() << "PCX format not recognized!" << std::endl;
            return false;
        }

        PcxFile & pcxFile = (PcxFile &)pcxData.value()[0];
        if ( pcxFile.bitCount != 8 )
        {
            logger.error() << "PCX bit count not recognized!" << std::endl;
            return false;
        }

        u8* paletteData = &pcxData.value()[pcxData->size()-PcxFile::PaletteSize];
        size_t dataOffset = 0;
        size_t pixelCount = (size_t(pcxFile.rightMargin)+1)*(size_t(pcxFile.lowerMargin)+1);
        for ( size_t pixel = 0; pixel < pixelCount; )
        {
            u8 compSect = pcxFile.data[dataOffset++];
            if ( compSect < PcxFile::MaxOffset ) // Single RGB from palette starting at compSect*3
            {
                rgbaPalette.push_back(Sc::Color<float>(paletteData[compSect*3], paletteData[compSect*3+1], paletteData[compSect*3+2]));
                bgraPalette.push_back(Sc::SystemColor(paletteData[compSect*3], paletteData[compSect*3+1], paletteData[compSect*3+2]));
                rgbaPalette.back().null = compSect;
                bgraPalette.back().null = compSect;
                paletteIndex.push_back(compSect);
                pixel++;
            }
            else // Repeat color at palette starting at colorIndex*3, compSect-MaxOffset times
            {
                u8 colorIndex = pcxFile.data[dataOffset++];
                Sc::Color<float> rgbaColor(paletteData[colorIndex*3], paletteData[colorIndex*3+1], paletteData[colorIndex*3+2]);
                rgbaColor.null = colorIndex;
                Sc::SystemColor bgraColor(paletteData[colorIndex*3], paletteData[colorIndex*3+1], paletteData[colorIndex*3+2]);
                bgraColor.null = colorIndex;
                rgbaPalette.insert(rgbaPalette.end(), compSect-PcxFile::MaxOffset, rgbaColor);
                bgraPalette.insert(bgraPalette.end(), compSect-PcxFile::MaxOffset, bgraColor);
                paletteIndex.insert(paletteIndex.end(), compSect-PcxFile::MaxOffset, colorIndex);
                pixel += (compSect-PcxFile::MaxOffset);
            }
        }
        return true;
    }
    return false;
}

void Sc::Isom::TerrainTypeShapes::populateJutInEastWest(Span<TileGroup> tilesetCv5s, Span<ShapeTileGroup> shapeTileGroups) {
    // The right sides of JutInE are not always present in CV5, when missing they're filled by a merge of EdgeNe/EdgeSe
    if ( jutInEast.topRight.left == Link::None )
    {
        jutInEast.topRight.left = tilesetCv5s[shapeTileGroups[Shape::EdgeNorthEast].bottomLeft].links.left;
        jutInEast.topRight.bottom = tilesetCv5s[shapeTileGroups[Shape::EdgeNorthEast].bottomLeft].links.bottom;
        jutInEast.bottomRight.left = tilesetCv5s[shapeTileGroups[Shape::EdgeSouthEast].topLeft].links.left;
        jutInEast.bottomRight.top = tilesetCv5s[shapeTileGroups[Shape::EdgeSouthEast].topLeft].links.top;
    }

    // The left sides of JutInW are not always present in CV5, when missing they're filled in by a merge of EdgeNw/EdgeSw
    if ( jutInWest.topLeft.right == Link::None )
    {
        jutInWest.topLeft.right = tilesetCv5s[shapeTileGroups[Shape::EdgeNorthWest].bottomRight].links.right;
        jutInWest.topLeft.bottom = tilesetCv5s[shapeTileGroups[Shape::EdgeNorthWest].bottomRight].links.bottom;
        jutInWest.bottomLeft.top = tilesetCv5s[shapeTileGroups[Shape::EdgeSouthWest].topRight].links.top;
        jutInWest.bottomLeft.right = tilesetCv5s[shapeTileGroups[Shape::EdgeSouthWest].topRight].links.right;
    }
}

// Populate the links in quadrants that are not part of the primary shape using adjacent link values
void Sc::Isom::TerrainTypeShapes::populateEmptyQuadrantLinks() {
    edgeNorthWest.topLeft.right = edgeNorthWest.topRight.left;
    edgeNorthWest.topLeft.bottom = edgeNorthWest.bottomLeft.top;

    edgeNorthEast.topRight.left = edgeNorthEast.topLeft.right;
    edgeNorthEast.topRight.bottom = edgeNorthEast.bottomRight.top;

    edgeSouthEast.bottomRight.left = edgeSouthEast.bottomLeft.right;
    edgeSouthEast.bottomRight.top = edgeSouthEast.topRight.bottom;

    edgeSouthWest.bottomLeft.top = edgeSouthWest.topLeft.bottom;
    edgeSouthWest.bottomLeft.right = edgeSouthWest.bottomRight.left;

    jutOutNorth.topLeft.bottom = jutOutNorth.bottomLeft.top;
    jutOutNorth.topLeft.right = jutOutNorth.topLeft.bottom;
    jutOutNorth.topRight.bottom = jutOutNorth.bottomRight.top;
    jutOutNorth.topRight.left = jutOutNorth.topRight.bottom;

    auto fillLink = jutOutEast.topLeft.right;
    jutOutEast.topRight.left = fillLink;
    jutOutEast.topRight.bottom = fillLink;
    jutOutEast.bottomRight.left = fillLink;
    jutOutEast.bottomRight.top = fillLink;
                
    jutOutSouth.bottomRight.top = jutOutSouth.topRight.bottom;
    jutOutSouth.bottomRight.left = jutOutSouth.bottomRight.top;
    jutOutSouth.bottomLeft.top = jutOutSouth.topLeft.bottom;
    jutOutSouth.bottomLeft.right = jutOutSouth.bottomLeft.top;

    fillLink = jutOutWest.topRight.left;
    jutOutWest.topLeft.right = fillLink;
    jutOutWest.topLeft.bottom = fillLink;
    jutOutWest.bottomLeft.right = fillLink;
    jutOutWest.bottomLeft.top = fillLink;
}

// Fill in the hardcoded linkIds (which are always the same for the set of 14 shapes making up one terrain type)
void Sc::Isom::TerrainTypeShapes::populateHardcodedLinkIds()
{
    for ( size_t shapeIndex=0; shapeIndex<shapes.size(); ++shapeIndex )
    {
        const auto & shape = shapes[shapeIndex];
        if ( shape.topLeft.linkId >= LinkId::OnlyMatchSameType )
            (*this)[shapeIndex].topLeft.linkId = shape.topLeft.linkId;
        if ( shape.topRight.linkId >= LinkId::OnlyMatchSameType )
            (*this)[shapeIndex].topRight.linkId = shape.topRight.linkId;
        if ( shape.bottomRight.linkId >= LinkId::OnlyMatchSameType )
            (*this)[shapeIndex].bottomRight.linkId = shape.bottomRight.linkId;
        if ( shape.bottomLeft.linkId >= LinkId::OnlyMatchSameType )
            (*this)[shapeIndex].bottomLeft.linkId = shape.bottomLeft.linkId;
    }
}

void Sc::Isom::TerrainTypeShapes::fillOuterLinkIds(LinkId linkId)
{
    edgeNorthWest.topLeft.linkId = linkId;
                
    edgeNorthEast.topRight.linkId = linkId;
                
    edgeSouthEast.bottomRight.linkId = linkId;
                
    edgeSouthWest.bottomLeft.linkId = linkId;
                
    jutOutNorth.topLeft.linkId = linkId;
    jutOutNorth.topRight.linkId = linkId;
                
    jutOutEast.topRight.linkId = linkId;
    jutOutEast.bottomRight.linkId = linkId;
                
    jutOutWest.topLeft.linkId = linkId;
    jutOutWest.bottomLeft.linkId = linkId;
                
    jutOutSouth.bottomRight.linkId = linkId;
    jutOutSouth.bottomLeft.linkId = linkId;
}

void Sc::Isom::TerrainTypeShapes::fillInnerLinkIds(LinkId linkId)
{
    edgeNorthWest.bottomRight.linkId = linkId;
                
    edgeNorthEast.bottomLeft.linkId = linkId;
                
    edgeSouthEast.topLeft.linkId = linkId;
                
    edgeSouthWest.topRight.linkId = linkId;
                
    jutInEast.topRight.linkId = linkId;
    jutInEast.bottomRight.linkId = linkId;
                
    jutInWest.topLeft.linkId = linkId;
    jutInWest.bottomLeft.linkId = linkId;
                
    jutInNorth.bottomRight.linkId = linkId;
    jutInNorth.bottomLeft.linkId = linkId;
                
    jutInSouth.topLeft.linkId = linkId;
    jutInSouth.topRight.linkId = linkId;
}

void Sc::Isom::TerrainTypeShapes::populateLinkIdsToSolidBrushes(Span<TileGroup> tilesetCv5s, Span<ShapeTileGroup> shapeTileGroups,
    size_t totalSolidBrushEntries, const std::vector<ShapeLinks> & isomLinks)
{
    // Using completed edge links, lookup and fill in the linkIds to the solid brushes
    for ( size_t i=0; i<totalSolidBrushEntries; ++i )
    {
        auto brushLink = isomLinks[i].topLeft.right; // Arbitrary quadrant/direction since links/ids are all the same across a given solid brush
        auto brushLinkId = isomLinks[i].topLeft.linkId;

        if ( brushLink == tilesetCv5s[shapeTileGroups[Shape::EdgeNorthWest].topRight].links.left ) // Found the outer solid brush
            fillOuterLinkIds(brushLinkId);

        if ( brushLink == tilesetCv5s[shapeTileGroups[Shape::EdgeNorthWest].bottomRight].links.right ) // Found the inner solid brush
            fillInnerLinkIds(brushLinkId);
    }
}

NOTE(ZergUnits, Json::Name{"Zerg"})
struct ZergUnits
{
    NOTE(misc, Json::Name{"*"})
    static constexpr u16 misc[] { 59, 36, 35, 97 };

    NOTE(air, Json::Name{"Air Units"})
    static constexpr u16 air[] { 62, 44, 43, 42, 45, 47 };

    NOTE(buildings, Json::Name{"Buildings"})
    static constexpr u16 buildings[] { 130, 143, 136, 139, 149, 137, 131, 133, 135, 132, 134, 138, 142, 141, 144, 146, 140 };

    NOTE(ground, Json::Name{"Ground Units"})
    static constexpr u16 ground[] { 50, 40, 46, 41, 38, 103, 39, 37 };

    NOTE(heroes, Json::Name{"Heroes"})
    static constexpr u16 heroes[] { 54, 53, 104, 51, 56, 55, 49, 48, 52, 57 };

    NOTE(special, Json::Name{"Special"})
    static constexpr u16 special[] { 194, 197, 191 };

    NOTE(specialBuildings, Json::Name{"Special Buildings"})
    static constexpr u16 specialBuildings[] { 150, 201, 151, 152, 148, 147 };

    NOTE(zerg, Json::Name{"Zerg"})
    static constexpr u16 zerg[] { 145, 153 };

    REFLECT_NOTED(ZergUnits, misc, air, buildings, ground, heroes, special, specialBuildings, zerg)
};

NOTE(TerranUnits, Json::Name{"Terran"})
struct TerranUnits
{
    NOTE(misc, Json::Name{"*"})
    static constexpr u16 misc[] { 18, 24, 26, 4, 33, 6, 31, 91, 92, 119 };

    NOTE(addons, Json::Name{"Addons"})
    static constexpr u16 addons[] { 107, 115, 117, 120, 108, 118 };

    NOTE(air, Json::Name{"Air Units"})
    static constexpr u16 air[] { 12, 11, 9, 58, 8 };

    NOTE(buildings, Json::Name{"Buildings"})
    static constexpr u16 buildings[] { 112, 123, 111, 125, 106, 122, 113, 124, 110, 116, 114, 109 };

    NOTE(ground, Json::Name{"Ground Units"})
    static constexpr u16 ground[] { 32, 1, 3, 0, 34, 7, 30, 5, 2 };

    NOTE(heroes, Json::Name{"Heroes"})
    static constexpr u16 heroes[] { 17, 100, 27, 25, 23, 102, 10, 28, 20, 19, 22, 29, 99, 16, 15, 21 };

    NOTE(special, Json::Name{"Special"})
    static constexpr u16 special[] { 14, 195, 198, 192, 13 };

    NOTE(specialBuildings, Json::Name{"Special Buildings"})
    static constexpr u16 specialBuildings[] { 127, 126, 200, 190 };

    NOTE(terran, Json::Name{"Terran"})
    static constexpr u16 terran[] { 121 };

    REFLECT_NOTED(TerranUnits, misc, addons, air, buildings, ground, heroes, special, specialBuildings, terran)
};

NOTE(ProtossUnits, Json::Name{"Protoss"})
struct ProtossUnits
{
    NOTE(air, Json::Name{"Air Units"})
    static constexpr u16 air[] { 71, 72, 60, 73, 84, 70, 69 };

    NOTE(buildings, Json::Name{"Buildings"})
    static constexpr u16 buildings[] { 170, 157, 163, 164, 169, 166, 160, 154, 159, 162, 156, 155, 171, 172, 167, 165 };

    NOTE(ground, Json::Name{"Ground Units"})
    static constexpr u16 ground[] { 68, 63, 61, 66, 67, 64, 83, 85, 65 };

    NOTE(heroes, Json::Name{"Heroes"})
    static constexpr u16 heroes[] { 87, 88, 86, 74, 78, 77, 82, 80, 98, 79, 76, 81, 75 };

    NOTE(protoss, Json::Name{"Protoss"})
    static constexpr u16 protoss[] { 158, 161 };

    NOTE(special, Json::Name{"Special"})
    static constexpr u16 special[] { 196, 199, 193 };

    NOTE(specialBuildings, Json::Name{"Special Buildings"})
    static constexpr u16 specialBuildings[] { 173, 174, 168, 189, 175 };

    REFLECT_NOTED(ProtossUnits, air, buildings, ground, heroes, protoss, special, specialBuildings)
};

NOTE(NeutralUnits, Json::Name{"Neutral"})
struct NeutralUnits
{
    NOTE(critters, Json::Name{"Critters"})
    static constexpr u16 critters[] { 90, 94, 95, 89, 93, 96 };

    NOTE(doodads, Json::Name{"Doodads"})
    static constexpr u16 doodads[] { 209, 204, 203, 207, 205, 211, 210, 208, 206, 213, 212 };

    NOTE(neutral, Json::Name{"Neutral"})
    static constexpr u16 neutral[] { 181, 179, 180, 185, 187, 182, 186 };

    NOTE(powerups, Json::Name{"Powerups"})
    static constexpr u16 powerups[] { 218, 129, 219, 217, 128, 216 };

    NOTE(protoss, Json::Name{"Protoss"})
    static constexpr u16 protoss[] { 105 };

    NOTE(resources, Json::Name{"Resources"})
    static constexpr u16 resources[] { 220, 221, 176, 177, 178, 188, 222, 223, 224, 225, 226, 227 };

    NOTE(special, Json::Name{"Special"})
    static constexpr u16 special[] { 215, 101 };

    NOTE(startLocation, Json::Name{"Start Location"})
    static constexpr u16 startLocation[] { 214 };

    NOTE(zerg, Json::Name{"Zerg"})
    static constexpr u16 zerg[] { 202 };

    REFLECT_NOTED(NeutralUnits, critters, doodads, neutral, powerups, protoss, resources, special, startLocation, zerg)
};

NOTE(UndefinedUnits, Json::Name{"Undefined"})
struct UndefinedUnits
{
    NOTE(independent, Json::Name{"Independent"})
    static constexpr u16 independent[] {183, 184};

    REFLECT_NOTED(UndefinedUnits, independent)
};

using UnitGroups = std::tuple<ZergUnits, TerranUnits, ProtossUnits, NeutralUnits, UndefinedUnits>;

bool Sc::Data::loadUnitGroups()
{
    RareTs::forIndexes<std::tuple_size_v<UnitGroups>>([&](auto i) {
        constexpr std::size_t I = decltype(i)::value;
        using UnitGroup = std::tuple_element_t<I, UnitGroups>;

        auto groupName = RareTs::Notes<UnitGroup>::template getNote<Json::Name>().value;
        auto & unitGroup = units.unitGroups.emplace_back(Sc::Unit::UnitGroup{std::string(groupName)});

        RareTs::Members<UnitGroup>::forEach([&](auto member) {
            auto subGroupName = member.template getNote<Json::Name>().value;
            auto & values = member.value();
            auto & subUnitGroup = unitGroup.subGroups.emplace_back(Sc::Unit::UnitGroup{std::string(subGroupName)});
            subUnitGroup.memberUnits = std::vector<u16>(std::begin(values), std::end(values));
        });
    });
    
    return true;
}

bool Sc::Data::loadSpriteNames(const Sc::Sprite::SpriteGroup & spriteGroup)
{
    auto & spriteGroups = sprites.spriteGroups;
    for ( const auto & subGroup : spriteGroup.subGroups )
        loadSpriteNames(subGroup);
    
    for ( const auto & memberSprite : spriteGroup.memberSprites )
    {
        if ( !memberSprite.isUnit )
            sprites.spriteNames[memberSprite.spriteIndex] = memberSprite.spriteName;
    }

    return true;
}

#ifdef _DEBUG
void appendSpriteIndexes(std::set<u16> & indexes, const Sc::Sprite::SpriteGroup & spriteGroup)
{
    for ( const auto & subGroup : spriteGroup.subGroups )
        appendSpriteIndexes(indexes, subGroup);
    
    for ( const auto & memberSprite : spriteGroup.memberSprites )
        indexes.insert(memberSprite.spriteIndex);
}
#endif

bool Sc::Data::loadSpriteGroups(Sc::TblFilePtr imagesTbl, Sc::TblFilePtr statTxt)
{
    constexpr auto totalSprites = Sc::Sprite::TotalSprites;

    sprites.spriteAutoRestart.assign(totalSprites, true);
    auto & doodads = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Doodads"});
    for ( u16 tilesetIndex = Sc::Terrain::Tileset::Badlands; tilesetIndex < Sc::Terrain::NumTilesets; ++tilesetIndex )
    {
        std::string tilesetName = terrain.TilesetNames[tilesetIndex];
        tilesetName[0] = std::toupper(tilesetName[0]);
        auto & tilesetDoodads = doodads.subGroups.emplace_back(Sc::Sprite::SpriteGroup{tilesetName});
        const auto & tileset = terrain.get(Sc::Terrain::Tileset(tilesetIndex));
        for ( const auto & doodadGroup : tileset.doodadGroups )
        {
            auto & doodadGroupSprites = tilesetDoodads.subGroups.emplace_back(Sc::Sprite::SpriteGroup{doodadGroup.name});
            Sc::Sprite::SpriteGroup* spriteGroup = nullptr;
            for ( const auto & doodadStartTileGroup : doodadGroup.doodadStartTileGroup )
            {
                const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[doodadStartTileGroup];
                bool hasSprite = (doodadDat.flags & Sc::Terrain::Doodad::Flags::DrawAsSprite) == Sc::Terrain::Doodad::Flags::DrawAsSprite;
                if ( hasSprite )
                {
                    const auto & spriteDat = sprites.getSprite(doodadDat.overlayIndex);
                    const auto & imageDat = sprites.getImage(spriteDat.imageFile);
                    const auto & imageFileStr = imagesTbl->getString(imageDat.grpFile);
                    std::string imageFileName = getSystemFileName(imageFileStr);

                    doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{doodadDat.overlayIndex, imageFileName});
                }
            }

            switch ( tilesetIndex )
            {
                case Sc::Terrain::Tileset::Badlands:
                    if ( doodadGroup.name == "Asphalt" )
                    {
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{112, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{113, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{114, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{115, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{116, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{117, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{118, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{125, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{126, "LCSignCC.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{129, "LCSignCC.grp (unused)"});
                    }
                    break;
                case Sc::Terrain::Tileset::Desert:
                    if ( doodadGroup.name == "High Dirt" )
                    {
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{452, "HDPLNT03.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{471, "HDLbox01.grp (flipped, unused)"});
                    }
                    else if ( doodadGroup.name == "Dirt" )
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{475, "LDLbox01.grp (flipped, unused)"});
                    else if ( doodadGroup.name == "Sand Dunes" )
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{477, "LDMachn1.grp (flipped, unused)"});
                    break;
                case Sc::Terrain::Tileset::Arctic:
                    if ( doodadGroup.name == "High Snow" )
                    {
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{406, "HDradarl.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{410, "HDSTre01.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{411, "HDSTre02.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{412, "HDSTre03.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{413, "HDSTre04.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{439, "HDRadr02.grp (flipped, unused)"});
                    }
                    else if ( doodadGroup.name == "Snow" )
                    {
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{416, "HDTwr02.grp (unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{435, "LDRck01.grp (flipped, unused)"});
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{436, "LDRck02.grp (flipped, unused)"});
                    }
                    break;
                case Sc::Terrain::Tileset::Twilight:
                    if ( doodadGroup.name == "Dirt" )
                        doodadGroupSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{393, "Lddrill.grp (unused)"});
                    break;
            }

            if ( doodadGroupSprites.memberSprites.empty() )
                tilesetDoodads.subGroups.pop_back();
            else
            {
                std::sort(tilesetDoodads.subGroups.back().memberSprites.begin(), tilesetDoodads.subGroups.back().memberSprites.end(),
                    [](const auto & l, const auto & r) { return l.spriteIndex < r.spriteIndex; });

                for ( const auto & sprite : doodadGroupSprites.memberSprites ) {
                    sprites.spriteAutoRestart[sprite.spriteIndex] = false;
                }
            }
        }
    }
    
    sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Unit Pure Sprites"});
    auto & spriteUnits = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Sprite Units"});
    auto & unitPureSprites = sprites.spriteGroups[sprites.spriteGroups.size()-2];
    RareTs::forIndexes<std::tuple_size_v<UnitGroups>>([&](auto i) {
        constexpr std::size_t I = decltype(i)::value;
        using UnitGroup = std::tuple_element_t<I, UnitGroups>;

        auto groupName = RareTs::Notes<UnitGroup>::template getNote<Json::Name>().value;
        auto & pureSpriteUnitGroup = unitPureSprites.subGroups.emplace_back(Sc::Sprite::SpriteGroup{std::string(groupName)});
        auto & spriteUnitsUnitGroup = spriteUnits.subGroups.emplace_back(Sc::Sprite::SpriteGroup{std::string(groupName)});

        RareTs::Members<UnitGroup>::forEach([&](auto member) {
            auto subGroupName = member.template getNote<Json::Name>().value;
            auto & values = member.value();
            auto & subPureSpriteUnitGroup = pureSpriteUnitGroup.subGroups.emplace_back(Sc::Sprite::SpriteGroup{std::string(subGroupName)});
            auto & subSpriteUnitsUnitGroup = spriteUnitsUnitGroup.subGroups.emplace_back(Sc::Sprite::SpriteGroup{std::string(subGroupName)});
            for ( u16 unitId : values )
            {
                const auto & unitDat = units.getUnit(Sc::Unit::Type(unitId));
                const auto & flingyDat = units.getFlingy(unitDat.graphics);
                subPureSpriteUnitGroup.memberSprites.push_back(Sc::Sprite::TreeSprite{flingyDat.sprite, units.defaultDisplayNames[unitId]});
                subSpriteUnitsUnitGroup.memberSprites.push_back(Sc::Sprite::TreeSprite{unitId, units.defaultDisplayNames[unitId], true});
                sprites.spriteAutoRestart[flingyDat.sprite] = false;
            }
        });
    });
    
    auto & remains = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{236, "Marine Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{230, "Ghost Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{490, "Medic Remains"});
    
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{150, "Larva Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{143, "Egg Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{141, "Drone Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{134, "Broodling Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{160, "Zergling Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{147, "Hydralisk Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{484, "Lurker Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{158, "Ultralisk Remains"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{139, "Defiler Remains"});

    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{192, "Dragoon Remains"});
    
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{186, "Zerg Building Rubble - Small"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{187, "Zerg Building Rubble - Large"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{223, "Protoss Building Rubble - Small"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{224, "Protoss Building Rubble - Large"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{273, "Terran Building Rubble - Small"});
    remains.memberSprites.push_back(Sc::Sprite::TreeSprite{274, "Terran Building Rubble - Large"});

    auto & construction = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Construction"});
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{270, "Terran Construction - Large"});
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{271, "Terran Construction - Small"});
    sprites.spriteAutoRestart[270] = false;
    sprites.spriteAutoRestart[271] = false;
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{182, "Zergling Building Spawn - Small"});
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{183, "Zergling Building Spawn - Medium"});
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{184, "Zergling Building Spawn - Large"});
    construction.memberSprites.push_back(Sc::Sprite::TreeSprite{319, "Egg Spawn"});

    auto & explosions = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Explosions"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{136, "Infested Terran Explosion"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{185, "Zerg Building Explosion"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{222, "Explosion - Large"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{267, "Nuke Hit"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{272, "Building Explosion - Large"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{316, "Small Explosion (Unused)"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{317, "Double Explosion"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{131, "Scourge Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{132, "Scourge Explosion"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{145, "Guardian Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{152, "Mutalisk Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{154, "Overlord Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{156, "Queen Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{483, "Devourer Death"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{374, "Hallucination Death 1"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{375, "Hallucination Death 2"});
    explosions.memberSprites.push_back(Sc::Sprite::TreeSprite{376, "Hallucination Death 3"});
    
    auto removeWeaponLabelHotkey = [](const std::string & label) -> std::string {
        if ( label.size() >= 2 && label[1] == '\3' )
        {
            std::string newLabel {};
            for ( std::size_t i=2; i<label.size(); ++i )
            {
                if ( label[i] != '\1' && label[i] != '\3' )
                    newLabel += label[i];
            }
            return newLabel;
        }
        else
            return label;
    };

    std::set<u16> knownWeaponSprites {};
    sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Weapons"});
    auto & abilities = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Abilities"});
    auto & weaponSprites = sprites.spriteGroups[sprites.spriteGroups.size()-2];
    for ( size_t i=0; i<Weapon::Total; ++i )
    {
        const auto & weapon = weapons.get(Sc::Weapon::Type(i));
        const auto & flingyDat = units.getFlingy(weapon.graphics);
        if ( knownWeaponSprites.find(flingyDat.sprite) == knownWeaponSprites.end() )
        {
            if ( flingyDat.sprite == Sprite::Type::ZergScourge || flingyDat.sprite == Sprite::Type::WhiteCircle370 ||
                flingyDat.sprite == Sprite::Type::ZergBeaconOverlay ) {
                continue; // Duplicate of scourge sprite from unit group, or otherwise poorly grouped white circle/zerg marker
            }

            knownWeaponSprites.insert(flingyDat.sprite);
            const auto & spriteDat = sprites.getSprite(flingyDat.sprite);
            const auto & imageDat = sprites.getImage(spriteDat.imageFile);
            const auto & imageFileStr = imagesTbl->getString(imageDat.grpFile);
            std::string imageFileName = getSystemFileName(imageFileStr);
            std::string weaponLabel = removeWeaponLabelHotkey(statTxt->getString(weapon.label));
            if ( flingyDat.sprite == Sprite::Type::DualPhotonBlastersHit )
                weaponLabel = "Dual Photon Blasters Hit"; // Fix poor name: "Unused"
            else if ( flingyDat.sprite == Sprite::Type::NeedleSpineHit )
                weaponLabel = "Needle Spine Hit"; // Fix repeated name: "Needle Spine"

            if ( flingyDat.sprite == Sprite::Type::PsionicStorm || flingyDat.sprite == Sprite::Type::YamatoGun ||
                flingyDat.sprite == Sprite::Type::EmpShockwave || flingyDat.sprite == Sprite::Type::Parasite || flingyDat.sprite == Sprite::Type::Plague ||
                flingyDat.sprite == Sprite::Type::Consume || flingyDat.sprite == Sprite::Type::OpticalFlare )
            {
                abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{flingyDat.sprite, weaponLabel}); // Better grouped under abilities than weapons
            }
            else
                weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{flingyDat.sprite, weaponLabel});
        }
    }
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{309, "Smoke Trail"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{310, "Smoke Grenade"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{332, "Needle Spines"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{365, "Spore Hit"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{367, "Spore Trail"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{368, "Spore Smoke"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{371, "Acid Spray"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{372, "Plasma Drip"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{373, "Protoss Trail"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{378, "Flamethrower"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{505, "Halo Rocket Trail"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{507, "Neutron Flare Overlay"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{513, "Corrosive Acid Hit"});
    
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{354, "Plasma Drip Hit (unused)"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{369, "Phase Distruptor (unused)"});
    weaponSprites.memberSprites.push_back(Sc::Sprite::TreeSprite{511, "Subterranean Spines"});

    std::sort(weaponSprites.memberSprites.begin(), weaponSprites.memberSprites.end(),
        [](const auto & l, const auto & r) { return l.spriteName < r.spriteName; });

    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{231, "Nuke Target Dot"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{322, "Burrowing Dust"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{361, "Stasis"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{364, "Ensnare"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{379, "Recall Field"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{380, "Scanner Sweep Hit"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{500, "Feedback - Small"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{501, "Feedback - Medium"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{502, "Feedback - Large"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{514, "Maelstrom Hit"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{351, "Yamato Gun Trail"});

    std::sort(abilities.memberSprites.begin(), abilities.memberSprites.end(),
        [](const auto & l, const auto & r) { return l.spriteIndex < r.spriteIndex; });

    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{323, "Building Landing Dust 1"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{324, "Building Landing Dust 2"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{325, "Building Landing Dust 3"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{326, "Building Landing Dust 4"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{327, "Building Landing Dust 5"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{328, "Building Lifting Dust 1"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{329, "Building Lifting Dust 2"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{330, "Building Lifting Dust 3"});
    abilities.memberSprites.push_back(Sc::Sprite::TreeSprite{331, "Building Lifting Dust 4"});

    auto & misc = sprites.spriteGroups.emplace_back(Sc::Sprite::SpriteGroup{"Misc"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{318, "Cursor"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{320, "High Templar Glow"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{321, "Psi Field - Right Upper"});
    sprites.spriteAutoRestart[321] = false;
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{344, "Magna Pulse"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{300, "White Circle"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{370, "White Circle"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{504, "White Circle"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{247, "Science Vessel Turret"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{377, "Bunker Overlay"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{510, "Zerg Beacon Overlay"});

    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{311, "Vespene Puff 1"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{312, "Vespene Puff 2"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{313, "Vespene Puff 3"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{314, "Vespene Puff 4"});
    misc.memberSprites.push_back(Sc::Sprite::TreeSprite{315, "Vespene Puff 5"});
    
    sprites.spriteNames.assign(Sc::Sprite::TotalSprites, "");
    for ( auto & spriteGroup : sprites.spriteGroups )
        loadSpriteNames(spriteGroup);

    #ifdef _DEBUG
    // Validate that every sprite is included
    std::set<u16> treeSpriteIndexes {};
    for ( const auto & spriteGroup : sprites.spriteGroups )
        appendSpriteIndexes(treeSpriteIndexes, spriteGroup);

    for ( std::size_t i=0; i<Sc::Sprite::TotalSprites; ++i )
    {
        if ( !treeSpriteIndexes.contains(u16(i)) )
            throw std::logic_error("All sprites not included!");
    }
    #endif

    return true;
}

bool Sc::Data::load(Sc::DataFile::BrowserPtr dataFileBrowser, const std::vector<Sc::DataFile::Descriptor> & dataFiles,
    const std::string & expectedStarCraftDirectory, FileBrowserPtr<u32> starCraftBrowser)
{
    auto start = std::chrono::high_resolution_clock::now();
    logger.debug("Loading StarCraft Data...");

    if ( dataFileBrowser == nullptr )
    {
        logger.error("No data-file browser was provided to Sc::Data::load");
        return false;
    }

    auto archiveCluster = dataFileBrowser->openScDataFiles(dataFiles, expectedStarCraftDirectory, starCraftBrowser);
    if ( archiveCluster == nullptr || !archiveCluster->isOpen() )
    {
        logger.error("No archives selected, many features will not work without the game files.\n\nInstall or locate StarCraft for the best experience.");
        return false;
    }

    Sc::TblFilePtr statTxt = Sc::TblFilePtr(new Sc::TblFile());
    if ( !statTxt->load(*archiveCluster, "Rez\\stat_txt.tbl") )
        CHKD_ERR("Failed to load stat_txt.tbl");
    
    if ( !terrain.load(*archiveCluster, statTxt) )
        CHKD_ERR("Failed to load terrain");

    if ( !ai.load(*archiveCluster, statTxt) )
        CHKD_ERR("Failed to load AiScripts");

    if ( !upgrades.load(*archiveCluster) )
        CHKD_ERR("Failed to load upgrades");

    if ( !techs.load(*archiveCluster) )
        CHKD_ERR("Failed to load techs");

    if ( !units.load(*archiveCluster) )
        CHKD_ERR("Failed to load unit dat");
    else
        terrain.mergeSpriteFlags(units);

    if ( !weapons.load(*archiveCluster) )
        CHKD_ERR("Failed to load Weapons.dat");
    
    Sc::TblFilePtr imagesTbl = Sc::TblFilePtr(new Sc::TblFile());
    if ( !imagesTbl->load(*archiveCluster, "arr\\images.tbl") )
        CHKD_ERR("Failed to load arr\\images.tbl");

    if ( !sprites.load(*archiveCluster, imagesTbl) )
        CHKD_ERR("Failed to load sprites!");

    if ( !spk.load(*archiveCluster) )
        CHKD_ERR("Failed to load star.spk!");

    if ( !tunit.load(*archiveCluster, "game\\tunit.pcx") )
        CHKD_ERR("Failed to load tunit.pcx");

    if ( !tminimap.load(*archiveCluster, "game\\tminimap.pcx") )
        CHKD_ERR("Failed to load tminimap.pcx");

    if ( !tselect.load(*archiveCluster, "game\\tselect.pcx") )
        CHKD_ERR("Failed to load tselect.pcx");

    if ( !loadUnitGroups() )
        CHKD_ERR("Failed to load unit groups");

    if ( !loadSpriteGroups(imagesTbl, statTxt) )
        CHKD_ERR("Failed to load sprite groups");
    
    auto finish = std::chrono::high_resolution_clock::now();
    logger.debug() << "StarCraft data loading completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
    return true;
}

std::optional<std::vector<u8>> Sc::Data::GetAsset(ArchiveCluster & archiveCluster, bool & isFirst,
    const std::string & firstAssetArchivePathOption, const std::string & secondAssetArchivePathOption)
{
    if ( auto result = archiveCluster.getFile(firstAssetArchivePathOption) )
    {
        isFirst = true;
        return result;
    }
    else if ( auto result = archiveCluster.getFile(secondAssetArchivePathOption) )
    {
        isFirst = false;
        return result;
    }
    logger.error() << "Failed to get StarCraft asset: \"" << firstAssetArchivePathOption << "\" or \"" << secondAssetArchivePathOption << "\"" << std::endl;
    return std::nullopt;
}

std::optional<std::vector<u8>> Sc::Data::GetAsset(ArchiveCluster & archiveCluster, const std::string & assetArchivePath, bool silent)
{
    auto result = archiveCluster.getFile(assetArchivePath);
    if ( result )
        return result;
    else if ( !silent )
        logger.error() << "Failed to get StarCraft asset: " << assetArchivePath << std::endl;

    return std::nullopt;
}

std::optional<std::vector<u8>> Sc::Data::GetAsset(const std::string & assetArchivePath,
    Sc::DataFile::BrowserPtr dataFileBrowser,
    const std::vector<Sc::DataFile::Descriptor> & dataFiles,
    const std::string & expectedStarCraftDirectory,
    FileBrowserPtr<u32> starCraftBrowser)
{
    auto archiveCluster = dataFileBrowser->openScDataFiles(dataFiles, expectedStarCraftDirectory, starCraftBrowser);
    return Sc::Data::GetAsset(*archiveCluster, assetArchivePath);
}

bool Sc::Data::ExtractAsset(ArchiveCluster & archiveCluster, const std::string & assetArchivePath, const std::string & systemFilePath)
{
    return archiveCluster.extractFile(assetArchivePath, systemFilePath);
}

bool Sc::Data::ExtractAsset(const std::string & assetArchivePath, const std::string & systemFilePath,
    Sc::DataFile::BrowserPtr dataFileBrowser,
    const std::vector<Sc::DataFile::Descriptor> & dataFiles,
    const std::string & expectedStarCraftDirectory,
    FileBrowserPtr<u32> starCraftBrowser)
{
    auto archiveCluster = dataFileBrowser->openScDataFiles(dataFiles, expectedStarCraftDirectory, starCraftBrowser);
    return Sc::Data::ExtractAsset(*archiveCluster, assetArchivePath, systemFilePath);
}
