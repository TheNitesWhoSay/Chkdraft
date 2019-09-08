#include "ModifiedAsset.h"

u64 ModifiedAsset::nextAssetId(0);

ModifiedAsset::ModifiedAsset(const std::string &assetMpqPath, AssetAction actionTaken, WavQuality wavQualitySelected)
    : assetMpqPath(assetMpqPath), wavQualitySelected(WavQuality::Uncompressed), actionTaken(actionTaken)
{
    assetTempMpqPath = std::to_string(nextAssetId);
    nextAssetId ++;
}

ModifiedAsset::~ModifiedAsset()
{

}
