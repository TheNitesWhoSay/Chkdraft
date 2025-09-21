#include "cross_cut/logger.h"
#include "cross_cut/simple_icu.h"
#include "map_file.h"
#include "system_io.h"
#include "escape_strings.h"
#include <cstdio>
#include <cstdarg>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <sstream>
#include <chrono>

extern Logger logger;

#undef PlaySound

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

MapFile::MapFile(const std::string & filePath) : MpqAssetManager()
{
    initializeVirtualSoundTable();
    load(filePath);
}

MapFile::MapFile(FileBrowserPtr<SaveType> fileBrowser) : MpqAssetManager()
{
    initializeVirtualSoundTable();
    load(fileBrowser);
}

MapFile::MapFile(Sc::Terrain::Tileset tileset, u16 width, u16 height, size_t terrainTypeIndex, SaveType saveType, const Sc::Terrain::Tiles* tilesetData)
    : Scenario(tileset, width, height, terrainTypeIndex, saveType, tilesetData), MpqAssetManager()
{
    initializeVirtualSoundTable();
}

MapFile::MapFile() : Scenario(), MpqAssetManager()
{

}

bool MapFile::load(const std::string & filePath)
{
    return !filePath.empty() && openMapFile(filePath);
}

bool MapFile::load(FileBrowserPtr<SaveType> fileBrowser)
{
    std::string browseFilePath = "";
    SaveType saveType = SaveType::Unknown;
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
        logger.info() << "Saving to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(read.saveType) << "\"" << std::endl;
        auto start = std::chrono::high_resolution_clock::now();
        bool versionCorrect = true;
        if ( read.saveType == SaveType::StarCraftScm || read.saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Original);
        else if ( read.saveType == SaveType::HybridScm || read.saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Hybrid);
        else if ( read.saveType == SaveType::ExpansionScx || read.saveType == SaveType::ExpansionChk || read.saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_BroodWar);
        else if ( read.saveType == SaveType::RemasteredScx || read.saveType == SaveType::RemasteredChk || read.saveType == SaveType::AllMaps ) // Remastered Map, edit to match
            versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Remastered);

        if ( !versionCorrect )
        {
            logger.error("Failed to update version, save failed!");
            return false;
        }

        updateSaveSections();

        if ( (read.saveType == SaveType::StarCraftScm || read.saveType == SaveType::HybridScm ||
              read.saveType == SaveType::ExpansionScx || read.saveType == SaveType::RemasteredScx) || read.saveType == SaveType::AllMaps ) // Must be packed into an MPQ
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
                        else if ( !MpqAssetManager::saveAssets((MpqFile &)(*this)) )
                        {
                            CHKD_ERR("Processing assets failed!");
                            success = false;
                        }

                        MpqFile::setUpdatingListFile(updateListFile);
                        MpqFile::close();
                        mapFilePath = saveFilePath;
                        
                        auto finish = std::chrono::high_resolution_clock::now();
                        logger.info() << "Successfully saved to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(read.saveType) << "\" in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
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
                        logger.info() << "Successfully saved to: " << saveFilePath << " with saveType: \"" << saveTypeToStr(read.saveType) << "\" in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
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
        auto edit = createAction(ActionDescriptor::UpdateSaveSections);
        // If scenario changed such that save type should have changed, adjust it in advance so filter type is correct
        SaveType newSaveType = read.saveType;
        if ( Scenario::getVersion() >= Chk::Version::StarCraft_Remastered || read.saveType == SaveType::RemasteredScx || read.saveType == SaveType::RemasteredChk )
            newSaveType = isChkSaveType(read.saveType) ? SaveType::RemasteredChk : SaveType::RemasteredScx;
        else if ( Scenario::getVersion() >= Chk::Version::StarCraft_BroodWar || read.saveType == SaveType::ExpansionScx || read.saveType == SaveType::ExpansionChk )
            newSaveType = isChkSaveType(read.saveType) ? SaveType::ExpansionChk : SaveType::ExpansionScx;
        else if ( Scenario::getVersion() >= Chk::Version::StarCraft_Hybrid || read.saveType == SaveType::HybridScm || read.saveType == SaveType::HybridChk )
            newSaveType = isChkSaveType(read.saveType) ? SaveType::HybridChk : SaveType::HybridScm;
        else // version < Chk::Version::StarCraft_Hybrid
            newSaveType = isChkSaveType(read.saveType) ? SaveType::StarCraftChk : SaveType::StarCraftScm;

        if ( newSaveType != read.saveType )
            edit->saveType = newSaveType;

        bool overwriting = false;
        std::string newMapFilePath;
        if ( (saveAs || mapFilePath.empty()) && fileBrowser != nullptr ) // saveAs specified or filePath not yet determined, and a fileBrowser is available
        {
            if ( getSaveDetails(newSaveType, newMapFilePath, overwriting, fileBrowser) && read.saveType != newSaveType )
                edit->saveType = newSaveType;
        }
        else if ( !saveAs )
            newMapFilePath = mapFilePath;

        if ( !newMapFilePath.empty() )
            return save(newMapFilePath, overwriting, updateListFile, lockAnywhere, autoDefragmentLocations);
    }
    return false;
}

