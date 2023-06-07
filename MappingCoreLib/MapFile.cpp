#include "../CrossCutLib/Logger.h"
#include <SimpleIcu.h>
#include "MapFile.h"
#include "SystemIO.h"
#include "EscapeStrings.h"
#include <cstdio>
#include <cstdarg>
#include <fstream>
#include <sstream>
#include <chrono>

extern Logger logger;

#undef PlaySound

std::hash<std::string> MapFile::strHash;
std::map<size_t, std::string> MapFile::virtualSoundTable;
u64 MapFile::nextAssetFileId(0);

FileBrowserPtr<SaveType> MapFile::getDefaultOpenMapBrowser()
{
    return FileBrowserPtr<SaveType>(new FileBrowser<SaveType>(getOpenMapFilters(), "Open Map", true, false));
}

FileBrowserPtr<SaveType> MapFile::getDefaultSaveMapBrowser()
{
    return FileBrowserPtr<SaveType>(new FileBrowser<SaveType>(getSaveMapFilters(), "Save Map", false, true));
}

MapFile::MapFile(const std::string & filePath) :
    saveType(SaveType::Unknown), mapFilePath(""), temporaryMpqPath(""), temporaryMpq(true, true)
{
    load(filePath);
}

MapFile::MapFile(FileBrowserPtr<SaveType> fileBrowser) :
    saveType(SaveType::Unknown), mapFilePath(""), temporaryMpqPath(""), temporaryMpq(true, true)
{
    load(fileBrowser);
}

MapFile::MapFile(Sc::Terrain::Tileset tileset, u16 width, u16 height)
    : Scenario(tileset, width, height), saveType(SaveType::HybridScm), mapFilePath(""), temporaryMpqPath(""), temporaryMpq(true, true)
{
    if ( MapFile::virtualSoundTable.size() == 0 )
    {
        size_t numVirtualSounds = Sc::Sound::virtualSoundPaths.size();
        for ( size_t i=0; i<numVirtualSounds; i++ )
        {
            std::string soundPath(Sc::Sound::virtualSoundPaths[i]);
            size_t hash = strHash(soundPath);
            virtualSoundTable.insert(std::pair<size_t, std::string>(hash, soundPath));
        }
    }
}

MapFile::~MapFile()
{

}

bool MapFile::load(const std::string & filePath)
{
    return !filePath.empty() && openMapFile(filePath);
}

bool MapFile::load(FileBrowserPtr<SaveType> fileBrowser)
{
    std::string browseFilePath = "";
    SaveType saveType;
    return fileBrowser != nullptr && fileBrowser->browseForOpenPath(browseFilePath, saveType) && openMapFile(browseFilePath);
}

