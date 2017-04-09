#include "MapFile.h"
#include "FileIO.h"
#include <cstdio>
#include <cstdarg>

std::hash<std::string> MapFile::strHash;
std::map<u32, std::string> MapFile::virtualWavTable;
u64 MapFile::nextAssetFileId(0);

MapFile::MapFile() : saveType(SaveType::Unknown), mapFilePath(""), temporaryMpq(nullptr)
{
    if ( MapFile::virtualWavTable.size() == 0 )
    {
        for ( s32 i=0; i<NumVirtualSounds; i++ )
        {
            std::string wavPath(VirtualSoundFiles[i]);
            u32 hash = strHash(wavPath);
            virtualWavTable.insert(std::pair<u32, std::string>(hash, wavPath));
        }
    }
}

bool MapFile::LoadMapFile(std::string &path)
{
    if ( path.length() > 0 )
        mapFilePath = path;
    else if ( GetPath() )
        path = std::string(mapFilePath); // Have the pointer point to the new filePath

    return mapFilePath.length() > 0 && OpenMapFile();
}

bool MapFile::LoadMapFile()
{
    return GetPath() && OpenMapFile();
}

bool MapFile::SaveFile(bool SaveAs)
{
    std::string prevFilePath(mapFilePath);

    if ( isProtected() )
        MessageBox(NULL, "Cannot save protected maps!", "Error!", MB_OK|MB_ICONEXCLAMATION);
    else
    {
        if ( SaveAs || mapFilePath.length() <= 0 ) // SaveAs specified or filePath not yet determined
        {
            char szFilePath[FILENAME_MAX] = "";
            OPENFILENAME ofn = GetScSaveOfn(szFilePath);
            if ( GetSaveFileName(&ofn) )
            {
                mapFilePath = szFilePath;
                saveType = (SaveType)ofn.nFilterIndex;

                const char* ext = std::strrchr(mapFilePath.c_str(), '.'); // Find the last occurrence of '.'
                if ( ext == nullptr ) // No extension specified, need to add
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
                    if ( std::strcmp(ext, ".chk") == 0 && (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm ||
                        saveType == SaveType::ExpansionScx) )
                    {
                        saveType = SaveType::HybridChk;
                    }
                    else if ( std::strcmp(ext, ".scm") == 0 && saveType != SaveType::StarCraftScm )
                        saveType = SaveType::HybridScm;
                    else if ( std::strcmp(ext, ".scx") == 0 )
                        saveType = SaveType::ExpansionScx;
                    else if ( saveType == SaveType::AllMaps )
                        saveType = SaveType::ExpansionScx;
                }
            }
        }

        if ( mapFilePath[0] != '\0' ) // Map for sure has a path
        {
            std::FILE* pFile(nullptr);

            if ( saveType == SaveType::StarCraftScm || saveType == SaveType::StarCraftChk ) // StarCraft Map, edit to match
                ChangeToScOrig();
            else if ( saveType == SaveType::HybridScm || saveType == SaveType::HybridChk ) // Hybrid Map, edit to match
                ChangeToHybrid();
            else if ( saveType == SaveType::ExpansionScx || saveType == SaveType::ExpansionChk ||
                saveType == SaveType::AllMaps ) // BroodWar Map, edit to match
            {
                ChangeToScExp();
            }

            if ( (saveType == SaveType::StarCraftScm || saveType == SaveType::HybridScm || saveType == SaveType::ExpansionScx)
                || saveType == SaveType::AllMaps ) // Must be packed into an MPQ
            {
                if ( !SaveAs || (SaveAs && MakeFileCopy(prevFilePath, mapFilePath)) )
                {
                    DeleteFileA("chk.tmp"); // Remove any existing chk.tmp files
                    pFile = std::fopen("chk.tmp", "wb");
                    Scenario::WriteFile(pFile);
                    std::fclose(pFile);

                    MPQHANDLE hMpq = MpqOpenArchiveForUpdate(mapFilePath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
                    if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
                    {
                        BOOL addedNewScenarioFile = FALSE;
                        if ( ProcessModifiedAssets(hMpq) )
                        {
                            addedNewScenarioFile =
                                MpqAddFileToArchive(hMpq, "chk.tmp", "staredit\\scenario.chk", MAFA_COMPRESS | MAFA_REPLACE_EXISTING);

                            if ( !addedNewScenarioFile )
                                MessageBox(NULL, "Failed to add scenario file!", "Error!", MB_OK | MB_ICONEXCLAMATION);
                        }
                        else
                            MessageBox(NULL, "Processing assets failed!", "Error!", MB_OK | MB_ICONEXCLAMATION);

                        DeleteFileA("chk.tmp");

                        MpqCloseUpdatedArchive(hMpq, 0);
                        return addedNewScenarioFile == TRUE;
                    }
                    else
                        MessageBox(NULL, std::string(std::string("Failed to open for updates!\n\nThe file may be in use elsewhere. ") + std::to_string(GetLastError())).c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
                }
                MessageBox(NULL, "Failed to create the new MPQ file!", "Error!", MB_OK | MB_ICONEXCLAMATION);

                DeleteFileA("chk.tmp");
            }
            else // Is a chk file or unrecognized format, write out chk file
            {
                DeleteFileA(mapFilePath.c_str()); // Remove any existing files of the same name
                pFile = std::fopen(mapFilePath.c_str(), "wb");
                WriteFile(pFile);
                std::fclose(pFile);
                return true;
            }
        }
    }
    return false;
}

bool MapFile::GetPath()
{
    OPENFILENAME ofn = { };
    char szFileName[FILENAME_MAX] = { };
    char initPath[FILENAME_MAX] = { };

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.lpstrFilter = "All Maps\0*.scm;*.scx;*.chk\0Starcraft Maps\0*.scm\0Broodwar Maps\0*.scx\0Raw Scenario Files\0*.chk\0";
    ofn.lpstrFile = szFileName;

    if ( GetRegScPath(initPath, FILENAME_MAX) )
    {
        std::strcat(initPath, "\\Maps");
        ofn.lpstrInitialDir = initPath;
    }
    else
        ofn.lpstrInitialDir = NULL;

    ofn.hwndOwner = NULL;
    ofn.nMaxFile = FILENAME_MAX;
    ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT;

    if ( GetOpenFileName(&ofn) == TRUE )
    {
        mapFilePath = szFileName;
        return true;
    }
    else if ( CommDlgExtendedError() != 0 )
        CHKD_ERR("Error Retrieving File Name.");

    return false;
}

bool MapFile::GetTemporaryMpqPath(std::string &temporaryMpqFilePath)
{
    if ( temporaryMpq.get() != nullptr && temporaryMpq->GetFilePath().length() > 0 )
    {
        temporaryMpqFilePath = temporaryMpq->GetFilePath();
        return true;
    }

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
        while ( FindFile(assetFilePath.c_str()) ); // Try again if the file already exists
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
            } while ( FindFile(assetFilePath.c_str()) ); // Try again if the file already exists
        }
        else // Use the C library to find an appropriate temporary location
        {
            assetFilePath = std::string(tmpnam(NULL));
        }
    }

    if ( assetFilePath.length() > 0 )
    {
        try {
            temporaryMpq = std::unique_ptr<TemporaryFile>(new TemporaryFile(assetFilePath));
            temporaryMpqFilePath = temporaryMpq->GetFilePath();
            return true;
        } catch ( std::exception ) {
            CHKD_ERR("Failed to setup temporary asset storage, out of memory!");
        }
    }
    else
        CHKD_ERR("Failed to setup temporary asset storage, an appropriate file path could not be found!");

    return false;
}

