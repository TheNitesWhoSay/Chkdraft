#ifndef TESTASSETS_H
#define TESTASSETS_H
#include "../MappingCoreLib/MappingCore.h"

extern bool getPreSavePath(std::string & outPreSavePath);
extern bool getEmptyDirectory(std::string & outEmptyDirectoryPath);

struct TestAssets
{
    static bool LoadScData(Sc::Data & data);
};

#endif