bool MapFile::save(const std::string & saveFilePath, bool overwriting, bool updateListFile, bool lockAnywhere, bool autoDefragmentLocations)
{
    bool savePathChanged = saveFilePath.compare(mapFilePath) != 0;
    bool saveAs = !mapFilePath.empty() && savePathChanged;

    if ( isProtected() )
        CHKD_ERR("Cannot save protected maps!");
    else if ( !saveFilePath.empty() )
    {
        logger.info() << "Saving to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(saveType) << "\"" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        bool versionCorrect = true;
        if ( saveType == SaveType::StarCraftScm || saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Original);
        else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Hybrid);
        else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk || saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_BroodWar);
        else if ( saveType == SaveType::RemasteredScx || saveType == SaveType::RemasteredChk || saveType == SaveType::AllMaps ) // Remastered Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Remastered);

        if ( !versionCorrect )
        {
            logger.error("Failed to update version, save failed!");
            return false;
        }

        updateSaveSections();

        if ( (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm ||
              saveType == SaveType::ExpansionScx || saveType == SaveType::RemasteredScx) || saveType == SaveType::AllMaps ) // Must be packed into an MPQ
        {
            if ( savePathChanged && overwriting && !::removeFile(saveFilePath) )
            {
                logger.error() << "Failed to delete old file being overwritten: \"" << saveFilePath << "\"" << std::endl;
                return false;
            }

            if ( !saveAs || (saveAs && (!MpqFile::isValid(mapFilePath) || makeFileCopy(mapFilePath, saveFilePath))) ) // If using save-as and existing is mpq, copy mpq to the new location
            {
                std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                Scenario::write(chk);
                if ( chk.good() )
                {
                    if ( MpqFile::open(saveFilePath, false, true) )
                    {
                        bool success = true;
                        if ( !MpqFile::addFile("staredit\\scenario.chk", chk) )
                        {
                            CHKD_ERR("Failed to add scenario file!");
                            success = false;
                        }
                        else if ( !processModifiedAssets(updateListFile) )
                        {
                            CHKD_ERR("Processing assets failed!");
                            success = false;
                        }

                        MpqFile::setUpdatingListFile(updateListFile);
                        MpqFile::close();
                        mapFilePath = saveFilePath;
                        
                        auto finish = std::chrono::high_resolution_clock::now();
                        logger.info() << "Successfully saved to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(saveType) << "\" in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                        return success;
                    }
                    else
                        CHKD_ERR("Failed to create the new MPQ file!");
                }
                else
                    CHKD_ERR("Failed to compile the scenario file!");
            }
        }
        else // Is a chk file or unrecognized format, write out chk file
        {
            if ( ::removeFile(saveFilePath) ) // Remove any existing files of the same name
            {
                std::ofstream outFile(icux::toFilestring(saveFilePath).c_str(), std::ios_base::out|std::ios_base::binary);
                if ( outFile.is_open() )
                {
                    Scenario::write(outFile);
                    if ( outFile.good() )
                    {
                        mapFilePath = saveFilePath;
                        auto finish = std::chrono::high_resolution_clock::now();
                        logger.info() << "Successfully saved to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(saveType) << "\" in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                        return true;
                    }
                    else
                    {
                        logger.error("Failed to write scenario file!");
                        return false;
                    }
                }
            }
            CHKD_ERR("Failed to create the new map file!");
        }
    }
    return false;
}

bool isChkSaveType(SaveType saveType)
{
    return saveType == SaveType::AllChk || saveType == SaveType::RemasteredChk ||
        saveType == SaveType::ExpansionChk || saveType == SaveType::HybridChk ||
        saveType == SaveType::StarCraftChk;
}

bool MapFile::save(bool saveAs, bool updateListFile, FileBrowserPtr<SaveType> fileBrowser, bool lockAnywhere, bool autoDefragmentLocations)
{
    if ( saveAs && mapFilePath.empty() )
        saveAs = false; // This is a new map or a map without a path, use regular save operation

    if ( isProtected() )
        CHKD_ERR("Cannot save protected maps!");
    else
    {
        // If scenario changed such that save type should have changed, adjust it in advance so filter type is correct
        if ( Scenario::getVersion() >= Chk::Version::StarCraft_Remastered )
            saveType = isChkSaveType(saveType) ? SaveType::RemasteredChk : SaveType::RemasteredScx;
        else if ( Scenario::getVersion() >= Chk::Version::StarCraft_BroodWar )
            saveType = isChkSaveType(saveType) ? SaveType::ExpansionChk : SaveType::ExpansionScx;
        else if ( Scenario::getVersion() >= Chk::Version::StarCraft_Hybrid )
            saveType = isChkSaveType(saveType) ? SaveType::HybridChk : SaveType::HybridScm;
        else // version < Chk::Version::StarCraft_Hybrid
            saveType = isChkSaveType(saveType) ? SaveType::StarCraftChk : SaveType::StarCraftScm;

        bool overwriting = false;
        std::string newMapFilePath;
        if ( (saveAs || mapFilePath.empty()) && fileBrowser != nullptr ) // saveAs specified or filePath not yet determined, and a fileBrowser is available
            getSaveDetails(saveType, newMapFilePath, overwriting, fileBrowser);
        else if ( !saveAs )
            newMapFilePath = mapFilePath;

        if ( !newMapFilePath.empty() )
            return save(newMapFilePath, overwriting, updateListFile, lockAnywhere, autoDefragmentLocations);
    }
    return false;
}

