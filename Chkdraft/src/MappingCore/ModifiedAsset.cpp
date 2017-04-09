#include "ModifiedAsset.h"
#include <iostream>

u64 ModifiedAsset::nextAssetId(0);

ModifiedAsset::ModifiedAsset(const std::string &assetMpqPath, AssetAction actionTaken)
    : assetMpqPath(assetMpqPath), wavQualitySelected(WavQuality::Uncompressed), actionTaken(actionTaken)
{
    assetTempMpqPath = std::to_string(nextAssetId);
    nextAssetId ++;
}

ModifiedAsset::ModifiedAsset(const std::string &assetMpqPath, WavQuality wavQualitySelected, AssetAction actionTaken)
    : assetMpqPath(assetMpqPath), wavQualitySelected(wavQualitySelected), actionTaken(actionTaken)
{
    assetTempMpqPath = std::to_string(nextAssetId);
    nextAssetId ++;
}