std::string MapFile::getTemporaryMpqPath()
{
    if ( !MpqAssetManager::tempMpqPath.empty() )
        return MpqAssetManager::tempMpqPath;

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
        else // Use the C++ library to find an appropriate temporary location
        {
            std::error_code ec {};
            auto tempDirectoryPath = std::filesystem::temp_directory_path(ec);
            auto tempDirectoryPathStr = std::string((const char*)tempDirectoryPath.u8string().c_str());
            do
            {
                assetFilePath = makeSystemFilePath(tempDirectoryPathStr, std::to_string(nextAssetFileId) + ".mpq");
                nextAssetFileId ++;
            } while ( ::findFile(assetFilePath) ); // Try again if the file already exists
        }
    }

    return assetFilePath;
}

std::size_t MapFile::getCurrentActionIndex()
{
    return Tracked::getPendingActionIndex();
}

struct MpqAutoClose {
    MpqFile & mpqFile;
    ~MpqAutoClose() { mpqFile.close(); }
};

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
                MpqAutoClose autoCloseMpq { static_cast<MpqFile &>(*this) }; // Automatically calls MpqFile::close() when this goes out of scope
                this->mapFilePath = MpqFile::getFilePath();

                auto tryGetScenario = [&]() -> std::optional<std::string> {
                    try {
                    if ( auto chkData = MpqFile::getFile("staredit\\scenario.chk", false) )
                    {
                        std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                        std::copy(chkData->begin(), chkData->end(), std::ostream_iterator<u8>(chk));
                        if ( Scenario::parse(chk, true) )
                        {
                            auto finish = std::chrono::high_resolution_clock::now();
                            logger.info() << "Map " << mapFilePath << " opened in " << std::chrono::duration_cast<std::chrono::milliseconds>(finish-start).count() << "ms" << std::endl;
                            return std::nullopt; // No error
                        }
                        else
                            return std::make_optional(std::string("Invalid or missing Scenario file."));
                    }
                    else
                        return std::make_optional(std::string("Invalid or missing Scenario file."));
                    } catch ( std::exception & e ) {
                        return std::make_optional(std::string("Exception occurred while parsing scenario ") + e.what());
                    } catch ( ... ) {
                        return std::make_optional(std::string("Unknown exception occurred while parsing scenario"));
                    }
                };

                auto result = tryGetScenario();
                if ( result == std::nullopt ) // No error to report
                    return true;
                else
                {
                    auto prevLocale = MpqFile::getLocale();
                    auto locales = MpqFile::getLocales("staredit\\scenario.chk");
                    for ( auto locale : locales )
                    {
                        MpqFile::setLocale(locale);
                        if ( tryGetScenario() == std::nullopt ) // No error to report
                        {
                            MpqFile::setLocale(prevLocale); // Restore original locale
                            Scenario::setProtected();
                            return true;
                        }
                    }
                    MpqFile::setLocale(prevLocale); // Restore original locale
                    CHKD_ERR(*result); // All locales failed, report the original error
                    return false;
                }
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
            if ( Scenario::parse(chk, false) )
            {
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

void MapFile::initializeVirtualSoundTable()
{
    if ( MapFile::virtualSoundTable.size() == 0 )
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

std::string MapFile::GetStandardSoundDir()
{
    return "staredit\\wav\\";
}

std::optional<std::vector<u8>> MapFile::getMpqAsset(const std::string & assetMpqFilePath)
{
    if ( auto found = MpqAssetManager::getRecentlyModdedAsset(assetMpqFilePath) )
        return found;
    else if ( MpqFile::open(mapFilePath, true, false) )
    {
        auto asset = MpqFile::getFile(assetMpqFilePath);
        MpqFile::close();
        return asset;
    }
    return std::nullopt;
}

std::optional<std::vector<std::string>> MapFile::getListfile()
{
    if ( MpqFile::isOpen() )
        return MpqFile::getListfile();
    else if ( MpqFile::open(mapFilePath, true, false) )
    {
        auto result = MpqFile::getListfile();
        MpqFile::close();
        return result;
    }
    else
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

std::optional<std::vector<u8>> MapFile::getSound(const std::string & soundPath)
{
    if ( !soundPath.empty() )
        return getMpqAsset(soundPath);
    else
        return std::nullopt;
}

bool MapFile::addSound(size_t stringId)
{
    auto soundString = Scenario::getString<RawString>(stringId, Chk::Scope::Game);
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
    std::string mpqFilePath = makeArchiveFilePath(mpqSoundDirectory, getSystemFileName(srcFilePath));
    return addSound(srcFilePath, mpqFilePath, wavQuality, virtualFile);
}

bool MapFile::addSound(const std::string & srcFilePath, const std::string & destMpqPath, WavQuality wavQuality, bool virtualFile)
{
    auto edit = createAction(ActionDescriptor::AddSound);
    if ( virtualFile )
    {
        size_t soundStringId = Scenario::addString(RawString(srcFilePath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
            return Scenario::addSound(soundStringId) != Chk::TotalSounds;
    }
    else if ( auto result = MpqAssetManager::addAsset(destMpqPath, srcFilePath, wavQuality) ) // Add, Register
    {
        size_t soundStringId = Scenario::addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::addSound(soundStringId);
            return result;
        }
        else
            MpqAssetManager::removeAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::addSound(const std::string & destMpqPath, const std::vector<u8> & soundContents, WavQuality wavQuality)
{
    bool success = false;
    if ( auto result = MpqAssetManager::addAsset(destMpqPath, soundContents, wavQuality) )
    {
        size_t soundStringId = Scenario::addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::addSound(soundStringId);
            return result;
        }
        else
            MpqAssetManager::removeAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::removeSoundBySoundIndex(u16 soundIndex, bool removeIfUsed)
{
    bool result = false;
    size_t soundStringId = Scenario::getSoundStringId(soundIndex);
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        Scenario::setSoundStringId(soundIndex, 0);
        if ( soundString )
        {
            result = MpqAssetManager::removeAsset(*soundString);
            Scenario::deleteString(soundStringId);
        }
    }
    return result;
}

bool MapFile::removeSoundByStringId(size_t soundStringId, bool removeIfUsed)
{
    bool result = false;
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        Scenario::deleteSoundReferences(soundStringId);
        Scenario::deleteString(soundStringId, Chk::Scope::Both, true);
        if ( soundString )
            result = MpqAssetManager::removeAsset(*soundString);
    }
    return result;
}

SoundStatus MapFile::getSoundStatus(size_t soundStringId)
{
    if ( soundStringId == Chk::StringId::NoString )
        return SoundStatus::NoMatch;
    else if ( Scenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
        !Scenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        return SoundStatus::NoMatch; // Extended strings are not used in SC and therefore never match
    else
    {
        auto soundString = Scenario::getString<RawString>(soundStringId, Chk::Scope::Game);
        if ( soundString )
        {
            if ( auto found = MpqAssetManager::getRecentlyModdedAsset(*soundString) )
                return SoundStatus::PendingMatch;

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
        
        if ( Scenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
            !Scenario::stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        { // Extended strings are not used in SC and therefore never match
            outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatchExtended));
        }
        else if ( auto soundString = Scenario::getString<RawString>(soundStringId) )
        {
            if ( auto found = MpqAssetManager::getRecentlyModdedAsset(*soundString) )
                outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::PendingMatch));

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
    size_t hash = std::hash<std::string>{}(soundMpqPath);
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