bool MapFile::openTemporaryMpq()
{
    if ( temporaryMpq.isOpen() )
        return true;
    else if ( !temporaryMpqPath.empty() )
        return temporaryMpq.open(temporaryMpqPath, false, true);

    std::string assetFileDirectory("");
    std::string assetFilePath("");
#ifdef CHKDRAFT
    // If this is compiled as part of Chkdraft, use the Pre-Save directory
    if ( auto assetFileDirectory = getPreSavePath() )
    {
        do
        {
            // Try the nextAssetFileId filename
            assetFilePath = makeSystemFilePath(*assetFileDirectory, std::to_string(nextAssetFileId) + ".mpq");
            nextAssetFileId ++;
        }
        while ( ::findFile(assetFilePath) ); // Try again if the file already exists
    }
#endif

    if ( assetFilePath.length() <= 0 ) // If this is not Chkdraft or you could not get the Pre-Save directory
    {
        if ( mapFilePath.length() > 0 ) // Use the same directory as the map file, if the map file already has a directory
        {
            do
            {
                assetFileDirectory = getSystemFileDirectory(mapFilePath, true);
                assetFilePath = makeSystemFilePath(assetFileDirectory, std::to_string(nextAssetFileId) + ".mpq");
                nextAssetFileId ++;
            } while ( ::findFile(assetFilePath) ); // Try again if the file already exists
        }
        else // Use the C library to find an appropriate temporary location
        {
            char* temporaryFilePath = tmpnam(nullptr);
            assetFilePath = std::string(temporaryFilePath != NULL ? temporaryFilePath : "");
        }
    }

    if ( assetFilePath.length() > 0 )
    {
        try {
            if ( temporaryMpq.create(assetFilePath) )
            {
                temporaryMpqPath = temporaryMpq.getFilePath();
                return true;
            }
        } catch ( std::exception ) {
            CHKD_ERR("Failed to setup temporary asset storage, out of memory!");
        }
    }
    else
        CHKD_ERR("Failed to setup temporary asset storage, an appropriate file path could not be found!");

    return false;
}

bool MapFile::openMapFile(const std::string & filePath)
{
    logger.info() << "Opening map file: " << filePath << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    std::string extension = getSystemFileExtension(filePath);
    if ( !extension.empty() )
    {
        if ( extension == ".scm" || extension == ".scx" )
        {
            if ( MpqFile::open(filePath, true, false) )
            {
                this->mapFilePath = MpqFile::getFilePath();
                if ( auto chkData = MpqFile::getFile("staredit\\scenario.chk") )
                {
                    MpqFile::close();

                    std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                    std::copy(chkData->begin(), chkData->end(), std::ostream_iterator<u8>(chk));
                    if ( Scenario::read(chk) )
                    {
                        if ( Scenario::isOriginal() )
                            saveType = SaveType::StarCraftScm; // Vanilla
                        else if ( Scenario::isHybrid() )
                            saveType = SaveType::HybridScm; // Hybrid
                        else if ( Scenario::isExpansion() )
                            saveType = SaveType::ExpansionScx; // Expansion
                        else if ( Scenario::isRemastered() )
                            saveType = SaveType::RemasteredScx; // Remastered
                    
                        auto finish = std::chrono::high_resolution_clock::now();
                        logger.info() << "Map " << mapFilePath << " opened in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                        return true;
                    }
                    else
                        CHKD_ERR("Invalid or missing Scenario file.");
                }
                else
                    CHKD_ERR("Failed to get scenario file from MPQ.");
            }
            else if ( ::lastErrorIndicatedFileNotFound() )
                CHKD_ERR("File Not Found");
            else
                CHKD_ERR("%d", ::getLastError());
        }
        else if ( extension == ".chk" )
        {
            this->mapFilePath = filePath;
            std::ifstream chk(filePath, std::ios_base::binary|std::ios_base::in);
            if ( Scenario::read(chk) )
            {
                if ( Scenario::isOriginal() )
                    saveType = SaveType::StarCraftChk; // Vanilla chk
                else if ( Scenario::isHybrid() )
                    saveType = SaveType::HybridChk; // Hybrid chk
                else if ( Scenario::isExpansion() )
                    saveType = SaveType::ExpansionChk; // Expansion chk
                else
                    saveType = SaveType::RemasteredChk;
                
                auto finish = std::chrono::high_resolution_clock::now();
                logger.info() << "Map " << mapFilePath << " opened in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                return true;
            }
            else
                CHKD_ERR("Parsing Failed!");
        }
        else
            CHKD_ERR("Unrecognized Extension!");
    }
    return false;
}

