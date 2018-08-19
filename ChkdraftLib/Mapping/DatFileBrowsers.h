#ifndef DATFILEBROWSERS_H
#define DATFILEBROWSERS_H
#include "../../MappingCoreLib/MappingCore.h"
#include "Settings.h"

class ChkdDatFileBrowser : public DatFileBrowser
{
    public:
        ChkdDatFileBrowser();

        virtual std::vector<MpqFilePtr> openScDatFiles(
            const std::unordered_map<DatFilePriority, DatFileDescriptor> &datFiles,
            const std::string &expectedStarCraftDirectory = Settings::starCraftPath,
            FileBrowserPtr starCraftBrowser = DatFileBrowser::defaultStarCraftBrowser);

        virtual bool findStarCraftDirectory(output_param std::string &starCraftDirectory, const std::string &expectedStarCraftDirectory = "", FileBrowserPtr starCraftBrowser = nullptr);

        virtual MpqFilePtr openDatFile(const std::string &datFilePath, const DatFileDescriptor &datFileDescriptor);

    private:
        bool updatedPaths;
};

#endif