#include "MapFile.h"
#include "SystemIO.h"
#include "EscapeStrings.h"
#include <cstdio>
#include <cstdarg>
#include <SimpleIcu.h>
#include <sstream>

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
    LoadMapFile(filePath);
}

MapFile::MapFile(FileBrowserPtr<SaveType> fileBrowser) :
    saveType(SaveType::Unknown), mapFilePath(""), temporaryMpqPath(""), temporaryMpq(true, true)
{
    LoadMapFile(fileBrowser);
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

bool MapFile::LoadMapFile(const std::string &filePath)
{
    return !filePath.empty() && OpenMapFile(filePath);
}

bool MapFile::LoadMapFile(FileBrowserPtr<SaveType> fileBrowser)
{
    std::string browseFilePath = "";
    SaveType saveType;
    return fileBrowser != nullptr && fileBrowser->browseForOpenPath(browseFilePath, saveType) && OpenMapFile(browseFilePath);
}

bool MapFile::SaveFile(bool saveAs, bool updateListFile, FileBrowserPtr<SaveType> fileBrowser, bool lockAnywhere, bool autoDefragmentLocations)
{
    std::string prevFilePath(mapFilePath);
    if ( saveAs && prevFilePath.empty() )
        saveAs = false; // This is a new map or a map without a path, use regular save operation

    if ( isProtected() )
        CHKD_ERR("Cannot save protected maps!");
    else
    {
        bool overwriting = true;

        if ( (saveAs || mapFilePath.empty()) && fileBrowser != nullptr ) // saveAs specified or filePath not yet determined, and a fileBrowser is available
            getSaveDetails(saveType, mapFilePath, overwriting, fileBrowser);

        if ( !mapFilePath.empty() ) // Map path has been determined
        {
            logger.info() << "Saving to: " << mapFilePath << " with saveType: " << (int)saveType << std::endl;
            bool versionCorrect = true;
            if ( saveType == SaveType::StarCraftScm || saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
                versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Original);
            else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
                versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_Hybrid);
            else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk || saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
                versionCorrect = Scenario::changeVersionTo(Chk::Version::StarCraft_BroodWar);

            if ( !versionCorrect )
            {
                logger.error("Failed to update version, save failed!");
                return false;
            }

            if ( (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm || saveType == SaveType::ExpansionScx) || saveType == SaveType::AllMaps ) // Must be packed into an MPQ
            {
                if ( !saveAs || (saveAs && MakeFileCopy(prevFilePath, mapFilePath)) ) // If using save-as, copy the existing mpq to the new location
                {
                    std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                    Scenario::Write(chk);
                    if ( chk.good() )
                    {
                        if ( MpqFile::open(mapFilePath, false, true) )
                        {
                            if ( !MpqFile::addFile("staredit\\scenario.chk", chk) )
                                CHKD_ERR("Failed to add scenario file!");

                            if ( !ProcessModifiedAssets(updateListFile) )
                                CHKD_ERR("Processing assets failed!");

                            MpqFile::setUpdatingListFile(updateListFile);
                            MpqFile::close();
                            return true;
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
                if ( RemoveFile(mapFilePath) ) // Remove any existing files of the same name
                {
                    std::ofstream outFile(icux::toFilestring(mapFilePath).c_str(), std::ios_base::out|std::ios_base::binary);
                    if ( outFile.is_open() )
                    {
                        Scenario::Write(outFile);
                        bool success = outFile.good();
                        if ( !success )
                            logger.error("Failed to write scenario file!");

                        return success;
                    }
                }
                CHKD_ERR("Failed to create the new map file!");
            }
        }
    }
    return false;
}

bool MapFile::OpenTemporaryMpq()
{
    if ( temporaryMpq.isOpen() )
        return true;
    else if ( !temporaryMpqPath.empty() )
        return temporaryMpq.open(temporaryMpqPath, false, true);

    std::string assetFileDirectory("");
    std::string assetFilePath("");
#ifdef CHKDRAFT
    // If this is compiled as part of Chkdraft, use the Pre-Save directory
    if ( GetPreSavePath(assetFileDirectory) )
    {
        do
        {
            // Try the nextAssetFileId filename
            assetFilePath = MakeSystemFilePath(assetFileDirectory, std::to_string(nextAssetFileId) + ".mpq");
            nextAssetFileId ++;
        }
        while ( FindFile(assetFilePath) ); // Try again if the file already exists
    }
#endif

    if ( assetFilePath.length() <= 0 ) // If this is not Chkdraft or you could not get the Pre-Save directory
    {
        if ( mapFilePath.length() > 0 ) // Use the same directory as the map file, if the map file already has a directory
        {
            do
            {
                assetFileDirectory = GetSystemFileDirectory(mapFilePath, true);
                assetFilePath = MakeSystemFilePath(assetFileDirectory, std::to_string(nextAssetFileId) + ".mpq");
            } while ( FindFile(assetFilePath) ); // Try again if the file already exists
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

bool MapFile::OpenMapFile(const std::string &filePath)
{
    std::string extension = GetSystemFileExtension(filePath);
    if ( !extension.empty() )
    {
        if ( extension == ".scm" || extension == ".scx" )
        {
            if ( MpqFile::open(filePath, true, false) )
            {
                this->mapFilePath = MpqFile::getFilePath();
                std::vector<u8> chkData;
                if ( !MpqFile::getFile("staredit\\scenario.chk", chkData) )
                    CHKD_ERR("Failed to get scenario file from MPQ.");
                
                MpqFile::close();

                std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
                std::copy(chkData.begin(), chkData.end(), std::ostream_iterator<u8>(chk));
                if ( Scenario::ParseScenario(chk) )
                {
                    if ( Scenario::versions.isOriginal() )
                        saveType = SaveType::StarCraftScm; // Vanilla
                    else if ( Scenario::versions.isHybrid() )
                        saveType = SaveType::HybridScm; // Hybrid
                    else
                        saveType = SaveType::ExpansionScx; // Expansion

                    return true;
                }
                else
                    CHKD_ERR("Invalid or missing Scenario file.");
            }
            else if ( GetLastError() == ERROR_FILE_NOT_FOUND )
                CHKD_ERR("File Not Found");
            else
                CHKD_ERR("%d", GetLastError());
        }
        else if ( extension == ".chk" )
        {
            std::ifstream chk(filePath);
            if ( Scenario::ParseScenario(chk) )
            {
                if ( Scenario::versions.isOriginal() )
                    saveType = SaveType::StarCraftChk; // Vanilla chk
                else if ( Scenario::versions.isHybrid() )
                    saveType = SaveType::HybridChk; // Hybrid chk
                else
                    saveType = SaveType::ExpansionChk; // Expansion chk
                    
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

bool MapFile::ProcessModifiedAssets(bool updateListFile)
{
    std::vector<std::vector<ModifiedAssetPtr>::iterator> processedAssets;
    if ( OpenTemporaryMpq() )
    {
        for ( auto modifiedAsset = modifiedAssets.begin(); modifiedAsset != modifiedAssets.end(); ++modifiedAsset )
        {
            const std::string &assetMpqPath = (*modifiedAsset)->assetMpqPath;
            if ( (*modifiedAsset)->actionTaken == AssetAction::Add )
            {
                buffer assetBuffer = buffer("TmpA");
                if ( temporaryMpq.getFile((*modifiedAsset)->assetTempMpqPath, assetBuffer) )
                {
                    temporaryMpq.removeFile((*modifiedAsset)->assetTempMpqPath);
                    if ( MpqFile::addFile(assetMpqPath, assetBuffer, (*modifiedAsset)->wavQualitySelected) )
                        processedAssets.push_back(modifiedAsset);
                    else
                        CHKD_ERR("Failed to save %s to destination file", assetMpqPath.c_str());
                }
                else
                    CHKD_ERR("Failed to read in temporary asset!");
            }
            else if ( (*modifiedAsset)->actionTaken == AssetAction::Remove )
            {
                if ( MpqFile::removeFile(assetMpqPath) )
                    processedAssets.push_back(modifiedAsset);
                else
                    CHKD_ERR("Failed to remove %s from map archive", assetMpqPath.c_str());
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

void MapFile::SetSaveType(SaveType newSaveType)
{
    saveType = newSaveType;
}

std::string MapFile::GetStandardSoundDir()
{
    return "staredit\\wav\\";
}

bool MapFile::AddMpqAsset(const std::string &assetSystemFilePath, const std::string &assetMpqFilePath, WavQuality wavQuality)
{
    bool success = false;
    if ( FindFile(assetSystemFilePath) )
    {
        if ( OpenTemporaryMpq() )
        {
            ModifiedAssetPtr modifiedAssetPtr = ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, AssetAction::Add, wavQuality));
            const std::string &tempMpqPath = modifiedAssetPtr->assetTempMpqPath;
            if ( temporaryMpq.addFile(tempMpqPath, assetSystemFilePath) )
            {
                modifiedAssets.push_back(modifiedAssetPtr);
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

bool MapFile::AddMpqAsset(const std::string &assetMpqFilePath, const buffer &asset, WavQuality wavQuality)
{
    bool success = false;
    if ( OpenTemporaryMpq() )
    {
        ModifiedAssetPtr modifiedAssetPtr = ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, AssetAction::Add, wavQuality));
        const std::string &tempMpqPath = modifiedAssetPtr->assetTempMpqPath;
        if ( temporaryMpq.open(temporaryMpqPath, false, true) )
        {
            if ( temporaryMpq.addFile(modifiedAssetPtr->assetTempMpqPath, asset) )
            {
                modifiedAssets.push_back(modifiedAssetPtr);
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

void MapFile::RemoveMpqAsset(const std::string &assetMpqFilePath)
{
    auto recentlyAddedAsset = modifiedAssets.end();
    for ( auto asset = modifiedAssets.begin(); asset != modifiedAssets.end(); asset++ )
    {
        if ( (*asset) != nullptr && (*asset)->assetMpqPath == assetMpqFilePath )
        {
            recentlyAddedAsset = asset;
            break;
        }
    }

    if ( recentlyAddedAsset != modifiedAssets.end() ) // Asset was added between last save and now, cancel its addition
    {
        // Get the temp mpq file path used for this asset
        const std::string tempMpqFilePath = (*recentlyAddedAsset)->assetTempMpqPath;

        // Remove the asset from the list of assets to be added to the actual map file
        modifiedAssets.erase(recentlyAddedAsset);

        // Try to remove it from the temporary file for some short-term disk saving; ignore errors, if any
        if ( OpenTemporaryMpq() )
        {
            temporaryMpq.removeFile(tempMpqFilePath);
            temporaryMpq.save();
        }
    }
    else // The given file was not added recently, mark it for deletion at the next save
        modifiedAssets.push_back(ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, AssetAction::Remove)));
}

bool MapFile::GetMpqAsset(const std::string &assetMpqFilePath, buffer &outAssetBuffer)
{
    bool success = false;
    for ( auto asset : modifiedAssets ) // Check if it's a recently added asset
    {
        if ( asset->actionTaken == AssetAction::Add && asset->assetMpqPath.compare(assetMpqFilePath) == 0 ) // Asset was recently added
            return OpenTemporaryMpq() && temporaryMpq.getFile(asset->assetTempMpqPath, outAssetBuffer);
    }

    if ( MpqFile::open(mapFilePath, true, false) )
    {
        success = MpqFile::getFile(assetMpqFilePath, outAssetBuffer);
        MpqFile::close();
    }

    return success;
}

bool MapFile::ExtractMpqAsset(const std::string &assetMpqFilePath, const std::string &systemFilePath)
{
    buffer assetBuffer("AsBu");
    if ( GetMpqAsset(assetMpqFilePath, assetBuffer) )
    {
        FILE* systemFile = std::fopen(systemFilePath.c_str(), "wb");
        assetBuffer.write(systemFile, false);
        std::fclose(systemFile);
        return true;
    }
    return false;
}

bool MapFile::GetSound(u16 soundIndex, size_t &outStringId)
{
    outStringId = Scenario::triggers.getSoundStringId(soundIndex);
    return outStringId != Chk::StringId::UnusedSound;
}

bool MapFile::GetSound(size_t stringId, buffer & outSoundData)
{
    RawStringPtr soundString = Scenario::strings.getString<RawString>(stringId);
    if ( soundString != nullptr )
        return GetMpqAsset(*soundString, outSoundData);
    else
        return false;
}

bool MapFile::AddSound(size_t stringId)
{
    RawStringPtr soundString = Scenario::strings.getString<RawString>(stringId, Chk::Scope::Game);
    if ( soundString != nullptr && MpqFile::findFile(mapFilePath, *soundString) )
    {
        Scenario::triggers.addSound(stringId);
        return true;
    }
    return false;
}

bool MapFile::AddSound(const std::string &srcFilePath, WavQuality wavQuality, bool virtualFile)
{
    std::string mpqSoundDirectory = GetStandardSoundDir();
    std::string mpqFilePath = MakeMpqFilePath(mpqSoundDirectory, GetSystemFileName(srcFilePath));
    return AddSound(srcFilePath, mpqFilePath, wavQuality, virtualFile);
}

bool MapFile::AddSound(const std::string &srcFilePath, const std::string &destMpqPath, WavQuality wavQuality, bool virtualFile)
{
    if ( virtualFile )
    {
        size_t soundStringId = Scenario::strings.addString(RawString(srcFilePath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::triggers.addSound(soundStringId);
            return true;
        }
    }
    else if ( AddMpqAsset(srcFilePath, destMpqPath, wavQuality) ) // Add, Register
    {
        size_t soundStringId = Scenario::strings.addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::triggers.addSound(soundStringId);
            return true;
        }
        else
            RemoveMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::AddSound(const std::string &destMpqPath, buffer &soundContents, WavQuality wavQuality)
{
    bool success = false;
    if ( AddMpqAsset(destMpqPath, soundContents, wavQuality) )
    {
        size_t soundStringId = Scenario::strings.addString(RawString(destMpqPath), Chk::Scope::Game);
        if ( soundStringId != Chk::StringId::NoString )
        {
            Scenario::triggers.addSound(soundStringId);
            return true;
        }
        else
            RemoveMpqAsset(destMpqPath); // Try to remove the sound, ignore errors if any
    }
    return false;
}

bool MapFile::RemoveSoundBySoundIndex(u16 soundIndex, bool removeIfUsed)
{
    size_t soundStringId = Scenario::triggers.getSoundStringId(soundIndex);
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        RawStringPtr soundString = Scenario::strings.getString<RawString>(soundStringId, Chk::Scope::Game);
        Scenario::triggers.setSoundStringId(soundIndex, 0);
        if ( soundString != nullptr )
        {
            Scenario::strings.deleteString(soundStringId);
            RemoveMpqAsset(*soundString);
        }
        return true;
    }
    return false;
}

bool MapFile::RemoveSoundByStringId(size_t soundStringId, bool removeIfUsed)
{
    if ( soundStringId != Chk::StringId::UnusedSound )
    {
        RawStringPtr soundString = Scenario::strings.getString<RawString>(soundStringId, Chk::Scope::Game);
        Scenario::strings.deleteString(soundStringId);
        if ( soundString != nullptr )
            RemoveMpqAsset(*soundString);

        return true;
    }
    return false;
}

SoundStatus MapFile::GetSoundStatus(size_t soundStringId)
{
    if ( Scenario::strings.stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
        !Scenario::strings.stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        return SoundStatus::NoMatch; // Extended strings are not used in SC and therefore never match
    else
    {
        RawStringPtr soundString = Scenario::strings.getString<RawString>(soundStringId, Chk::Scope::Game);
        if ( soundString != nullptr )
        {
            for ( ModifiedAssetPtr modifiedAsset : modifiedAssets )
            {
                if ( modifiedAsset->actionTaken == AssetAction::Add && soundString->compare(modifiedAsset->assetMpqPath) == 0 )
                    return SoundStatus::PendingMatch;
            }

            HANDLE hMpq = NULL;
            if ( MpqFile::open(mapFilePath, true, false) )
            {
                SoundStatus soundStatus = SoundStatus::NoMatch;
                if ( MpqFile::findFile(*soundString) )
                    soundStatus = SoundStatus::CurrentMatch;
                else if ( IsInVirtualSoundList(*soundString) )
                    soundStatus = SoundStatus::VirtualFile;
                MpqFile::close();
                return soundStatus;
            }
            else if ( IsInVirtualSoundList(*soundString) )
                return SoundStatus::VirtualFile;
            else
                return SoundStatus::FileInUse;
        }
    }
    return SoundStatus::Unknown;
}

bool MapFile::GetSoundStatusMap(std::map<size_t/*stringId*/, SoundStatus> &outSoundStatus, bool includePureStringSounds)
{
    std::map<size_t/*stringId*/, u16/*soundIndex*/> soundMap;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
        soundMap.insert(std::pair<size_t, u16>(i, (u16)Scenario::triggers.getSoundStringId(i)));
    for ( size_t i=0; i<Scenario::triggers.numTriggers(); i++ )
    {
        Chk::TriggerPtr trigger = Scenario::triggers.getTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::PlaySound ||
                trigger->actions[actionIndex].actionType == Chk::Action::Type::Transmission) &&
                trigger->actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<size_t, u16>((size_t)trigger->actions[actionIndex].soundStringId, u16_max));
            }
        }
    }
    for ( size_t i=0; i<Scenario::triggers.numBriefingTriggers(); i++ )
    {
        Chk::TriggerPtr trigger = Scenario::triggers.getBriefingTrigger(i);
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            if ( (trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingPlaySound ||
                trigger->actions[actionIndex].actionType == Chk::Action::Type::BriefingTransmission) &&
                trigger->actions[actionIndex].soundStringId != Chk::StringId::NoString )
            {
                soundMap.insert(std::pair<size_t, u16>((size_t)trigger->actions[actionIndex].soundStringId, u16_max));
            }
        }
    }

    for ( auto entry : soundMap )
    {
        size_t soundStringId = entry.first;
        
        if ( Scenario::strings.stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Editor) &&
            !Scenario::strings.stringUsed(soundStringId, Chk::Scope::Either, Chk::Scope::Game) )
        { // Extended strings are not used in SC and therefore never match
            outSoundStatus.insert(std::pair<size_t, SoundStatus>(soundStringId, SoundStatus::NoMatchExtended));
        }
        else
        {
            RawStringPtr soundString = Scenario::strings.getString<RawString>(soundStringId);
            if ( soundString != nullptr )
            {
                for ( ModifiedAssetPtr modifiedAsset : modifiedAssets )
                {
                    if ( modifiedAsset->actionTaken == AssetAction::Add && soundString->compare(modifiedAsset->assetMpqPath) == 0 )
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
    }
    return true;
}

bool MapFile::IsInVirtualSoundList(const std::string &soundMpqPath)
{
    size_t hash = MapFile::strHash(soundMpqPath);
    size_t numMatching = MapFile::virtualSoundTable.count(hash);
    if ( numMatching == 1 )
    {
        std::string &tableSoundPath = MapFile::virtualSoundTable.find(hash)->second;
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

std::string MapFile::GetFileName()
{
    return GetSystemFileName(mapFilePath);

    std::string sFilePath(mapFilePath);
    std::string separator = GetSystemFileSeparator();
    auto lastBackslashPos = sFilePath.find_last_of('\\');

    if ( lastBackslashPos != std::string::npos && lastBackslashPos+1 < sFilePath.size() )
        return std::string(sFilePath.substr(lastBackslashPos+1, sFilePath.size() - lastBackslashPos+1));
    else
        return std::string("");
}

const std::string &MapFile::getFilePath() const
{
    return mapFilePath;
}

bool MapFile::getSaveDetails(inout_param SaveType &saveType, output_param std::string &saveFilePath, output_param bool &overwriting, FileBrowserPtr<SaveType> fileBrowser)
{
    if ( fileBrowser != nullptr )
    {
        SaveType newSaveType = saveType;
        std::string newSaveFilePath;
        while ( fileBrowser->browseForSavePath(newSaveFilePath, newSaveType) )
        {
            std::string extension = GetSystemFileExtension(newSaveFilePath);
            bool inferredExtension = extension.empty();
            if ( inferredExtension ) // No extension specified, infer based on the SaveType
            {
                if ( newSaveType == SaveType::StarCraftScm || newSaveType == SaveType::HybridScm )
                    newSaveFilePath += ".scm";
                else if ( newSaveType == SaveType::ExpansionScx || newSaveType == SaveType::AllMaps || newSaveType == SaveType::AllFiles )
                {
                    newSaveFilePath += ".scx";
                    newSaveType = SaveType::ExpansionScx;
                }
                else if ( newSaveType == SaveType::StarCraftChk || newSaveType == SaveType::HybridChk || newSaveType == SaveType::ExpansionChk )
                    newSaveFilePath += ".chk";
                else
                    inferredExtension = false;
            }
            else // Extension specified, give it precedence over filterIndex (that is, update newSaveType if it's not already appropriate for the extension)
            {
                if ( extension == ".chk" && newSaveType != SaveType::StarCraftChk && newSaveType != SaveType::HybridChk && newSaveType != SaveType::ExpansionChk && newSaveType != SaveType::AllChk )
                {
                    if ( newSaveType == SaveType::ExpansionScx )
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
                else if ( extension == ".scx" && newSaveType != SaveType::ExpansionScx )
                    newSaveType = SaveType::ExpansionScx;
                else if ( newSaveType == SaveType::AllMaps )
                    newSaveType = SaveType::ExpansionScx; // Default to expansion scx
            }

            bool fileExists = FindFile(newSaveFilePath);
            bool mustConfirmOverwrite = fileExists && inferredExtension;
            if ( !mustConfirmOverwrite || fileBrowser->confirmOverwrite(GetSystemFileName(newSaveFilePath) + " already exists.\nDo you want to replace it?") )
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
        FilterEntry<SaveType>(SaveType::StarCraftChk, "*.chk", "Raw StarCraft Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::HybridChk, "*.chk", "Raw StarCraft Hybrid Map(*.chk)", ".chk"),
        FilterEntry<SaveType>(SaveType::ExpansionChk, "*.chk", "Raw BroodWar Map(*.chk)", ".chk"),
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