bool MapFile::processModifiedAssets(bool updateListFile)
{
    std::vector<std::vector<ModifiedAsset>::iterator> processedAssets;
    if ( openTemporaryMpq() )
    {
        for ( auto modifiedAsset = modifiedAssets.begin(); modifiedAsset != modifiedAssets.end(); ++modifiedAsset )
        {
            const std::string & assetMpqPath = modifiedAsset->assetMpqPath;
            if ( modifiedAsset->actionTaken == AssetAction::Add )
            {
                if ( auto assetBuffer = temporaryMpq.getFile(modifiedAsset->assetTempMpqPath) )
                {
                    temporaryMpq.removeFile(modifiedAsset->assetTempMpqPath);
                    if ( MpqFile::addFile(assetMpqPath, *assetBuffer, modifiedAsset->wavQualitySelected) )
                        processedAssets.push_back(modifiedAsset);
                    else
                        CHKD_ERR("Failed to save %s to destination file", assetMpqPath.c_str());
                }
                else
                    CHKD_ERR("Failed to read in temporary asset!");
            }
            else if ( modifiedAsset->actionTaken == AssetAction::Remove )
            {
                if ( MpqFile::removeFile(assetMpqPath) )
                    processedAssets.push_back(modifiedAsset);
                else
                {
                    logger.error() << assetMpqPath.c_str() << std::endl;
                    CHKD_ERR("Failed to remove %s from map archive", assetMpqPath.c_str());
                }
            }
        }
        temporaryMpq.setUpdatingListFile(updateListFile);
        temporaryMpq.save();
    }

    if ( processedAssets.size() == modifiedAssets.size() )
    {
        modifiedAssets.clear();
        return true;
    }
    else
    {
        for ( auto processedAsset : processedAssets )
            modifiedAssets.erase(processedAsset);

        return false;
    }
}

SaveType MapFile::getSaveType()
{
    return saveType;
}

void MapFile::setSaveType(SaveType newSaveType)
{
    saveType = newSaveType;
}

std::string MapFile::GetStandardSoundDir()
{
    return "staredit\\wav\\";
}

bool MapFile::addMpqAsset(const std::string & assetSystemFilePath, const std::string & assetMpqFilePath, WavQuality wavQuality)
{
    bool success = false;
    if ( ::findFile(assetSystemFilePath) )
    {
        if ( openTemporaryMpq() )
        {
            ModifiedAsset modifiedAsset(assetMpqFilePath, AssetAction::Add, wavQuality);
            const std::string & tempMpqPath = modifiedAsset.assetTempMpqPath;
            if ( temporaryMpq.addFile(tempMpqPath, assetSystemFilePath) )
            {
                modifiedAssets.push_back(modifiedAsset);
                success = true;
            }
            else
                CHKD_ERR("Failed to add file!");
            
            temporaryMpq.setUpdatingListFile(true);
            temporaryMpq.save();
        }
        else
            CHKD_ERR("Failed to setup asset temporary storage!");
    }
    else
        CHKD_ERR("Failed to find asset file!");

    return success;
}

bool MapFile::addMpqAsset(const std::string & assetMpqFilePath, const std::vector<u8> & asset, WavQuality wavQuality)
{
    bool success = false;
    if ( openTemporaryMpq() )
    {
        ModifiedAsset modifiedAsset = ModifiedAsset(assetMpqFilePath, AssetAction::Add, wavQuality);
        const std::string & tempMpqPath = modifiedAsset.assetTempMpqPath;
        if ( temporaryMpq.open(temporaryMpqPath, false, true) )
        {
            if ( temporaryMpq.addFile(modifiedAsset.assetTempMpqPath, asset) )
            {
                modifiedAssets.push_back(modifiedAsset);
                success = true;
            }
            else
                CHKD_ERR("Failed to add file!");
            
            temporaryMpq.setUpdatingListFile(true);
            temporaryMpq.save();
        }
        else
            CHKD_ERR("Failed to open temp file!\n\nThe file may be in use elsewhere.");
    }
    else
        CHKD_ERR("Failed to setup asset temporary storage!");

    return success;
}

