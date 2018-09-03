#include "MapFile.h"
#include "SystemIO.h"
#include "EscapeStrings.h"
#include "ModifiedAsset.h"
#include <cstdio>
#include <cstdarg>
#include <SimpleIcu.h>

std::hash<std::string> MapFile::strHash;
std::map<size_t, std::string> MapFile::virtualWavTable;
u64 MapFile::nextAssetFileId(0);

FileBrowserPtr MapFile::getDefaultOpenMapBrowser()
{
    return FileBrowserPtr(new FileBrowser(getOpenMapFilters(), "Open Map", true, false));
}

FileBrowserPtr MapFile::getDefaultSaveMapBrowser()
{
    return FileBrowserPtr(new FileBrowser(getSaveMapFilters(), "Save Map", false, true));
}

MapFile::MapFile() : saveType(SaveType::Unknown), mapFilePath(""), temporaryMpqPath(""), temporaryMpq(true)
{
    if ( MapFile::virtualWavTable.size() == 0 )
    {
        for ( s32 i=0; i<NumVirtualSounds; i++ )
        {
            std::string wavPath(VirtualSoundFiles[i]);
            size_t hash = strHash(wavPath);
            virtualWavTable.insert(std::pair<size_t, std::string>(hash, wavPath));
        }
    }
}

MapFile::~MapFile()
{

}

bool MapFile::LoadMapFile(std::string &filePath)
{
    return !filePath.empty() && OpenMapFile(filePath);
}

bool MapFile::LoadMapFile(FileBrowserPtr fileBrowser)
{
    std::string browseFilePath = "";
    u32 filterIndex = 0;
    return fileBrowser != nullptr && fileBrowser->browseForOpenPath(browseFilePath, filterIndex) && OpenMapFile(browseFilePath);
}

