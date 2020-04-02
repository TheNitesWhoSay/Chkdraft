#ifndef TESTASSETS_H
#define TESTASSETS_H
#include "../MappingCoreLib/MappingCore.h"

extern bool GetPreSavePath(std::string & outPreSavePath);

struct TestAssets
{
    static bool LoadScData(Sc::Data & data);
};

#endif