bool MapFile::OpenMapFile()
{
    if ( mapFilePath.length() > 0 )
    {
        buffer chk("oMAP");

        const char* ext = std::strrchr(mapFilePath.c_str(), '.'); // Find the last occurrence of '.'
        if ( ext != nullptr )
        {
            if ( strcmp(ext, ".scm") == 0 || strcmp(ext, ".scx") == 0 )
            {
                MPQHANDLE hMpq = MpqOpenArchiveForUpdate(mapFilePath.c_str(), MOAU_OPEN_EXISTING|MOAU_READ_ONLY, 1000);
                if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
                {
                    if ( !FileToBuffer(hMpq, "staredit\\scenario.chk", chk) )
                        CHKD_ERR("Failed to get scenario file from MPQ.");

                    MpqCloseUpdatedArchive(hMpq, 0);
                    
                    if ( chk.size() > 0 && ParseScenario(chk) )
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
                        else if ( strcmp(ext, ".scx") == 0 )
                            saveType = SaveType::ExpansionScx; // Expansion
                        else if ( true ) // Could search for clues to map version here
                            saveType = SaveType::ExpansionChk; // Otherwise set to expansion to prevent data loss

                        return true;
                    }
                    else
                        CHKD_ERR("Invalid or missing Scenario file.");
                }
                else if ( GetLastError() == ERROR_FILE_NOT_FOUND )
                    CHKD_ERR("File Not Found");
                else if ( GetLastError() == MPQ_ERROR_MPQ_INVALID )
                    CHKD_ERR("Invalid or corrupt MPQ file");
                else
                    CHKD_ERR("%d", GetLastError());
            }
            else if ( strcmp(ext, ".chk") == 0 )
            {
                if ( FileToBuffer(mapFilePath, chk) )
                {
                    if ( ParseScenario(chk) )
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

                        return true;
                    }
                    else
                        CHKD_ERR("Parsing Failed!");
                }
                else
                    CHKD_ERR("Error Reading CHK File");
            }
        }
    }
    return false;
}