void MapFile::removeMpqAsset(const std::string & assetMpqFilePath)
{
    auto recentlyAddedAsset = modifiedAssets.end();
    for ( auto asset = modifiedAssets.begin(); asset != modifiedAssets.end(); asset++ )
    {
        if ( asset->actionTaken == AssetAction::Remove )
            return; // Already scheduled for deletion, take no further action
        else if ( asset->assetMpqPath == assetMpqFilePath )
        {
            recentlyAddedAsset = asset;
            break;
        }
    }

    if ( recentlyAddedAsset != modifiedAssets.end() ) // Asset was added between last save and now, cancel its addition
    {
        // Get the temp mpq file path used for this asset
        const std::string tempMpqFilePath = recentlyAddedAsset->assetTempMpqPath;

        // Remove the asset from the list of assets to be added to the actual map file
        modifiedAssets.erase(recentlyAddedAsset);

        // Try to remove it from the temporary file for some short-term disk saving; ignore errors, if any
        if ( openTemporaryMpq() )
        {
            temporaryMpq.removeFile(tempMpqFilePath);
            temporaryMpq.save();
        }
    }
    else // The given file was not added recently, mark it for deletion at the next save
        modifiedAssets.push_back(ModifiedAsset(assetMpqFilePath, AssetAction::Remove));
}

std::optional<std::vector<u8>> MapFile::getMpqAsset(const std::string & assetMpqFilePath)
{
    bool success = false;
    for ( auto & asset : modifiedAssets ) // Check if it's a recently added asset
    {
        if ( asset.actionTaken == AssetAction::Add && asset.assetMpqPath.compare(assetMpqFilePath) == 0 ) // Asset was recently added
            return openTemporaryMpq() ? temporaryMpq.getFile(asset.assetTempMpqPath) : std::nullopt;
    }

    if ( MpqFile::open(mapFilePath, true, false) )
    {
        auto asset = MpqFile::getFile(assetMpqFilePath);
        MpqFile::close();
        return asset;
    }

    return std::nullopt;
}

bool MapFile::extractMpqAsset(const std::string & assetMpqFilePath, const std::string & systemFilePath)
{
    if ( auto asset = getMpqAsset(assetMpqFilePath) )
    {
        FILE* systemFile = std::fopen(systemFilePath.c_str(), "wb");
        fwrite(&asset.value()[0], (size_t)asset->size(), 1, systemFile);
        std::fclose(systemFile);
        return true;
    }
    return false;
}

std::optional<std::vector<u8>> MapFile::getSound(size_t stringId)
{
    if ( auto soundString = Scenario::getString<RawString>(stringId) )
        return getMpqAsset(*soundString);
    else
        return std::nullopt;
}

bool MapFile::addSound(size_t stringId)
{
    auto soundString = Scenario::getString<RawString>(stringId, Chk::StrScope::Game);
    if ( soundString && MpqFile::findFile(mapFilePath, *soundString) )
    {
        Scenario::addSound(stringId);
        return true;
    }
    return false;
}

bool MapFile::addSound(const std::string & srcFilePath, WavQuality wavQuality, bool virtualFile)
{
    std::string mpqSoundDirectory = GetStandardSoundDir();
    std::string mpqFilePath = makeMpqFilePath(mpqSoundDirectory, getSystemFileName(srcFilePath));
    return addSound(srcFilePath, mpqFilePath, wavQuality, virtualFile);
}

