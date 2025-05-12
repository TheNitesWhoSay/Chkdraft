#include <cross_cut/logger.h>
#include <cross_cut/simple_icu.h>
#include "lite_map_file.h"
#include "system_io.h"
#include "escape_strings.h"
#include <cstdio>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <chrono>

extern Logger logger;

#undef PlaySound

std::map<size_t, std::string> LiteMapFile::virtualSoundTable;
u64 LiteMapFile::nextAssetFileId(0);

FileBrowserPtr<SaveType> LiteMapFile::getDefaultOpenMapBrowser()
{
    return FileBrowserPtr<SaveType>(new FileBrowser<SaveType>(getOpenMapFilters(), "Open Map", true, false));
}

FileBrowserPtr<SaveType> LiteMapFile::getDefaultSaveMapBrowser()
{
    return FileBrowserPtr<SaveType>(new FileBrowser<SaveType>(getSaveMapFilters(), "Save Map", false, true));
}

LiteMapFile::LiteMapFile(const std::string & filePath) : temporaryMpqPath(""), temporaryMpq(true, true)
{
    initializeVirtualSoundTable();
    load(filePath);
}

LiteMapFile::LiteMapFile(FileBrowserPtr<SaveType> fileBrowser) : temporaryMpqPath(""), temporaryMpq(true, true)
{
    initializeVirtualSoundTable();
    load(fileBrowser);
}

LiteMapFile::LiteMapFile(Sc::Terrain::Tileset tileset, u16 width, u16 height)
    : LiteScenario(tileset, width, height), temporaryMpqPath(""), temporaryMpq(true, true)
{
    initializeVirtualSoundTable();
}

LiteMapFile::LiteMapFile() : LiteScenario(), temporaryMpqPath(""), temporaryMpq(true, true)
{

}

bool LiteMapFile::load(const std::string & filePath)
{
    return !filePath.empty() && openMapFile(filePath);
}

bool LiteMapFile::load(FileBrowserPtr<SaveType> fileBrowser)
{
    std::string browseFilePath = "";
    SaveType saveType = SaveType::Unknown;
    return fileBrowser != nullptr && fileBrowser->browseForOpenPath(browseFilePath, saveType) && openMapFile(browseFilePath);
}

bool LiteMapFile::save(const std::string & saveFilePath, bool overwriting, bool updateListFile, bool lockAnywhere, bool autoDefragmentLocations)
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
            versionCorrect = LiteScenario::changeVersionTo(Chk::Version::StarCraft_Original);
        else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
            versionCorrect = LiteScenario::changeVersionTo(Chk::Version::StarCraft_Hybrid);
        else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk || saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
            versionCorrect = LiteScenario::changeVersionTo(Chk::Version::StarCraft_BroodWar);
        else if ( saveType == SaveType::RemasteredScx || saveType == SaveType::RemasteredChk || saveType == SaveType::AllMaps ) // Remastered Map, edit to match
            versionCorrect = LiteScenario::changeVersionTo(Chk::Version::StarCraft_Remastered);

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
                LiteScenario::write(chk);
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
                    LiteScenario::write(outFile);
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

bool liteIsChkSaveType(SaveType saveType)
{
    return saveType == SaveType::AllChk || saveType == SaveType::RemasteredChk ||
        saveType == SaveType::ExpansionChk || saveType == SaveType::HybridChk ||
        saveType == SaveType::StarCraftChk;
}

bool LiteMapFile::save(bool saveAs, bool updateListFile, FileBrowserPtr<SaveType> fileBrowser, bool lockAnywhere, bool autoDefragmentLocations)
{
    if ( saveAs && mapFilePath.empty() )
        saveAs = false; // This is a new map or a map without a path, use regular save operation

    if ( isProtected() )
        CHKD_ERR("Cannot save protected maps!");
    else
    {
        // If scenario changed such that save type should have changed, adjust it in advance so filter type is correct
        if ( LiteScenario::getVersion() >= Chk::Version::StarCraft_Remastered )
            saveType = liteIsChkSaveType(saveType) ? SaveType::RemasteredChk : SaveType::RemasteredScx;
        else if ( LiteScenario::getVersion() >= Chk::Version::StarCraft_BroodWar )
            saveType = liteIsChkSaveType(saveType) ? SaveType::ExpansionChk : SaveType::ExpansionScx;
        else if ( LiteScenario::getVersion() >= Chk::Version::StarCraft_Hybrid )
            saveType = liteIsChkSaveType(saveType) ? SaveType::HybridChk : SaveType::HybridScm;
        else // version < Chk::Version::StarCraft_Hybrid
            saveType = liteIsChkSaveType(saveType) ? SaveType::StarCraftChk : SaveType::StarCraftScm;

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

bool LiteMapFile::openTemporaryMpq()
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
        } catch ( ... ) {
            CHKD_ERR("Failed to setup temporary asset storage, out of memory!");
        }
    }
    else
        CHKD_ERR("Failed to setup temporary asset storage, an appropriate file path could not be found!");

    return false;
}

