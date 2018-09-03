#ifndef MAPFILE_H
#define MAPFILE_H
#include "Scenario.h"
#include "SystemIO.h"
#include "FileBrowser.h"
#include "MpqFile.h"
#include <cstdio>
#include <time.h>
#include <map>

#ifdef CHKDRAFT
bool GetPreSavePath(std::string &outPreSavePath); // Gets path holding assets to be written to the map file on save
#endif

class SimpleMapBrowser;
enum class SaveType;
enum class WavStatus;
class MapFile;
using MapFilePtr = std::shared_ptr<MapFile>;

class MapFile : public Scenario, public MpqFile // MapFile is a scenario file and usually an MpqFile
{
    public:
        MapFile();
        virtual ~MapFile();

        virtual bool SaveFile(bool saveAs = false, bool updateListFile = true, FileBrowserPtr fileBrowser = getDefaultSaveMapBrowser());

        bool LoadMapFile(std::string &filePath); // If you're not providing a path, pass in an empty string
        bool LoadMapFile(FileBrowserPtr fileBrowser = getDefaultOpenMapBrowser());

        void SetSaveType(SaveType newSaveType);

        static std::string GetStandardWavDir();
        bool AddMpqAsset(const std::string &assetSystemFilePath, const std::string &assetMpqFilePath, WavQuality wavQuality);
        bool AddMpqAsset(const std::string &assetMpqFilePath, const buffer &asset, WavQuality wavQuality);
        void RemoveMpqAsset(const std::string &assetMpqFilePath);
        bool GetMpqAsset(const std::string &assetMpqFilePath, buffer &outAssetBuffer);
        bool ExtractMpqAsset(const std::string &assetMpqFilePath, const std::string &systemFilePath);
        virtual bool GetWav(u16 wavIndex, u32 &outStringIndex);
        bool GetWav(u32 stringIndex, buffer &outWavData);
        bool AddWav(u32 stringIndex); // Adds a WAV string to the WAV list
        bool AddWav(const std::string &srcFilePath, WavQuality wavQuality, bool virtualFile);
        bool AddWav(const std::string &srcFilePath, const std::string &destMpqPath, WavQuality wavQuality, bool virtualFile);
        bool AddWav(const std::string &destMpqPath, buffer &wavContents, WavQuality wavQuality);
        virtual bool RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed);
        virtual bool RemoveWavByStringIndex(u32 stringIndex, bool removeIfUsed);
        WavStatus GetWavStatus(u32 wavStringIndex);
        bool GetWavStatusMap(std::map<u32/*stringIndex*/, WavStatus> &outWavStatus, bool includePureStringWavs);
        bool IsInVirtualWavList(const std::string &wavMpqPath);

        std::string GetFileName();
        virtual const std::string &getFilePath() const;

        static FileBrowserPtr getDefaultOpenMapBrowser();
        static FileBrowserPtr getDefaultSaveMapBrowser();

    private:
        std::string mapFilePath;
        std::string temporaryMpqPath;
        MpqFile temporaryMpq;
        SaveType saveType;
        std::vector<ModifiedAssetPtr> modifiedAssets; // A record of all MPQ assets changes since the last save

        static std::hash<std::string> strHash; // A hasher to help generate tables
        static std::map<size_t, std::string> virtualWavTable;
        static u64 nextAssetFileId; // Changes are needed if this is accessed in a multi-threaded environment

        bool OpenMapFile(std::string &filePath);
        bool OpenTemporaryMpq();
        bool ProcessModifiedAssets(bool updateListfile);
};

/** The types of files a map can be saved as, one
    through the number of selectable save types should
    correspond to the selectable types in save dialogs */
enum class SaveType
{
    CustomFilter = 0,
    StarCraftScm = 1,
    HybridScm = 2,
    ExpansionScx = 3,
    StarCraftChk = 4,
    HybridChk = 5,
    ExpansionChk = 6,
    AllMaps = 7,
    Unknown // Have this higher than all other SaveTypes
};

enum class WavStatus
{
    PendingMatch,
    CurrentMatch,
    VirtualFile,
    NoMatch,
    NoMatchExtended,
    FileInUse,
    Unknown
};

#endif