bool MapFile::addSound(const std::string & srcFilePath, const std::string & destMpqPath, WavQuality wavQuality, bool virtualFile)
{
    if ( virtualFile )
    {
        size_t soundStringId = Scenario::addString(RawString(srcFilePath), Chk::StrScope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::addSound(soundStringId);
            return true;
        }
    }
    else if ( addMpqAsset(srcFilePath, destMpqPath, wavQuality) ) // Add, Register
    {
        size_t soundStringId = Scenario::addString(RawString(destMpqPath), Chk::StrScope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::addSound(soundStringId);
            return true;
        }
        else
            removeMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::addSound(const std::string & destMpqPath, const std::vector<u8> & soundContents, WavQuality wavQuality)
{
    bool success = false;
    if ( addMpqAsset(destMpqPath, soundContents, wavQuality) )
    {
        size_t soundStringId = Scenario::addString(RawString(destMpqPath), Chk::StrScope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::addSound(soundStringId);
            return true;
        }
        else
            removeMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::removeSoundBySoundIndex(u16 soundIndex, bool removeIfUsed)
{
    size_t soundStringId = Scenario::getSoundStringId(soundIndex);
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::StrScope::Game);
        Scenario::setSoundStringId(soundIndex, 0);
        if ( soundString )
        {
            Scenario::deleteString(soundStringId);
            removeMpqAsset(*soundString);
        }
        return true;
    }
    return false;
}

bool MapFile::removeSoundByStringId(size_t soundStringId, bool removeIfUsed)
{
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::StrScope::Game);
        Scenario::deleteString(soundStringId);
        if ( soundString )
            removeMpqAsset(*soundString);

        return true;
    }
    return false;
}

SoundStatus MapFile::getSoundStatus(size_t soundStringId)
{
    if ( Scenario::stringUsed(soundStringId, Chk::StrScope::Either, Chk::StrScope::Editor) &&
        !Scenario::stringUsed(soundStringId, Chk::StrScope::Either, Chk::StrScope::Game) )
        return SoundStatus::NoMatch; // Extended strings are not used in SC and therefore never match
    else
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::StrScope::Game);
        if ( soundString )
        {
            for ( ModifiedAsset & modifiedAsset : modifiedAssets )
            {
                if ( modifiedAsset.actionTaken == AssetAction::Add && soundString->compare(modifiedAsset.assetMpqPath) == 0 )
                    return SoundStatus::PendingMatch;
            }

            HANDLE hMpq = NULL;
            if ( MpqFile::open(mapFilePath, true, false) )
            {
                SoundStatus soundStatus = SoundStatus::NoMatch;
                if ( MpqFile::findFile(*soundString) )
                    soundStatus = SoundStatus::CurrentMatch;
                else if ( isInVirtualSoundList(*soundString) )
                    soundStatus = SoundStatus::VirtualFile;
                MpqFile::close();
                return soundStatus;
            }
            else if ( isInVirtualSoundList(*soundString) )
                return SoundStatus::VirtualFile;
            else
                return SoundStatus::FileInUse;
        }
    }
    return SoundStatus::Unknown;
}

bool MapFile::getSoundStatuses(std::map<size_t/*stringId*/, SoundStatus> & outSoundStatus, bool includePureStringSounds)
{
    std::map<size_t/*stringId*/, u16/*soundIndex*/> soundMap;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
        soundMap.insert(std::pair<size_t, u16>(i, (u16)Scenario::getSoundStringId(i)));
    for ( size_t i=0; i<Scenario::numTriggers(); i++ )
    {
        const auto & trigger = Scenario::getTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                trigger.actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                trigger.actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<size_t, u16>((size_t)trigger.actions[actionIndex].soundStringId, u16_max));
            }
        }
    }
    for ( size_t i=0; i<Scenario::numBriefingTriggers(); i++ )
    {
        const auto & trigger = Scenario::getBriefingTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                trigger.actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                trigger.actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<size_t, u16>((size_t)trigger.actions[actionIndex].soundStringId, u16_max));
            }
        }
    }

    for ( auto entry : soundMap )
    {
        size_t soundStringId = entry.first;
        
        if ( Scenario::stringUsed(soundStringId, Chk::StrScope::Either, Chk::StrScope::Editor) &&
            !Scenario::stringUsed(soundStringId, Chk::StrScope::Either, Chk::StrScope::Game) )
        { // Extended strings are not used in SC and therefore never match
            outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatchExtended));
        }
        else if ( auto soundString = Scenario::getString<RawString>(soundStringId) )
        {
            for ( ModifiedAsset & modifiedAsset : modifiedAssets )
            {
                if ( modifiedAsset.actionTaken == AssetAction::Add && soundString->compare(modifiedAsset.assetMpqPath) == 0 )
                {
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::PendingMatch));
                    continue;
                }
            }

            if ( MapFile::open(mapFilePath, true, false) )
            {
                if ( MapFile::findFile(*soundString) )
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::CurrentMatch));
                else
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatch));

                MapFile::close();
            }
            else
                outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::FileInUse));
        }
    }
    return true;
}

