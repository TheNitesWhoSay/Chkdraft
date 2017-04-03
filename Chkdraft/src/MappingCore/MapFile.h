#ifndef MAPFILE_H
#define MAPFILE_H
#include "Scenario.h"
#include "FileIO.h"
#include "TemporaryFile.h"
#include <cstdio>
#include <time.h>
#include <map>

#ifdef CHKDRAFT
bool GetPreSavePath(std::string &outPreSavePath); // Gets path holding assets to be written to the map file on save
#endif

enum class SaveType;
class MapFile;
using MapFilePtr = std::shared_ptr<MapFile>;

class MapFile : public Scenario // MapFile is a scenario file + the stuff here
{
    public:
        MapFile();

        virtual bool SaveFile(bool SaveAs);

        bool LoadMapFile(std::string &path); // If you're not providing a path, pass in an empty string
        bool LoadMapFile();

        bool GetPath();
        void SetSaveType(SaveType newSaveType);

        bool AddMpqAsset(const std::string &assetSystemFilePath, const std::string &assetMpqFilePath, WavQuality wavQuality);
        void RemoveMpqAsset(const std::string &assetMpqFilePath);
        bool AddWav(u32 stringIndex); // Adds a WAV string to the WAV list
        bool AddWav(std::string &wavFilePath, WavQuality wavQuality); // Attempts to add a WAV at the given file path to the map
        virtual bool RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed);
        virtual bool RemoveWavByStringIndex(u32 stringIndex, bool removeIfUsed);
        WavStatus GetWavStatus(u32 wavStringIndex);
        bool GetWavStatusMap(std::map<u32/*stringIndex*/, WavStatus> &outWavStatus, bool includePureStringWavs);

        std::string GetFileName();
        std::string GetFilePath();

    protected:
        bool GetTemporaryMpqPath(std::string &temporaryMpqFilePath);
        bool OpenMapFile();
        bool ProcessModifiedAssets(MPQHANDLE hMpq);

    private:
        std::string mapFilePath;
        std::unique_ptr<TemporaryFile> temporaryMpq;
        SaveType saveType;
        std::vector<ModifiedAssetPtr> modifiedAssets; // A record of all MPQ assets changes since the last save

        static u64 nextAssetFileId; // Changes are needed if this is accessed in a multi-threaded environment
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

#endif