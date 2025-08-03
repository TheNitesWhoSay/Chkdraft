#ifndef MPQASSETMANAGER_H
#define MPQASSETMANAGER_H
#include "mpq_file.h"
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

class MpqAssetManager
{
public:
    static constexpr std::uint64_t NoAsset = 0;

    struct AssetChange
    {
        enum class Type : std::uint8_t {
            Add,
            Remove
        };

        Type type = Type::Add;
        WavQuality wavQuality = WavQuality::Uncompressed;
        std::string archivePath = "";
        std::uint64_t assetId = NoAsset; // Same as the path the asset is stored at for this session
        std::uint64_t replacedAssetId = NoAsset; // Same as the path the replaced asset is stored at for this session

        friend constexpr bool operator==(const AssetChange & lhs, const AssetChange & rhs);
    };

protected:

    struct Committable
    {
        bool undo = false;
        std::size_t index = 0;
    };

    std::string tempMpqPath {};
    MpqFile tempMpq;
    std::vector<AssetChange> assetChangeHist {}; // All asset changes in this session
    std::unordered_map<std::size_t, std::size_t> actionToAssetChange {}; // Maps action indexes to assetChangeHist indexes
    std::vector<Committable> committables {}; // All assetChangeHist entries to be committed on next save
    std::uint64_t nextAssetId = 0;

    virtual std::string getTemporaryMpqPath() = 0;

    bool openTemporaryMpq();

    std::optional<std::vector<std::uint8_t>> getAssetById(std::uint64_t assetId);

    std::uint64_t findReplacedAssetInChangeHist(const std::string & archivePath);

    std::uint64_t findOrCreateAsset(const std::vector<std::uint8_t> & assetData, bool & existing);

    virtual std::size_t getCurrentActionIndex() = 0;

    void queueComittable(Committable && committable);

public:

    MpqAssetManager();

    bool addAsset(const std::string & archivePath, const std::vector<std::uint8_t> & assetData, WavQuality wavQuality = WavQuality::Uncompressed);

    bool addAsset(const std::string & archivePath, const std::string & assetSystemFilePath, WavQuality wavQuality = WavQuality::Uncompressed);

    bool removeAsset(const std::string & archivePath);

    void undoAssetAction(std::size_t actionIndex);

    void redoAssetAction(std::size_t actionIndex);

    bool saveAssets(MpqFile & saveMpq); // Precondition: saveMpq is open for writing

    std::optional<std::vector<std::uint8_t>> getRecentlyModdedAsset(const std::string & archivePath);

    bool isAlreadyRemoved(const std::string & archivePath);
};

#endif