bool MapFile::isInVirtualSoundList(const std::string & soundMpqPath) const
{
    size_t hash = MapFile::strHash(soundMpqPath);
    size_t numMatching = MapFile::virtualSoundTable.count(hash);
    if ( numMatching == 1 )
    {
        std::string & tableSoundPath = MapFile::virtualSoundTable.find(hash)->second;
        if ( soundMpqPath.compare(tableSoundPath) == 0 )
            return true;
    }
    else if ( numMatching > 1 )
    {
        auto range = MapFile::virtualSoundTable.equal_range(hash);
        for ( auto pair = range.first; pair != range.second; ++ pair)
        {
            if ( soundMpqPath.compare(pair->second) == 0 )
                return true;
        }
    }
    return false;
}

std::string MapFile::getFileName() const
{
    return getSystemFileName(mapFilePath);
}

const std::string & MapFile::getFilePath() const
{
    return mapFilePath;
}

bool MapFile::getSaveDetails(SaveType & saveType, std::string & saveFilePath, bool & overwriting, FileBrowserPtr<SaveType> fileBrowser) const
{
    if ( fileBrowser != nullptr )
    {
        SaveType newSaveType = saveType;
        std::string newSaveFilePath;
        while ( fileBrowser->browseForSavePath(newSaveFilePath, newSaveType) )
        {
            std::string extension = getSystemFileExtension(newSaveFilePath);
            bool inferredExtension = extension.empty();
            if ( inferredExtension ) // No extension specified, infer based on the SaveType
            {
                if ( newSaveType == SaveType::StarCraftScm || newSaveType == SaveType::HybridScm )
                    newSaveFilePath += ".scm";
                else if ( newSaveType == SaveType::ExpansionScx || newSaveType == SaveType::RemasteredScx ||
                          newSaveType == SaveType::AllMaps || newSaveType == SaveType::AllFiles )
                {
                    newSaveFilePath += ".scx";
                    if ( newSaveType == SaveType::AllMaps || newSaveType == SaveType::AllFiles )
                        newSaveType = SaveType::RemasteredScx;
                }
                else if ( newSaveType == SaveType::StarCraftChk || newSaveType == SaveType::HybridChk || newSaveType == SaveType::ExpansionChk )
                    newSaveFilePath += ".chk";
                else
                    inferredExtension = false;
            }
            else // Extension specified, give it precedence over filterIndex (that is, update newSaveType if it's not already appropriate for the extension)
            {
                if ( extension == ".chk" && newSaveType != SaveType::StarCraftChk && newSaveType != SaveType::HybridChk &&
                     newSaveType != SaveType::ExpansionChk && newSaveType != SaveType::RemasteredChk && newSaveType != SaveType::AllChk )
                {
                    if ( newSaveType == SaveType::RemasteredScx )
                        newSaveType = SaveType::RemasteredChk;
                    else if ( newSaveType == SaveType::ExpansionScx )
                        newSaveType = SaveType::ExpansionChk;
                    else if ( newSaveType == SaveType::StarCraftScm )
                        newSaveType = SaveType::StarCraftChk;
                    else // Default to hybrid chk
                        newSaveType = SaveType::HybridChk;
                }
                else if ( extension == ".scm" && newSaveType != SaveType::StarCraftScm && newSaveType != SaveType::HybridScm )
                {
                    if ( newSaveType == SaveType::StarCraftChk )
                        newSaveType = SaveType::StarCraftScm;
                    else // Default to hybrid scm
                        newSaveType = SaveType::HybridScm;
                }
                else if ( extension == ".scx" && newSaveType != SaveType::ExpansionScx && newSaveType != SaveType::RemasteredScx )
                    newSaveType = SaveType::ExpansionScx;
                else if ( newSaveType == SaveType::AllMaps )
                    newSaveType = SaveType::RemasteredScx; // Default to remastered scx
            }

            bool fileExists = ::findFile(newSaveFilePath);
            bool mustConfirmOverwrite = fileExists && inferredExtension;
            if ( !mustConfirmOverwrite || fileBrowser->confirmOverwrite(getSystemFileName(newSaveFilePath) + " already exists.\nDo you want to replace it?") )
            { // Either the save path was complete during browsing and the browser checked
                saveType = newSaveType;
                saveFilePath = newSaveFilePath;
                overwriting = fileExists;
                return true;
            } // Else try browsing again
        }
    }
    return false;
}