bool LiteMapFile::openMapFile(const std::string & filePath)
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
                    if ( LiteScenario::read(chk) )
                    {
                        if ( LiteScenario::isOriginal() )
                            saveType = SaveType::StarCraftScm; // Vanilla
                        else if ( LiteScenario::isHybrid() )
                            saveType = SaveType::HybridScm; // Hybrid
                        else if ( LiteScenario::isExpansion() )
                            saveType = SaveType::ExpansionScx; // Expansion
                        else if ( LiteScenario::isRemastered() )
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
            else if ( ::lastErrorIndicatedBadFormat() )
                CHKD_ERR("Selected file was not a valid MPQ");
            else
                CHKD_ERR("%d", ::getLastError());
        }
        else if ( extension == ".chk" )
        {
            this->mapFilePath = filePath;
            std::ifstream chk(std::filesystem::path(asUtf8(filePath)), std::ios_base::binary|std::ios_base::in);
            if ( LiteScenario::read(chk) )
            {
                if ( LiteScenario::isOriginal() )
                    saveType = SaveType::StarCraftChk; // Vanilla chk
                else if ( LiteScenario::isHybrid() )
                    saveType = SaveType::HybridChk; // Hybrid chk
                else if ( LiteScenario::isExpansion() )
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

bool LiteMapFile::processModifiedAssets(bool updateListFile)
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

void LiteMapFile::initializeVirtualSoundTable()
{
    if ( LiteMapFile::virtualSoundTable.size() == 0 )
    {
        size_t numVirtualSounds = Sc::Sound::virtualSoundPaths.size();
        for ( size_t i=0; i<numVirtualSounds; i++ )
        {
            std::string soundPath(Sc::Sound::virtualSoundPaths[i]);
            size_t hash = std::hash<std::string>{}(soundPath);
            virtualSoundTable.insert(std::pair<size_t, std::string>(hash, soundPath));
        }
    }
}

std::string LiteMapFile::GetStandardSoundDir()
{
    return "staredit\\wav\\";
}

bool LiteMapFile::addMpqAsset(const std::string & assetSystemFilePath, const std::string & assetMpqFilePath, WavQuality wavQuality)
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

bool LiteMapFile::addMpqAsset(const std::string & assetMpqFilePath, const std::vector<u8> & asset, WavQuality wavQuality)
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

void LiteMapFile::removeMpqAsset(const std::string & assetMpqFilePath)
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

std::optional<std::vector<u8>> LiteMapFile::getMpqAsset(const std::string & assetMpqFilePath)
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

bool LiteMapFile::extractMpqAsset(const std::string & assetMpqFilePath, const std::string & systemFilePath)
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

std::optional<std::vector<u8>> LiteMapFile::getSound(size_t stringId)
{
    if ( auto soundString = LiteScenario::getString<RawString>(stringId) )
        return getMpqAsset(*soundString);
    else
        return std::nullopt;
}

bool LiteMapFile::addSound(size_t stringId)
{
    auto soundString = LiteScenario::getString<RawString>(stringId, Chk::Scope::Game);
    if ( soundString && MpqFile::findFile(mapFilePath, *soundString) )
    {
        LiteScenario::addSound(stringId);
        return true;
    }
    return false;
}

bool LiteMapFile::addSound(const std::string & srcFilePath, WavQuality wavQuality, bool virtualFile)
{
    std::string mpqSoundDirectory = GetStandardSoundDir();
    std::string mpqFilePath = makeArchiveFilePath(mpqSoundDirectory, getSystemFileName(srcFilePath));
    return addSound(srcFilePath, mpqFilePath, wavQuality, virtualFile);
}

bool LiteMapFile::addSound(const std::string & srcFilePath, const std::string & destMpqPath, WavQuality wavQuality, bool virtualFile)
{
    if ( virtualFile )
    {
        size_t soundStringId = LiteScenario::addString(RawString(srcFilePath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            LiteScenario::addSound(soundStringId);
            return true;
        }
    }
    else if ( addMpqAsset(srcFilePath, destMpqPath, wavQuality) ) // Add, Register
    {
        size_t soundStringId = LiteScenario::addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            LiteScenario::addSound(soundStringId);
            return true;
        }
        else
            removeMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool LiteMapFile::addSound(const std::string & destMpqPath, const std::vector<u8> & soundContents, WavQuality wavQuality)
{
    bool success = false;
    if ( addMpqAsset(destMpqPath, soundContents, wavQuality) )
    {
        size_t soundStringId = LiteScenario::addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            LiteScenario::addSound(soundStringId);
            return true;
        }
        else
            removeMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool LiteMapFile::removeSoundBySoundIndex(u16 soundIndex, bool removeIfUsed)
{
    size_t soundStringId = LiteScenario::getSoundStringId(soundIndex);
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = LiteScenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        LiteScenario::setSoundStringId(soundIndex, 0);
        if ( soundString )
        {
            LiteScenario::deleteString(soundStringId);
            removeMpqAsset(*soundString);
        }
        return true;
    }
    return false;
}

bool LiteMapFile::removeSoundByStringId(size_t soundStringId, bool removeIfUsed)
{
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = LiteScenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        LiteScenario::deleteString(soundStringId);
        if ( soundString )
            removeMpqAsset(*soundString);

        return true;
    }
    return false;
}

SoundStatus LiteMapFile::getSoundStatus(size_t soundStringId)
{
    if ( LiteScenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
        !LiteScenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        return SoundStatus::NoMatch; // Extended strings are not used in SC and therefore never match
    else
    {
        auto soundString = LiteScenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        if ( soundString )
        {
            for ( const ModifiedAsset & modifiedAsset : modifiedAssets )
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

bool LiteMapFile::getSoundStatuses(std::map<size_t/*stringId*/, SoundStatus> & outSoundStatus, bool includePureStringSounds)
{
    std::map<size_t/*stringId*/, u16/*soundIndex*/> soundMap;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
        soundMap.insert(std::pair<size_t, u16>(i, (u16)LiteScenario::getSoundStringId(i)));
    for ( size_t i=0; i<LiteScenario::numTriggers(); i++ )
    {
        const auto & trigger = LiteScenario::getTrigger(i);
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
    for ( size_t i=0; i<LiteScenario::numBriefingTriggers(); i++ )
    {
        const auto & trigger = LiteScenario::getBriefingTrigger(i);
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
        
        if ( LiteScenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
            !LiteScenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        { // Extended strings are not used in SC and therefore never match
            outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatchExtended));
        }
        else if ( auto soundString = LiteScenario::getString<RawString>(soundStringId) )
        {
            for ( const ModifiedAsset & modifiedAsset : modifiedAssets )
            {
                if ( modifiedAsset.actionTaken == AssetAction::Add && soundString->compare(modifiedAsset.assetMpqPath) == 0 )
                {
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::PendingMatch));
                    continue;
                }
            }

            if ( LiteMapFile::open(mapFilePath, true, false) )
            {
                if ( LiteMapFile::findFile(*soundString) )
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::CurrentMatch));
                else
                    outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatch));

                LiteMapFile::close();
            }
            else
                outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::FileInUse));
        }
    }
    return true;
}

bool LiteMapFile::isInVirtualSoundList(const std::string & soundMpqPath) const
{
    size_t hash = std::hash<std::string>{}(soundMpqPath);
    size_t numMatching = LiteMapFile::virtualSoundTable.count(hash);
    if ( numMatching == 1 )
    {
        std::string & tableSoundPath = LiteMapFile::virtualSoundTable.find(hash)->second;
        if ( soundMpqPath.compare(tableSoundPath) == 0 )
            return true;
    }
    else if ( numMatching > 1 )
    {
        auto range = LiteMapFile::virtualSoundTable.equal_range(hash);
        for ( auto pair = range.first; pair != range.second; ++ pair)
        {
            if ( soundMpqPath.compare(pair->second) == 0 )
                return true;
        }
    }
    return false;
}

std::string LiteMapFile::getFileName() const
{
    return getSystemFileName(mapFilePath);
}

const std::string & LiteMapFile::getFilePath() const
{
    return mapFilePath;
}

bool LiteMapFile::getSaveDetails(SaveType & saveType, std::string & saveFilePath, bool & overwriting, FileBrowserPtr<SaveType> fileBrowser) const
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