bool MapFile::ProcessModifiedAssets(MPQHANDLE hMpq)
{
    bool success = true;
    std::string temporaryMpqPath = "";
    if ( GetTemporaryMpqPath(temporaryMpqPath) )
    {
        MPQHANDLE hTempMpq = MpqOpenArchiveForUpdate(temporaryMpqPath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
        if ( hTempMpq != NULL && hTempMpq != INVALID_HANDLE_VALUE )
        {
            for ( auto asset : modifiedAssets )
            {
                const std::string &assetMpqPath = asset->assetMpqPath;
                if ( asset->actionTaken == AssetAction::Add )
                {
                    const std::string &assetTempMpqPath = asset->assetTempMpqPath;
                    WavQuality wavQuality = asset->wavQualitySelected;
                    
                    // Pull the file out from the temporary mpq
                    buffer assetBuffer = buffer("TmpA");
                    if ( FileToBuffer(hTempMpq, assetTempMpqPath, assetBuffer) )
                    {
                        MpqDeleteFile(hTempMpq, assetTempMpqPath.c_str()); // Remove the file from the temporary MPQ, ignore any errors

                        // Put the asset into the map mpq
                        if ( wavQuality == WavQuality::Uncompressed )
                        {
                            if ( !BufferToArchive(hMpq, assetBuffer, assetMpqPath) )
                            {
                                CHKD_SHOUT("Failed to save %s to destination file", assetMpqPath.c_str());
                                success = false;
                            }
                        }
                        else
                        {
                            if ( !WavBufferToArchive(hMpq, assetBuffer, assetMpqPath, wavQuality) )
                            {
                                CHKD_SHOUT("Failed to save %s to destination file", assetMpqPath.c_str());
                                success = false;
                            }
                        }
                    }
                    else
                    {
                        CHKD_SHOUT("Failed to read in temporary asset!");
                        success = false;
                    }
                }
                else if ( asset->actionTaken == AssetAction::Remove )
                {
                    if ( !MpqDeleteFile(hMpq, assetMpqPath.c_str()) ) // Remove the file from the MPQ, log any errors but don't modify success 
                        CHKD_ERR("Failed to remove %s from map archive", assetMpqPath.c_str());
                }
            }
            MpqCloseUpdatedArchive(hTempMpq, 0);
        }
    }

    if ( success )
        modifiedAssets.clear();

    return success;
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
    if ( FindFile(assetSystemFilePath.c_str()) )
    {
        std::string temporaryMpqPath = "";
        if ( GetTemporaryMpqPath(temporaryMpqPath) )
        {
            MPQHANDLE hMpq = MpqOpenArchiveForUpdate(temporaryMpqPath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
            if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
            {
                ModifiedAssetPtr modifiedAssetPtr = ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, wavQuality, AssetAction::Add));
                const char* tempMpqPath = modifiedAssetPtr->assetTempMpqPath.c_str();
                if ( MpqAddFileToArchive(hMpq, assetSystemFilePath.c_str(), tempMpqPath, MAFA_COMPRESS | MAFA_REPLACE_EXISTING) == TRUE )
                {
                    modifiedAssets.push_back(modifiedAssetPtr);
                    success = true;
                }
                else
                    MessageBox(NULL, "Failed to add file!", "Error!", MB_OK | MB_ICONEXCLAMATION);

                MpqCloseUpdatedArchive(hMpq, 0);
            }
            else
                MessageBox(NULL, std::string(std::string("Failed to open temp file!\n\nThe file may be in use elsewhere. ") + std::to_string(GetLastError())).c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
        }
        else
            MessageBox(NULL, "Failed to setup asset temporary storage!", "Error!", MB_OK | MB_ICONEXCLAMATION);
    }
    else
        MessageBox(NULL, "Failed to find asset file!", "Error!", MB_OK | MB_ICONEXCLAMATION);

    return success;
}

bool MapFile::AddMpqAsset(const std::string &assetMpqFilePath, const buffer &asset, WavQuality wavQuality)
{
    bool success = false;
    std::string temporaryMpqPath = "";
    if ( GetTemporaryMpqPath(temporaryMpqPath) )
    {
        MPQHANDLE hMpq = MpqOpenArchiveForUpdate(temporaryMpqPath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
        if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
        {
            ModifiedAssetPtr modifiedAssetPtr = ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, wavQuality, AssetAction::Add));
            const char* tempMpqPath = modifiedAssetPtr->assetTempMpqPath.c_str();

            if ( BufferToArchive(hMpq, asset, tempMpqPath) )
            {
                modifiedAssets.push_back(modifiedAssetPtr);
                success = true;
            }
            else
                MessageBox(NULL, "Failed to add file!", "Error!", MB_OK | MB_ICONEXCLAMATION);

            MpqCloseUpdatedArchive(hMpq, 0);
        }
        else
            MessageBox(NULL, std::string(std::string("Failed to open temp file!\n\nThe file may be in use elsewhere. ") + std::to_string(GetLastError())).c_str(), "Error!", MB_OK | MB_ICONEXCLAMATION);
    }
    else
        MessageBox(NULL, "Failed to setup asset temporary storage!", "Error!", MB_OK | MB_ICONEXCLAMATION);

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
        std::string temporaryMpqPath = "";
        if ( GetTemporaryMpqPath(temporaryMpqPath) )
        {
            MPQHANDLE hMpq = MpqOpenArchiveForUpdate(temporaryMpqPath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
            if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
            {
                MpqDeleteFile(hMpq, tempMpqFilePath.c_str());
                MpqCloseUpdatedArchive(hMpq, 0);
            }
        }
    }
    else // The given file was not added recently, mark it for deletion at the next save
    {
        modifiedAssets.push_back(ModifiedAssetPtr(new ModifiedAsset(assetMpqFilePath, WavQuality::Uncompressed, AssetAction::Remove)));
    }
}

