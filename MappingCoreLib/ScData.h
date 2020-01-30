#ifndef SCDATA_H
#define SCDATA_H
#include "Basics.h"
#include "Buffer.h"
#include "MpqFile.h"
#include "SystemIO.h"
#include "Sc.h"
#include <unordered_map>
#include <string>
#include <tuple>
#include <map>

// TOOD: Finish Sc.h and delete this file

class ScData
{
public:

    virtual ~ScData();

    Sc::Unit units;
    Sc::Sprite sprites;
    Sc::Terrain terrain;
    Sc::Pcx tunit;
    Sc::Pcx tselect;
    Sc::Pcx tminimap;
    Sc::Ai ai;
    Sc::Upgrade upgrades;
    Sc::Tech techs;
    Sc::Weapon weapons;

    bool Load(Sc::DataFile::BrowserPtr dataFileBrowser = Sc::DataFile::BrowserPtr(new Sc::DataFile::Browser()),
        const std::unordered_map<Sc::DataFile::Priority, Sc::DataFile::Descriptor> & dataFiles = Sc::DataFile::getDefaultDataFiles(),
        const std::string & expectedStarCraftDirectory = GetDefaultScPath(),
        FileBrowserPtr<u32> starCraftBrowser = Sc::DataFile::Browser::getDefaultStarCraftBrowser());
};

#endif