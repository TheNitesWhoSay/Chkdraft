#include <gtest/gtest.h>
#include <fstream>
#include "test_assets.h"

std::optional<std::string> getPreSavePath()
{
    return std::nullopt; // Necessary default to avoid unresolved external for the same method in ChkdraftLib/Settings
}

std::optional<std::string> getEmptyDirectory()
{
    if ( auto moduleDirectory = getModuleDirectory() )
    {
        makeDirectory(*moduleDirectory + "\\gtest");
        return *moduleDirectory + "\\gtest";
    }
    return std::nullopt;
}

bool TestAssets::LoadScData(Sc::Data & data)
{
    char* cScAssetPath = std::getenv("SC_ASSET");
    std::string scAssetPath(cScAssetPath != nullptr ? cScAssetPath : "");
    if ( !scAssetPath.empty() )
    {
        Sc::DataFile::BrowserPtr dataFileBrowser(new Sc::DataFile::Browser());
    
        std::vector<Sc::DataFile::Descriptor> dataFiles {
            Sc::DataFile::Descriptor(Sc::DataFile::Priority::StarDat, false, false, true,
                Sc::DataFile::starDatFileName, makeSystemFilePath(scAssetPath, Sc::DataFile::starDatFileName), nullptr, false),
            Sc::DataFile::Descriptor(Sc::DataFile::Priority::BrooDat, false, false, true,
                Sc::DataFile::brooDatFileName, makeSystemFilePath(scAssetPath, Sc::DataFile::brooDatFileName), nullptr, false),
            Sc::DataFile::Descriptor(Sc::DataFile::Priority::PatchRt, false, false, true,
                Sc::DataFile::patchRtFileName, makeSystemFilePath(scAssetPath, Sc::DataFile::patchRtFileName), nullptr, false),
        };

        bool loadedScData = data.load(dataFileBrowser, dataFiles, getDefaultScPath(), nullptr);
        EXPECT_TRUE(loadedScData);
        return loadedScData;
    }
    return false;
}