bool MapFile::GetMpqAsset(const std::string &assetMpqFilePath, buffer &outAssetBuffer)
{
    bool success = false;
    for ( auto asset : modifiedAssets ) // Check if it's a recently added asset
    {
        if ( asset->actionTaken == AssetAction::Add && asset->assetMpqPath.compare(assetMpqFilePath) == 0 ) // Asset was recently added
        {
            std::string temporaryMpqPath = "";
            if ( GetTemporaryMpqPath(temporaryMpqPath) )
            {
                MPQHANDLE hTempMpq = MpqOpenArchiveForUpdate(temporaryMpqPath.c_str(), MOAU_OPEN_ALWAYS | MOAU_MAINTAIN_LISTFILE, 1000);
                if ( hTempMpq != NULL && hTempMpq != INVALID_HANDLE_VALUE )
                {
                    success = FileToBuffer(hTempMpq, asset->assetTempMpqPath, outAssetBuffer);
                    MpqCloseUpdatedArchive(hTempMpq, 0);
                }
            }
            return success;
        }
    }

    MPQHANDLE hMapMpq = MpqOpenArchiveForUpdate(mapFilePath.c_str(), MOAU_OPEN_EXISTING | MOAU_READ_ONLY, 1000);
    if ( hMapMpq != NULL && hMapMpq != INVALID_HANDLE_VALUE )
    {
        success = FileToBuffer(hMapMpq, assetMpqFilePath, outAssetBuffer);
        MpqCloseUpdatedArchive(hMapMpq, 0);
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
        FindFileInMpq(GetFilePath().c_str(), wavString.c_str()) &&
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
            MessageBox(NULL, "Failed to register WAV in scenario file!", "Error!", MB_OK | MB_ICONEXCLAMATION);
    }
    else if ( AddMpqAsset(srcFilePath, destMpqPath, wavQuality) ) // Add, Register
    {
        if ( Scenario::AddWav(RawString(destMpqPath)) )
            success = true;
        else
        {
            RemoveMpqAsset(destMpqPath); // Try to remove the wav, ignore errors if any
            MessageBox(NULL, "Failed to register WAV in scenario file!", "Error!", MB_OK | MB_ICONEXCLAMATION);
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
            MessageBox(NULL, "Failed to register WAV in scenario file!", "Error!", MB_OK | MB_ICONEXCLAMATION);
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

            MPQHANDLE hMpq = MpqOpenArchiveForUpdate(mapFilePath.c_str(), MOAU_OPEN_EXISTING | MOAU_READ_ONLY, 1000);
            if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
            {
                WavStatus wavStatus = WavStatus::NoMatch;
                if ( FindFileInMpq(hMpq, wavString.c_str()) )
                    wavStatus = WavStatus::CurrentMatch;
                else if ( IsInVirtualWavList(wavString) )
                    wavStatus = WavStatus::VirtualFile;
                MpqCloseUpdatedArchive(hMpq, 0);
                return wavStatus;
            }
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
        MPQHANDLE hMpq = NULL; // To be opened only if necessary
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

                    if ( hMpq == NULL || hMpq == INVALID_HANDLE_VALUE )
                        hMpq = MpqOpenArchiveForUpdate(mapFilePath.c_str(), MOAU_OPEN_EXISTING|MOAU_READ_ONLY, 1000);

                    if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
                    {
                        if ( FindFileInMpq(hMpq, wavString.c_str()) )
                            outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::CurrentMatch));
                        else
                            outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::NoMatch));
                    }
                    else
                        outWavStatus.insert(std::pair<u32, WavStatus>(wavStringIndex, WavStatus::FileInUse));
                }
            }
        }

        if ( hMpq != NULL && hMpq != INVALID_HANDLE_VALUE )
            MpqCloseUpdatedArchive(hMpq, 0);

        return true;
    }
    return false;
}

bool MapFile::IsInVirtualWavList(const std::string &wavMpqPath)
{
    u32 hash = MapFile::strHash(wavMpqPath);
    int numMatching = MapFile::virtualWavTable.count(hash);
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

std::string MapFile::GetFilePath()
{
    return mapFilePath;
}
