#ifndef MAPFILE_H
#define MAPFILE_H
#include "scenario.h"
#include "file_browser.h"
#include "mpq_asset_manager.h"
#include "mpq_file.h"
#include <utility>
#include <map>

/**
    A map file is a Scenario file wrapped inside of an MpqFile file and given an .scm or .scx extension

    Sometimes a scenario file alone, which has a .chk extension, may be considered a map file and may still be fully edited as one
    Though any attempt to save a map file as a scenario file will result in any sounds and any mpq assets not being included
*/

#ifdef CHKDRAFT
std::optional<std::string> getPreSavePath(); // Gets path holding assets to be written to the map file on save
#endif

class SimpleMapBrowser;
enum class SoundStatus;

struct MapFile : Scenario, MpqFile, MpqAssetManager // MapFile is a scenario file and usually an MpqFile
{
    MapFile(const std::string & filePath); // Load map at filePath
    MapFile(FileBrowserPtr<SaveType> fileBrowser); // Load map selected from browser, can use getDefaultOpenMapBrowser()
    MapFile(Sc::Terrain::Tileset tileset, u16 width = 64, u16 height = 64, size_t terrainTypeIndex = 0,
        Chk::DefaultTriggers defaultTriggers = Chk::DefaultTriggers::NoTriggers, SaveType saveType = SaveType::HybridScm,
        const Sc::Terrain::Tiles* tilesetData = nullptr); // Create new map
    MapFile();
        
    bool save(const std::string & saveFilePath, bool overwriting = false, bool updateListFile = true, bool lockAnywhere = true, bool autoDefragmentLocations = true);
    bool save(bool saveAs = false, bool updateListFile = true, FileBrowserPtr<SaveType> fileBrowser = getDefaultSaveMapBrowser(),
        bool lockAnywhere = true, bool autoDefragmentLocations = true);

    bool load(const std::string & filePath);
    bool load(FileBrowserPtr<SaveType> fileBrowser = getDefaultOpenMapBrowser());

    static std::string GetStandardSoundDir();

    std::optional<std::vector<u8>> getMpqAsset(const std::string & assetMpqFilePath);
    std::optional<std::vector<std::string>> getListfile();
    bool extractMpqAsset(const std::string & assetMpqFilePath, const std::string & systemFilePath);
    
    std::optional<std::vector<u8>> getSound(size_t stringId);
    std::optional<std::vector<u8>> getSound(const std::string & soundPath);
    bool addSound(size_t stringId); // Adds a sound string to the sound list
    bool addSound(const std::string & srcFilePath, WavQuality wavQuality, bool virtualFile);
    bool addSound(const std::string & srcFilePath, const std::string & destMpqPath, WavQuality wavQuality, bool virtualFile);
    bool addSound(const std::string & destMpqPath, const std::vector<u8> & soundContents, WavQuality wavQuality);
    bool removeSoundBySoundIndex(u16 soundIndex, bool removeIfUsed);
    bool removeSoundByStringId(size_t stringId, bool removeIfUsed);
    SoundStatus getSoundStatus(size_t soundStringId);
    bool getSoundStatuses(std::map<size_t/*stringId*/, SoundStatus> & outSoundStatus, bool includePureStringSounds);
    bool isInVirtualSoundList(const std::string & soundMpqPath) const;

    std::string getFileName() const;
    const std::string & getFilePath() const override;

    static FileBrowserPtr<SaveType> getDefaultOpenMapBrowser();
    static FileBrowserPtr<SaveType> getDefaultSaveMapBrowser();

private:
    std::string mapFilePath = "";

    static std::map<size_t, std::string> virtualSoundTable;
    static u64 nextAssetFileId; // Changes are needed if this is accessed in a multi-threaded environment

    bool getSaveDetails(SaveType & saveType, std::string & saveFilePath, bool & overwriting, FileBrowserPtr<SaveType> fileBrowser) const;
    bool openMapFile(const std::string & filePath);
    std::string getTemporaryMpqPath() override;
    std::size_t getCurrentActionIndex() override;
    void initializeVirtualSoundTable();
};

extern std::string saveTypeToStr(const SaveType & saveType);

extern std::vector<FilterEntry<SaveType>> getOpenMapFilters();
extern std::vector<FilterEntry<SaveType>> getSaveMapFilters();
extern std::vector<FilterEntry<u32>> getSaveTextFilters();
extern std::vector<FilterEntry<u32>> getSoundFilters();
extern std::vector<FilterEntry<u32>> getSaveSoundFilters();

#endif