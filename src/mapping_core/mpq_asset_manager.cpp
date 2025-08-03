#include "mpq_asset_manager.h"
#include <cross_cut/logger.h>

extern Logger logger;

constexpr bool operator==(const MpqAssetManager::AssetChange & lhs, const MpqAssetManager::AssetChange & rhs)
{
    return lhs.type == rhs.type &&
        lhs.wavQuality == rhs.wavQuality &&
        lhs.archivePath == rhs.archivePath &&
        lhs.assetId == rhs.assetId &&
        lhs.replacedAssetId == rhs.replacedAssetId;
}

bool MpqAssetManager::openTemporaryMpq()
{
    if ( tempMpq.isOpen() )
        return true;
    
    if ( !tempMpqPath.empty() )
        return tempMpq.open(tempMpqPath, false, true);
    
    std::string newTempMpqPath = getTemporaryMpqPath();
    if ( !newTempMpqPath.empty() )
    {
        try {
            if ( tempMpq.create(newTempMpqPath) )
            {
                this->tempMpqPath = tempMpq.getFilePath();
                return true;
            }
        } catch ( ... ) {
            CHKD_ERR("Failed to setup temporary asset storage, out of memory!");
        }
    }
    else
        CHKD_ERR("Failed to setup temporary asset storage, an appropriate file path could not be found!");

    return false;
}

std::optional<std::vector<std::uint8_t>> MpqAssetManager::getAssetById(std::uint64_t assetId)
{
    if ( assetId == NoAsset || !openTemporaryMpq() )
        return std::nullopt;
    else
        return tempMpq.getFile(std::to_string(assetId));
}

std::uint64_t MpqAssetManager::findReplacedAssetInChangeHist(const std::string & archivePath) // This won't return an asset if it was in the MPQ, only in the hist
{
    for ( auto it = assetChangeHist.rbegin(); it != assetChangeHist.rend(); ++it )
    {
        if ( it->archivePath.compare(archivePath) == 0 )
        {
            if ( it->type == AssetChange::Type::Remove ) // Remove for this path came after any adds, no replaced asset
                return NoAsset;
            else // Add/replace came after any removes, replacing it
                return it->assetId;
        }
    }
    return NoAsset; // archivePath not present in change hist
}

std::uint64_t MpqAssetManager::findOrCreateAsset(const std::vector<std::uint8_t> & assetData, bool & existing) // Returns the assetId matching this assetData
{
    if ( !openTemporaryMpq() )
        return NoAsset;

    std::size_t assetDataSize = assetData.size();
    for ( std::size_t existingAssetId = 1; existingAssetId <= nextAssetId; ++existingAssetId )
    {
        auto existingAssetPath = std::to_string(existingAssetId);
        auto size = tempMpq.getFileSize(existingAssetPath);
        if ( assetDataSize == size )
        {
            if ( auto existingAssetData = tempMpq.getFile(existingAssetPath) )
            {
                if ( assetDataSize == existingAssetData->size() && std::memcmp(&assetData[0], &(*existingAssetData)[0], assetDataSize) == 0 )
                {
                    existing = true;
                    return existingAssetId;
                }
            }
        }
    }

    // No existing asset matches, create one
    std::uint64_t assetId = ++(this->nextAssetId);
    tempMpq.addFile(std::to_string(assetId), assetData);
    existing = false;
    return assetId;
}

void MpqAssetManager::queueComittable(Committable && committable)
{
    auto & assetChange = assetChangeHist[committable.index];
    bool addAsset = (assetChange.type == AssetChange::Type::Add && !committable.undo) ||
        (assetChange.type == AssetChange::Type::Remove && committable.undo);

    if ( addAsset )
    {
        for ( auto it = committables.begin(); it != committables.end(); ++it )
        {
            if ( committable.undo != it->undo && committable.index == it->index )
            {
                committables.erase(it); // Cancel previous removal due to opposing undo/redos
                return;
            }
            else
            {
                auto & prev = assetChangeHist[it->index];
                if ( prev.type == AssetChange::Type::Remove && prev.assetId == assetChange.assetId )
                {
                    committables.erase(it); // Same asset being re-added, cancel previous removal
                    return;
                }
                else if ( prev.type == AssetChange::Type::Add && prev.assetId == assetChange.assetId )
                    return; // Already added the same asset
            }
        }
    }
    else // remove asset
    {
        for ( auto it = committables.begin(); it != committables.end(); ++it )
        {
            if ( committable.undo != it->undo && committable.index == it->index )
            {
                committables.erase(it); // Cancel previous addition due to opposing undo/redos
                return;
            }
            else
            {
                auto & prev = assetChangeHist[it->index];
                if ( prev.type == AssetChange::Type::Add && prev.assetId == assetChange.assetId )
                {
                    committables.erase(it); // Same asset being removed, cancel previous addition
                    return;
                }
            }
        }
    }
    committables.push_back(committable);
}