std::string saveTypeToStr(const SaveType & saveType)
{
    switch ( saveType )
    {
        case SaveType::StarCraftScm: return "StarCraft Map(*.scm)"; break;
        case SaveType::HybridScm: return "StarCraft Hybrid Map(*.scm)"; break;
        case SaveType::ExpansionScx: return "BroodWar Map(*.scx)"; break;
        case SaveType::RemasteredScx: return "StarCraft Remastered Map(*.scx)"; break;
        case SaveType::StarCraftChk: return "Raw StarCraft Map(*.chk)"; break;
        case SaveType::HybridChk: return "Raw StarCraft Hybrid Map(*.chk)"; break;
        case SaveType::ExpansionChk: return "Raw BroodWar Map(*.chk)"; break;
        case SaveType::RemasteredChk: return "Raw StarCraft Remastered Map(*.chk)"; break;
        case SaveType::AllScm: return "All Scm"; break;
        case SaveType::AllScx: return "All Scx"; break;
        case SaveType::AllChk: return "All Chk"; break;
        case SaveType::AllMaps: return "All Maps"; break;
        case SaveType::AllFiles: return "All Files"; break;
        default: return "Unknown"; break;
    }
}

std::vector<FilterEntry<SaveType>> getOpenMapFilters()
{
    return std::vector<FilterEntry<SaveType>> {
        FilterEntry<SaveType>(SaveType::AllScm, "*.scm", "StarCraft Maps"),
        FilterEntry<SaveType>(SaveType::AllScx, "*.scx", "BroodWar Maps"),
        FilterEntry<SaveType>(SaveType::AllChk, "*.chk", "Raw Scenario Files"),
        FilterEntry<SaveType>(SaveType::AllMaps, "*.scm;*.scx;*.chk", "All Maps")
    };
}
std::vector<FilterEntry<SaveType>> getSaveMapFilters()
{
    return std::vector<FilterEntry<SaveType>> {
        FilterEntry<SaveType>(SaveType::StarCraftScm, "*.scm", "StarCraft Map(*.scm)", ".scm"),
        FilterEntry<SaveType>(SaveType::HybridScm, "*.scm", "StarCraft Hybrid Map(*.scm)", ".scm"),
        FilterEntry<SaveType>(SaveType::ExpansionScx, "*.scx", "BroodWar Map(*.scx)", ".scx"),
        FilterEntry<SaveType>(SaveType::RemasteredScx, "*.scx", "StarCraft Remastered Map(*.scx)", ".scx"),
        FilterEntry<SaveType>(SaveType::StarCraftChk, "*.chk", "Raw StarCraft Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::HybridChk, "*.chk", "Raw StarCraft Hybrid Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::ExpansionChk, "*.chk", "Raw BroodWar Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::RemasteredChk, "*.chk", "Raw StarCraft Remastered Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::AllMaps, "*.scm;*.scx;*.chk", "All Maps", ".scx"),
        FilterEntry<SaveType>(SaveType::AllFiles, "*", "All Files")
    };
}
std::vector<FilterEntry<u32>> getSaveTextFilters()
{
    return std::vector<FilterEntry<u32>> {
        FilterEntry<u32>("*.txt", "Text Documents(*.txt)", ".txt"),
        FilterEntry<u32>("*", "All Files")
    };
}
std::vector<FilterEntry<u32>> getSoundFilters()
{
    return std::vector<FilterEntry<u32>> {
        FilterEntry<u32>("*.wav", "Waveform Audio(*.wav)"),
        FilterEntry<u32>("*.ogg", "Ogg Audio(*.ogg)")
    };
}
std::vector<FilterEntry<u32>> getSaveSoundFilters()
{
    return std::vector<FilterEntry<u32>> {
        FilterEntry<u32>("*.wav", "Waveform Audio(*.wav)", ".wav"),
        FilterEntry<u32>("*.ogg", "Ogg Audio(*.ogg)", ".ogg")
    };
}
