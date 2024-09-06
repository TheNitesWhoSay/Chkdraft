#ifndef TESTASSETS_H
#define TESTASSETS_H
#include <mapping_core/mapping_core.h>

extern std::optional<std::string> getPreSavePath();
extern std::optional<std::string> getEmptyDirectory();

struct TestAssets
{
    static bool LoadScData(Sc::Data & data);
};

#endif