MpqAssetManager::MpqAssetManager() : tempMpq(true, true)
{

}

bool MpqAssetManager::addAsset(const std::string & archivePath, const std::vector<std::uint8_t> & assetData, WavQuality wavQuality)
{
    if ( assetData.empty() )
    {
        logger.error("Asset data was empty!");
        return false;
    }
    else if ( archivePath.empty() )
    {
        logger.error("Archive path was empty!");
        return false;
    }

    std::size_t assetChangeHistIndex = assetChangeHist.size();
    bool existing = false;
    assetChangeHist.push_back(AssetChange{
        .type = AssetChange::Type::Add,
        .wavQuality = wavQuality,
        .archivePath = archivePath,
        .assetId = findOrCreateAsset(assetData, existing),
        .replacedAssetId = findReplacedAssetInChangeHist(archivePath) // This may be resolved when saving if not found here
        });
    actionToAssetChange.insert(std::make_pair(getCurrentActionIndex(), assetChangeHistIndex));
    queueComittable(Committable{.undo = false, .index = assetChangeHistIndex});
    return true;
}

bool MpqAssetManager::addAsset(const std::string & archivePath, const std::string & assetSystemFilePath, WavQuality wavQuality)
{
    if ( auto buffer = fileToBuffer(assetSystemFilePath) )
        return addAsset(archivePath, *buffer, wavQuality);
    else
    {
        logger.error("Failed to read file \"" + assetSystemFilePath + "\"");
        return false;
    }
}

bool MpqAssetManager::removeAsset(const std::string & archivePath)
{
    if ( archivePath.empty() )
    {
        logger.error("Archive path was empty!");
        return false;
    }

    std::size_t assetChangeHistIndex = assetChangeHist.size();
    assetChangeHist.push_back(AssetChange{
        .type = AssetChange::Type::Remove,
        .wavQuality = WavQuality::Uncompressed,
        .archivePath = archivePath,
        .assetId = findReplacedAssetInChangeHist(archivePath), // This may be resolved when saving if not found here
        .replacedAssetId = 0
        });
    actionToAssetChange.insert(std::make_pair(getCurrentActionIndex(), assetChangeHistIndex));
    queueComittable(Committable{.undo = false, .index = assetChangeHistIndex});
    return true;
}

void MpqAssetManager::undoAssetAction(std::size_t actionIndex)
{
    auto found = actionToAssetChange.find(actionIndex);
    if ( found != actionToAssetChange.end() )
    {
        std::size_t assetChangeHistIndex = found->second;
        queueComittable(Committable{.undo = true, .index = assetChangeHistIndex});
    }
}

void MpqAssetManager::redoAssetAction(std::size_t actionIndex)
{
    auto found = actionToAssetChange.find(actionIndex);
    if ( found != actionToAssetChange.end() )
    {
        std::size_t assetChangeHistIndex = found->second;
        queueComittable(Committable{.undo = false, .index = assetChangeHistIndex});
    }
}

