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
bool GetPreSavePath(std::string & outPreSavePath); // Gets path holding assets to be written to the map file on save
#endif

class SimpleMapBrowser;
enum class SaveType;
enum class SoundStatus;
class MapFile;
using MapFilePtr = std::shared_ptr<MapFile>;

class MapFile : public Scenario, public MpqFile // MapFile is a scenario file and usually an MpqFile
{
    public:
        MapFile(const std::string & filePath);
        MapFile(FileBrowserPtr<SaveType> fileBrowser = getDefaultOpenMapBrowser());
        MapFile(Sc::Terrain::Tileset tileset = Sc::Terrain::Tileset::Badlands, u16 width = 64, u16 height = 64);

        virtual ~MapFile();
        
        virtual bool save(const std::string & saveFilePath, bool updateListFile = true, bool lockAnywhere = true, bool autoDefragmentLocations = true);
        virtual bool save(bool saveAs = false, bool updateListFile = true, FileBrowserPtr<SaveType> fileBrowser = getDefaultSaveMapBrowser(),
            bool lockAnywhere = true, bool autoDefragmentLocations = true);

        bool LoadMapFile(const std::string & filePath);
        bool LoadMapFile(FileBrowserPtr<SaveType> fileBrowser = getDefaultOpenMapBrowser());

        void SetSaveType(SaveType newSaveType);

        static std::string GetStandardSoundDir();
        bool AddMpqAsset(const std::string & assetSystemFilePath, const std::string & assetMpqFilePath, WavQuality wavQuality);
        bool AddMpqAsset(const std::string & assetMpqFilePath, const std::vector<u8> & asset, WavQuality wavQuality);
        void RemoveMpqAsset(const std::string & assetMpqFilePath);
        bool GetMpqAsset(const std::string & assetMpqFilePath, std::vector<u8> & outAssetBuffer);
        bool ExtractMpqAsset(const std::string & assetMpqFilePath, const std::string & systemFilePath);
        virtual bool GetSound(u16 soundIndex, size_t & outStringId);
        bool GetSound(size_t stringId, std::vector<u8> & outSoundData);
        bool AddSound(size_t stringId); // Adds a sound string to the sound list
        bool AddSound(const std::string & srcFilePath, WavQuality wavQuality, bool virtualFile);
        bool AddSound(const std::string & srcFilePath, const std::string & destMpqPath, WavQuality wavQuality, bool virtualFile);
        bool AddSound(const std::string & destMpqPath, const std::vector<u8> & soundContents, WavQuality wavQuality);
        virtual bool RemoveSoundBySoundIndex(u16 soundIndex, bool removeIfUsed);
        virtual bool RemoveSoundByStringId(size_t stringId, bool removeIfUsed);
        SoundStatus GetSoundStatus(size_t soundStringId);
        bool GetSoundStatusMap(std::map<size_t/*stringId*/, SoundStatus> & outSoundStatus, bool includePureStringSounds);
        bool IsInVirtualSoundList(const std::string & soundMpqPath);

        std::string GetFileName();
        virtual const std::string & getFilePath() const;

        static FileBrowserPtr<SaveType> getDefaultOpenMapBrowser();
        static FileBrowserPtr<SaveType> getDefaultSaveMapBrowser();

    protected:
        virtual bool getSaveDetails(inout_param SaveType & saveType, output_param std::string & saveFilePath, output_param bool & overwriting, FileBrowserPtr<SaveType> fileBrowser);

    private:
        std::string mapFilePath;
        std::string temporaryMpqPath;
        MpqFile temporaryMpq;
        SaveType saveType;
        std::vector<ModifiedAssetPtr> modifiedAssets; // A record of all MPQ assets changes since the last save

        static std::hash<std::string> strHash; // A hasher to help generate tables
        static std::map<size_t, std::string> virtualSoundTable;
        static u64 nextAssetFileId; // Changes are needed if this is accessed in a multi-threaded environment

        bool OpenMapFile(const std::string & filePath);
        bool OpenTemporaryMpq();
        bool ProcessModifiedAssets(bool updateListfile);
        
        MapFile();
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

enum class SoundStatus
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
extern std::vector<FilterEntry<u32>> getSaveSoundFilters();

#endif