bool MapFile::SaveFile(bool saveAs, bool updateListFile, FileBrowserPtr fileBrowser)
{
    std::string prevFilePath(mapFilePath);

    if ( isProtected() )
        CHKD_ERR("Cannot save protected maps!");
    else
    {
        if ( (saveAs || mapFilePath.empty()) && fileBrowser != nullptr ) // saveAs specified or filePath not yet determined, and a fileBrowser is available
        {
            u32 filterIndex = (u32)saveType;
            if ( fileBrowser != nullptr && fileBrowser->browseForSavePath(mapFilePath, filterIndex) )
            {
                saveType = (SaveType)filterIndex;
                std::string extension = GetSystemFileExtension(mapFilePath);
                if ( extension.empty() ) // No extension specified, need to add
                {
                    if ( saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm )
                        mapFilePath += ".scm";
                    else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::AllMaps )
                        mapFilePath += ".scx";
                    else if ( saveType == SaveType::StarCraftChk || saveType == SaveType::HybridChk || saveType == SaveType::ExpansionChk )
                        mapFilePath += ".chk";
                }
                else // Extension specified, give it precedence over filterIndex
                {
                    if ( extension == ".chk" && (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm || saveType == SaveType::ExpansionScx) )
                    {
                        if ( saveType == SaveType::ExpansionScx )
                            saveType = SaveType::ExpansionChk;
                        else
                            saveType = SaveType::HybridChk;
                    }
                    else if ( extension == ".scm" && saveType != SaveType::StarCraftScm )
                        saveType = SaveType::HybridScm;
                    else if ( extension == ".scx" )
                        saveType = SaveType::ExpansionScx;
                    else if ( saveType == SaveType::AllMaps )
                        saveType = SaveType::ExpansionScx;
                }
            }
        }

        if ( !mapFilePath.empty() ) // Map path has been determined
        {
            if ( saveType == SaveType::StarCraftScm || saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
                Scenario::ChangeToScOrig();
            else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
                Scenario::ChangeToHybrid();
            else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk || saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
                Scenario::ChangeToScExp();

            if ( (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm || saveType == SaveType::ExpansionScx) || saveType == SaveType::AllMaps ) // Must be packed into an MPQ
            {
                if ( !saveAs || (saveAs && MakeFileCopy(prevFilePath, mapFilePath)) ) // If using save-as, copy the existing mpq to the new location
                {
                    buffer chk;
                    if ( Scenario::ToSingleBuffer(chk) )
                    {
                        if ( MpqFile::open(mapFilePath) )
                        {
                            if ( !MpqFile::addFile("staredit\\scenario.chk", chk) )
                                CHKD_ERR("Failed to add scenario file!");

                            if ( !ProcessModifiedAssets(updateListFile) )
                                CHKD_ERR("Processing assets failed!");

                            MpqFile::close(updateListFile);
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
                    #ifdef WINDOWS_UTF16
                        std::FILE* pFile = _wfopen(icux::toFilestring(mapFilePath).c_str(), icux::toFilestring("wb").c_str());
                    #else
                        std::FILE* pFile = std::fopen(mapFilePath.c_str(), "wb");
                    #endif

                    if ( pFile != nullptr )
                    {
                        Scenario::WriteFile(pFile);
                        std::fclose(pFile);
                        return true;
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
        return temporaryMpq.open(temporaryMpqPath, false);

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
            assetFilePath = std::string(tmpnam(NULL));
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

bool MapFile::OpenMapFile(std::string &filePath)
{
    std::string extension = GetSystemFileExtension(filePath);
    if ( !extension.empty() )
    {
        buffer chk("oMAP");
        if ( extension == ".scm" || extension == ".scx" )
        {
            if ( MpqFile::open(filePath, false) )
            {
                this->mapFilePath = MpqFile::getFilePath();
                if ( !MpqFile::getFile("staredit\\scenario.chk", chk) )
                    CHKD_ERR("Failed to get scenario file from MPQ.");
                
                MpqFile::close();
                
                if ( chk.size() > 0 && Scenario::ParseScenario(chk) )
                {
                    if ( HasVerSection() )
                    {
                        if ( IsScOrig() )
                            saveType = SaveType::StarCraftScm; // Vanilla
                        else if ( IsHybrid() )
                            saveType = SaveType::HybridScm; // Hybrid
                        else
                            saveType = SaveType::ExpansionScx; // Expansion
                    }
                    else if ( extension == ".scx" )
                        saveType = SaveType::ExpansionScx; // Expansion
                    else if ( true ) // Could search for clues to map version here
                        saveType = SaveType::ExpansionChk; // Otherwise set to expansion to prevent data loss
                    
                    filePath = this->mapFilePath;
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
            if ( FileToBuffer(filePath, chk) )
            {
                if ( Scenario::ParseScenario(chk) )
                {
                    if ( HasVerSection() )
                    {
                        if ( IsScOrig() )
                            saveType = SaveType::StarCraftChk; // Vanilla chk
                        else if ( IsHybrid() )
                            saveType = SaveType::HybridChk; // Hybrid chk
                        else
                            saveType = SaveType::ExpansionChk; // Expansion chk
                    }
                    else if ( true ) // Could search for clues to map version here
                        saveType = SaveType::ExpansionChk; // Otherwise set to expansion to prevent data loss
                    
                    this->mapFilePath = filePath;
                    return true;
                }
                else
                    CHKD_ERR("Parsing Failed!");
            }
            else
                CHKD_ERR("Error Reading CHK File");
        }
        else
            CHKD_ERR("Unrecognized Extension!");
    }
    return false;
}

bool MapFile::ProcessModifiedAssets(bool updateListfile)
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
        temporaryMpq.save(updateListfile);
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

std::string MapFile::GetStandardWavDir()
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
            
            temporaryMpq.save(true);
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
        if ( temporaryMpq.open(temporaryMpqPath) )
        {
            if ( temporaryMpq.addFile(modifiedAssetPtr->assetTempMpqPath, asset) )
            {
                modifiedAssets.push_back(modifiedAssetPtr);
                success = true;
            }
            else
                CHKD_ERR("Failed to add file!");

            temporaryMpq.save(true);
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
    auto recentlyAddedAset = modifiedAssets.end();
    for ( auto asset = modifiedAssets.begin(); asset != modifiedAssets.end(); asset++ )
    {
        if ( (*asset) != nullptr && (*asset)->assetMpqPath == assetMpqFilePath )
        {
            recentlyAddedAset = asset;
            break;
        }
    }

    if ( recentlyAddedAset != modifiedAssets.end() ) // Asset was added between last save and now, cancel its addition
    {
        // Get the temp mpq file path used for this asset
        const std::string &tempMpqFilePath = (*recentlyAddedAset)->assetTempMpqPath;

        // Remove the asset from the list of assets to be added to the actual map file
        modifiedAssets.erase(recentlyAddedAset);

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

    if ( MpqFile::open(mapFilePath) )
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

bool MapFile::GetWav(u16 wavIndex, u32 &outStringIndex)
{
    return Scenario::GetWav(wavIndex, outStringIndex);
}

bool MapFile::GetWav(u32 stringIndex, buffer &outWavData)
{
    RawString wavString;
    return Scenario::GetString(wavString, stringIndex) &&
        GetMpqAsset(wavString, outWavData);
}

bool MapFile::AddWav(u32 stringIndex)
{
    RawString wavString;
    return Scenario::GetString(wavString, stringIndex) &&
        MpqFile::findFile(mapFilePath, wavString) &&
        Scenario::AddWav(stringIndex);
}

bool MapFile::AddWav(const std::string &srcFilePath, WavQuality wavQuality, bool virtualFile)
{
    std::string mpqWavDirectory = GetStandardWavDir();
    std::string mpqFilePath = MakeMpqFilePath(mpqWavDirectory, GetSystemFileName(srcFilePath));
    return AddWav(srcFilePath, mpqFilePath, wavQuality, virtualFile);
}

bool MapFile::AddWav(const std::string &srcFilePath, const std::string &destMpqPath, WavQuality wavQuality, bool virtualFile)
{
    bool success = false;
    if ( virtualFile )
    {
        if ( Scenario::AddWav(RawString(srcFilePath)) )
            success = true;
        else
            CHKD_ERR("Failed to register WAV in scenario file!");
    }
    else if ( AddMpqAsset(srcFilePath, destMpqPath, wavQuality) ) // Add, Register
    {
        if ( Scenario::AddWav(RawString(destMpqPath)) )
            success = true;
        else
        {
            RemoveMpqAsset(destMpqPath); // Try to remove the wav, ignore errors if any
            CHKD_ERR("Failed to register WAV in scenario file!");
        }
    }
    return success;
}

bool MapFile::AddWav(const std::string &destMpqPath, buffer &wavContents, WavQuality wavQuality)
{
    bool success = false;
    if ( AddMpqAsset(destMpqPath, wavContents, wavQuality) )
    {
        if ( Scenario::AddWav(RawString(destMpqPath)) )
            success = true;
        else
        {
            RemoveMpqAsset(destMpqPath); // Try to remove the wav, ignore errors if any
            CHKD_ERR("Failed to register WAV in scenario file!");
        }
    }
    return success;
}

bool MapFile::RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed)
{
    RawString wavString;
    bool hasString = Scenario::GetWavString(wavIndex, wavString);
    if ( Scenario::RemoveWavByWavIndex(wavIndex, removeIfUsed) )
    {
        if ( hasString )
            RemoveMpqAsset(wavString);

        return true;
    }
    return false;
}

bool MapFile::RemoveWavByStringIndex(u32 wavStringIndex, bool removeIfUsed)
{
    RawString wavString;
    bool hasString = Scenario::GetString(wavString, wavStringIndex);
    if ( Scenario::RemoveWavByStringIndex(wavStringIndex, removeIfUsed) )
    {
        if ( hasString )
            RemoveMpqAsset(wavString);

        return true;
    }
    return false;
}

WavStatus MapFile::GetWavStatus(u32 wavStringIndex)
{
    if ( Scenario::isExtendedString(wavStringIndex) ) // Extended strings are not used in SC and therefore never match
        return WavStatus::NoMatch;
    else
    {
        RawString wavString;
        if ( Scenario::GetString(wavString, wavStringIndex) )
        {
            for ( ModifiedAssetPtr modifiedAsset : modifiedAssets )
            {
                if ( modifiedAsset->actionTaken == AssetAction::Add && modifiedAsset->assetMpqPath == wavString )
                    return WavStatus::PendingMatch;
            }

            HANDLE hMpq = NULL;
            if ( MpqFile::open(mapFilePath, false) )
            {
                WavStatus wavStatus = WavStatus::NoMatch;
                if ( MpqFile::findFile(wavString) )
                    wavStatus = WavStatus::CurrentMatch;
                else if ( IsInVirtualWavList(wavString) )
                    wavStatus = WavStatus::VirtualFile;
                MpqFile::close();
                return wavStatus;
            }
            else if ( IsInVirtualWavList(wavString) )
                return WavStatus::VirtualFile;
            else
                return WavStatus::FileInUse;
        }
    }
    return WavStatus::Unknown;
}

bool MapFile::GetWavStatusMap(std::map<u32/*stringIndex*/, WavStatus> &outWavStatus, bool includePureStringWavs)
{
    std::map<u32/*stringIndex*/, u16/*wavIndex*/> wavMap;
    if ( Scenario::GetWavs(wavMap, includePureStringWavs) )
    {
        for ( auto entry : wavMap )
        {
            u32 wavStringIndex = entry.first;
            if ( Scenario::isExtendedString(wavStringIndex) ) // Extended strings are not used in SC and therefore never match
                outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::NoMatchExtended));
            else
            {
                RawString wavString;
                if ( Scenario::GetString(wavString, wavStringIndex) )
                {
                    for ( ModifiedAssetPtr modifiedAsset : modifiedAssets )
                    {
                        if ( modifiedAsset->actionTaken == AssetAction::Add && modifiedAsset->assetMpqPath == wavString )
                        {
                            outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::PendingMatch));
                            continue;
                        }
                    }

                    if ( MapFile::open(mapFilePath) )
                    {
                        if ( MapFile::findFile(wavString) )
                            outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::CurrentMatch));
                        else
                            outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::NoMatch));

                        MapFile::close();
                    }
                    else
                        outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::FileInUse));
                }
            }
        }
        return true;
    }
    return false;
}

bool MapFile::IsInVirtualWavList(const std::string &wavMpqPath)
{
    size_t hash = MapFile::strHash(wavMpqPath);
    size_t numMatching = MapFile::virtualWavTable.count(hash);
    if ( numMatching == 1 )
    {
        std::string &tableWavPath = MapFile::virtualWavTable.find(hash)->second;
        if ( wavMpqPath.compare(tableWavPath) == 0 )
            return true;
    }
    else if ( numMatching > 1 )
    {
        auto range = MapFile::virtualWavTable.equal_range(hash);
        for ( auto pair = range.first; pair != range.second; ++ pair)
        {
            if ( wavMpqPath.compare(pair->second) == 0 )
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