bool MpqAssetManager::saveAssets(MpqFile & saveMpq)
{
    if ( committables.empty() )
        return true;
    else if ( !openTemporaryMpq() )
        return false;

    if ( !saveMpq.isOpen() )
        throw std::logic_error("saveMpq passed to MpqAssetManager::saveAssets was not open!");

    for ( std::size_t i=0; i<committables.size(); ++i )
    {
        const auto & committable = committables[i];
        auto & assetChange = assetChangeHist[committable.index];
        bool addAsset = (assetChange.type == AssetChange::Type::Add && !committable.undo) ||
            (assetChange.type == AssetChange::Type::Remove && committable.undo);

        if ( addAsset )
        {
            auto fileData = tempMpq.getFile(std::to_string(assetChange.assetId));
            if ( !fileData )
                logger.error("Failed to get asset to add from temporary MPQ!");
            else if ( !saveMpq.findFile(assetChange.archivePath) ) // Not replacement
                saveMpq.addFile(assetChange.archivePath, *fileData); // Perform actual asset add
            else if ( auto replacedFileData = saveMpq.getFile(assetChange.archivePath) ) // Replacement
            {
                if ( assetChange.replacedAssetId == NoAsset )
                {
                    bool existing = false;
                    std::uint64_t replacementAssetId = findOrCreateAsset(*replacedFileData, existing);
                    if ( existing && assetChange.assetId == replacementAssetId )
                        continue; // Replacing with the same file, skip adding the asset
                    else
                        assetChange.replacedAssetId = replacementAssetId; // Replacing with other file
                }

                saveMpq.addFile(assetChange.archivePath, *fileData); // Perform actual asset replacement
            }
            else // Should have been replacement, but couldn't get the file from the target MPQ
                logger.error("Failed to get asset to add from temporary MPQ!");
        }
        else // remove asset
        {
            if ( assetChange.assetId == NoAsset ) // Preserve or resolve the asset being removed so it can be undone
            {
                if ( auto fileData = saveMpq.getFile(assetChange.archivePath) )
                {
                    bool existing = false;
                    assetChange.assetId = findOrCreateAsset(*fileData, existing);
                }
            }
            std::optional<std::vector<std::uint8_t>> replacedAsset {};
            if ( assetChange.replacedAssetId != NoAsset ) // This is an undo-asset-add which replaced a previous version of an asset
            {
                if ( auto replacedAsset = tempMpq.getFile(std::to_string(assetChange.replacedAssetId)) ) // Fetch previous version from tempMpq
                {
                    saveMpq.removeFile(assetChange.archivePath); // Remove this version of the asset
                    saveMpq.addFile(assetChange.archivePath, *replacedAsset); // Add the previous version of the asset
                }
                else
                    logger.error("Failed to get previous version of asset from temporary MPQ!");
            }
            else // No replaced asset
                saveMpq.removeFile(assetChange.archivePath); // Perform actual asset removal
        }
    }

    if ( saveMpq.save() )
        committables.clear(); // Else might have to retry

    return true;
}

std::optional<std::vector<std::uint8_t>> MpqAssetManager::getRecentlyModdedAsset(const std::string & archivePath)
{
    Committable* recentlyModded = nullptr;
    for ( auto it = committables.rbegin(); it != committables.rend(); ++it )
    {
        auto & assetChange = assetChangeHist[it->index];
        if ( assetChange.archivePath.compare(archivePath) == 0 )
        {
            recentlyModded = &(*it);
            break;
        }
    }

    if ( recentlyModded )
    {
        auto & assetChange = assetChangeHist[recentlyModded->index];
        if ( recentlyModded->undo )
        {
            if ( assetChange.type == AssetChange::Type::Remove ) // Undo removal -> return re-added
                return getAssetById(assetChange.assetId);
            else if ( assetChange.replacedAssetId != NoAsset ) // Undo add replacement -> return replaced
                return getAssetById(assetChange.replacedAssetId);
        }
        else if ( assetChange.type != AssetChange::Type::Remove ) // Do/redo add/replace -> return new
            return getAssetById(assetChange.assetId);
    }
    return std::nullopt;
}

bool MpqAssetManager::isAlreadyRemoved(const std::string & archivePath)
{
    for ( auto it = committables.rbegin(); it != committables.rend(); ++it )
    {
        auto & assetChange = assetChangeHist[it->index];
        if ( assetChange.archivePath.compare(archivePath) == 0 )
        {
            bool addAsset = (assetChange.type == AssetChange::Type::Add && !it->undo) ||
                (assetChange.type == AssetChange::Type::Remove && it->undo);
            
            return !addAsset;
        }
    }
    return false;
}
