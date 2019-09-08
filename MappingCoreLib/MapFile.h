#ifndef MAPFILE_H
#define MAPFILE_H
#include "Scenario.h"
#include "SystemIO.h"
#include "FileBrowser.h"
#include "MpqFile.h"
#include <memory>
#include <cstdio>
#include <time.h>
#include <map>

/**
    A map file is a Scenario file wrapped inside of an MpqFile file and given an .scm or .scx extension

    Sometimes a scenario file alone, which has a .chk extension, may be considered a map file and may still be fully edited as one
    Though any attempt to save a map file as a scenario file will result in any sounds and any mpq assets not being included
*/

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

        virtual bool SaveFile(bool saveAs = false, bool updateListFile = true, FileBrowserPtr<SaveType> fileBrowser = getDefaultSaveMapBrowser());

        bool LoadMapFile(const std::string &filePath);
        bool LoadMapFile(FileBrowserPtr<SaveType> fileBrowser = getDefaultOpenMapBrowser());

        void SetSaveType(SaveType newSaveType);

        static std::string GetStandardWavDir();
        bool AddMpqAsset(const std::string &assetSystemFilePath, const std::string &assetMpqFilePath, WavQuality wavQuality);
        bool AddMpqAsset(const std::string &assetMpqFilePath, const buffer &asset, WavQuality wavQuality);
        void RemoveMpqAsset(const std::string &assetMpqFilePath);
        bool GetMpqAsset(const std::string &assetMpqFilePath, buffer &outAssetBuffer);
        bool ExtractMpqAsset(const std::string &assetMpqFilePath, const std::string &systemFilePath);
        virtual bool GetWav(u16 wavIndex, u32 &outStringId);
        bool GetWav(u32 stringId, buffer &outWavData);
        bool AddWav(u32 stringId); // Adds a WAV string to the WAV list
        bool AddWav(const std::string &srcFilePath, WavQuality wavQuality, bool virtualFile);
        bool AddWav(const std::string &srcFilePath, const std::string &destMpqPath, WavQuality wavQuality, bool virtualFile);
        bool AddWav(const std::string &destMpqPath, buffer &wavContents, WavQuality wavQuality);
        virtual bool RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed);
        virtual bool RemoveWavByStringId(u32 stringId, bool removeIfUsed);
        WavStatus GetWavStatus(u32 wavStringId);
        bool GetWavStatusMap(std::map<u32/*stringId*/, WavStatus> &outWavStatus, bool includePureStringWavs);
        bool IsInVirtualWavList(const std::string &wavMpqPath);

        std::string GetFileName();
        virtual const std::string &getFilePath() const;

        static FileBrowserPtr<SaveType> getDefaultOpenMapBrowser();
        static FileBrowserPtr<SaveType> getDefaultSaveMapBrowser();

    protected:
        virtual bool getSaveDetails(inout_param SaveType &saveType, output_param std::string &saveFilePath, output_param bool &overwriting, FileBrowserPtr<SaveType> fileBrowser);

    private:
        std::string mapFilePath;
        std::string temporaryMpqPath;
        MpqFile temporaryMpq;
        SaveType saveType;
        std::vector<ModifiedAssetPtr> modifiedAssets; // A record of all MPQ assets changes since the last save

        static std::hash<std::string> strHash; // A hasher to help generate tables
        static std::map<size_t, std::string> virtualWavTable;
        static u64 nextAssetFileId; // Changes are needed if this is accessed in a multi-threaded environment

        bool OpenMapFile(const std::string &filePath);
        bool OpenTemporaryMpq();
        bool ProcessModifiedAssets(bool updateListfile);
};

/** The types of files a map can be saved as, one
through the number of selectable save types should
correspond to the selectable types in save dialogs */
enum class SaveType
{
    StarCraftScm = 1,
    HybridScm = 2,
    ExpansionScx = 3,
    StarCraftChk = 4,
    HybridChk = 5,
    ExpansionChk = 6,
    AllScm = 7,
    AllScx = 8,
    AllChk = 9,
    AllMaps = 10,
    AllFiles = 11,
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

extern std::vector<FilterEntry<SaveType>> getOpenMapFilters();
extern std::vector<FilterEntry<SaveType>> getSaveMapFilters();
extern std::vector<FilterEntry<u32>> getSaveTextFilters();
extern std::vector<FilterEntry<u32>> getSoundFilters();

#endif