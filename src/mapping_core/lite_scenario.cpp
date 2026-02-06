#include "lite_scenario.h"
#include "sha256.h"
#include "math.h"
#include "cross_cut/logger.h"
#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <chrono>

extern Logger logger;

using Chk::StrCompressFlag;
using Member = RareTs::IndexOf<LiteScenario>;

std::unordered_map<Chk::SectionName, size_t> liteSectionMemberIndex {
    {SectionName::TYPE, Member::type}, {SectionName::VER, Member::version}, {SectionName::IVER, Member::iVersion}, {SectionName::IVE2, Member::i2Version},
    {SectionName::VCOD, Member::validation}, {SectionName::IOWN, Member::iownSlotTypes}, {SectionName::OWNR, Member::slotTypes}, {SectionName::ERA, Member::tileset},
    {SectionName::DIM, Member::dimensions}, {SectionName::SIDE, Member::playerRaces}, {SectionName::MTXM, Member::tiles}, {SectionName::PUNI, Member::unitAvailability},
    {SectionName::UPGR, Member::origUpgradeLeveling}, {SectionName::PTEC, Member::origTechnologyAvailability}, {SectionName::UNIT, Member::units}, {SectionName::ISOM, Member::isomRects},
    {SectionName::TILE, Member::editorTiles}, {SectionName::DD2, Member::doodads}, {SectionName::THG2, Member::sprites}, {SectionName::MASK, Member::tileFog},
    {SectionName::STR, Member::strings}, {SectionName::UPRP, Member::createUnitProperties}, {SectionName::UPUS, Member::createUnitPropertiesUsed}, {SectionName::MRGN, Member::locations},
    {SectionName::TRIG, Member::triggers}, {SectionName::MBRF, Member::briefingTriggers}, {SectionName::FORC, Member::forces}, {SectionName::SPRP, Member::scenarioProperties},
    {SectionName::WAV, Member::soundPaths}, {SectionName::UNIS, Member::origUnitSettings}, {SectionName::UPGS, Member::origUpgradeCosts}, {SectionName::TECS, Member::origTechnologyCosts},
    {SectionName::SWNM, Member::switchNames}, {SectionName::COLR, Member::playerColors}, {SectionName::PUPx, Member::upgradeLeveling}, {SectionName::PTEx, Member::techAvailability},
    {SectionName::UNIx, Member::unitSettings}, {SectionName::UPGx, Member::upgradeCosts}, {SectionName::TECx, Member::techCosts}, {SectionName::STRx, Member::strings},
    {SectionName::CRGB, Member::customColors}, {SectionName::OSTR, Member::editorStringOverrides}, {SectionName::KSTR, Member::editorStrings}, {SectionName::KTRG, Member::triggerExtensions},
    {SectionName::KTGP, Member::triggerGroupings}
};

LiteScenario::LiteScenario() : MapData {
    .playerRaces{
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    },
    .playerColors{
        Chk::PlayerColor::Red, Chk::PlayerColor::Blue, Chk::PlayerColor::Teal, Chk::PlayerColor::Purple,
        Chk::PlayerColor::Orange, Chk::PlayerColor::Brown, Chk::PlayerColor::White, Chk::PlayerColor::Yellow
    },
    .slotTypes{
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    },
    .iownSlotTypes{
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    },
    .dimensions{0, 0}
} {}

LiteScenario::LiteScenario(Sc::Terrain::Tileset tileset, u16 width, u16 height) : MapData {
    .playerRaces{
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    },
    .playerColors{
        Chk::PlayerColor::Red, Chk::PlayerColor::Blue, Chk::PlayerColor::Teal, Chk::PlayerColor::Purple,
        Chk::PlayerColor::Orange, Chk::PlayerColor::Brown, Chk::PlayerColor::White, Chk::PlayerColor::Yellow
    },
    .slotTypes{
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    },
    .iownSlotTypes{
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
        Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
    },
    .dimensions{width, height}, .tileset{tileset},
    .tiles = std::vector<u16>(size_t(width)*size_t(height), u16(0)),
    .editorTiles = std::vector<u16>(size_t(width)*size_t(height), u16(0)),
    .tileFog = std::vector<u8>(size_t(width)*size_t(height), u8(0)),
    .isomRects = std::vector<Chk::IsomRect>((size_t(width) / size_t(2) + size_t(1)) * (size_t(height) + size_t(1))),
    .tailData{}, .tailLength{0}, .mapIsProtected{false}, .jumpCompress{false}
} {
    strings.push_back(std::nullopt); // 0 (always unused)
    strings.push_back("Untitled Scenario"); // 1
    strings.push_back("Destroy all enemy buildings."); // 2
    strings.push_back("Anywhere"); // 3
    strings.push_back("Force 1"); // 4
    strings.push_back("Force 2"); // 5
    strings.push_back("Force 3"); // 6
    strings.push_back("Force 4"); // 7
    for ( size_t i=strings.size(); i<=1024; ++i )
        strings.push_back(std::nullopt);

    for ( size_t i=0; i<Chk::LocationId::Anywhere; i++ ) // Note: Index 0 is unused
        locations.push_back(Chk::Location{});

    Chk::Location anywhere {};
    anywhere.right = (s32)width*32;
    anywhere.bottom = (s32)height*32;
    anywhere.stringId = 3;
    locations.push_back(anywhere);

    if ( this->isHybridOrAbove() )
    {
        for ( size_t i=Chk::LocationId::Anywhere; i<Chk::TotalLocations; i++ )
            locations.push_back(Chk::Location{});
    }

    if ( this->isHybridOrAbove() )
        saveSections.push_back({Chk::SectionName::TYPE});
    
    saveSections.push_back({Chk::SectionName::VER});

    if ( !this->isHybridOrAbove() )
        saveSections.push_back({Chk::SectionName::IVER});

    saveSections.insert(saveSections.end(), {
        {Chk::SectionName::IVE2}, {Chk::SectionName::VCOD}, {Chk::SectionName::IOWN}, {Chk::SectionName::OWNR},
        {Chk::SectionName::ERA }, {Chk::SectionName::DIM }, {Chk::SectionName::SIDE}, {Chk::SectionName::MTXM},
        {Chk::SectionName::PUNI}, {Chk::SectionName::UPGR}, {Chk::SectionName::PTEC}, {Chk::SectionName::UNIT},
        {Chk::SectionName::ISOM}, {Chk::SectionName::TILE}, {Chk::SectionName::DD2 }, {Chk::SectionName::THG2},
        {Chk::SectionName::MASK}, {Chk::SectionName::STR }, {Chk::SectionName::UPRP}, {Chk::SectionName::UPUS},
        {Chk::SectionName::MRGN}, {Chk::SectionName::TRIG}, {Chk::SectionName::MBRF}, {Chk::SectionName::SPRP},
        {Chk::SectionName::FORC}, {Chk::SectionName::WAV }, {Chk::SectionName::UNIS}, {Chk::SectionName::UPGS},
        {Chk::SectionName::TECS}, {Chk::SectionName::SWNM}, {Chk::SectionName::COLR}, {Chk::SectionName::PUPx},
        {Chk::SectionName::PTEx}, {Chk::SectionName::UNIx}, {Chk::SectionName::UPGx}, {Chk::SectionName::TECx}
    });
}

bool LiteScenario::empty() const
{
    return saveSections.empty() && tailLength == 0;
}

bool LiteScenario::isProtected() const
{
    return mapIsProtected;
}

bool LiteScenario::hasPassword() const
{
    return tailLength == 7;
}

bool LiteScenario::isPassword(const std::string & password) const
{
    if ( hasPassword() )
    {
        SHA256 sha256;
        std::string hashStr = sha256(password);
        if ( hashStr.length() >= 7 )
        {
            u64 eightHashBytes = std::stoull(hashStr.substr(0, 8), nullptr, 16);
            u8* hashBytes = (u8*)&eightHashBytes;

            for ( u8 i = 0; i < tailLength && i < 8; i++ )
            {
                if ( tailData[i] != hashBytes[i] )
                    return false;
            }
            return true;
        }
        else
            return false;
    }
    else // No password
        return false;
}

bool LiteScenario::setPassword(const std::string & oldPass, const std::string & newPass)
{
    if ( !hasPassword() || isPassword(oldPass) )
    {
        if ( newPass == "" )
        {
            for ( u8 i = 0; i < tailLength && i < 8; i++ )
                tailData[i] = 0;

            tailLength = 0;
            return true;
        }
        else
        {
            SHA256 sha256;
            std::string hashStr = sha256(newPass);
            if ( hashStr.length() >= 7 )
            {
                u64 eightHashBytes = stoull(hashStr.substr(0, 8), nullptr, 16);
                u8* hashBytes = (u8*)&eightHashBytes;

                tailLength = 7;
                for ( u8 i = 0; i < tailLength && i < 8; i++ )
                    tailData[i] = hashBytes[i];

                return true;
            }
        }
    }
    return false;
}

bool LiteScenario::login(const std::string & password)
{
    if ( isPassword(password) )
    {
        mapIsProtected = false;
        return true;
    }
    return false;
}

template <typename T>
constexpr size_t liteSize()
{
    if constexpr ( RareTs::is_static_array_v<T> )
        return ::liteSize<RareTs::element_type_t<T>>() * std::extent_v<T>;
    else if constexpr ( RareTs::is_iterable_v<T> )
        throw std::logic_error("Cannot get a static size for a non-static-array iterable type");
    else if constexpr ( std::is_enum_v<T> )
        return sizeof(std::underlying_type_t<T>);
    else if constexpr ( RareTs::is_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([](auto ... member) {
            return (liteSize<typename std::remove_reference_t<decltype(member)>::type>() + ...);
        });
    }
    else
        return sizeof(T);
}

template <typename T, typename Value>
constexpr s32 liteSize(const Value & value)
{
    if constexpr ( RareTs::has_begin_end_v<T> )
        return s32(::liteSize<RareTs::element_type_t<T>>() * value.size());
    else
        return s32(::liteSize<T>());
}

template <typename T, typename Value>
void liteRead(std::istream & is, Value & value, std::streamsize sectionSize)
{
    if constexpr ( RareTs::is_static_array_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( size_t i=0; i<std::extent_v<T>; i++ )
            ::liteRead<Element>(is, value[i], sectionSize);
        //is.read(reinterpret_cast<char*>(&value[0]), std::streamsize(std::extent_v<T>*sizeof(Element))); // Performs better, TODO: static check for safety then use
    }
    else if constexpr ( RareTs::is_specialization_v<T, std::vector> )
    {
        using Element = RareTs::element_type_t<T>;
        size_t wholeElements = size_t(sectionSize)/sizeof(Element) + (size_t(sectionSize)%sizeof(Element) > 0 ? 1 : 0);
        value = std::vector<Element>(wholeElements);
        for ( size_t i=0; i<wholeElements; ++i )
            ::liteRead<Element>(is, value[i], sectionSize);
        //is.read(reinterpret_cast<char*>(&value[0]), std::streamsize(wholeElements*sizeof(Element))); // Performs better, TODO: static check for safety then use
    }
    else if constexpr ( RareTs::is_in_class_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([&](auto ... member) {
            (::liteRead<typename std::remove_reference_t<decltype(member)>::type>(is, member.value(value), sectionSize), ...);
        });
    }
    else if constexpr ( std::is_enum_v<T> )
        is.read(reinterpret_cast<char*>(&value), sizeof(std::underlying_type_t<T>));
    else
        is.read(reinterpret_cast<char*>(&value), sizeof(T));
}

void LiteScenario::read(std::istream & is, Chk::SectionName sectionName, Chk::SectionSize sectionSize)
{
    auto & section = addSaveSection(Section{sectionName});
    switch ( sectionName )
    {
        case SectionName::MRGN: // Manual deserialization to account for zeroth location being unused
        {
            size_t numLocations = size_t(sectionSize) / ::liteSize<Chk::Location>();
            locations.assign(numLocations+1, Chk::Location{});
            for ( size_t i=1; i<=numLocations; ++i )
                ::liteRead<Chk::Location>(is, locations[i], std::streamsize(::liteSize<Chk::Location>()));
        }
        break;
        case SectionName::MTXM:
        {
            size_t totalTiles = sectionSize/2 + (sectionSize % 2 > 0 ? 1 : 0);
            this->tiles = std::vector<u16>(totalTiles);
            is.read(reinterpret_cast<char*>(&this->tiles[0]), sectionSize);
        }
        break;
        case SectionName::STR:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            if ( !hasSection(SectionName::STRx) )
                syncBytesToStrings(bytes);
        }
        break;
        case SectionName::STRx:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            syncRemasteredBytesToStrings(bytes);
        }
        break;
        case SectionName::KSTR:
        {
            std::vector<u8> bytes(sectionSize);
            is.read(reinterpret_cast<char*>(&bytes[0]), sectionSize);
            syncBytesToKstrings(bytes);
        }
        break;
        default:
        {
            auto memberIndex = liteSectionMemberIndex.find(sectionName);
            if ( memberIndex != liteSectionMemberIndex.end() )
            {
                RareTs::Members<MapData>::at(memberIndex->second, (MapData &)(*this), [&](auto member, auto & value) {
                    std::streamsize bytesRemaining = std::streamsize(sectionSize);
                    ::liteRead<typename decltype(member)::type>(is, value, bytesRemaining);
                });
            }
            else
            {
                logger.info() << "Encountered unknown section: \"" << Chk::getNameString(sectionName) << "\"" << std::endl;
                section.sectionData = std::make_optional<std::vector<u8>>(size_t(sectionSize));
                if ( sectionSize > 0 )
                    is.read((char*)&section.sectionData.value()[0], std::streamsize(sectionSize));
            }
        }
        break;
    }
}

void LiteScenario::clear()
{
    version = Chk::Version::StarCraft_Hybrid;
    type = Chk::Type::RAWS;
    iVersion = Chk::IVersion::Current;
    i2Version = Chk::I2Version::StarCraft_1_04;
    validation = Chk::VCOD{};
    strings.clear();
    editorStrings.clear();
    editorStringsVersion = Chk::KstrVersion::Current;
    editorStringOverrides = Chk::OSTR{};
    scenarioProperties = Chk::SPRP{};

    Chk::Race playerRaces[Sc::Player::Total] {
        Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
        Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
        Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
    };
    for ( size_t i=0; i<Sc::Player::Total; ++i )
        this->playerRaces[i] = playerRaces[i];

    for ( size_t i=0; i<Sc::Player::TotalSlots; ++i )
        playerColors[i] = Chk::PlayerColor(i);

    customColors = Chk::CRGB{};
    forces = Chk::FORC{};

    for ( size_t i=0; i<Sc::Player::TotalSlots; ++i )
    {
        slotTypes[i] = Sc::Player::SlotType::GameOpen;
        iownSlotTypes[i] = Sc::Player::SlotType::Inactive;
    }
    for ( size_t i=Sc::Player::TotalSlots; i<Sc::Player::Total; ++i )
    {
        slotTypes[i] = Sc::Player::SlotType::GameOpen;
        iownSlotTypes[i] = Sc::Player::SlotType::Inactive;
    }

    sprites.clear();
    doodads.clear();
    units.clear();
    locations.clear();

    dimensions = Chk::DIM{0, 0};
    tileset = Sc::Terrain::Tileset::Badlands;
    tiles.clear();
    editorTiles.clear();
    tileFog.clear();
    isomRects.clear();

    unitAvailability = Chk::PUNI{};
    unitSettings = Chk::UNIx{};
    upgradeCosts = Chk::UPGx{};
    upgradeLeveling = Chk::PUPx{};
    techCosts = Chk::TECx{};
    techAvailability = Chk::PTEx{};

    origUnitSettings = Chk::UNIS{};
    origUpgradeCosts = Chk::UPGS{};
    origUpgradeLeveling = Chk::UPGR{};
    origTechnologyCosts = Chk::TECS{};
    origTechnologyAvailability = Chk::PTEC{};

    std::memset(&createUnitProperties, 0, sizeof(Chk::Cuwp)*Sc::Unit::MaxCuwps);
    std::memset(&createUnitPropertiesUsed, 0, sizeof(Chk::CuwpUsed)*Sc::Unit::MaxCuwps);
    triggers.clear();
    briefingTriggers.clear();
    std::memset(&switchNames, 0, sizeof(u32)*Chk::TotalSwitches);
    std::memset(&soundPaths, 0, sizeof(u32)*Chk::TotalSounds);
    triggerExtensions.clear();
    triggerGroupings.clear();

    saveSections.clear();

    for ( size_t i=0; i<tailData.size(); i++ )
        tailData[i] = u8(0);

    tailLength = 0;
    
    mapIsProtected = false;
    jumpCompress = false;

    strTailData.clear();
}

bool LiteScenario::read(std::istream & is)
{
    auto parsingFailed = [&](const std::string & error)
    {
        logger.error(error);
        clear();
        return false;
    };

    clear();
    bool hasLegacyKstr = false;

    // First read contents of "is" to "chk", this will allow jumping backwards when reading chks with jump sections
    std::stringstream chk(std::ios_base::binary|std::ios_base::in|std::ios_base::out);
    chk << is.rdbuf();
    if ( !is.good() && !is.eof() )
    {
        logger.error("Unexpected failure reading scenario contents!");
        return false; // Read error on "is"
    }

    chk.seekg(std::ios_base::beg); // Move to start of chk
    auto streamStart = chk.tellg();
    do
    {
        Chk::SectionHeader sectionHeader = {};
        chk.read((char*)&sectionHeader, sizeof(Chk::SectionHeader));
        std::streamsize headerBytesRead = chk.good() ? sizeof(Chk::SectionHeader) : chk.eof() ? chk.gcount() : 0;
        if ( headerBytesRead == 0 && chk.eof() )
            break;
        else if ( headerBytesRead == 0 )
            return parsingFailed("Unexpected failure reading chk section header!");

        if ( headerBytesRead == sizeof(Chk::SectionHeader) ) // Valid section header
        {
            if ( sectionHeader.sizeInBytes >= 0 ) // Regular section
            {
                auto begin = chk.tellg();
                read(chk, sectionHeader.name, sectionHeader.sizeInBytes);

                if ( chk.good() || chk.eof() )
                {
                    if ( Chk::SectionSize(chk.tellg() - begin) != sectionHeader.sizeInBytes ) // Undersized section
                        mapIsProtected = true;
                }
                else
                    return parsingFailed("Unexpected error reading chk section contents!");
            }
            else // if ( sectionHeader.sizeInBytes < 0 ) // Jump section
            {
                if ( sectionHeader.sizeInBytes < s32(streamStart)-s32(chk.tellg()) )
                {
                    mapIsProtected = true;
                    break;
                }

                chk.seekg(sectionHeader.sizeInBytes, std::ios_base::cur);
                if ( !chk.good() )
                    return parsingFailed("Unexpected error processing chk jump section!");

                jumpCompress = true;
            }
        }
        else // if ( bytesRead < sizeof(Chk::SectionHeader) ) // Partial section header
        {
            for ( std::streamsize i=0; i<headerBytesRead; i++ )
                tailData[size_t(i)] = ((u8*)&sectionHeader)[size_t(i)];
            for ( size_t i=size_t(headerBytesRead); i<tailData.size(); i++ )
                tailData[i] = u8(0);

            tailLength = (u8)headerBytesRead;
            mapIsProtected = true;
        }
    }
    while ( chk.good() );

    if ( !hasSection(SectionName::VER) )
        return parsingFailed("Map was missing the VER section!");
    else if ( this->version > Chk::Version::StarCraft_Remastered )
    {
        logger.error() << "Map has a newer CHK version than Chkdraft supports: " << this->version << std::endl;
        mapIsProtected = true;
    }

    this->fixTerrainToDimensions();
    this->fixTriggerExtensions();
    upgradeKstrToCurrent();

    return true;
}

bool LiteScenario::hasSection(SectionName sectionName) const
{
    for ( auto & section : saveSections )
    {
        if ( section.sectionName == sectionName )
            return true;
    }
    return false;
}

template <typename T, typename Value>
void liteWrite(std::ostream & os, const Value & value)
{
    if constexpr ( std::is_array_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( size_t i=0; i<std::extent_v<T>; i++ )
            liteWrite<Element>(os, value[i]);
    }
    else if constexpr ( RareTs::has_begin_end_v<T> )
    {
        using Element = RareTs::element_type_t<T>;
        for ( const auto & element : value )
            liteWrite<Element>(os, element);
    }
    else if constexpr ( RareTs::is_reflected_v<T> )
    {
        return RareTs::Members<T>::pack([&](auto ... member) {
            (liteWrite<typename decltype(member)::type>(os, member.value(value)), ...);
        });
    }
    else if constexpr ( std::is_enum_v<T> )
        os.write(reinterpret_cast<const char*>(&value), sizeof(std::underlying_type_t<T>));
    else
        os.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

template <typename Member, typename Value>
void liteWriteData(std::ostream & os, const Value & value)
{
    s32 size = ::liteSize<typename Member::type>(value);
    os.write(reinterpret_cast<const char*>(&size), sizeof(size));
    liteWrite<typename Member::type>(os, value);
}

void LiteScenario::writeSection(std::ostream & os, const Section & section, bool includeHeader)
{
    SectionName sectionName = section.sectionName;
    if ( includeHeader )
        os.write(reinterpret_cast<const char*>(&sectionName), sizeof(std::underlying_type_t<SectionName>)); // Write section name

    switch ( sectionName )
    {
        case SectionName::MRGN: // Manual serialization to account for zeroth location being unused
        {
            if ( includeHeader )
            {
                s32 size = s32((locations.size()-1) * ::liteSize<Chk::Location>());
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
            }

            for ( size_t i=1; i<locations.size(); ++i )
                ::liteWrite<Chk::Location>(os, locations[i]);
        }
        break;
        case SectionName::STR:
        {
            std::vector<u8> bytes;
            syncStringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        case SectionName::STRx:
        {
            std::vector<u8> bytes;
            syncRemasteredStringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        case SectionName::KSTR:
        {
            std::vector<u8> bytes;
            syncKstringsToBytes(bytes);
            s32 size = s32(bytes.size());
            if ( includeHeader )
                os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));

            os.write(reinterpret_cast<const char*>(&bytes[0]), std::streamsize(size));
        }
        break;
        default:
        {
            auto memberIndex = liteSectionMemberIndex.find(sectionName);
            if ( memberIndex != liteSectionMemberIndex.end() ) // This is a section that can be auto-serialized using reflection
            {
                RareTs::Members<LiteScenario>::at(memberIndex->second, *this, [&](auto member, auto & value) {
                    ::liteWriteData<decltype(member)>(os, value);
                });
            }
            else // This is an unknown/custom section
            {
                logger.info() << "[" << Chk::getNameString(sectionName) << "] unknown/custom" << std::endl;
                if ( section.sectionData )
                {
                    s32 size = s32(section.sectionData->size());
                    os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
                    if ( size > 0 )
                        os.write(reinterpret_cast<const char*>(&section.sectionData.value()[0]), std::streamsize(size));
                }
                else
                {
                    s32 size = 0;
                    os.write(reinterpret_cast<const char*>(&size), std::streamsize(sizeof(size)));
                }
            }
        }
        break;
    }
}

void LiteScenario::writeSection(std::ostream & os, Chk::SectionName sectionName, bool includeHeader)
{
    auto memberIndex = liteSectionMemberIndex.find(sectionName);
    if ( memberIndex != liteSectionMemberIndex.end() ) // This is a section that can be auto-serialized using reflection
    {
        RareTs::Members<LiteScenario>::at(memberIndex->second, *this, [&](auto member, auto & value) {
            ::liteWriteData<decltype(member)>(os, value);
        });
    }
    else // This may be a section with special serialization, a custom, or an unknown section
    {
        bool found = false;
        for ( const auto & section : saveSections )
        {
            if ( sectionName == section.sectionName )
            {
                found = true;
                writeSection(os, section, includeHeader);
                break;
            }
        }
        if ( !found )
            throw std::invalid_argument("The given section was not present in the scenario file!");
    }
}

void LiteScenario::write(std::ostream & os)
{
    try
    {
        for ( const auto & section : saveSections )
            writeSection(os, section, true);

        if ( tailLength > 0 )
            os.write(reinterpret_cast<const char*>(&tailData[0]), std::streamsize(tailLength < 8 ? tailLength : 7));
    }
    catch ( std::exception & e )
    {
        os.setstate(std::ios_base::failbit);
        logger.error("Error writing scenario file ", e);
    }
}

std::vector<u8> LiteScenario::serialize()
{
    Chk::Size size = 0;
    std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
    chk.write((const char*)&Chk::CHK, sizeof(Chk::CHK)); // Header
    chk.write((const char*)&size, sizeof(size)); // Size

    write(chk);
    chk.unsetf(std::ios_base::skipws);
    auto start = std::istream_iterator<u8>(chk);
    std::vector<u8> chkBytes(start, std::istream_iterator<u8>());

    (Chk::Size &)chkBytes[sizeof(Chk::CHK)] = Chk::Size(chkBytes.size() - sizeof(Chk::CHK) - sizeof(size));

    return chkBytes;
}

bool LiteScenario::deserialize(Chk::SerializedChk* data)
{
    if ( data->header.name == Chk::CHK )
    {
        Chk::Size size = data->header.sizeInBytes;
        std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
        std::copy(&data->data[0], &data->data[size], std::ostream_iterator<u8>(chk));
    }
    return false;
}

LiteScenario::Section & LiteScenario::addSaveSection(Section section)
{
    if ( hasSection(section.sectionName) )
    {
        for ( auto & existing : saveSections )
        {
            if ( section.sectionName == existing.sectionName )
                return existing;
        }
        throw std::logic_error("An internal error occured");
    }
    else
    {
        saveSections.push_back(section);
        return saveSections.back();
    }
}

void LiteScenario::addSaveSection(Chk::SectionName sectionName)
{
    if ( !hasSection(sectionName) )
        LiteScenario::addSaveSection(LiteScenario::Section{sectionName, std::nullopt});
}

void LiteScenario::removeSaveSection(Chk::SectionName sectionName)
{
    if ( hasSection(sectionName) )
    {
        auto toRemove = std::remove_if(saveSections.begin(), saveSections.end(), [&](auto & section){ return section.sectionName == sectionName; });
        saveSections.erase(toRemove);
    }
}

void LiteScenario::updateSaveSections()
{
    if ( this->hasExtendedStrings() )
    {
        addSaveSection(Section{SectionName::OSTR});
        addSaveSection(Section{SectionName::KSTR});
    }

    if ( !this->triggerExtensions.empty() )
        addSaveSection(Section{SectionName::KTRG});

    if ( !this->triggerGroupings.empty() )
        addSaveSection(Section{SectionName::KTGP});
}

bool LiteScenario::changeVersionTo(Chk::Version version, bool lockAnywhere, bool autoDefragmentLocations)
{
    auto oldVersion = this->version;

    if ( version < Chk::Version::StarCraft_BroodWar ) // Original or Hybrid: include all original properties
    {
        if ( version < Chk::Version::StarCraft_Hybrid ) // Original: No COLR, TYPE, IVE2, or expansion properties
        {
            if ( !this->trimLocationsToOriginal(lockAnywhere, autoDefragmentLocations) )
            {
                logger.error("Cannot save as original with over 64 locations in use!");
                return false;
            }
            this->type = Chk::Type::RAWS;
            this->iVersion = Chk::IVersion::Current;
            removeSaveSection(SectionName::TYPE);
            removeSaveSection(SectionName::PUPx);
            removeSaveSection(SectionName::PTEx);
            removeSaveSection(SectionName::UNIx);
            removeSaveSection(SectionName::TECx);
            removeSaveSection(SectionName::COLR);
        }
        else
        {
            this->type = Chk::Type::RAWS;
            this->iVersion = Chk::IVersion::Current;
            this->i2Version = Chk::I2Version::StarCraft_1_04;
            this->expandToScHybridOrExpansion();
        }
        addSaveSection(Section{SectionName::IVER});
        addSaveSection(Section{SectionName::UPGR});
        addSaveSection(Section{SectionName::PTEC});
        addSaveSection(Section{SectionName::UNIS});
        addSaveSection(Section{SectionName::UPGS});
        addSaveSection(Section{SectionName::TECS});
    }
    else // if ( version >= Chk::Version::StarCraft_BroodWar ) // Broodwar: No IVER or original properties
    {
        this->type = Chk::Type::RAWB;
        this->iVersion = Chk::IVersion::Current;
        this->i2Version = Chk::I2Version::StarCraft_1_04;
        this->expandToScHybridOrExpansion();

        removeSaveSection(SectionName::IVER);
        removeSaveSection(SectionName::UPGR);
        removeSaveSection(SectionName::PTEC);
        removeSaveSection(SectionName::UNIS);
        removeSaveSection(SectionName::UPGS);
        removeSaveSection(SectionName::TECS);
    }
        
    if ( version >= Chk::Version::StarCraft_Hybrid ) // Hybrid or BroodWar: Include TYPE, IVE2, COLR, and all expansion properties
    {
        this->type = version >= Chk::Version::StarCraft_BroodWar ? Chk::Type::RAWB : Chk::Type::RAWS;
        addSaveSection(Section{SectionName::TYPE});
        addSaveSection(Section{SectionName::PUPx});
        addSaveSection(Section{SectionName::PTEx});
        addSaveSection(Section{SectionName::UNIx});
        addSaveSection(Section{SectionName::UPGx});
        addSaveSection(Section{SectionName::TECx});

        if ( !hasSection(SectionName::COLR) )
        {
            addSaveSection(Section{SectionName::COLR});
            for ( size_t i=size_t(Chk::PlayerColor::Red); i<=Chk::PlayerColor::Yellow; ++i )
                this->setPlayerColor(i, Chk::PlayerColor(i));
        }
    }
    
    this->version = version;
    this->deleteUnusedStrings(Chk::Scope::Both);
    return true;
}

Chk::Version LiteScenario::getVersion() const
{
    return this->version;
}

bool LiteScenario::isVersion(Chk::Version version) const
{
    return version == this->version;
}

bool LiteScenario::isOriginal() const
{
    return this->version < Chk::Version::StarCraft_Hybrid;
}

bool LiteScenario::isHybrid() const
{
    return this->version >= Chk::Version::StarCraft_Hybrid && this->version < Chk::Version::StarCraft_BroodWar;
}

bool LiteScenario::isExpansion() const
{
    return this->version >= Chk::Version::StarCraft_BroodWar && this->version < Chk::Version::StarCraft_Remastered;
}

bool LiteScenario::isHybridOrAbove() const
{
    return this->version >= Chk::Version::StarCraft_Hybrid;
}

bool LiteScenario::isRemastered() const
{
    return this->version >= Chk::Version::StarCraft_Remastered;
}

bool LiteScenario::hasDefaultValidation() const
{
    Chk::VCOD vcod{};
    for ( size_t i=0; i<Chk::TotalValidationSeeds; ++i )
    {
        if ( this->validation.seedValues[i] != vcod.seedValues[i] )
            return false;
    }
    for ( size_t i=0; i<Chk::TotalValidationOpCodes; ++i )
    {
        if ( this->validation.opCodes[i] != vcod.opCodes[i] )
            return false;
    }
    return true;
}

void LiteScenario::setToDefaultValidation()
{
    Chk::VCOD vcod = Chk::VCOD {};
    for ( size_t i=0; i<Chk::TotalValidationSeeds; ++i )
        this->validation.seedValues[i] = vcod.seedValues[i];
    for ( size_t i=0; i<Chk::TotalValidationOpCodes; ++i )
        this->validation.opCodes[i] = vcod.opCodes[i];
}

bool LiteScenario::hasExtendedStrings() const
{
    return !this->editorStrings.empty();
}

size_t LiteScenario::getCapacity(Chk::Scope storageScope) const
{
    if ( storageScope == Chk::Scope::Game )
        return this->strings.size();
    else if ( storageScope == Chk::Scope::Editor )
        return this->editorStrings.size();
    else
        return 0;
}

size_t LiteScenario::getBytesUsed(Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        std::vector<u8> bytes {};
        if ( hasSection(SectionName::STRx) )
            syncRemasteredStringsToBytes(bytes);
        else
            syncStringsToBytes(bytes);

        return bytes.size();
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        std::vector<u8> bytes {};
        syncKstringsToBytes(bytes);
        return bytes.size();
    }
    else
        return 0;
}

bool LiteScenario::stringStored(size_t stringId, Chk::Scope storageScope) const
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
        return stringId < this->strings.size() && this->strings[stringId];
    else if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
        return stringId < this->editorStrings.size() && this->editorStrings[stringId];

    return false;
}

void LiteScenario::appendUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope, u32 userMask) const
{
    if ( stringId == Chk::StringId::NoString )
        return;

    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( (userMask & Chk::StringUserFlag::ScenarioProperties) != Chk::StringUserFlag::None )
            {
                if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && this->scenarioProperties.scenarioNameStringId == (u16)stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioName));

                if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && this->scenarioProperties.scenarioDescriptionStringId == (u16)stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioDescription));
            }
            if ( (userMask & Chk::StringUserFlag::Force) != Chk::StringUserFlag::None )
            {
                appendForceStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::BothUnitSettings) != Chk::StringUserFlag::None )
            {
                appendUnitStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::Location) != Chk::StringUserFlag::None )
            {
                appendLocationStrUsage(stringId, stringUsers, userMask);
            }
            if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None )
            {
                appendTriggerStrUsage(stringId, stringUsers, storageScope, userMask);
            }
        }
        else if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None ) // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            appendTriggerStrUsage(stringId, stringUsers, storageScope, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( this->editorStringOverrides.scenarioName == stringId )
            stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioName));

        if ( this->editorStringOverrides.scenarioDescription == stringId )
            stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ScenarioDescription));

        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( this->editorStringOverrides.forceName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Force, i));
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->editorStringOverrides.unitName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::OriginalUnitSettings, i));
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->editorStringOverrides.expUnitName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExpansionUnitSettings, i));
        }

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( this->editorStringOverrides.soundPath[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Sound, i));
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( this->editorStringOverrides.switchName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Switch, i));
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            if ( this->editorStringOverrides.locationName[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Location, i));
        }
    }
}

bool LiteScenario::stringUsed(size_t stringId, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask, bool ensureStored) const
{
    if ( storageScope == Chk::Scope::Game && ((stringId < this->strings.size() && this->strings[stringId]) || !ensureStored) )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( usageScope == Chk::Scope::Editor )
                return locationStringUsed(stringId, storageScope, userMask) || triggerEditorStringUsed(stringId, storageScope, userMask);
            else if ( usageScope == Chk::Scope::Game )
            {
                return ((userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && this->scenarioProperties.scenarioNameStringId == (u16)stringId ) ||
                    ((userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && this->scenarioProperties.scenarioDescriptionStringId == (u16)stringId ) ||
                    forceStringUsed(stringId, userMask) || unitStringUsed(stringId, userMask) || triggerGameStringUsed(stringId, userMask);
            }
            else // if ( usageScope == Chk::Scope::Either )
            {
                return ((userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && this->scenarioProperties.scenarioNameStringId == (u16)stringId ) ||
                    ((userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && this->scenarioProperties.scenarioDescriptionStringId == (u16)stringId ) ||
                    forceStringUsed(stringId, userMask) || unitStringUsed(stringId, userMask) ||
                    locationStringUsed(stringId, storageScope, userMask) || triggerStringUsed(stringId, storageScope, userMask);
            }
        }
        else // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            return usageScope == Chk::Scope::Either && triggerStringUsed(stringId, storageScope, userMask) ||
                usageScope == Chk::Scope::Game && triggerGameStringUsed(stringId, userMask) ||
                usageScope == Chk::Scope::Editor && triggerEditorStringUsed(stringId, storageScope, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && ((stringId < this->editorStrings.size() && this->editorStrings[stringId]) || !ensureStored) )
    {
        if ( this->editorStringOverrides.scenarioName == stringId || this->editorStringOverrides.scenarioDescription == stringId )
            return true;

        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( this->editorStringOverrides.forceName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->editorStringOverrides.unitName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->editorStringOverrides.expUnitName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( this->editorStringOverrides.soundPath[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( this->editorStringOverrides.switchName[i] == stringId )
                return true;
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            if ( this->editorStringOverrides.locationName[i] == stringId )
                return true;
        }
    }
    return false;
}

void LiteScenario::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        bool markGameStrings = (usageScope & Chk::Scope::Game) == Chk::Scope::Game;
        bool markEditorStrings = (usageScope & Chk::Scope::Editor) == Chk::Scope::Editor;

        if ( markGameStrings )
        {
            // {SPRP, Game, u16}: Scenario Name and Scenario Description
            if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && this->scenarioProperties.scenarioNameStringId > 0 )
                stringIdUsed[this->scenarioProperties.scenarioNameStringId] = true;

            if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && this->scenarioProperties.scenarioDescriptionStringId > 0 )
                stringIdUsed[this->scenarioProperties.scenarioDescriptionStringId] = true;

            markUsedForceStrings(stringIdUsed, userMask); // {FORC, Game, u16}: Force Names
            markUsedUnitStrings(stringIdUsed, userMask); // {UNIS, Game, u16}: Unit Names (original); {UNIx, Game, u16}: Unit names (expansion)
            if ( markEditorStrings ) // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, ...
                markUsedTriggerStrings(stringIdUsed, storageScope, userMask); // ... transmission text, next scenario, sound path, comment; {MBRF, Game, u32}: mission objectives, sound, text message
            else
                markUsedTriggerGameStrings(stringIdUsed, userMask); // {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, transmission text, next scenario, sound path
        }

        if ( markEditorStrings )
        {
            markUsedLocationStrings(stringIdUsed, userMask); // {MRGN, Editor, u16}: location name
            if ( !markGameStrings )
                markUsedTriggerEditorStrings(stringIdUsed, storageScope, userMask); // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: comment
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( (userMask & Chk::StringUserFlag::ScenarioName) == Chk::StringUserFlag::ScenarioName && this->editorStringOverrides.scenarioName != 0 )
            stringIdUsed[this->editorStringOverrides.scenarioName] = true;

        if ( (userMask & Chk::StringUserFlag::ScenarioDescription) == Chk::StringUserFlag::ScenarioDescription && this->editorStringOverrides.scenarioDescription != 0 )
            stringIdUsed[this->editorStringOverrides.scenarioDescription] = true;

        if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
        {
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( this->editorStringOverrides.forceName[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.forceName[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
        {
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( this->editorStringOverrides.unitName[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.unitName[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
        {
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( this->editorStringOverrides.expUnitName[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.expUnitName[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        {
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( this->editorStringOverrides.soundPath[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.soundPath[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->editorStringOverrides.switchName[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.switchName[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
        {
            for ( size_t i=0; i<Chk::TotalLocations; i++ )
            {
                if ( this->editorStringOverrides.locationName[i] != 0 )
                    stringIdUsed[this->editorStringOverrides.locationName[i]] = true;
            }
        }
        markUsedTriggerStrings(stringIdUsed, storageScope, userMask);
    }
}

void LiteScenario::markValidUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask) const
{
    markUsedStrings(stringIdUsed, usageScope, storageScope, userMask);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
        {
            size_t limit = std::min((size_t)Chk::MaxStrings, this->strings.size());
            size_t stringId = 1;
            for ( ; stringId < limit; ++stringId )
            {
                if ( stringIdUsed[stringId] && !this->strings[stringId])
                    stringIdUsed[stringId] = false;
            }
            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        }
        break;
        case Chk::Scope::Editor:
        {
            size_t limit = std::min((size_t)Chk::MaxKStrings, this->editorStrings.size());
            size_t stringId = 1;
            for ( ; stringId < limit; ++stringId )
            {
                if ( stringIdUsed[stringId] && !this->editorStrings[stringId] )
                    stringIdUsed[stringId] = false;
            }
            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        }
        break;
        case Chk::Scope::Either:
        {
            size_t limit = std::min(std::min((size_t)Chk::MaxStrings, getCapacity(Chk::Scope::Game)), getCapacity(Chk::Scope::Editor));
            size_t stringId = 1;
            for ( ; stringId < limit; stringId++ )
            {
                if ( stringIdUsed[stringId] && !(stringId < this->strings.size() && this->strings[stringId]) && !(stringId < this->editorStrings.size() && this->editorStrings[stringId]) )
                    stringIdUsed[stringId] = false;
            }

            if ( getCapacity(Chk::Scope::Game) > getCapacity(Chk::Scope::Editor) )
            {
                for ( ; stringId < getCapacity(Chk::Scope::Game); stringId++ )
                {
                    if ( stringIdUsed[stringId] && !(stringId < this->strings.size() && this->strings[stringId]) )
                        stringIdUsed[stringId] = false;
                }
            }
            else if ( getCapacity(Chk::Scope::Editor) > getCapacity(Chk::Scope::Game) )
            {
                for ( ; stringId < getCapacity(Chk::Scope::Editor); stringId++ )
                {
                    if ( stringIdUsed[stringId] && !(stringId < this->editorStrings.size() && this->editorStrings[stringId]) )
                        stringIdUsed[stringId] = false;
                }
            }

            for ( ; stringId < Chk::MaxStrings; stringId++ )
            {
                if ( stringIdUsed[stringId] )
                    stringIdUsed[stringId] = false;
            }
        
        }
        break;
    }
}

StrProp LiteScenario::getProperties(size_t editorStringId) const
{
    return hasSection(Chk::SectionName::KSTR) && editorStringId < editorStrings.size() ? editorStrings[editorStringId]->properties() : StrProp();
}

void LiteScenario::setProperties(size_t editorStringId, const StrProp & strProp)
{
    if ( hasSection(Chk::SectionName::KSTR) )
    {
        if ( editorStringId < editorStrings.size() && editorStrings[editorStringId] )
            editorStrings[editorStringId]->properties() = strProp;
    }
}

template <typename StringType>
std::optional<StringType> LiteScenario::getString(size_t stringId, Chk::Scope storageScope) const
{
    auto getGameString = [&](){
        return stringId < this->strings.size() && this->strings[stringId] ?
            std::optional<StringType>(this->strings[stringId]->toString<StringType>()) : std::nullopt;
    };
    auto getEditorString = [&](){
        return stringId < editorStrings.size() && editorStrings[stringId] ?
            std::optional<StringType>(editorStrings[stringId]->toString<StringType>()) : std::nullopt;
    };
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame:
        {
            auto editorResult = getEditorString();
            return editorResult ? editorResult : getGameString();
        }
        case Chk::Scope::Game: return getGameString();
        case Chk::Scope::Editor: return getEditorString();
        case Chk::Scope::GameOverEditor:
        {
            auto gameResult = getGameString();
            return gameResult ? gameResult : getEditorString();
        }
        default: return std::nullopt;
    }
}
template std::optional<RawString> LiteScenario::getString<RawString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getString<EscString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getString<ChkdString>(size_t stringId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getString<SingleLineChkdString>(size_t stringId, Chk::Scope storageScope) const;

template <typename StringType>
size_t LiteScenario::findString(const StringType & str, Chk::Scope storageScope) const
{
    auto findGameString = [&](){
        for ( size_t stringId=1; stringId<strings.size(); stringId++ )
        {
            if ( strings[stringId] && strings[stringId]->compare<StringType>(str) == 0 )
                return stringId;
        }
        return size_t(Chk::StringId::NoString);
    };
    auto findEditorString = [&](){
        for ( size_t stringId=1; stringId<editorStrings.size(); stringId++ )
        {
            if ( editorStrings[stringId] && editorStrings[stringId]->compare<StringType>(str) == 0 )
                return stringId;
        }
        return size_t(Chk::StringId::NoString);
    };
    switch ( storageScope )
    {
        case Chk::Scope::Game: return findGameString();
        case Chk::Scope::Editor: return findEditorString();
        case Chk::Scope::GameOverEditor:
        case Chk::Scope::Either:
            {
                size_t gameResult = findGameString();
                return gameResult != Chk::StringId::NoString ? gameResult : findEditorString();
            }
        case Chk::Scope::EditorOverGame:
            {
                size_t editorResult = findEditorString();
                return editorResult != Chk::StringId::NoString ? editorResult : findGameString();
            }
    }
    return size_t(Chk::StringId::NoString);
}
template size_t LiteScenario::findString<RawString>(const RawString & str, Chk::Scope storageScope) const;
template size_t LiteScenario::findString<EscString>(const EscString & str, Chk::Scope storageScope) const;
template size_t LiteScenario::findString<ChkdString>(const ChkdString & str, Chk::Scope storageScope) const;
template size_t LiteScenario::findString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope) const;

bool LiteScenario::setCapacity(size_t stringCapacity, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringCapacity > Chk::MaxStrings )
            throw Chk::MaximumStringsExceeded();

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragment(Chk::Scope::Game, false);
            else
                throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        while ( strings.size() <= stringCapacity )
            strings.push_back(std::nullopt);

        while ( strings.size() > stringCapacity )
            strings.pop_back();

        return true;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( stringCapacity > Chk::MaxKStrings )
            throw Chk::MaximumStringsExceeded();

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markValidUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t numValidUsedStrings = 0;
        size_t highestValidUsedStringId = 0;
        for ( size_t stringId = 1; stringId<Chk::MaxStrings; stringId++ )
        {
            if ( stringIdUsed[stringId] )
            {
                numValidUsedStrings ++;
                highestValidUsedStringId = stringId;
            }
        }

        if ( numValidUsedStrings > stringCapacity )
            throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragment(Chk::Scope::Editor, false);
            else
                throw Chk::InsufficientStringCapacity(Chk::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        while ( editorStrings.size() < stringCapacity )
            editorStrings.push_back(std::nullopt);

        while ( editorStrings.size() > stringCapacity )
            editorStrings.pop_back();

        return true;
    }
    return false;
}

template <typename StringType>
size_t LiteScenario::addString(const StringType & str, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        size_t stringId = findString<StringType>(str);
        if ( stringId != (size_t)Chk::StringId::NoString )
            return stringId; // String already exists, return the id

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t nextUnusedStringId = Chk::MaxStrings;
        size_t limit = Chk::MaxStrings;
        for ( size_t i=1; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
            {
                nextUnusedStringId = i;
                break;
            }
        }

        if ( nextUnusedStringId == Chk::MaxStrings )
            throw Chk::MaximumStringsExceeded();
        else if ( nextUnusedStringId >= strings.size() )
            setCapacity(nextUnusedStringId+1, Chk::Scope::Game, autoDefragment);

        strings[nextUnusedStringId] = rawString;
        return nextUnusedStringId;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        size_t stringId = findString<StringType>(str);
        if ( stringId != (size_t)Chk::StringId::NoString )
            return stringId; // String already exists, return the id

        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t nextUnusedStringId = Chk::MaxKStrings;
        size_t limit = Chk::MaxStrings;
        for ( size_t i=1; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
            {
                nextUnusedStringId = i;
                break;
            }
        }
        
        if ( nextUnusedStringId == Chk::MaxKStrings )
            throw Chk::MaximumStringsExceeded();
        else if ( nextUnusedStringId >= editorStrings.size() )
            setCapacity(nextUnusedStringId+1, Chk::Scope::Editor, autoDefragment);

        editorStrings[nextUnusedStringId] = rawString;
        return nextUnusedStringId;
    }
    return (size_t)Chk::StringId::NoString;
}
template size_t LiteScenario::addString<RawString>(const RawString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t LiteScenario::addString<EscString>(const EscString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t LiteScenario::addString<ChkdString>(const ChkdString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t LiteScenario::addString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::replaceString(size_t stringId, const StringType & str, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        if ( stringId < strings.size() )
            strings[stringId] = rawString;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        RawString rawString;
        convertStr<StringType, RawString>(str, rawString);

        if ( stringId < editorStrings.size() )
            editorStrings[stringId] = rawString;
    }
}
template void LiteScenario::replaceString<RawString>(size_t stringId, const RawString & str, Chk::Scope storageScope);
template void LiteScenario::replaceString<EscString>(size_t stringId, const EscString & str, Chk::Scope storageScope);
template void LiteScenario::replaceString<ChkdString>(size_t stringId, const ChkdString & str, Chk::Scope storageScope);
template void LiteScenario::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString & str, Chk::Scope storageScope);

void LiteScenario::deleteUnusedStrings(Chk::Scope storageScope)
{
    auto deleteUnusedGameStrings = [&]() {
        std::bitset<65536> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        for ( size_t i=0; i<strings.size(); i++ )
        {
            if ( !stringIdUsed[i] && strings[i] )
                strings[i] = std::nullopt;
        }
    };
    auto deleteUnusedEditorStrings = [&]() {
        std::bitset<65536> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        for ( size_t i=0; i<editorStrings.size(); i++ )
        {
            if ( !stringIdUsed[i] && editorStrings[i] )
                editorStrings[i] = std::nullopt;
        }
    };
    switch ( storageScope )
    {
        case Chk::Scope::Game: deleteUnusedGameStrings(); break;
        case Chk::Scope::Editor: deleteUnusedEditorStrings(); break;
        case Chk::Scope::Both: deleteUnusedGameStrings(); deleteUnusedEditorStrings(); break;
    }
}

void LiteScenario::deleteString(size_t stringId, Chk::Scope storageScope, bool deleteOnlyIfUnused)
{
    auto deleteGameString = [&](){
        return false;
    };
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Game) )
        {
            if ( stringId < strings.size() )
                strings[stringId] = std::nullopt;

            if ( this->scenarioProperties.scenarioNameStringId == stringId )
                this->scenarioProperties.scenarioNameStringId = 0;

            if ( this->scenarioProperties.scenarioDescriptionStringId == stringId )
                this->scenarioProperties.scenarioDescriptionStringId = 0;

            deleteForceString(stringId);
            deleteUnitString(stringId);
            deleteLocationString(stringId);
            deleteTriggerString(stringId, Chk::Scope::Game);
        }
    }
    
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
        {
            if ( stringId < editorStrings.size() )
                editorStrings[stringId] = std::nullopt;

            if ( this->editorStringOverrides.scenarioName == stringId )
                this->editorStringOverrides.scenarioName = 0;
    
            if ( this->editorStringOverrides.scenarioDescription == stringId )
                this->editorStringOverrides.scenarioDescription = 0;

            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( this->editorStringOverrides.forceName[i] == stringId )
                    this->editorStringOverrides.forceName[i] = 0;
            }

            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( this->editorStringOverrides.unitName[i] == stringId )
                    this->editorStringOverrides.unitName[i] = 0;
            }

            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( this->editorStringOverrides.expUnitName[i] == stringId )
                    this->editorStringOverrides.expUnitName[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( this->editorStringOverrides.soundPath[i] == stringId )
                    this->editorStringOverrides.soundPath[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->editorStringOverrides.switchName[i] == stringId )
                    this->editorStringOverrides.switchName[i] = 0;
            }

            for ( size_t i=0; i<Chk::TotalLocations; i++ )
            {
                if ( this->editorStringOverrides.locationName[i] == stringId )
                    this->editorStringOverrides.locationName[i] = 0;
            }
            deleteTriggerString(stringId, Chk::Scope::Editor);
        }
    }
}

void LiteScenario::moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= strings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Game);
            auto selected = strings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            auto highestString = strings[stringId-1];
                            strings[stringId-1] = std::nullopt;
                            stringIdUsed[stringId-1] = false;
                            strings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            auto lowestString = strings[stringId+1];
                            strings[stringId+1] = std::nullopt;
                            stringIdUsed[stringId+1] = false;
                            strings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                            break;
                        }
                    }
                }
            }
            strings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Game);
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= editorStrings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Editor);
            auto selected = editorStrings[stringIdFrom];
            stringIdUsed[stringIdFrom] = false;
            std::map<u32, u32> stringIdRemappings;
            if ( stringIdTo < stringIdFrom ) // Move to a lower stringId, if there are strings in the way, cascade towards stringIdFrom
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo+1; stringId <= stringIdFrom; stringId ++ ) // Find the lowest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the highest stringId remaining in the block to the next available stringId
                        {
                            auto highestString = editorStrings[stringId-1];
                            editorStrings[stringId-1] = std::nullopt;
                            stringIdUsed[stringId-1] = false;
                            editorStrings[stringId] = highestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId-1), (u32)stringId));
                            break;
                        }
                    }
                }
                stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            }
            else if ( stringIdTo > stringIdFrom ) // Move to a higher stringId, if there are strings in the way, cascade towards stringIdTo
            {
                while ( stringIdUsed[stringIdTo] ) // There is a block of one or more strings where the selected string needs to go
                {
                    for ( size_t stringId = stringIdTo-1; stringId >= stringIdFrom; stringId -- ) // Find the highest available stringId past the block
                    {
                        if ( !stringIdUsed[stringId] ) // Move the lowest stringId in the block to the available stringId
                        {
                            auto lowestString = editorStrings[stringId+1];
                            editorStrings[stringId+1] = std::nullopt;
                            stringIdUsed[stringId+1] = false;
                            editorStrings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                            break;
                        }
                    }
                }
            }
            editorStrings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        }
    }
}

size_t LiteScenario::rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo, bool autoDefragment)
{
    if ( changeStorageScopeTo == Chk::Scope::Editor && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, Chk::StringUserFlag::All, true) )
    {
        RawString toRescope = getString<RawString>(stringId, Chk::Scope::Game).value();
        size_t newStringId = addString<RawString>(toRescope, Chk::Scope::Editor, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == this->scenarioProperties.scenarioNameStringId )
            {
                stringIdsReplaced.insert(this->editorStringOverrides.scenarioName);
                this->editorStringOverrides.scenarioName = u32(newStringId);
            }
            if ( stringId == this->scenarioProperties.scenarioDescriptionStringId )
            {
                stringIdsReplaced.insert(this->editorStringOverrides.scenarioDescription);
                this->editorStringOverrides.scenarioDescription = u32(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == this->forces.forceString[i] )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.forceName[i]);
                    this->editorStringOverrides.forceName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == this->origUnitSettings.nameStringId[i] )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.unitName[i]);
                    this->editorStringOverrides.unitName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == this->unitSettings.nameStringId[i] )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.expUnitName[i]);
                    this->editorStringOverrides.expUnitName[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == this->soundPaths[i] )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.soundPath[i]);
                    this->editorStringOverrides.soundPath[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == this->switchNames[i] )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.switchName[i]);
                    this->editorStringOverrides.switchName[i] = u32(newStringId);
                }
            }
            size_t numLocations = this->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == this->locations[i].stringId )
                {
                    stringIdsReplaced.insert(this->editorStringOverrides.locationName[i]);
                    this->editorStringOverrides.locationName[i] = u32(newStringId);
                }
            }

            deleteString(stringId, Chk::Scope::Game, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Editor, true);
        }
    }
    else if ( changeStorageScopeTo == Chk::Scope::Game && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
    {
        RawString toRescope = getString<RawString>(stringId, Chk::Scope::Editor).value();
        size_t newStringId = addString<RawString>(toRescope, Chk::Scope::Game, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == this->editorStringOverrides.scenarioName )
            {
                stringIdsReplaced.insert((u32)this->scenarioProperties.scenarioNameStringId);
                this->scenarioProperties.scenarioNameStringId = u16(newStringId);
            }
            if ( stringId == this->editorStringOverrides.scenarioDescription )
            {
                stringIdsReplaced.insert((u32)this->scenarioProperties.scenarioDescriptionStringId);
                this->scenarioProperties.scenarioDescriptionStringId = u16(newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == this->editorStringOverrides.forceName[i] )
                {
                    stringIdsReplaced.insert(u32(this->forces.forceString[i]));
                    this->forces.forceString[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == this->editorStringOverrides.unitName[i] )
                {
                    stringIdsReplaced.insert(u32(this->origUnitSettings.nameStringId[i]));
                    this->origUnitSettings.nameStringId[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == this->editorStringOverrides.expUnitName[i] )
                {
                    stringIdsReplaced.insert(u32(this->unitSettings.nameStringId[i]));
                    this->unitSettings.nameStringId[i] = u16(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == this->editorStringOverrides.soundPath[i] )
                {
                    stringIdsReplaced.insert((u32)soundPaths[i]);
                    this->soundPaths[i] = u32(newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == this->editorStringOverrides.switchName[i] )
                {
                    stringIdsReplaced.insert((u32)this->switchNames[i]);
                    this->switchNames[i] = u32(newStringId);
                }
            }
            size_t numLocations = this->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == this->editorStringOverrides.locationName[i] )
                {
                    Chk::Location & location = getLocation(i);
                    stringIdsReplaced.insert(location.stringId);
                    location.stringId = (u16)newStringId;
                }
            }

            deleteString(stringId, Chk::Scope::Editor, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Game, true);
        }
    }
    return 0;
}

std::vector<u8> & LiteScenario::getStrTailData()
{
    return strTailData;
}

size_t LiteScenario::getStrTailDataOffset()
{
    std::vector<u8> stringBytes {};
    if ( hasSection(SectionName::STRx) )
        syncRemasteredStringsToBytes(stringBytes);
    else
        syncStringsToBytes(stringBytes);

    return stringBytes.size();
}

size_t LiteScenario::getScenarioNameStringId(Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? this->editorStringOverrides.scenarioName : this->scenarioProperties.scenarioNameStringId;
}

size_t LiteScenario::getScenarioDescriptionStringId(Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? this->editorStringOverrides.scenarioDescription : this->scenarioProperties.scenarioDescriptionStringId;
}

size_t LiteScenario::getForceNameStringId(Chk::Force force, Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? this->editorStringOverrides.forceName[force] : this->forces.forceString[force];;
}

size_t LiteScenario::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    if ( storageScope == Chk::Scope::Game )
        return useExpansionUnitSettings(useExp) ? unitSettings.nameStringId[unitType] : origUnitSettings.nameStringId[unitType];
    else if ( storageScope == Chk::Scope::Editor )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto: return isHybridOrAbove() ? this->editorStringOverrides.expUnitName[unitType] : this->editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::Yes: return this->editorStringOverrides.expUnitName[unitType];
            case Chk::UseExpSection::No: return this->editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::YesIfAvailable: return this->editorStringOverrides.expUnitName[unitType] != 0 ? this->editorStringOverrides.expUnitName[unitType] : this->editorStringOverrides.unitName[unitType];
            case Chk::UseExpSection::NoIfOrigAvailable: return this->editorStringOverrides.unitName[unitType] != 0 ? this->editorStringOverrides.unitName[unitType] : this->editorStringOverrides.expUnitName[unitType];
        }
    }
    return 0;
}

size_t LiteScenario::getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope) const
{
    return storageScope == Chk::Scope::Editor ? this->editorStringOverrides.soundPath[soundIndex] : this->soundPaths[soundIndex];
}

size_t LiteScenario::getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope) const
{
    if ( switchIndex < Chk::TotalSwitches )
    {
        if ( storageScope == Chk::Scope::Game )
            return this->switchNames[switchIndex];
        else
            return this->editorStringOverrides.switchName[switchIndex];
    }
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

size_t LiteScenario::getLocationNameStringId(size_t locationId, Chk::Scope storageScope) const
{
    if ( storageScope == Chk::Scope::Editor )
        return this->editorStringOverrides.locationName[locationId];
    else if ( locationId < numLocations() )
        return locations[locationId].stringId;
    else
        return 0;
}

void LiteScenario::setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStringOverrides.scenarioName = u32(scenarioNameStringId);
    else
        this->scenarioProperties.scenarioNameStringId = u16(scenarioNameStringId);
}

void LiteScenario::setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStringOverrides.scenarioDescription = u32(scenarioDescriptionStringId);
    else
        this->scenarioProperties.scenarioDescriptionStringId = u16(scenarioDescriptionStringId);
}

void LiteScenario::setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStringOverrides.forceName[force] = u32(forceNameStringId);
    else
        this->forces.forceString[force] = u16(forceNameStringId);
}

void LiteScenario::setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    if ( storageScope == Chk::Scope::Game )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: unitSettings.nameStringId[unitType] = u16(unitNameStringId); origUnitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::Yes: unitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::No: origUnitSettings.nameStringId[unitType] = u16(unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
                if ( this->hasSection(Chk::SectionName::UNIx) )
                    unitSettings.nameStringId[unitType] = u16(unitNameStringId);
                else
                    origUnitSettings.nameStringId[unitType] = u16(unitNameStringId);
                break;
            case Chk::UseExpSection::NoIfOrigAvailable:
                if ( this->hasSection(Chk::SectionName::UNIS) )
                    origUnitSettings.nameStringId[unitType] = u16(unitNameStringId);
                else
                    unitSettings.nameStringId[unitType] = u16(unitNameStringId);
                break;
        }
    }
    else
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: editorStringOverrides.unitName[unitType] = u32(unitNameStringId); editorStringOverrides.expUnitName[unitType] = u32(unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
            case Chk::UseExpSection::Yes: editorStringOverrides.expUnitName[unitType] = u32(unitNameStringId); break;
            case Chk::UseExpSection::NoIfOrigAvailable:
            case Chk::UseExpSection::No: editorStringOverrides.unitName[unitType] = u32(unitNameStringId); break;
        }
    }
}

void LiteScenario::setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStringOverrides.soundPath[soundIndex] = u32(soundPathStringId);
    else
        this->soundPaths[soundIndex] = u32(soundPathStringId);
}

void LiteScenario::setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope)
{
    if ( switchIndex < Chk::TotalSwitches )
    {
        if ( storageScope == Chk::Scope::Game )
            this->switchNames[switchIndex] = u32(switchNameStringId);
        else
            this->editorStringOverrides.switchName[switchIndex] = u32(switchNameStringId);
    }
    else
        throw std::out_of_range(std::string("switchIndex: ") + std::to_string((u32)switchIndex) + " is out of range for the SWNM section!");
}

void LiteScenario::setLocationNameStringId(size_t locationId, size_t locationNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStringOverrides.locationName[locationId] = u32(locationNameStringId);
    else if ( locationId < numLocations() )
        locations[locationId].stringId = u16(locationNameStringId);
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::optional<StringType> LiteScenario::getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: return getString<StringType>(gameStringId, Chk::Scope::Game);
        case Chk::Scope::Editor: return getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::GameOverEditor: return gameStringId != 0 ? getString<StringType>(gameStringId, Chk::Scope::Game) : getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return editorStringId != 0 ? getString<StringType>(editorStringId, Chk::Scope::Editor) : getString<StringType>(gameStringId, Chk::Scope::Game);
    }
    return std::nullopt;
}
template std::optional<RawString> LiteScenario::getString<RawString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getString<EscString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getString<ChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getString<SingleLineChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getScenarioName(Chk::Scope storageScope) const
{
    return getString<StringType>(this->scenarioProperties.scenarioNameStringId, this->editorStringOverrides.scenarioName, storageScope);
}
template std::optional<RawString> LiteScenario::getScenarioName<RawString>(Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getScenarioName<EscString>(Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getScenarioName<ChkdString>(Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getScenarioName<SingleLineChkdString>(Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getScenarioDescription(Chk::Scope storageScope) const
{
    return getString<StringType>(this->scenarioProperties.scenarioDescriptionStringId, this->editorStringOverrides.scenarioDescription, storageScope);
}
template std::optional<RawString> LiteScenario::getScenarioDescription<RawString>(Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getScenarioDescription<EscString>(Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getScenarioDescription<ChkdString>(Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getScenarioDescription<SingleLineChkdString>(Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getForceName(Chk::Force force, Chk::Scope storageScope) const
{
    return getString<StringType>(this->forces.forceString[force], this->editorStringOverrides.forceName[force], storageScope);
}
template std::optional<RawString> LiteScenario::getForceName<RawString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getForceName<EscString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getForceName<ChkdString>(Chk::Force force, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getForceName<SingleLineChkdString>(Chk::Force force, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getUnitName(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const
{
    auto mapUnitName = unitType < Sc::Unit::TotalTypes ? getString<StringType>(
        this->useExpansionUnitSettings(useExp) ? this->unitSettings.nameStringId[unitType] : this->origUnitSettings.nameStringId[unitType],
        this->useExpansionUnitSettings(useExp) ? this->editorStringOverrides.expUnitName[unitType] : this->editorStringOverrides.unitName[unitType],
        storageScope) : std::nullopt;

    if ( mapUnitName )
        return mapUnitName;
    else if ( scData != nullptr && unitType < scData->units.displayNames.size() )
        return std::optional<StringType>(scData->units.displayNames[unitType])
    else if ( unitType < Sc::Unit::TotalTypes )
        return std::optional<StringType>(Sc::Unit::defaultDisplayNames[unitType]);
    else
        return std::optional<StringType>("ID:" + std::to_string(unitType));
}
template std::optional<RawString> LiteScenario::getUnitName<RawString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getUnitName<EscString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getUnitName<ChkdString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const Sc::Data* scData, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getSoundPath(size_t soundIndex, Chk::Scope storageScope) const
{
    return getString<StringType>(this->soundPaths[soundIndex], this->editorStringOverrides.soundPath[soundIndex], storageScope);
}
template std::optional<RawString> LiteScenario::getSoundPath<RawString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getSoundPath<EscString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getSoundPath<ChkdString>(size_t soundIndex, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getSoundPath<SingleLineChkdString>(size_t soundIndex, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getSwitchName(size_t switchIndex, Chk::Scope storageScope) const
{
    return getString<StringType>(this->switchNames[switchIndex], this->editorStringOverrides.switchName[switchIndex], storageScope);
}
template std::optional<RawString> LiteScenario::getSwitchName<RawString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getSwitchName<EscString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getSwitchName<ChkdString>(size_t switchIndex, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getSwitchName<SingleLineChkdString>(size_t switchIndex, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getLocationName(size_t locationId, Chk::Scope storageScope) const
{
    return getString<StringType>((locationId > 0 && locationId <= numLocations() ? getLocation(locationId).stringId : 0), this->editorStringOverrides.locationName[locationId], storageScope);
}
template std::optional<RawString> LiteScenario::getLocationName<RawString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<EscString> LiteScenario::getLocationName<EscString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<ChkdString> LiteScenario::getLocationName<ChkdString>(size_t locationId, Chk::Scope storageScope) const;
template std::optional<SingleLineChkdString> LiteScenario::getLocationName<SingleLineChkdString>(size_t locationId, Chk::Scope storageScope) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getComment(size_t triggerIndex) const
{
    return getString<StringType>(getCommentStringId(triggerIndex), Chk::Scope::Game);
}
template std::optional<RawString> LiteScenario::getComment<RawString>(size_t triggerIndex) const;
template std::optional<EscString> LiteScenario::getComment<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> LiteScenario::getComment<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> LiteScenario::getComment<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getExtendedComment(size_t triggerIndex) const
{
    return getString<StringType>(getExtendedCommentStringId(triggerIndex), Chk::Scope::Editor);
}
template std::optional<RawString> LiteScenario::getExtendedComment<RawString>(size_t triggerIndex) const;
template std::optional<EscString> LiteScenario::getExtendedComment<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> LiteScenario::getExtendedComment<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> LiteScenario::getExtendedComment<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
std::optional<StringType> LiteScenario::getExtendedNotes(size_t triggerIndex) const
{
    return getString<StringType>(getExtendedNotesStringId(triggerIndex), Chk::Scope::Editor);
}
template std::optional<RawString> LiteScenario::getExtendedNotes<RawString>(size_t triggerIndex) const;
template std::optional<EscString> LiteScenario::getExtendedNotes<EscString>(size_t triggerIndex) const;
template std::optional<ChkdString> LiteScenario::getExtendedNotes<ChkdString>(size_t triggerIndex) const;
template std::optional<SingleLineChkdString> LiteScenario::getExtendedNotes<SingleLineChkdString>(size_t triggerIndex) const;

template <typename StringType>
void LiteScenario::setScenarioName(const StringType & scenarioNameString, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioNameString, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                this->scenarioProperties.scenarioNameStringId = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                this->editorStringOverrides.scenarioName = u32(newStringId);
        }
    }
}
template void LiteScenario::setScenarioName<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioName<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioName<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioName<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setScenarioDescription(const StringType & scenarioDescription, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioDescription, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                this->scenarioProperties.scenarioDescriptionStringId = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                this->editorStringOverrides.scenarioDescription = u32(newStringId);
        }
    }
}
template void LiteScenario::setScenarioDescription<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioDescription<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioDescription<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setScenarioDescription<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setForceName(Chk::Force force, const StringType & forceName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (u32)force < Chk::TotalForces )
    {
        size_t newStringId = addString<StringType>(forceName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                this->forces.forceString[force] = u16(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                this->editorStringOverrides.forceName[force] = u32(newStringId);
        }
    }
}
template void LiteScenario::setForceName<RawString>(Chk::Force force, const RawString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setForceName<EscString>(Chk::Force force, const EscString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setForceName<ChkdString>(Chk::Force force, const ChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setForceName<SingleLineChkdString>(Chk::Force force, const SingleLineChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setUnitName(Sc::Unit::Type unitType, const StringType & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && unitType < Sc::Unit::TotalTypes )
    {
        size_t newStringId = addString<StringType>(unitName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
            {
                if ( this->useExpansionUnitSettings(useExp) )
                    this->unitSettings.nameStringId[unitType] = u16(newStringId);
                else
                    this->origUnitSettings.nameStringId[unitType] = u16(newStringId);
            }
            else if ( storageScope == Chk::Scope::Editor )
            {
                switch ( useExp )
                {
                    case Chk::UseExpSection::Auto:
                        if ( this->isHybridOrAbove() )
                            this->editorStringOverrides.expUnitName[unitType] = u32(newStringId);
                        else
                            this->editorStringOverrides.unitName[unitType] = u32(newStringId);
                        break;
                    case Chk::UseExpSection::Yes: this->editorStringOverrides.expUnitName[unitType] = u32(newStringId); break;
                    case Chk::UseExpSection::No: this->editorStringOverrides.unitName[unitType] = u32(newStringId); break;
                    default:
                        this->editorStringOverrides.unitName[unitType] = u32(newStringId);
                        this->editorStringOverrides.expUnitName[unitType] = u32(newStringId);
                        break;
                }
            }
        }
    }
}
template void LiteScenario::setUnitName<RawString>(Sc::Unit::Type unitType, const RawString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setUnitName<EscString>(Sc::Unit::Type unitType, const EscString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setUnitName<ChkdString>(Sc::Unit::Type unitType, const ChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const SingleLineChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setSoundPath(size_t soundIndex, const StringType & soundPath, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && soundIndex < Chk::TotalSounds )
    {
        size_t newStringId = addString<StringType>(soundPath, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                setSoundStringId(soundIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                this->editorStringOverrides.soundPath[soundIndex] = u32(newStringId);
        }
    }
}
template void LiteScenario::setSoundPath<RawString>(size_t soundIndex, const RawString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSoundPath<EscString>(size_t soundIndex, const EscString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSoundPath<ChkdString>(size_t soundIndex, const ChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSoundPath<SingleLineChkdString>(size_t soundIndex, const SingleLineChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setSwitchName(size_t switchIndex, const StringType & switchName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && switchIndex < Chk::TotalSwitches )
    {
        size_t newStringId = addString<StringType>(switchName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                createAction()->switchNames[switchIndex] = u32(newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                createAction()->editorStringOverrides.switchName[switchIndex] = u32(newStringId);
        }
    }
}
template void LiteScenario::setSwitchName<RawString>(size_t switchIndex, const RawString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSwitchName<EscString>(size_t switchIndex, const EscString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSwitchName<ChkdString>(size_t switchIndex, const ChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setSwitchName<SingleLineChkdString>(size_t switchIndex, const SingleLineChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setLocationName(size_t locationId, const StringType & locationName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && locationId > 0 && locationId <= numLocations() )
    {
        size_t newStringId = addString<StringType>(locationName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                createAction()->locations[locationId].stringId = (u16)newStringId;
            else if ( storageScope == Chk::Scope::Editor )
                createAction()->editorStringOverrides.locationName[locationId] = u32(newStringId);
        }
    }
}
template void LiteScenario::setLocationName<RawString>(size_t locationId, const RawString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setLocationName<EscString>(size_t locationId, const EscString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setLocationName<ChkdString>(size_t locationId, const ChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void LiteScenario::setLocationName<SingleLineChkdString>(size_t locationId, const SingleLineChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void LiteScenario::setExtendedComment(size_t triggerIndex, const StringType & comment, bool autoDefragment)
{
    Chk::ExtendedTrigData & extension = getTriggerExtension(triggerIndex, true);
    size_t newStringId = addString<StringType>(comment, Chk::Scope::Editor, autoDefragment);
    if ( newStringId != (size_t)Chk::StringId::NoString )
        extension.commentStringId = (u32)newStringId;
}
template void LiteScenario::setExtendedComment<RawString>(size_t triggerIndex, const RawString & comment, bool autoDefragment);
template void LiteScenario::setExtendedComment<EscString>(size_t triggerIndex, const EscString & comment, bool autoDefragment);
template void LiteScenario::setExtendedComment<ChkdString>(size_t triggerIndex, const ChkdString & comment, bool autoDefragment);
template void LiteScenario::setExtendedComment<SingleLineChkdString>(size_t triggerIndex, const SingleLineChkdString & comment, bool autoDefragment);

template <typename StringType>
void LiteScenario::setExtendedNotes(size_t triggerIndex, const StringType & notes, bool autoDefragment)
{
    Chk::ExtendedTrigData & extension = getTriggerExtension(triggerIndex, true);
    size_t newStringId = addString<StringType>(notes, Chk::Scope::Editor, autoDefragment);
    if ( newStringId != (size_t)Chk::StringId::NoString )
        extension.notesStringId = (u32)newStringId;
}
template void LiteScenario::setExtendedNotes<RawString>(size_t triggerIndex, const RawString & notes, bool autoDefragment);
template void LiteScenario::setExtendedNotes<EscString>(size_t triggerIndex, const EscString & notes, bool autoDefragment);
template void LiteScenario::setExtendedNotes<ChkdString>(size_t triggerIndex, const ChkdString & notes, bool autoDefragment);
template void LiteScenario::setExtendedNotes<SingleLineChkdString>(size_t triggerIndex, const SingleLineChkdString & notes, bool autoDefragment);

void LiteScenario::syncStringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the basic, staredit standard, STR section format, and not allowing section sizes over 65536

        u16 numStrings;
        u16 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */
    
    constexpr size_t maxStrings = (size_t(u16_max) - sizeof(u16))/sizeof(u16);
    size_t numStrings = strings.size() > 0 ? strings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::STR), numStrings, maxStrings);
        
    size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u16) + sizeof(u16)*numStrings + 1;
    size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] )
            sectionSize += strings[i]->length();
    }

    constexpr size_t maxStandardSize = u16_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::STR), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);

    stringBytes.assign(sizeof(u16)+sizeof(u16)*numStrings, u8(0));
    (u16 &)stringBytes[0] = (u16)numStrings;
    u16 initialNulOffset = u16(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( !strings[i] )
            (u16 &)stringBytes[sizeof(u16)*i] = initialNulOffset;
        else
        {
            (u16 &)stringBytes[sizeof(u16)*i] = u16(stringBytes.size());
            stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+(strings[i]->length()+1));
        }
    }
}

void LiteScenario::syncRemasteredStringsToBytes(std::vector<u8> & stringBytes)
{
    /**
        Uses the standard SC:R STRx section format

        u32 numStrings;
        u32 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */
    constexpr size_t maxStrings = (size_t(u32_max) - sizeof(u32))/sizeof(u32);
    size_t numStrings = strings.size() > 0 ? strings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::STRx), numStrings, maxStrings);
        
    size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u32) + sizeof(u32)*numStrings + 1;
    size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] )
            sectionSize += strings[i]->length();
    }
    constexpr size_t maxStandardSize = u32_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::STRx), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);
    stringBytes.assign(sizeof(u32)+sizeof(u32)*numStrings, u8(0));
    (u32 &)stringBytes[0] = (u32)numStrings;
    u32 initialNulOffset = u32(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] )
        {
            (u32 &)stringBytes[sizeof(u32)*i] = u32(stringBytes.size());
            stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+(strings[i]->length()+1));
        }
        else
            (u32 &)stringBytes[sizeof(u32)*i] = initialNulOffset;
    }
}

void LiteScenario::syncKstringsToBytes(std::vector<u8> & stringBytes, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the standard KSTR format
        
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        StringProperties[numStrings] stringProperties; // String properties
        void[] stringData; // List of strings, each null terminated
    */

    constexpr size_t maxStrings = (size_t(s32_max) - 2*sizeof(u32))/sizeof(u32);
    size_t numStrings = editorStrings.size() > 0 ? editorStrings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw Chk::MaximumStringsExceeded(Chk::getNameString(SectionName::KSTR), numStrings, maxStrings);

    size_t stringPropertiesStart = 2*sizeof(u32)+2*numStrings;
    size_t versionAndSizeAndOffsetAndStringPropertiesAndNulSpace = 2*sizeof(u32) + 2*sizeof(u32)*numStrings + 1;
    size_t sectionSize = versionAndSizeAndOffsetAndStringPropertiesAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( editorStrings[i] )
            sectionSize += editorStrings[i]->length();
    }

    constexpr size_t maxStandardSize = s32_max;
    if ( sectionSize > maxStandardSize )
        throw Chk::MaximumCharactersExceeded(Chk::getNameString(SectionName::KSTR), sectionSize-versionAndSizeAndOffsetAndStringPropertiesAndNulSpace, maxStandardSize);

    stringBytes.assign(2*sizeof(u32)+2*sizeof(u32)*numStrings, u8(0));
    (u32 &)stringBytes[0] = Chk::KSTR::CurrentVersion;
    (u32 &)stringBytes[sizeof(u32)] = (u32)numStrings;
    u32 initialNulOffset = u32(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( !editorStrings[i] )
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = initialNulOffset;
        else
        {
            auto prop = editorStrings[i]->properties();
            (u32 &)stringBytes[stringPropertiesStart+sizeof(u32)*i] = (u32 &)Chk::StringProperties(prop.red, prop.green, prop.blue, prop.isUsed, prop.hasPriority, prop.isBold, prop.isUnderlined, prop.isItalics, prop.size);
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = u32(stringBytes.size());
            stringBytes.insert(stringBytes.end(), editorStrings[i]->str, editorStrings[i]->str+editorStrings[i]->length()+1);
        }
    }
}

void LiteScenario::syncBytesToStrings(const std::vector<u8> & stringBytes)
{
    size_t numBytes = stringBytes.size();
    u16 rawNumStrings = numBytes >= 2 ? (u16 &)stringBytes[0] : numBytes == 1 ? (u16)stringBytes[0] : 0;
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 4 ? 0 : numBytes/2-1);
    strings.clear();
    strings.push_back(std::nullopt); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    size_t sectionLastCharacter = 0;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u16)*stringId;
        size_t stringOffset = size_t((u16 &)stringBytes[offsetPos]);
        size_t lastCharacter = loadString(stringBytes, stringOffset, numBytes);

        if ( lastCharacter > sectionLastCharacter )
            sectionLastCharacter = lastCharacter;
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 2 > 0 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16)stringBytes[numBytes-1]);
            loadString(stringBytes, stringOffset, numBytes);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
            strings.push_back(std::nullopt);
    }

    size_t offsetsEnd = sizeof(u16) + sizeof(u16)*rawNumStrings;
    size_t charactersEnd = sectionLastCharacter+1;
    size_t regularStrSectionEnd = std::max(offsetsEnd, charactersEnd);
    if ( regularStrSectionEnd < numBytes ) // Tail data exists starting at regularStrSectionEnd
    {
        auto tailStart = std::next(stringBytes.begin(), regularStrSectionEnd);
        auto tailEnd = stringBytes.end();
        strTailData.assign(tailStart, tailEnd);
        logger.info() << "Read " << strTailData.size() << " bytes of tailData after the STR section" << std::endl;
        if ( strTailData.size() > 8 )
        {
            logger.info() << "This is most likely a compiled EUD map, unlike the source map, compiled EUD maps cannot be edited further." << std::endl;
            logger.info() << "Additionally, everything you see in Chkdraft may be radically different from what you see in StarCraft." << std::endl;
        }
    }
    else // No tail data exists
        strTailData.clear();
}

void LiteScenario::syncRemasteredBytesToStrings(const std::vector<u8> & stringBytes)
{
    size_t numBytes = stringBytes.size();
    u32 rawNumStrings = numBytes >= 4 ? (u32 &)stringBytes[0] : numBytes == 1 ? u32((u8 &)stringBytes[0]) : 0;
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 4 ? 0 : numBytes/4-1);
    strings.clear();
    strings.push_back(std::nullopt); // Fill the non-existant 0th stringId
    size_t stringId = 1;
    size_t sectionLastCharacter = 0;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u32)*stringId;
        size_t stringOffset = size_t((u32 &)stringBytes[offsetPos]);
        size_t lastCharacter = loadString(stringBytes, stringOffset, numBytes);
        if ( lastCharacter > sectionLastCharacter )
            sectionLastCharacter = lastCharacter;
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 4 > 0 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u32)stringBytes[numBytes-1]);
            loadString(stringBytes, stringOffset, numBytes);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining strings are fully out of bounds
            strings.push_back(std::nullopt);
    }
    size_t offsetsEnd = sizeof(u32) + sizeof(u32)*rawNumStrings;
    size_t charactersEnd = sectionLastCharacter+1;
    size_t regularStrxSectionEnd = std::max(offsetsEnd, charactersEnd);
    if ( regularStrxSectionEnd < numBytes ) // Tail data exists starting at regularStrxSectionEnd
    {
        auto tailStart = std::next(stringBytes.begin(), regularStrxSectionEnd);
        auto tailEnd = stringBytes.end();
        strTailData.assign(tailStart, tailEnd);
        logger.info() << "Read " << strTailData.size() << " bytes of tailData after the STRx section" << std::endl;
        if ( strTailData.size() > 8 )
        {
            logger.info() << "This is most likely a compiled EUD map, unlike the source map, compiled EUD maps cannot be edited further." << std::endl;
            logger.info() << "Additionally, everything you see in Chkdraft may be radically different from what you see in StarCraft." << std::endl;
        }
    }
    else // No tail data exists
        strTailData.clear();
}

void LiteScenario::syncBytesToKstrings(const std::vector<u8> & stringBytes)
{
    size_t numBytes = stringBytes.size();
    u32 version = 0;
    if ( numBytes >= 4 )
        version = (u32 &)stringBytes[0];
    else
        version = 0;

    if ( version > Chk::KSTR::CurrentVersion )
        throw Chk::SectionValidationException(Chk::SectionName::KSTR, "Unrecognized KSTR Version: " + std::to_string(version));

    this->editorStringsVersion = Chk::KstrVersion(version);

    u32 rawNumStrings = 0;
    if ( numBytes >= 8 )
        rawNumStrings = (u32 &)stringBytes[4];
    else if ( numBytes == 7 )
    {
        u8 paddedTriplet[4] = { stringBytes[4], stringBytes[5], stringBytes[6], u8(0) };
        rawNumStrings = (u32 &)paddedTriplet[0];
    }
    else if ( numBytes == 6 )
        rawNumStrings = u32((u16 &)stringBytes[4]);
    else if ( numBytes == 5 )
        rawNumStrings = u32(stringBytes[4]);
    
    size_t highestStringWithValidOffset = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/4);
    size_t highestStringWithValidProperties = std::min(size_t(rawNumStrings), numBytes < 12 ? 0 : (numBytes-8)/8);
    size_t propertiesStartMinusFour = sizeof(u32)+sizeof(u32)*rawNumStrings;
    editorStrings.clear();
    editorStrings.push_back(std::nullopt); // Fill the non-existant 0th stringId

    size_t stringId = 1;
    for ( ; stringId <= highestStringWithValidOffset; ++stringId )
    {
        size_t offsetPos = sizeof(u32)+sizeof(u32)*stringId;
        size_t stringOffset = size_t((u32 &)stringBytes[offsetPos]);
        loadKstring(stringBytes, stringOffset, numBytes);
        if ( stringId <= highestStringWithValidProperties && editorStrings[stringId] )
        {
            size_t propertiesPos = propertiesStartMinusFour + sizeof(u32)*stringId;
            Chk::StringProperties properties = (Chk::StringProperties &)stringBytes[propertiesPos];
            editorStrings[stringId]->properties() = StrProp(properties);
        }
    }
    if ( highestStringWithValidOffset < size_t(rawNumStrings) ) // Some offsets aren't within bounds
    {
        if ( numBytes % 4 == 3 ) // Can read three bytes of an offset
        {
            stringId ++;
            u8 paddedTriplet[4] = { stringBytes[numBytes-3], stringBytes[numBytes-2], stringBytes[numBytes-1], u8(0) };
            size_t stringOffset = size_t((u32 &)paddedTriplet[0]);
            loadKstring(stringBytes, stringOffset, numBytes);
        }
        else if ( numBytes % 4 == 2 ) // Can read two bytes of an offset
        {
            stringId ++;
            size_t stringOffset = size_t((u16 &)stringBytes[numBytes]);
            loadKstring(stringBytes, stringOffset, numBytes);
        }
        else if ( numBytes % 4 == 1 ) // Can read one byte of an offset
        {
            stringId ++;
            size_t stringOffset = size_t(stringBytes[sizeof(u32)*highestStringWithValidOffset]);
            loadKstring(stringBytes, stringOffset, numBytes);
        }
        for ( ; stringId <= size_t(rawNumStrings); ++stringId ) // Any remaining editorStrings are fully out of bounds
            editorStrings.push_back(std::nullopt);
    }
}

size_t LiteScenario::loadString(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
            {
                strings.push_back(&stringBytes[stringOffset]);
                return nullIndex;
            }
            else // String ends where section ends
            {
                strings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
                return sectionSize-1;
            }
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
        {
            strings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
            return sectionSize-1;
        }
        else // Character data would be out of bounds
            strings.push_back(std::nullopt);
    }
    else // Offset is out of bounds
        strings.push_back(std::nullopt);

    return 0;
}

void LiteScenario::loadKstring(const std::vector<u8> & stringBytes, const size_t & stringOffset, const size_t & sectionSize)
{
    if ( stringOffset < sectionSize )
    {
        auto nextNull = std::find(stringBytes.begin()+stringOffset, stringBytes.end(), u8('\0'));
        if ( nextNull != stringBytes.end() )
        {
            auto nullIndex = std::distance(stringBytes.begin(), nextNull);
            if ( size_t(nullIndex) >= stringOffset ) // Regular string
                editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset]));
            else // String ends where section ends
                editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
            editorStrings.push_back(std::string((const char*)&stringBytes[stringOffset], sectionSize-stringOffset));
        else // Character data would be out of bounds
            editorStrings.push_back(std::nullopt);
    }
    else // Offset is out of bounds
        editorStrings.push_back(std::nullopt);
}

void LiteScenario::upgradeKstrToCurrent()
{
    if ( this->editorStringsVersion >= Chk::KstrVersion::Current )
        return;

    auto ver = this->editorStringsVersion;
    if ( 0 == ver || 2 == ver )
    {
        size_t strCapacity = getCapacity(Chk::Scope::Game);
        for ( size_t triggerIndex=0; triggerIndex<triggers.size(); triggerIndex++ )
        {
            auto & trigger = triggers[triggerIndex];
            size_t extendedCommentStringId = getExtendedCommentStringId(triggerIndex);
            size_t extendedNotesStringId = getExtendedNotesStringId(triggerIndex);
            for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & action = trigger.actions[actionIndex];
                if ( action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::actionUsesStringArg[action.actionType] &&
                        action.stringId > strCapacity &&
                        action.stringId != Chk::StringId::NoString &&
                        action.stringId < 65536 &&
                        65536-action.stringId < editorStrings.size() )
                    {
                        if ( action.actionType == Chk::Action::Type::Comment &&
                            (extendedCommentStringId == Chk::StringId::NoString ||
                                extendedNotesStringId == Chk::StringId::NoString) )
                        { // Move comment to extended comment or to notes
                            if ( extendedCommentStringId == Chk::StringId::NoString )
                            {
                                setExtendedCommentStringId(triggerIndex, 65536-action.stringId);
                                action.stringId = 0;
                            }
                            else if ( extendedNotesStringId == Chk::StringId::NoString )
                            {
                                setExtendedNotesStringId(triggerIndex, 65536-action.stringId);
                                action.stringId = 0;
                            }
                        }
                        else // Extended string is lost
                        {
                            auto actionString = getString<ChkdString>(65536-action.stringId, Chk::Scope::Editor);
                            logger.warn() << "Trigger #" << triggerIndex << " action #" << actionIndex << " lost extended string: \""
                                << (actionString ? *actionString : "") << "\"" << std::endl;
                            action.stringId = Chk::StringId::NoString;
                        }
                    }

                    if ( Chk::Action::actionUsesSoundArg[action.actionType] &&
                        action.soundStringId > strCapacity &&
                        action.soundStringId != Chk::StringId::NoString &&
                        action.soundStringId < 65536 &&
                        65536-action.soundStringId < editorStrings.size() )
                    {
                        action.soundStringId = 65536 - action.soundStringId;
                        auto soundString = getString<ChkdString>(65536 - action.soundStringId);
                        logger.warn() << "Trigger #" << triggerIndex << " action #" << actionIndex << " lost extended sound string: \""
                            << (soundString ? *soundString : "") << "\"" << std::endl;
                        action.soundStringId = Chk::StringId::NoString;
                    }
                }
            }
        }
        
        for ( size_t briefingTriggerIndex=0; briefingTriggerIndex<briefingTriggers.size(); briefingTriggerIndex++ )
        {
            auto & briefingTrigger = briefingTriggers[briefingTriggerIndex];
            for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & briefingAction = briefingTrigger.actions[actionIndex];
                if ( briefingAction.actionType < Chk::Action::NumBriefingActionTypes )
                {
                    if ( Chk::Action::briefingActionUsesStringArg[briefingAction.actionType] &&
                        briefingAction.stringId > strCapacity &&
                        briefingAction.stringId != Chk::StringId::NoString &&
                        briefingAction.stringId < 65536 &&
                        65536-briefingAction.stringId < editorStrings.size() )
                    {
                        auto briefingString = getString<ChkdString>(65536 - briefingAction.stringId);
                        logger.warn() << "Briefing trigger #" << briefingTriggerIndex << " action #" << actionIndex << " lost extended string: \""
                            << (briefingString ? *briefingString : "") << "\"" << std::endl;
                        briefingAction.stringId = Chk::StringId::NoString;
                    }

                    if ( Chk::Action::briefingActionUsesSoundArg[briefingAction.actionType] &&
                        briefingAction.soundStringId > strCapacity &&
                        briefingAction.soundStringId != Chk::StringId::NoString &&
                        briefingAction.soundStringId < 65536 &&
                        65536-briefingAction.stringId < editorStrings.size() )
                    {
                        auto briefingSoundString = getString<ChkdString>(65536 - briefingAction.soundStringId);
                        logger.warn() << "Briefing trigger #" << briefingTriggerIndex << " action #" << actionIndex << " lost extended sound string: \""
                            << (briefingSoundString ? *briefingSoundString : "") << "\"" << std::endl;
                        briefingAction.soundStringId = Chk::StringId::NoString;
                    }
                }
            }
        }

        for ( size_t locationIndex=0; locationIndex<locations.size(); locationIndex++ )
        {
            auto & location = locations[locationIndex];
            if ( location.stringId > strCapacity &&
                 location.stringId != Chk::StringId::NoString &&
                 location.stringId < 65536 &&
                 size_t(65536-location.stringId) < editorStrings.size() )
            {
                editorStringOverrides.locationName[locationIndex] = 65536-location.stringId;
                location.stringId = Chk::StringId::NoString;
            }
        }

        if ( scenarioProperties.scenarioNameStringId > strCapacity &&
            scenarioProperties.scenarioNameStringId != Chk::StringId::NoString &&
            scenarioProperties.scenarioNameStringId < 65536 &&
            size_t(65536-scenarioProperties.scenarioNameStringId) < editorStrings.size() )
        {
            setScenarioNameStringId(65536-scenarioProperties.scenarioNameStringId, Chk::Scope::Editor);
            scenarioProperties.scenarioNameStringId = Chk::StringId::NoString;
        }

        if ( scenarioProperties.scenarioDescriptionStringId > strCapacity &&
            scenarioProperties.scenarioDescriptionStringId != Chk::StringId::NoString &&
            scenarioProperties.scenarioDescriptionStringId < 65536 &&
            size_t(65536-scenarioProperties.scenarioDescriptionStringId) < editorStrings.size() )
        {
            setScenarioDescriptionStringId(65536-scenarioProperties.scenarioDescriptionStringId, Chk::Scope::Editor);
            scenarioProperties.scenarioDescriptionStringId = Chk::StringId::NoString;
        }

        for ( Chk::Force i=Chk::Force::Force1; i<=Chk::Force::Force4; ((u8 &)i)++ )
        {
            if ( forces.forceString[i] > strCapacity &&
                forces.forceString[i] != Chk::StringId::NoString &&
                forces.forceString[i] < 65536 &&
                size_t(65536-forces.forceString[i]) < editorStrings.size() )
            {
                setForceNameStringId(i, 65536-forces.forceString[i], Chk::Scope::Editor);
                forces.forceString[i] = Chk::StringId::NoString;
            }
        }
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( soundPaths[i] > strCapacity &&
                soundPaths[i] != Chk::StringId::NoString &&
                soundPaths[i] < 65536 &&
                65536-soundPaths[i] < editorStrings.size() )
            {
                setSoundPathStringId(i, 65536-soundPaths[i], Chk::Scope::Editor);
                soundPaths[i] = Chk::StringId::NoString;
            }
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( switchNames[i] > strCapacity &&
                switchNames[i] != Chk::StringId::NoString &&
                switchNames[i] < 65536 &&
                65536-switchNames[i] < editorStrings.size() )
            {
                setSwitchNameStringId(i, 65536-switchNames[i], Chk::Scope::Editor);
                switchNames[i] = Chk::StringId::NoString;
            }
        }
        for ( Sc::Unit::Type i=Sc::Unit::Type::TerranMarine; i<Sc::Unit::TotalTypes; ((u16 &)i)++ )
        {
            if ( origUnitSettings.nameStringId[i] > strCapacity &&
                origUnitSettings.nameStringId[i] != Chk::StringId::NoString &&
                origUnitSettings.nameStringId[i] < 65536 &&
                size_t(65536-origUnitSettings.nameStringId[i]) < editorStrings.size() )
            {
                setUnitNameStringId(i, 65536-origUnitSettings.nameStringId[i], Chk::UseExpSection::No, Chk::Scope::Editor);
                origUnitSettings.nameStringId[i] = Chk::StringId::NoString;
            }
        }
        for ( Sc::Unit::Type i=Sc::Unit::Type::TerranMarine; i<Sc::Unit::TotalTypes; ((u16 &)i)++ )
        {
            if ( unitSettings.nameStringId[i] > strCapacity &&
                unitSettings.nameStringId[i] != Chk::StringId::NoString &&
                unitSettings.nameStringId[i] < 65536 &&
                size_t(65536-unitSettings.nameStringId[i]) < editorStrings.size() )
            {
                setUnitNameStringId(i, 65536-unitSettings.nameStringId[i], Chk::UseExpSection::Yes, Chk::Scope::Editor);
                unitSettings.nameStringId[i] = Chk::StringId::NoString;
            }
        }
        this->editorStringsVersion = Chk::KstrVersion::Current;
    }
}

const std::vector<u32> liteCompressionFlagsProgression = {
    StrCompressFlag::None,
    StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::LastStringTrick,
    StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::ReverseStacking,
    StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick | StrCompressFlag::DuplicateStringRecycling,
    StrCompressFlag::SubStringRecycling,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking,
    StrCompressFlag::SubStringRecycling | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::ReverseStacking | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OffsetInterlacing,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::OffsetInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SpareShuffledInterlacing,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::SizeBytesRecycling | StrCompressFlag::LastStringTrick,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing,
    StrCompressFlag::SubShuffledInterlacing | StrCompressFlag::SpareShuffledInterlacing | StrCompressFlag::OrderShuffledInterlacing
        | StrCompressFlag::SizeBytesRecycling
};

std::vector<std::optional<ScStr>> LiteScenario::copyStrings(Chk::Scope storageScope) const
{
    std::vector<std::optional<ScStr>> copy;
    if ( storageScope == Chk::Scope::Editor )
    {
        for ( auto & str : editorStrings )
            copy.push_back(str);
    }
    else
    {
        for ( auto & str : strings )
            copy.push_back(str);
    }

    return copy;
}

void LiteScenario::swapStrings(std::vector<std::optional<ScStr>> & strings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        this->editorStrings.swap(strings);
    else
        this->strings.swap(strings);
}

bool LiteScenario::defragment(Chk::Scope storageScope, bool matchCapacityToUsage)
{
    if ( storageScope & Chk::Scope::Game )
    {
        size_t nextCandidateStringId = 0;
        size_t numStrings = strings.size();
        std::map<u32, u32> stringIdRemappings;
        for ( size_t i=0; i<numStrings; i++ )
        {
            if ( !strings[i] )
            {
                for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
                {
                    if ( strings[j] )
                    {
                        strings[i] = strings[j];
                        stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                        break;
                    }
                }
            }
        }

        if ( !stringIdRemappings.empty() )
        {
            remapStringIds(stringIdRemappings, Chk::Scope::Game);
            return true;
        }
    }
    else if ( storageScope & Chk::Scope::Editor )
    {
        size_t nextCandidateStringId = 0;
        size_t numStrings = this->editorStrings.size();
        std::map<u32, u32> stringIdRemappings;
        for ( size_t i=0; i<numStrings; i++ )
        {
            if ( !this->editorStrings[i] )
            {
                for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
                {
                    if ( this->editorStrings[j] )
                    {
                        this->editorStrings[i] = this->editorStrings[j];
                        stringIdRemappings.insert(std::pair<u32, u32>((u32)j, (u32)i));
                        break;
                    }
                }
            }
        }

        if ( !stringIdRemappings.empty() )
        {
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
            return true;
        }
    }
    return false;
}

void LiteScenario::remapStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        auto scenarioNameRemapping = stringIdRemappings.find(this->scenarioProperties.scenarioNameStringId);
        auto scenarioDescriptionRemapping = stringIdRemappings.find(this->scenarioProperties.scenarioDescriptionStringId);

        if ( scenarioNameRemapping != stringIdRemappings.end() )
            this->scenarioProperties.scenarioNameStringId = scenarioNameRemapping->second;

        if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
            this->scenarioProperties.scenarioDescriptionStringId = scenarioDescriptionRemapping->second;

        remapForceStringIds(stringIdRemappings);
        remapUnitStringIds(stringIdRemappings);
        remapLocationStringIds(stringIdRemappings);
        remapTriggerStringIds(stringIdRemappings, storageScope);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        auto scenarioNameRemapping = stringIdRemappings.find(this->editorStringOverrides.scenarioName);
        auto scenarioDescriptionRemapping= stringIdRemappings.find(this->editorStringOverrides.scenarioDescription);
        auto forceOneRemapping = stringIdRemappings.find(this->editorStringOverrides.forceName[0]);
        auto forceTwoRemapping = stringIdRemappings.find(this->editorStringOverrides.forceName[1]);
        auto forceThreeRemapping = stringIdRemappings.find(this->editorStringOverrides.forceName[2]);
        auto forceFourRemapping = stringIdRemappings.find(this->editorStringOverrides.forceName[3]);

        if ( scenarioNameRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.scenarioName = scenarioNameRemapping->second;

        if ( scenarioDescriptionRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.scenarioDescription = scenarioDescriptionRemapping->second;
    
        if ( forceOneRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.forceName[0] = forceOneRemapping->second;

        if ( forceTwoRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.forceName[1] = forceTwoRemapping->second;

        if ( forceThreeRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.forceName[2] = forceThreeRemapping->second;

        if ( forceFourRemapping != stringIdRemappings.end() )
            this->editorStringOverrides.forceName[3] = forceFourRemapping->second;
    
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            auto found = stringIdRemappings.find(this->editorStringOverrides.unitName[i]);
            if ( found != stringIdRemappings.end() )
                this->editorStringOverrides.unitName[i] = found->second;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            auto found = stringIdRemappings.find(this->editorStringOverrides.expUnitName[i]);
            if ( found != stringIdRemappings.end() )
                this->editorStringOverrides.expUnitName[i] = found->second;
        }
    
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            auto found = stringIdRemappings.find(this->editorStringOverrides.soundPath[i]);
            if ( found != stringIdRemappings.end() )
                this->editorStringOverrides.soundPath[i] = found->second;
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            auto found = stringIdRemappings.find(this->editorStringOverrides.switchName[i]);
            if ( found != stringIdRemappings.end() )
                this->editorStringOverrides.switchName[i] = found->second;
        }

        for ( size_t i=0; i<Chk::TotalLocations; i++ )
        {
            auto found = stringIdRemappings.find(this->editorStringOverrides.locationName[i]);
            if ( found != stringIdRemappings.end() )
                this->editorStringOverrides.locationName[i] = found->second;
        }
        remapTriggerStringIds(stringIdRemappings, storageScope);
    }
}

Sc::Player::SlotType LiteScenario::getSlotType(size_t slotIndex, Chk::Scope scope) const
{
    if ( slotIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR/IOWN sections!");

    switch ( scope )
    {
        case Chk::Scope::Game: return this->slotTypes[slotIndex];
        case Chk::Scope::Editor: return this->iownSlotTypes[slotIndex];
        case Chk::Scope::EditorOverGame: return this->hasSection(Chk::SectionName::IOWN) ? this->iownSlotTypes[slotIndex] : this->slotTypes[slotIndex];
        default: return this->slotTypes[slotIndex];
    }
    return Sc::Player::SlotType::Inactive;
}

void LiteScenario::setSlotType(size_t slotIndex, Sc::Player::SlotType slotType, Chk::Scope scope)
{
    if ( slotIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string(slotIndex) + " is out of range for the OWNR/IOWN sections!");

    switch ( scope )
    {
        case Chk::Scope::Game: this->slotTypes[slotIndex] = slotType; break;
        case Chk::Scope::Editor: this->iownSlotTypes[slotIndex] = slotType; break;
        default: this->slotTypes[slotIndex] = slotType; this->iownSlotTypes[slotIndex] = slotType; break;
    }
}

Chk::Race LiteScenario::getPlayerRace(size_t playerIndex) const
{
    if ( playerIndex < Sc::Player::Total )
        return this->playerRaces[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

void LiteScenario::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    if ( playerIndex < Sc::Player::Total )
        this->playerRaces[playerIndex] = race;
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the SIDE section!");
}

Chk::PlayerColor LiteScenario::getPlayerColor(size_t slotIndex) const
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return this->playerColors[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

void LiteScenario::setPlayerColor(size_t slotIndex, Chk::PlayerColor color)
{
    if ( slotIndex < Sc::Player::TotalSlots )
    {
        if ( isUsingRemasteredColors() )
        {
            this->customColors.playerSetting[slotIndex] = Chk::PlayerColorSetting::UseId;
            this->customColors.playerColor[slotIndex][0] = u8(0); // R
            this->customColors.playerColor[slotIndex][1] = u8(0); // G
            this->customColors.playerColor[slotIndex][2] = playerColors[slotIndex]; // B (or color index)
        }
        this->playerColors[slotIndex] = color;
    }
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the COLR section!");
}

Chk::Force LiteScenario::getPlayerForce(size_t slotIndex) const
{
    if ( slotIndex < Sc::Player::TotalSlots )
        return this->forces.playerForce[slotIndex];
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

size_t LiteScenario::getForceStringId(Chk::Force force) const
{
    if ( force < Chk::TotalForces )
        return this->forces.forceString[(size_t)force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

u8 LiteScenario::getForceFlags(Chk::Force force) const
{
    if ( force < Chk::TotalForces )
        return this->forces.flags[force];
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void LiteScenario::setPlayerForce(size_t slotIndex, Chk::Force force)
{
    if ( slotIndex < Sc::Player::TotalSlots )
        this->forces.playerForce[slotIndex] = force;
    else
        throw std::out_of_range(std::string("SlotIndex: ") + std::to_string((u32)slotIndex) + " is out of range for the FORC section!");
}

void LiteScenario::setForceStringId(Chk::Force force, u16 forceStringId)
{
    if ( force < Chk::TotalForces )
        this->forces.forceString[(size_t)force] = forceStringId;
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void LiteScenario::setForceFlags(Chk::Force force, u8 forceFlags)
{
    if ( force < Chk::TotalForces )
        this->forces.flags[force] = Chk::ForceFlags(forceFlags);
    else
        throw std::out_of_range(std::string("Force: ") + std::to_string(force) + " is out of range for the FORC section!");
}

void LiteScenario::appendForceStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
    {
        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( this->forces.forceString[i] == stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Force, i));
        }
    }
}

bool LiteScenario::forceStringUsed(size_t stringId, u32 userMask) const
{
    return (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force &&
        this->forces.forceString[0] == (u16)stringId || this->forces.forceString[1] == (u16)stringId ||
        this->forces.forceString[2] == (u16)stringId || this->forces.forceString[3] == (u16)stringId;
}

void LiteScenario::markUsedForceStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
    {
        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( this->forces.forceString[i] != Chk::StringId::NoString )
                stringIdUsed[this->forces.forceString[i]] = true;
        }
    }
}

void LiteScenario::remapForceStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    auto forceOneRemapping = stringIdRemappings.find(this->forces.forceString[0]);
    auto forceTwoRemapping = stringIdRemappings.find(this->forces.forceString[1]);
    auto forceThreeRemapping = stringIdRemappings.find(this->forces.forceString[2]);
    auto forceFourRemapping = stringIdRemappings.find(this->forces.forceString[3]);

    if ( forceOneRemapping != stringIdRemappings.end() )
        this->forces.forceString[0] = forceOneRemapping->second;

    if ( forceTwoRemapping != stringIdRemappings.end() )
        this->forces.forceString[1] = forceTwoRemapping->second;

    if ( forceThreeRemapping != stringIdRemappings.end() )
        this->forces.forceString[2] = forceThreeRemapping->second;

    if ( forceFourRemapping != stringIdRemappings.end() )
        this->forces.forceString[3] = forceFourRemapping->second;
}

void LiteScenario::deleteForceString(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalForces; i++ )
    {
        if ( this->forces.forceString[i] == stringId )
            this->forces.forceString[i] = 0;
    }
}

bool LiteScenario::isUsingRemasteredColors()
{
    return hasSection(SectionName::CRGB);
}

void LiteScenario::upgradeToRemasteredColors()
{
    if ( !hasSection(SectionName::CRGB) )
    {
        addSaveSection(Section{SectionName::CRGB});
        for ( size_t i=0; i<Sc::Player::TotalSlots; ++i )
        {
            this->customColors.playerColor[i][0] = u8(0); // R
            this->customColors.playerColor[i][1] = u8(0); // G
            this->customColors.playerColor[i][2] = playerColors[i]; // B (or color index)
            this->customColors.playerSetting[i] = Chk::PlayerColorSetting::UseId;
        }
    }
}

Chk::PlayerColorSetting LiteScenario::getPlayerColorSetting(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::TotalSlots )
        return customColors.playerSetting[playerIndex];
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

void LiteScenario::setPlayerColorSetting(size_t playerIndex, Chk::PlayerColorSetting setting)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        upgradeToRemasteredColors();
        customColors.playerSetting[playerIndex] = setting;
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

Chk::Rgb LiteScenario::getPlayerCustomColor(size_t playerIndex)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        return Chk::Rgb { customColors.playerColor[playerIndex][0], customColors.playerColor[playerIndex][1], customColors.playerColor[playerIndex][2] };
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

void LiteScenario::setPlayerCustomColor(size_t playerIndex, Chk::Rgb rgb)
{
    if ( playerIndex < Sc::Player::TotalSlots )
    {
        upgradeToRemasteredColors();
        customColors.playerColor[playerIndex][0] = rgb.red;
        customColors.playerColor[playerIndex][1] = rgb.green;
        customColors.playerColor[playerIndex][2] = rgb.blue;
    }
    else
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the CRGB section!");
}

Sc::Terrain::Tileset LiteScenario::getTileset() const
{
    return this->tileset;
}

void LiteScenario::setTileset(Sc::Terrain::Tileset tileset)
{
    this->tileset = tileset;
}

size_t LiteScenario::getPixelWidth() const
{
    return this->dimensions.tileWidth * Sc::Terrain::PixelsPerTile;
}

size_t LiteScenario::getPixelHeight() const
{
    return this->dimensions.tileHeight * Sc::Terrain::PixelsPerTile;
}

void liteSetIsomDimensions(std::vector<Chk::IsomRect> & tiles, u16 newTileWidth, u16 newTileHeight, u16 /*oldTileWidth*/, u16 /*oldTileHeight*/, s32 /*leftEdge*/, s32 /*topEdge*/)
{
    size_t oldNumIndices = tiles.size();
    size_t newNumIndices = (size_t(newTileWidth) / size_t(2) + size_t(1)) * (size_t(newTileHeight) + size_t(1));
    if ( oldNumIndices < newNumIndices )
    {
        for ( size_t i=oldNumIndices; i<newNumIndices; i++ )
            tiles.push_back(Chk::IsomRect());
    }
    else if ( oldNumIndices > newNumIndices )
    {
        auto eraseStart = tiles.begin();
        std::advance(eraseStart, newNumIndices);
        tiles.erase(eraseStart, tiles.end());
    }
}

template <typename T>
void liteSetTiledSectionDimensions(std::vector<T> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    s64 oldLeft = 0, oldTop = 0, oldRight = oldTileWidth, oldBottom = oldTileHeight,
        newLeft = leftEdge, newTop = topEdge, newRight = (s64)leftEdge+(s64)newTileWidth, newBottom = (s64)topEdge+(s64)newTileHeight,
        currTileWidth = oldTileWidth, currTileHeight = oldTileHeight;

    // Remove tiles as neccessary in order: bottom, top, right, left
    if ( newBottom < oldBottom && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the bottom
    {
        s64 numRowsRemoved = oldBottom - newBottom;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)tiles.size() )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            tiles.erase(tiles.begin()+(tiles.size()-size_t(numTilesRemoved)), tiles.end());
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newTop > oldTop && currTileWidth > 0 && currTileHeight > 0 ) // Rows need to be removed from the top
    {
        s64 numRowsRemoved = newTop - oldTop;
        s64 numTilesRemoved = numRowsRemoved*currTileWidth;
        if ( numTilesRemoved >= (s64)tiles.size() )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            tiles.erase(tiles.begin(), tiles.begin()+size_t(numTilesRemoved));
            currTileHeight -= numRowsRemoved;
        }
    }

    if ( newRight < oldRight && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the right
    {
        s64 numColumnsRemoved = oldRight - newRight;
        if ( numColumnsRemoved >= currTileWidth )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            size_t firstRemovedColumn = size_t(currTileWidth - numColumnsRemoved);
            for ( size_t row = size_t(currTileHeight-1); row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = size_t(row*currTileWidth);
                size_t removedColumnsOffset = rowOffset+firstRemovedColumn;
                auto start = tiles.begin()+removedColumnsOffset;
                tiles.erase(start, start+size_t(numColumnsRemoved));
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( newLeft > oldLeft && currTileWidth > 0 && currTileHeight > 0 ) // Columns need to be removed from the left
    {
        s64 numColumnsRemoved = newLeft - oldLeft;
        if ( numColumnsRemoved >= currTileHeight )
        {
            tiles.clear();
            currTileWidth = 0;
            currTileHeight = 0;
        }
        else
        {
            for ( size_t row = size_t(currTileHeight-1); row < (size_t)currTileHeight; row-- )
            {
                size_t rowOffset = size_t(row*currTileWidth);
                auto start = tiles.begin()+rowOffset;
                tiles.erase(start, start+size_t(numColumnsRemoved));
            }
            currTileWidth -= numColumnsRemoved;
        }
    }

    if ( tiles.size() > (size_t)newTileWidth * (size_t)newTileHeight ) // Remove out of bounds tiles
        tiles.erase(tiles.begin() + (size_t)newTileWidth*(size_t)newTileHeight);

    if ( currTileWidth == 0 || currTileHeight == 0 )
        tiles.clear();
    else // currTileWidth > 0 && currTileHeight > 0
    {
        // Add tiles as necessary in order: right, left, top, bottom

        if ( newRight > oldRight ) // Columns need to be added to the right
        {
            s64 numColumnsAdded = newRight - oldRight;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*currTileWidth;
                s64 start = rowOffset + currTileWidth;
                tiles.insert(tiles.begin() + size_t(start), size_t(numColumnsAdded), u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newLeft < oldLeft ) // Columns need to be added to the left
        {
            s64 numColumnsAdded = oldLeft - newLeft;
            for ( s64 row = currTileHeight-1; row >= 0; row-- )
            {
                s64 rowOffset = row*numColumnsAdded;
                tiles.insert(tiles.begin() + size_t(rowOffset), size_t(numColumnsAdded), u16(0));
            }
            currTileWidth += numColumnsAdded;
        }

        if ( newTop < oldTop ) // Rows need to be added to the top
        {
            s64 numRowsAdded = oldTop - newTop;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            tiles.insert(tiles.begin(), size_t(numTilesAdded), u16(0));
            currTileHeight += numRowsAdded;
        }

        if ( newBottom > oldBottom ) // Rows need to be added to the bottom
        {
            s64 numRowsAdded = newBottom - oldBottom;
            s64 numTilesAdded = currTileWidth*numRowsAdded;
            tiles.insert(tiles.end(), size_t(numTilesAdded), u16(0));
        }

        if ( tiles.size() < (size_t)newTileWidth * (size_t)newTileHeight ) // Fill any missing tiles with nulls
            tiles.insert(tiles.end(), (size_t)newTileWidth*(size_t)newTileHeight - tiles.size(), u16(0));
    }
}

void liteSetTiledDimensions(std::vector<u8> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    liteSetTiledSectionDimensions(tiles, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

void liteSetTiledDimensions(std::vector<u16> & tiles, u16 newTileWidth, u16 newTileHeight, u16 oldTileWidth, u16 oldTileHeight, s32 leftEdge, s32 topEdge)
{
    liteSetTiledSectionDimensions(tiles, newTileWidth, newTileHeight, oldTileWidth, oldTileHeight, leftEdge, topEdge);
}

void LiteScenario::setTileWidth(u16 newTileWidth, u16 sizeValidationFlags, s32 leftEdge)
{
    u16 tileWidth = this->dimensions.tileWidth;
    u16 tileHeight = this->dimensions.tileHeight;
    ::liteSetIsomDimensions(this->isomRects, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    ::liteSetTiledDimensions(this->tiles, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    ::liteSetTiledDimensions(this->editorTiles, newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    this->dimensions.tileWidth = newTileWidth;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

void LiteScenario::setTileHeight(u16 newTileHeight, u16 sizeValidationFlags, s32 topEdge)
{
    u16 tileWidth = this->dimensions.tileWidth;
    u16 tileHeight = this->dimensions.tileHeight;
    ::liteSetIsomDimensions(this->isomRects, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    ::liteSetTiledDimensions(this->tiles, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    ::liteSetTiledDimensions(this->editorTiles, tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    this->dimensions.tileHeight = newTileHeight;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

void LiteScenario::setDimensions(u16 newTileWidth, u16 newTileHeight, u16 sizeValidationFlags, s32 leftEdge, s32 topEdge)
{
    u16 tileWidth = this->dimensions.tileWidth;
    u16 tileHeight = this->dimensions.tileHeight;
    ::liteSetIsomDimensions(this->isomRects, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    ::liteSetTiledDimensions(this->tiles, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    ::liteSetTiledDimensions(this->editorTiles, newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    this->dimensions.tileWidth = newTileWidth;
    this->dimensions.tileHeight = newTileHeight;
    validateSizes(sizeValidationFlags, tileWidth, tileHeight);
}

u16 LiteScenario::getTile(size_t tileXc, size_t tileYc, Chk::Scope scope) const
{
    size_t tileWidth = this->dimensions.tileWidth;
    size_t tileIndex = tileYc*tileWidth + tileXc;
    if ( scope == Chk::Scope::EditorOverGame )
    {
        if ( tileIndex < this->editorTiles.size() )
            return this->editorTiles[tileIndex];
        else if ( tileIndex < this->tiles.size() )
            return this->tiles[tileIndex];
    }
    else if ( scope == Chk::Scope::Editor )
    {
        if ( tileIndex < this->editorTiles.size() )
            return this->editorTiles[tileIndex];
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
    }
    else if ( scope == Chk::Scope::Game )
    {
        if ( tileIndex < this->tiles.size() )
            return this->tiles[tileIndex];
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
    }
    return 0;
}

u16 LiteScenario::getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope) const
{
    return getTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, scope);
}

void LiteScenario::setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope)
{
    size_t tileWidth = this->dimensions.tileWidth;
    size_t tileIndex = tileYc*tileWidth + tileXc;
    if ( scope & Chk::Scope::Game )
    {
        if ( tileIndex < this->tiles.size() )
            this->tiles[tileIndex] = tileValue;
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MTXM section!");
    }
    if ( scope & Chk::Scope::Editor )
    {
        if ( tileIndex < this->editorTiles.size() )
            this->editorTiles[tileIndex] = tileValue;
        else
            throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the TILE section!");
    }
}

void LiteScenario::setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope)
{
    setTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, tileValue, scope);
}

Chk::IsomRect & LiteScenario::getIsomRect(size_t isomRectIndex)
{
    if ( isomRectIndex < this->isomRects.size() )
        return this->isomRects[isomRectIndex];
    else
        throw std::out_of_range(std::string("IsomRectIndex: ") + std::to_string(isomRectIndex) + " is past the end of the ISOM section!");
}

const Chk::IsomRect & LiteScenario::getIsomRect(size_t isomRectIndex) const
{
    if ( isomRectIndex < this->isomRects.size() )
        return this->isomRects[isomRectIndex];
    else
        throw std::out_of_range(std::string("IsomRectIndex: ") + std::to_string(isomRectIndex) + " is past the end of the ISOM section!");
}
    
bool LiteScenario::placeIsomTerrain(Chk::IsomDiamond isomDiamond, size_t terrainType, size_t brushExtent, Chk::IsomCache & cache)
{
    uint16_t isomValue = cache.getTerrainTypeIsomValue(terrainType);
    if ( isomValue == 0 || !isomDiamond.isValid() || size_t(isomValue) >= cache.isomLinks.size() || cache.isomLinks[size_t(isomValue)].terrainType == 0 )
        return false;

    int brushMin = int(brushExtent) / -2;
    int brushMax = brushMin + int(brushExtent);
    if ( brushExtent%2 == 0 ) {
        ++brushMin;
        ++brushMax;
    }

    cache.resetChangedArea();

    std::deque<Chk::IsomDiamond> diamondsToUpdate {};
    for ( int brushOffsetX=brushMin; brushOffsetX<brushMax; ++brushOffsetX )
    {
        for ( int brushOffsetY=brushMin; brushOffsetY<brushMax; ++brushOffsetY )
        {
            size_t brushX = isomDiamond.x + brushOffsetX - brushOffsetY;
            size_t brushY = isomDiamond.y + brushOffsetX + brushOffsetY;
            if ( isInBounds({brushX, brushY}) )
            {
                setDiamondIsomValues({brushX, brushY}, isomValue, true, cache);
                if ( brushOffsetX == brushMin || brushOffsetX == brushMax-1 || brushOffsetY == brushMin || brushOffsetY == brushMax-1 )
                { // Mark diamonds on the edge of the brush for radial updates
                    for ( auto i : Chk::IsomDiamond::neighbors )
                    {
                        Chk::IsomDiamond neighbor = Chk::IsomDiamond{brushX, brushY}.getNeighbor(i);
                        if ( diamondNeedsUpdate(neighbor) )
                            diamondsToUpdate.push_back(Chk::IsomDiamond{neighbor.x, neighbor.y});
                    }
                }
            }
        }
    }
    radiallyUpdateTerrain(true, diamondsToUpdate, cache);
    return true;
}

void LiteScenario::copyIsomFrom(const LiteScenario & sourceMap, int32_t xTileOffset, int32_t yTileOffset, bool undoable, Chk::IsomCache & destCache)
{
    size_t sourceIsomWidth = sourceMap.getTileWidth()/2 + 1;
    size_t sourceIsomHeight = sourceMap.getTileHeight() + 1;

    if ( undoable )
    {
        for ( size_t y=0; y<destCache.isomHeight; ++y )
        {
            for ( size_t x=0; x<destCache.isomWidth; ++x )
                addIsomUndo({x, y}, destCache);
        }
    }

    int32_t diamondX = xTileOffset / 2;
    int32_t diamondY = yTileOffset;

    Sc::BoundingBox sourceRc { sourceIsomWidth, sourceIsomHeight, destCache.isomWidth, destCache.isomHeight, diamondX, diamondY };
    
    size_t copyWidth = sourceRc.right-sourceRc.left;
    size_t rowLimit = sourceRc.bottom;
    if ( diamondX > 0 )
        copyWidth -= diamondX;
    if ( diamondY > 0 )
        rowLimit -= diamondY;

    for ( size_t y=sourceRc.top; y<rowLimit; ++y )
    {
        const Chk::IsomRect* sourceRow = &sourceMap.isomRects[y*sourceIsomWidth + sourceRc.left];
        Chk::IsomRect* destRow = &isomRects[(y+diamondY)*destCache.isomWidth + sourceRc.left + diamondX];
        std::memcpy(destRow, sourceRow, sizeof(Chk::IsomRect)*copyWidth);
    }

    if ( undoable )
    {
        // Clear out-of-bounds isom values
        for ( size_t y=sourceIsomHeight; y<destCache.isomHeight; ++y )
        {
            for ( size_t x=0; x<destCache.isomWidth; ++x )
                isomRectAt({x, y}).clear();
        }

        if ( sourceIsomWidth < destCache.isomWidth )
        {
            for ( size_t y=0; y<destCache.isomHeight; ++y )
            {
                for ( size_t x=sourceIsomWidth; x<destCache.isomWidth; ++x )
                    isomRectAt({x, y}).clear();
            }
        }

        for ( size_t y=0; y<destCache.isomHeight; ++y )
        {
            for ( size_t x=0; x<destCache.isomWidth; ++x )
                destCache.undoMap[y*destCache.isomWidth + x]->setNewValue(getIsomRect({x, y})); // Update undo info for this position
        }
    }
}

void LiteScenario::updateTilesFromIsom(Chk::IsomCache & cache)
{
    for ( size_t y=cache.changedArea.top; y<=cache.changedArea.bottom; ++y )
    {
        for ( size_t x=cache.changedArea.left; x<=cache.changedArea.right; ++x )
        {
            Chk::IsomRect & isomRect = isomRectAt({x, y});
            if ( isomRect.isLeftOrRightModified() )
                updateTileFromIsom({x, y}, cache);

            isomRect.clearEditorFlags();
        }
    }
    cache.resetChangedArea();
}

bool LiteScenario::resizeIsom(int32_t xTileOffset, int32_t yTileOffset, size_t oldMapWidth, size_t oldMapHeight, bool fixBorders, Chk::IsomCache & cache)
{
    int32_t xDiamondOffset = xTileOffset/2;
    int32_t yDiamondOffset = yTileOffset;
    size_t oldIsomWidth = oldMapWidth/2 + 1;
    size_t oldIsomHeight = oldMapHeight + 1;
    Sc::BoundingBox sourceRc { oldIsomWidth, oldIsomHeight, cache.isomWidth, cache.isomHeight, xDiamondOffset, yDiamondOffset };
    Sc::BoundingBox innerArea {
        sourceRc.left+xDiamondOffset,
        sourceRc.top+yDiamondOffset,
        std::min(cache.isomWidth, sourceRc.right+xDiamondOffset-1),
        std::min(cache.isomHeight, sourceRc.bottom+yDiamondOffset-1)
    };
     
    std::vector<Chk::IsomDiamond> edges {};
    for ( size_t y=innerArea.top; y<=innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left+(innerArea.left+y)%2; x<innerArea.right+1; x+= 2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyInside = true;
            bool fullyOutside = true;
            uint16_t isomValue = 0;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) )
                {
                    if ( rectCoords.x >= innerArea.left && rectCoords.x < innerArea.right &&
                        rectCoords.y >= innerArea.top && rectCoords.y < innerArea.bottom )
                    {
                        isomValue = getIsomRect(rectCoords).getIsomValue(Sc::Isom::ProjectedQuadrant{i}.firstSide) >> 4;
                        fullyOutside = false;
                    }
                    else
                        fullyInside = false;
                }
            }

            if ( fullyOutside ) // Do not update diamonds completely outside the inner area
                continue;

            if ( !fullyInside ) // Update diamonds that are partially inside and mark them for radial updates
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( (rectCoords.x < innerArea.left || rectCoords.x >= innerArea.right || // Quadrant is outside inner area
                        rectCoords.y < innerArea.top || rectCoords.y >= innerArea.bottom) )
                    {
                        setIsomValue(rectCoords, Sc::Isom::quadrants[size_t(i)], isomValue, false, cache);
                    }
                }

                if ( fixBorders )
                {
                    for ( auto i : Chk::IsomDiamond::neighbors )
                    {
                        Chk::IsomDiamond neighbor = Chk::IsomDiamond{x, y}.getNeighbor(i);
                        if ( isInBounds(neighbor) && (
                            neighbor.x < innerArea.left || neighbor.x > innerArea.right || // Neighbor is outside inner area
                            neighbor.y < innerArea.top || neighbor.y > innerArea.bottom) )
                        {
                            edges.push_back(neighbor);
                        }
                    }
                }
            }

            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) )
                    isomRectAt(rectCoords).setModified(i);
            }
        }
    }

    // Order edges by distance from top-left over difference between x&y over x-coordinates
    std::sort(edges.begin(), edges.end(), [](const Chk::IsomDiamond & l, const Chk::IsomDiamond & r) {
        auto lDistance = l.x + l.y;
        auto rDistance = r.x + r.y;
        if ( lDistance != rDistance )
            return lDistance < rDistance; // Order by distance from top-left corner

        lDistance = std::max(l.x, l.y) - std::min(l.x, l.y);
        rDistance = std::max(r.x, r.y) - std::min(r.x, r.y);
        if ( lDistance != rDistance )
            return lDistance < rDistance; // Order by difference between x & y
        else
            return l.x < r.x; // Order by x difference
    });

    // Update all the edges
    std::deque<Chk::IsomDiamond> diamondsToUpdate;
    for ( const auto & edge : edges )
    {
        if ( diamondNeedsUpdate({edge.x, edge.y}) )
            diamondsToUpdate.push_back({edge.x, edge.y});
    }
    radiallyUpdateTerrain(false, diamondsToUpdate, cache);

    // Clear the changed and visited flags
    for ( size_t y=cache.changedArea.top; y<=cache.changedArea.bottom; ++y )
    {
        for ( size_t x=cache.changedArea.left; x<=cache.changedArea.right; ++x )
            isomRectAt({x, y}).clearEditorFlags();
    }

    for ( size_t y=innerArea.top; y<=innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left+(innerArea.left+y)%2; x<=innerArea.right; x+=2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyOutside = true;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) &&
                    rectCoords.x >= innerArea.left && rectCoords.x < innerArea.right && // Inside inner area
                    rectCoords.y >= innerArea.top && rectCoords.y < innerArea.bottom )
                {
                    fullyOutside = false;
                    break;
                }
            }

            if ( !fullyOutside ) // Only update diamonds that are at least partially inside
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( isInBounds(rectCoords) )
                        isomRectAt(rectCoords).setModified(i);
                }
            }
        }
    }
    diamondsToUpdate.clear();

    cache.setAllChanged();

    // Clear off the changed flags for the inner area
    for ( size_t y=innerArea.top; y<innerArea.bottom; ++y )
    {
        for ( size_t x=innerArea.left; x<innerArea.right; ++x )
            isomRectAt({x, y}).clearEditorFlags();
    }

    for ( size_t y=0; y<cache.isomHeight; ++y )
    {
        for ( size_t x=y%2; x<cache.isomWidth; x+=2 )
        {
            if ( (x+y)%2 != 0 )
                continue;

            bool fullyInside = true;
            for ( auto i : Sc::Isom::quadrants )
            {
                Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                if ( isInBounds(rectCoords) &&
                    (rectCoords.x < innerArea.left || rectCoords.x >= innerArea.right || // Quadrant is outside the inner area
                        rectCoords.y < innerArea.top || rectCoords.y < innerArea.bottom) )
                {
                    fullyInside = false;
                    break;
                }
            }

            if ( !fullyInside ) // Mark diamonds partially or fully outside the inner area as modified
            {
                for ( auto i : Sc::Isom::quadrants )
                {
                    Chk::IsomRect::Point rectCoords = Chk::IsomDiamond{x, y}.getRectangleCoords(i);
                    if ( isInBounds(rectCoords) )
                        isomRectAt(rectCoords).setModified(i);
                }
            }
        }
    }

    return true;
}

Chk::TileOccupationCache LiteScenario::getTileOccupationCache(const Sc::Terrain::Tiles & tileset, const Sc::Unit & unitData) const
{
    std::vector<bool> tileOccupied(size_t(dimensions.tileWidth)*size_t(dimensions.tileHeight), false);
    for ( const auto & doodad : doodads )
    {
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(doodad.type) )
        {
            const auto & doodadDat = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[*doodadGroupIndex];
            const auto & placability = tileset.doodadPlacibility[doodad.type];
            
            bool evenWidth = doodadDat.tileWidth % 2 == 0;
            bool evenHeight = doodadDat.tileHeight % 2 == 0;
            size_t halfWidth = 16*size_t(doodadDat.tileWidth);
            size_t halfHeight = 16*size_t(doodadDat.tileHeight);
            size_t left = size_t(doodad.xc) < halfWidth ? 0 : (size_t(doodad.xc) - halfWidth)/32;
            size_t top = size_t(doodad.yc) < halfHeight ? 0 : (size_t(doodad.yc) - halfHeight)/32;
            for ( size_t y=0; y<size_t(doodadDat.tileHeight); ++y )
            {
                for ( size_t x=0; x<size_t(doodadDat.tileWidth); ++x )
                {
                    if ( placability.tileGroup[y*doodadDat.tileWidth+x] != 0 )
                        tileOccupied[(top+y)*dimensions.tileWidth+(left+x)] = true;
                }
            }
        }
    }
    for ( const auto & unit : units )
    {
        if ( unit.type < unitData.numUnitTypes() )
        {
            const auto & unitDat = unitData.getUnit(unit.type);
            s32 xTileMin = (unit.xc - unitDat.unitSizeLeft)/32;
            s32 xTileMax = (unit.xc + unitDat.unitSizeRight)/32+1;
            s32 yTileMin = (unit.yc - unitDat.unitSizeUp)/32;
            s32 yTileMax = (unit.yc + unitDat.unitSizeDown)/32+1;
            for ( s32 y = std::max(0, yTileMin); y<std::min(s32(dimensions.tileHeight), yTileMax); ++y )
            {
                for ( s32 x = std::max(0, xTileMin); x<std::min(s32(dimensions.tileWidth), xTileMax); ++x )
                    tileOccupied[y*dimensions.tileWidth+x] = true;
            }
        }
    }
    return Chk::TileOccupationCache{tileOccupied};
}

void LiteScenario::validateSizes(u16 sizeValidationFlags, u16 prevWidth, u16 prevHeight)
{
    bool updateAnywhereIfAlreadyStandard = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhereIfAlreadyStandard) == SizeValidationFlag::UpdateAnywhereIfAlreadyStandard;
    bool updateAnywhere = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhere) == SizeValidationFlag::UpdateAnywhere;
    if ( (!updateAnywhereIfAlreadyStandard && updateAnywhere) || (updateAnywhereIfAlreadyStandard && anywhereIsStandardDimensions(prevWidth, prevHeight)) )
        matchAnywhereToDimensions();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsLocations) == SizeValidationFlag::UpdateOutOfBoundsLocations )
        downsizeOutOfBoundsLocations();

    if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsDoodads) == SizeValidationFlag::RemoveOutOfBoundsDoodads )
        removeOutOfBoundsDoodads();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsUnits) == SizeValidationFlag::UpdateOutOfBoundsUnits )
        updateOutOfBoundsUnits();
    else if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsUnits) == SizeValidationFlag::RemoveOutOfBoundsUnits )
        removeOutOfBoundsUnits();

    if ( (sizeValidationFlags & SizeValidationFlag::UpdateOutOfBoundsSprites) == SizeValidationFlag::UpdateOutOfBoundsSprites )
        updateOutOfBoundsSprites();
    else if ( (sizeValidationFlags & SizeValidationFlag::RemoveOutOfBoundsSprites) == SizeValidationFlag::RemoveOutOfBoundsSprites )
        removeOutOfBoundsSprites();
}

void LiteScenario::fixTerrainToDimensions()
{
    auto tileWidth = this->dimensions.tileWidth;
    auto tileHeight = this->dimensions.tileHeight;
    auto expectedIsomSize = this->getIsomWidth()*this->getIsomHeight();
    if ( this->isomRects.size() < expectedIsomSize )
        isomRects.insert(this->isomRects.end(), expectedIsomSize-this->isomRects.size(), Chk::IsomRect{});
    if ( this->editorTiles.size() != size_t(tileWidth)*size_t(tileHeight) )
        liteSetTiledDimensions(this->editorTiles, tileWidth, tileHeight, tileWidth, tileHeight, 0, 0);
    if ( this->tiles.size() != size_t(tileWidth)*size_t(tileHeight) )
        liteSetTiledDimensions(this->tiles, tileWidth, tileHeight, tileWidth, tileHeight, 0, 0);
}

u8 LiteScenario::getFog(size_t tileXc, size_t tileYc) const
{
    size_t tileWidth = this->dimensions.tileWidth;
    size_t tileIndex = tileWidth*tileYc + tileXc;
    if ( tileIndex < this->tileFog.size() )
        return this->tileFog[tileIndex];
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

u8 LiteScenario::getFogPx(size_t pixelXc, size_t pixelYc) const
{
    return getFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile);
}

void LiteScenario::setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers)
{
    size_t tileWidth = this->dimensions.tileWidth;
    size_t tileIndex = tileWidth*tileYc + tileXc;
    
    if ( tileIndex < this->tileFog.size() )
        this->tileFog[tileIndex] = fogOfWarPlayers;
    else
        throw std::out_of_range(std::string("TileIndex: ") + std::to_string(tileIndex) + " is past the end of the MASK section!");
}

void LiteScenario::setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers)
{
    setFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, fogOfWarPlayers);
}

size_t LiteScenario::numSprites() const
{
    return this->sprites.size();
}

Chk::Sprite & LiteScenario::getSprite(size_t spriteIndex)
{
    return this->sprites[spriteIndex];
}

const Chk::Sprite & LiteScenario::getSprite(size_t spriteIndex) const
{
    return this->sprites[spriteIndex];
}

size_t LiteScenario::addSprite(const Chk::Sprite & sprite)
{
    this->sprites.push_back(sprite);
    return sprites.size()-1;
}

void LiteScenario::insertSprite(size_t spriteIndex, const Chk::Sprite & sprite)
{
    if ( spriteIndex < sprites.size() )
    {
        auto position = std::next(sprites.begin(), spriteIndex);
        sprites.insert(position, sprite);
    }
    else
        sprites.push_back(sprite);
}

void LiteScenario::deleteSprite(size_t spriteIndex)
{
    if ( spriteIndex < sprites.size() )
    {
        auto sprite = std::next(sprites.begin(), spriteIndex);
        sprites.erase(sprite);
    }
}

void LiteScenario::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    size_t spriteIndexMin = std::min(spriteIndexFrom, spriteIndexTo);
    size_t spriteIndexMax = std::max(spriteIndexFrom, spriteIndexTo);
    if ( spriteIndexMax < sprites.size() && spriteIndexFrom != spriteIndexTo )
    {
        if ( spriteIndexMax-spriteIndexMin == 1 && spriteIndexMax < sprites.size() ) // Move up or down by 1 using swap
            std::swap(sprites[spriteIndexMin], sprites[spriteIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto sprite = sprites[spriteIndexFrom];
            auto toErase = std::next(sprites.begin(), spriteIndexFrom);
            sprites.erase(toErase);
            auto insertPosition = spriteIndexTo == 0 ? sprites.begin() : std::next(sprites.begin(), spriteIndexTo-1);
            sprites.insert(insertPosition, sprite);
        }
    }
}

void LiteScenario::updateOutOfBoundsSprites()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numSprites = this->numSprites();
    for ( size_t i=0; i<numSprites; i++ )
    {
        Chk::Sprite & sprite = this->getSprite(i);

        if ( sprite.xc >= pixelWidth )
            sprite.xc = u16(pixelWidth-1);

        if ( sprite.yc >= pixelHeight )
            sprite.yc = u16(pixelHeight-1);
    }
}

void LiteScenario::removeOutOfBoundsSprites()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numSprites = this->numSprites();
    for ( size_t i = numSprites-1; i < numSprites; i-- )
    {
        const Chk::Sprite & sprite = this->getSprite(i);
        if ( sprite.xc >= pixelWidth || sprite.yc >= pixelHeight )
            this->deleteSprite(i);
    }
}

size_t LiteScenario::numDoodads() const
{
    return this->doodads.size();
}

Chk::Doodad & LiteScenario::getDoodad(size_t doodadIndex)
{
    return doodads[doodadIndex];
}

const Chk::Doodad & LiteScenario::getDoodad(size_t doodadIndex) const
{
    return doodads[doodadIndex];
}

size_t LiteScenario::addDoodad(const Chk::Doodad & doodad)
{
    doodads.push_back(doodad);
    return doodads.size()-1;
}

void LiteScenario::insertDoodad(size_t doodadIndex, const Chk::Doodad & doodad)
{
    if ( doodadIndex < doodads.size() )
    {
        auto position = std::next(doodads.begin(), doodadIndex);
        doodads.insert(position, doodad);
    }
    else if ( doodadIndex == doodads.size() )
        doodads.push_back(doodad);
}

void LiteScenario::deleteDoodad(size_t doodadIndex)
{
    if ( doodadIndex < doodads.size() )
    {
        auto doodad = std::next(doodads.begin(), doodadIndex);
        doodads.erase(doodad);
    }
}

void LiteScenario::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    size_t doodadIndexMin = std::min(doodadIndexFrom, doodadIndexTo);
    size_t doodadIndexMax = std::max(doodadIndexFrom, doodadIndexTo);
    if ( doodadIndexMax < doodads.size() && doodadIndexFrom != doodadIndexTo )
    {
        if ( doodadIndexMax-doodadIndexMin == 1 && doodadIndexMax < doodads.size() ) // Move up or down by 1 using swap
            std::swap(doodads[doodadIndexMin], doodads[doodadIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto doodad = doodads[doodadIndexFrom];
            auto toErase = std::next(doodads.begin(), doodadIndexFrom);
            doodads.erase(toErase);
            auto insertPosition = doodadIndexTo == 0 ? doodads.begin() : std::next(doodads.begin(), doodadIndexTo-1);
            doodads.insert(insertPosition, doodad);
        }
    }
}

void LiteScenario::removeOutOfBoundsDoodads()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numDoodads = this->numDoodads();
    for ( size_t i = numDoodads-1; i < numDoodads; i-- )
    {
        const Chk::Doodad & doodad = this->getDoodad(i);
        if ( doodad.xc >= pixelWidth || doodad.yc >= pixelHeight )
            this->deleteDoodad(i);
    }
}

size_t LiteScenario::numUnits() const
{
    return units.size();
}

Chk::Unit & LiteScenario::getUnit(size_t unitIndex)
{
    return this->units[unitIndex];
}

const Chk::Unit & LiteScenario::getUnit(size_t unitIndex) const
{
    return this->units[unitIndex];
}

size_t LiteScenario::addUnit(const Chk::Unit & unit)
{
    units.push_back(unit);
    return units.size()-1;
}

void LiteScenario::insertUnit(size_t unitIndex, const Chk::Unit & unit)
{
    if ( unitIndex < units.size() )
    {
        auto position = std::next(units.begin(), unitIndex);
        units.insert(position, unit);
    }
    else if ( unitIndex == units.size() )
        units.push_back(unit);
}

void LiteScenario::deleteUnit(size_t unitIndex)
{
    if ( unitIndex < units.size() )
    {
        auto unit = std::next(units.begin(), unitIndex);
        units.erase(unit);
    }
}

void LiteScenario::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    size_t unitIndexMin = std::min(unitIndexFrom, unitIndexTo);
    size_t unitIndexMax = std::max(unitIndexFrom, unitIndexTo);
    if ( unitIndexMax < units.size() && unitIndexFrom != unitIndexTo )
    {
        if ( unitIndexMax-unitIndexMin == 1 && unitIndexMax < units.size() ) // Move up or down by 1 using swap
            std::swap(units[unitIndexMin], units[unitIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto unit = units[unitIndexFrom];
            auto toErase = std::next(units.begin(), unitIndexFrom);
            units.erase(toErase);
            auto insertPosition = unitIndexTo == 0 ? units.begin() : std::next(units.begin(), unitIndexTo-1);
            units.insert(insertPosition, unit);
        }
    }
}

void LiteScenario::updateOutOfBoundsUnits()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numUnits = this->numUnits();
    for ( size_t i=0; i<numUnits; i++ )
    {
        Chk::Unit & currUnit = this->getUnit(i);
        if ( currUnit.xc >= pixelWidth )
            currUnit.xc = u16(pixelWidth-1);

        if ( currUnit.yc >= pixelHeight )
            currUnit.yc = u16(pixelHeight-1);
    }
}

void LiteScenario::removeOutOfBoundsUnits()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numUnits = this->numUnits();
    for ( size_t i = numUnits-1; i < numUnits; i-- )
    {
        const Chk::Unit & currUnit = this->getUnit(i);
        if ( currUnit.xc >= pixelWidth || currUnit.yc >= pixelHeight )
            this->deleteUnit(i);
    }
}

size_t LiteScenario::numLocations() const
{
    return locations.size() > 0 ? locations.size()-1 : 0;
}

Chk::Location & LiteScenario::getLocation(size_t locationId)
{
    return locations[locationId];
}

const Chk::Location & LiteScenario::getLocation(size_t locationId) const
{
    return locations[locationId];
}

size_t LiteScenario::addLocation(const Chk::Location & location)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        if ( isBlank(i) )
        {
            locations[i] = location;
            return i;
        }
    }
    return Chk::LocationId::NoLocation;
}

void LiteScenario::replaceLocation(size_t locationId, const Chk::Location & location)
{
    if ( locationId > 0 && locationId < locations.size() )
    {
        if ( isBlank(locationId) )
            locations[locationId] = location;
    }
    else
        throw std::out_of_range(std::string("LocationId: ") + std::to_string((u32)locationId) + " is out of range for the MRGN section!");
}

void LiteScenario::deleteLocation(size_t locationId, bool deleteOnlyIfUnused)
{
    if ( !deleteOnlyIfUnused || !this->triggerLocationUsed(locationId) )
    {
        if ( locationId > 0 && locationId < locations.size() )
        {
            auto location = std::next(locations.begin(), locationId);
            location->stringId = 0;
            location->left = 0;
            location->right = 0;
            location->top = 0;
            location->bottom = 0;
            location->elevationFlags = 0;
        }
    }
}

bool LiteScenario::moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere)
{
    size_t locationIdMin = std::min(locationIdFrom, locationIdTo);
    size_t locationIdMax = std::max(locationIdFrom, locationIdTo);
    if ( locationIdFrom > 0 && locationIdTo > 0 && locationIdMax < locations.size() && locationIdFrom != locationIdTo &&
         (!lockAnywhere || (locationIdMin != Chk::LocationId::Anywhere && locationIdMax != Chk::LocationId::Anywhere)) )
    {
        if ( locationIdMax-locationIdMin == 1 && locationIdMax < locations.size() ) // Move up or down by 1 using swap
        {
            std::swap(locations[locationIdMin], locations[locationIdMax]);
            return true;
        }
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto location = locations[locationIdFrom];
            auto toErase = std::next(locations.begin(), locationIdFrom);
            locations.erase(toErase);
            auto insertPosition = locationIdTo == 0 ? locations.begin() : std::next(locations.begin(), locationIdTo-1);
            locations.insert(insertPosition, location);

            if ( lockAnywhere && locationIdMin < Chk::LocationId::Anywhere && locationIdMax > Chk::LocationId::Anywhere )
            {
                std::swap(*std::next(locations.begin(), Chk::LocationId::Anywhere-1), *std::next(locations.begin(), Chk::LocationId::Anywhere));
                return true;
            }
        }
    }
    return false;
}

bool LiteScenario::isBlank(size_t locationId) const
{
    return locationId >= locations.size() || locations[locationId].isBlank();
}

void LiteScenario::downsizeOutOfBoundsLocations()
{
    size_t pixelWidth = this->getPixelWidth();
    size_t pixelHeight = this->getPixelHeight();
    size_t numLocations = this->numLocations();
    for ( size_t i=1; i<=numLocations; i++ )
    {
        Chk::Location & location = this->getLocation(i);

        if ( location.left >= pixelWidth )
            location.left = u32(pixelWidth-1);

        if ( location.right >= pixelWidth )
            location.right = u32(pixelWidth-1);

        if ( location.top >= pixelHeight )
            location.top = u32(pixelHeight-1);

        if ( location.bottom >= pixelHeight )
            location.bottom = u32(pixelHeight-1);
    }
}

void LiteScenario::markNonZeroLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    size_t limit = std::min(Chk::TotalLocations, locations.size());
    for ( size_t i=1; i<limit; i++ )
    {
        auto & location = locations[i];
        if ( !location.isBlank() )
            locationIdUsed[i] = true;
    }
}

bool LiteScenario::locationsFitOriginal(bool lockAnywhere, bool autoDefragment)
{
    std::bitset<Chk::TotalLocations+1> locationIdUsed;
    this->markUsedTriggerLocations(locationIdUsed);
    markNonZeroLocations(locationIdUsed);

    size_t countUsedOrCreated = 0;
    for ( size_t i=1; i<=Chk::TotalLocations; i++ )
    {
        if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
            countUsedOrCreated++;
    }

    return countUsedOrCreated <= Chk::TotalOriginalLocations;
}

bool LiteScenario::trimLocationsToOriginal(bool lockAnywhere, bool autoDefragment)
{
    if ( locations.size() > Chk::TotalOriginalLocations )
    {
        std::bitset<Chk::TotalLocations+1> locationIdUsed;
        this->markUsedTriggerLocations(locationIdUsed);
        markNonZeroLocations(locationIdUsed);

        size_t countUsedOrCreated = 0;
        for ( size_t i=1; i<=Chk::TotalLocations; i++ )
        {
            if ( locationIdUsed[i] || (i == Chk::LocationId::Anywhere && lockAnywhere) )
                countUsedOrCreated++;
        }

        if ( countUsedOrCreated <= Chk::TotalOriginalLocations )
        {
            std::map<u32, u32> locationIdRemappings;
            for ( size_t firstUnused=1; firstUnused<=Chk::TotalLocations; firstUnused++ )
            {
                if ( !locationIdUsed[firstUnused] && (firstUnused != Chk::LocationId::Anywhere || !lockAnywhere) )
                {
                    for ( size_t i=firstUnused+1; i<=Chk::TotalLocations; i++ )
                    {
                        if ( locationIdUsed[i] && (i != Chk::LocationId::Anywhere || !lockAnywhere) )
                        {
                            locations[firstUnused] = locations[i];
                            locationIdUsed[firstUnused] = true;
                            locations[i] = Chk::Location{};
                            locationIdUsed[i] = false;
                            locationIdRemappings.insert(std::pair<u32, u32>(u32(i), u32(firstUnused)));
                            break;
                        }
                    }
                }
            }
            locations.erase(locations.begin()+Chk::TotalOriginalLocations+1, locations.end());
            this->remapTriggerLocationIds(locationIdRemappings);
            return true;
        }
    }
    return locations.size() <= Chk::TotalOriginalLocations;
}

void LiteScenario::expandToScHybridOrExpansion()
{
    size_t numLocations = locations.size();
    for ( size_t i=numLocations; i<=Chk::TotalLocations; i++ )
        locations.push_back(Chk::Location{});
}

bool LiteScenario::anywhereIsStandardDimensions(u16 prevWidth, u16 prevHeight) const
{
    const Chk::Location & anywhere = this->getLocation(Chk::LocationId::Anywhere);
    if ( prevWidth == 0 && prevHeight == 0 )
        return anywhere.left == 0 && anywhere.top == 0 && anywhere.right == this->getPixelWidth() && anywhere.bottom == this->getPixelHeight();
    else
        return anywhere.left == 0 && anywhere.top == 0 && anywhere.right == prevWidth*Sc::Terrain::PixelsPerTile && anywhere.bottom == prevHeight*Sc::Terrain::PixelsPerTile;
}

void LiteScenario::matchAnywhereToDimensions()
{
    Chk::Location & anywhere = this->getLocation(Chk::LocationId::Anywhere);
    anywhere.left = 0;
    anywhere.top = 0;
    anywhere.right = (u32)this->getPixelWidth();
    anywhere.bottom = (u32)this->getPixelHeight();
}

void LiteScenario::appendLocationStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        u16 u16StringId = (u16)stringId;
        for ( size_t i=0; i<locations.size(); i++ )
        {
            if ( stringId == locations[i].stringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Location, i));
        }
    }
}

bool LiteScenario::locationStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        auto usedByGame = [&](){
            for ( const auto & location : this->locations )
            {
                if ( stringId == location.stringId )
                    return true;
            }
            return false;
        };
        auto usedByEditor = [&](){
            for ( size_t i=0; i<Chk::TotalLocations; ++i )
            {
                if ( stringId == this->editorStringOverrides.locationName[i] )
                    return true;
            }
            return false;
        };
        switch ( storageScope )
        {
            case Chk::Scope::Either:
            case Chk::Scope::EditorOverGame:
            case Chk::Scope::GameOverEditor: return usedByGame() || usedByEditor();
            case Chk::Scope::Game: return usedByGame();
            case Chk::Scope::Editor: return usedByEditor();
            default: return false;
        }
    }
    return false;
}

void LiteScenario::markUsedLocationStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        for ( const auto & location : locations )
        {
            if ( location.stringId > 0 )
                stringIdUsed[location.stringId] = true;
        }
    }
}

void LiteScenario::remapLocationStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto & location = locations[i];
        auto found = stringIdRemappings.find(location.stringId);
        if ( found != stringIdRemappings.end() )
            location.stringId = found->second;
    }
}

void LiteScenario::deleteLocationString(size_t stringId)
{
    for ( size_t i=1; i<locations.size(); i++ )
    {
        auto & location = locations[i];
        if ( location.stringId == stringId )
            location.stringId = 0;
    }
}

bool LiteScenario::useExpansionUnitSettings(Chk::UseExpSection useExp, Sc::Weapon::Type weaponType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return weaponType >= Sc::Weapon::Type::NeutronFlare || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::UNIS);
        case Chk::UseExpSection::YesIfAvailable: 
        default: return this->hasSection(Chk::SectionName::UNIx);
    }
    return true;
}

bool LiteScenario::unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ?
        unitSettings.useDefault[unitType] != Chk::UseDefault::No : origUnitSettings.useDefault[unitType] != Chk::UseDefault::No;
}

u32 LiteScenario::getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.hitpoints[unitType] : origUnitSettings.hitpoints[unitType];
}

u16 LiteScenario::getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.shieldPoints[unitType] : origUnitSettings.shieldPoints[unitType];
}

u8 LiteScenario::getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.armorLevel[unitType] : origUnitSettings.armorLevel[unitType];
}

u16 LiteScenario::getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.buildTime[unitType] : origUnitSettings.buildTime[unitType];
}

u16 LiteScenario::getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.mineralCost[unitType] : origUnitSettings.mineralCost[unitType];
}

u16 LiteScenario::getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp) const
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    return useExpansionUnitSettings(useExp) ? unitSettings.gasCost[unitType] : origUnitSettings.gasCost[unitType];
}

u16 LiteScenario::getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp) const
{
    bool useExpansion = useExpansionUnitSettings(useExp, weaponType);
    size_t limit = useExpansion ? Sc::Weapon::Total : Sc::Weapon::TotalOriginal;
    if ( weaponType > limit )
    {
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
            " is out of range for the " + (useExpansion ? "UNIx" : "UNIS") + " section!");
    }
    return useExpansion ? unitSettings.baseDamage[weaponType] : origUnitSettings.baseDamage[weaponType];
}

u16 LiteScenario::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp) const
{
    bool useExpansion = useExpansionUnitSettings(useExp, weaponType);
    size_t limit = useExpansion ? Sc::Weapon::Total : Sc::Weapon::TotalOriginal;
    if ( weaponType > limit )
    {
        throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
            " is out of range for the " + (useExpansion ? "UNIx" : "UNIS") + " section!");
    }
    return useExpansion ? unitSettings.upgradeDamage[weaponType] : origUnitSettings.upgradeDamage[weaponType];
}

void LiteScenario::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.useDefault[unitType] = value; origUnitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::Yes: unitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::No: origUnitSettings.useDefault[unitType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.useDefault[unitType] = value;
            else
                origUnitSettings.useDefault[unitType] = value;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.useDefault[unitType] = value;
            else
                unitSettings.useDefault[unitType] = value;
            break;
    }
}

void LiteScenario::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.hitpoints[unitType] = hitpoints; origUnitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::Yes: unitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::No: origUnitSettings.hitpoints[unitType] = hitpoints; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.hitpoints[unitType] = hitpoints;
            else
                origUnitSettings.hitpoints[unitType] = hitpoints;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.hitpoints[unitType] = hitpoints;
            else
                unitSettings.hitpoints[unitType] = hitpoints;
            break;
    }
}

void LiteScenario::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.shieldPoints[unitType] = shieldPoints; origUnitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::Yes: unitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::No: origUnitSettings.shieldPoints[unitType] = shieldPoints; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.shieldPoints[unitType] = shieldPoints;
            else
                origUnitSettings.shieldPoints[unitType] = shieldPoints;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.shieldPoints[unitType] = shieldPoints;
            else
                unitSettings.shieldPoints[unitType] = shieldPoints;
            break;
    }
}

void LiteScenario::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.armorLevel[unitType] = armorLevel; origUnitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::Yes: unitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::No: origUnitSettings.armorLevel[unitType] = armorLevel; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.armorLevel[unitType] = armorLevel;
            else
                origUnitSettings.armorLevel[unitType] = armorLevel;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.armorLevel[unitType] = armorLevel;
            else
                unitSettings.armorLevel[unitType] = armorLevel;
            break;
    }
}

void LiteScenario::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.buildTime[unitType] = buildTime; origUnitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::Yes: unitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::No: origUnitSettings.buildTime[unitType] = buildTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.buildTime[unitType] = buildTime;
            else
                origUnitSettings.buildTime[unitType] = buildTime;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.buildTime[unitType] = buildTime;
            else
                unitSettings.buildTime[unitType] = buildTime;
            break;
    }
}

void LiteScenario::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.mineralCost[unitType] = mineralCost; origUnitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::Yes: unitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::No: origUnitSettings.mineralCost[unitType] = mineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.mineralCost[unitType] = mineralCost;
            else
                origUnitSettings.mineralCost[unitType] = mineralCost;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.mineralCost[unitType] = mineralCost;
            else
                unitSettings.mineralCost[unitType] = mineralCost;
            break;
    }
}

void LiteScenario::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp)
{
    if ( unitType >= Sc::Unit::TotalTypes )
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the UNIS/UNIx section!");

    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unitSettings.gasCost[unitType] = gasCost; origUnitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::Yes: unitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::No: origUnitSettings.gasCost[unitType] = gasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UNIx) )
                unitSettings.gasCost[unitType] = gasCost;
            else
                origUnitSettings.gasCost[unitType] = gasCost;
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UNIS) )
                origUnitSettings.gasCost[unitType] = gasCost;
            else
                unitSettings.gasCost[unitType] = gasCost;
            break;
    }
}

void LiteScenario::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp)
{
    auto checkLimit = [&weaponType](bool expansion){
        if ( (expansion && weaponType >= Sc::Weapon::Total) || (!expansion && weaponType >= Sc::Weapon::TotalOriginal) )
            throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
                " is out of range for the " + (expansion ? "UNIx" : "UNIS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType >= Sc::Weapon::TotalOriginal && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( weaponType < Sc::Weapon::TotalOriginal && hasSection(Chk::SectionName::UNIS) )
                this->origUnitSettings.baseDamage[weaponType] = baseDamage;
            checkLimit(true);
            this->unitSettings.baseDamage[weaponType] = baseDamage;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUnitSettings.baseDamage[weaponType] = baseDamage; this->unitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->unitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUnitSettings.baseDamage[weaponType] = baseDamage; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->unitSettings.baseDamage[weaponType] = baseDamage;
            } else {
                checkLimit(false);
                this->origUnitSettings.baseDamage[weaponType] = baseDamage;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUnitSettings.baseDamage[weaponType] = baseDamage;
            } else {
                checkLimit(true);
                this->unitSettings.baseDamage[weaponType] = baseDamage;
            }
            break;
    }
}

void LiteScenario::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp)
{
    auto checkLimit = [&weaponType](bool expansion){
        if ( (expansion && weaponType >= Sc::Weapon::Total) || (!expansion && weaponType >= Sc::Weapon::TotalOriginal) )
            throw std::out_of_range(std::string("WeaponType: ") + std::to_string((size_t)weaponType) +
                " is out of range for the " + (expansion ? "UNIx" : "UNIS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType >= Sc::Weapon::TotalOriginal && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( weaponType < Sc::Weapon::TotalOriginal && hasSection(Chk::SectionName::UNIS) )
                this->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            checkLimit(true);
            this->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage; this->unitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->unitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            } else {
                checkLimit(false);
                this->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUnitSettings.upgradeDamage[weaponType] = upgradeDamage;
            } else {
                checkLimit(true);
                this->unitSettings.upgradeDamage[weaponType] = upgradeDamage;
            }
            break;
    }
}

bool LiteScenario::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex) const
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return this->unitAvailability.playerUnitBuildable[playerIndex][unitType] != Chk::Available::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool LiteScenario::isUnitDefaultBuildable(Sc::Unit::Type unitType) const
{
    if ( unitType < Sc::Unit::TotalTypes )
        return this->unitAvailability.defaultUnitBuildable[unitType] != Chk::Available::No;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

bool LiteScenario::playerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex) const
{
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            return this->unitAvailability.playerUnitUsesDefault[playerIndex][unitType] != Chk::UseDefault::No;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void LiteScenario::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    Chk::Available unitBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            this->unitAvailability.playerUnitBuildable[playerIndex][unitType] = unitBuildable;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void LiteScenario::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    Chk::Available unitDefaultBuildable = buildable ? Chk::Available::Yes : Chk::Available::No;
    if ( unitType < Sc::Unit::TotalTypes )
        this->unitAvailability.defaultUnitBuildable[unitType] = unitDefaultBuildable;
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void LiteScenario::setPlayerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    Chk::UseDefault playerUnitUsesDefault = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    if ( unitType < Sc::Unit::TotalTypes )
    {
        if ( playerIndex < Sc::Player::Total )
            this->unitAvailability.playerUnitUsesDefault[playerIndex][unitType] = playerUnitUsesDefault;
        else
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the PUNI section!");
    }
    else
        throw std::out_of_range(std::string("UnitType: ") + std::to_string(unitType) + " is out of range for the PUNI section!");
}

void LiteScenario::setUnitsToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            this->origUnitSettings = Chk::UNIS {};
            this->unitSettings = Chk::UNIx {};
            this->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::Yes:
            this->unitSettings = Chk::UNIx {};
            this->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::No:
            this->origUnitSettings = Chk::UNIS {};
            this->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) )
                this->unitSettings = Chk::UNIx {};
            else
                this->origUnitSettings = Chk::UNIS {};
            
            this->unitAvailability = Chk::PUNI {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) )
                this->origUnitSettings = Chk::UNIS {};
            else
                this->unitSettings = Chk::UNIx {};
            
            this->unitAvailability = Chk::PUNI {};
            break;
    }
}

bool LiteScenario::useExpansionUpgradeCosts(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return this->hasSection(Chk::SectionName::UPGS) ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::UPGx);
    }
}

bool LiteScenario::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.useDefault[upgradeType] : this->origUpgradeCosts.useDefault[upgradeType];
}

u16 LiteScenario::getUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.baseMineralCost[upgradeType] : this->origUpgradeCosts.baseMineralCost[upgradeType];
}

u16 LiteScenario::getUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.mineralCostFactor[upgradeType] : this->origUpgradeCosts.mineralCostFactor[upgradeType];
}

u16 LiteScenario::getUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.baseGasCost[upgradeType] : this->origUpgradeCosts.baseGasCost[upgradeType];
}

u16 LiteScenario::getUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.gasCostFactor[upgradeType] : this->origUpgradeCosts.gasCostFactor[upgradeType];
}

u16 LiteScenario::getUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.baseResearchTime[upgradeType] : this->origUpgradeCosts.baseResearchTime[upgradeType];
}

u16 LiteScenario::getUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeCosts(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
            " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    }
    return expansion ? this->upgradeCosts.researchTimeFactor[upgradeType] : this->origUpgradeCosts.researchTimeFactor[upgradeType];
}

void LiteScenario::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.useDefault[upgradeType] = value;
            checkLimit(true);
            this->upgradeCosts.useDefault[upgradeType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.useDefault[upgradeType] = value; this->upgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.useDefault[upgradeType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.useDefault[upgradeType] = value;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.useDefault[upgradeType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.useDefault[upgradeType] = value;
            } else {
                checkLimit(true);
                this->upgradeCosts.useDefault[upgradeType] = value;
            }
            break;
    }
}

void LiteScenario::setUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            checkLimit(true);
            this->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; this->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            } else {
                checkLimit(true);
                this->upgradeCosts.baseMineralCost[upgradeType] = baseMineralCost;
            }
            break;
    }
}

void LiteScenario::setUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            checkLimit(true);
            this->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; this->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            } else {
                checkLimit(true);
                this->upgradeCosts.mineralCostFactor[upgradeType] = mineralCostFactor;
            }
            break;
    }
}

void LiteScenario::setUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            checkLimit(true);
            this->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost; this->upgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            } else {
                checkLimit(true);
                this->upgradeCosts.baseGasCost[upgradeType] = baseGasCost;
            }
            break;
    }
}

void LiteScenario::setUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            checkLimit(true);
            this->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; this->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            } else {
                checkLimit(true);
                this->upgradeCosts.gasCostFactor[upgradeType] = gasCostFactor;
            }
            break;
    }
}

void LiteScenario::setUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            checkLimit(true);
            this->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; this->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            } else {
                checkLimit(true);
                this->upgradeCosts.baseResearchTime[upgradeType] = baseResearchTime;
            }
            break;
    }
}

void LiteScenario::setUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string((size_t)upgradeType) +
                " is out of range for the " + (expansion ? "UPGx" : "UPGS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UNIS) )
                this->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            checkLimit(true);
            this->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; this->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::UNIx) ) {
                checkLimit(true);
                this->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            } else {
                checkLimit(false);
                this->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UNIS) ) {
                checkLimit(false);
                this->origUpgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            } else {
                checkLimit(true);
                this->upgradeCosts.researchTimeFactor[upgradeType] = researchTimeFactor;
            }
            break;
    }
}

bool LiteScenario::useExpansionUpgradeLeveling(Chk::UseExpSection useExp, Sc::Upgrade::Type upgradeType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::UPGR);
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::PUPx);
    }
}

size_t LiteScenario::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] : this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType];
}

size_t LiteScenario::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] : this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType];
}

size_t LiteScenario::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? this->upgradeLeveling.defaultMaxLevel[upgradeType] : this->origUpgradeLeveling.defaultMaxLevel[upgradeType];
}

size_t LiteScenario::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? this->upgradeLeveling.defaultStartLevel[upgradeType] : this->origUpgradeLeveling.defaultStartLevel[upgradeType];
}

bool LiteScenario::playerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    
    bool expansion = useExpansionUpgradeLeveling(useExp, upgradeType);
    if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
        throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");

    return expansion ? this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] != Chk::UseDefault::No :
        this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] != Chk::UseDefault::No;
}

void LiteScenario::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            checkLimit(true);
            this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(false);
                this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                this->origUpgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(true);
                this->upgradeLeveling.playerMaxUpgradeLevel[playerIndex][upgradeType] = u8(maxUpgradeLevel);
            }
            break;
    }
}

void LiteScenario::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            checkLimit(true);
            this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(false);
                this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                this->origUpgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(true);
                this->upgradeLeveling.playerStartUpgradeLevel[playerIndex][upgradeType] = u8(startUpgradeLevel);
            }
            break;
    }
}

void LiteScenario::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            checkLimit(true);
            this->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); this->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                this->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(false);
                this->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                this->origUpgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            } else {
                checkLimit(true);
                this->upgradeLeveling.defaultMaxLevel[upgradeType] = u8(maxUpgradeLevel);
            }
            break;
    }
}

void LiteScenario::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            checkLimit(true);
            this->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); this->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                this->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(false);
                this->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                this->origUpgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            } else {
                checkLimit(true);
                this->upgradeLeveling.defaultStartLevel[upgradeType] = u8(startUpgradeLevel);
            }
            break;
    }
}

void LiteScenario::setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    auto checkLimit = [&upgradeType, &playerIndex](bool expansion){
        if ( (expansion && upgradeType >= Sc::Upgrade::TotalTypes) || (!expansion && upgradeType >= Sc::Upgrade::TotalOriginalTypes) )
            throw std::out_of_range(std::string("UpgradeType: ") + std::to_string(upgradeType) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PUPx" : "UPGR") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType >= Sc::Upgrade::Type::UnusedUpgrade_46 && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            checkLimit(true);
            this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PUPx) ) {
                checkLimit(true);
                this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            } else {
                checkLimit(false);
                this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::UPGR) ) {
                checkLimit(false);
                this->origUpgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            } else {
                checkLimit(true);
                this->upgradeLeveling.playerUpgradeUsesDefault[playerIndex][upgradeType] = value;
            }
            break;
    }
}

void LiteScenario::setUpgradesToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            this->origUpgradeCosts = Chk::UPGS {};
            this->upgradeCosts = Chk::UPGx {};
            this->origUpgradeLeveling = Chk::UPGR {};
            this->upgradeLeveling = Chk::PUPx {};
            break;
        case Chk::UseExpSection::Yes:
            this->upgradeCosts = Chk::UPGx {};
            this->upgradeLeveling = Chk::PUPx {};
            break;
        case Chk::UseExpSection::No:
            this->origUpgradeCosts = Chk::UPGS {};
            this->origUpgradeLeveling = Chk::UPGR {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::UPGx) )
                this->upgradeCosts = Chk::UPGx {};
            else
                this->origUpgradeCosts = Chk::UPGS {};

            if ( this->hasSection(Chk::SectionName::PUPx) )
                this->upgradeLeveling = Chk::PUPx {};
            else
                this->origUpgradeLeveling = Chk::UPGR {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::UPGS) )
                this->origUpgradeCosts = Chk::UPGS {};
            else
                this->upgradeCosts = Chk::UPGx {};

            if ( this->hasSection(Chk::SectionName::UPGR) )
                this->origUpgradeLeveling = Chk::UPGR {};
            else
                this->upgradeLeveling = Chk::PUPx {};
            break;
    }
}

bool LiteScenario::useExpansionTechCosts(Chk::UseExpSection useExp, Sc::Tech::Type techType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return techType >= Sc::Tech::Type::Restoration || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return !this->hasSection(Chk::SectionName::TECS);
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::TECx);
    }
}

bool LiteScenario::techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? this->techCosts.useDefault[techType] != Chk::UseDefault::No : this->techCosts.useDefault[techType] != Chk::UseDefault::No;
}

u16 LiteScenario::getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? this->techCosts.mineralCost[techType] : this->techCosts.mineralCost[techType];
}

u16 LiteScenario::getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? this->techCosts.gasCost[techType] : this->techCosts.gasCost[techType];
}

u16 LiteScenario::getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? this->techCosts.researchTime[techType] : this->techCosts.researchTime[techType];
}

u16 LiteScenario::getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechCosts(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    }
    return expansion ? this->techCosts.energyCost[techType] : this->techCosts.energyCost[techType];
}

void LiteScenario::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts.useDefault[techType] = value;
            checkLimit(true);
            this->techCosts.useDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyCosts.useDefault[techType] = value; this->techCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyCosts.useDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                this->techCosts.useDefault[techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyCosts.useDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                this->origTechnologyCosts.useDefault[techType] = value;
            } else {
                checkLimit(true);
                this->techCosts.useDefault[techType] = value;
            }
            break;
    }
}

void LiteScenario::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts.mineralCost[techType] = mineralCost;
            checkLimit(true);
            this->techCosts.mineralCost[techType] = mineralCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyCosts.mineralCost[techType] = mineralCost; this->techCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyCosts.mineralCost[techType] = mineralCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                this->techCosts.mineralCost[techType] = mineralCost;
            } else {
                checkLimit(false);
                this->origTechnologyCosts.mineralCost[techType] = mineralCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                this->origTechnologyCosts.mineralCost[techType] = mineralCost;
            } else {
                checkLimit(true);
                this->techCosts.mineralCost[techType] = mineralCost;
            }
            break;
    }
}

void LiteScenario::setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts.gasCost[techType] = gasCost;
            checkLimit(true);
            this->techCosts.gasCost[techType] = gasCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyCosts.gasCost[techType] = gasCost; this->techCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyCosts.gasCost[techType] = gasCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                this->techCosts.gasCost[techType] = gasCost;
            } else {
                checkLimit(false);
                this->origTechnologyCosts.gasCost[techType] = gasCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                this->origTechnologyCosts.gasCost[techType] = gasCost;
            } else {
                checkLimit(true);
                this->techCosts.gasCost[techType] = gasCost;
            }
            break;
    }
}

void LiteScenario::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts.researchTime[techType] = researchTime;
            checkLimit(true);
            this->techCosts.researchTime[techType] = researchTime;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyCosts.researchTime[techType] = researchTime; this->techCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyCosts.researchTime[techType] = researchTime; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                this->techCosts.researchTime[techType] = researchTime;
            } else {
                checkLimit(false);
                this->origTechnologyCosts.researchTime[techType] = researchTime;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                this->origTechnologyCosts.researchTime[techType] = researchTime;
            } else {
                checkLimit(true);
                this->techCosts.researchTime[techType] = researchTime;
            }
            break;
    }
}

void LiteScenario::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "TECx" : "TECS") + " section!");
    };
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts.energyCost[techType] = energyCost;
            checkLimit(true);
            this->techCosts.energyCost[techType] = energyCost;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyCosts.energyCost[techType] = energyCost; this->techCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyCosts.energyCost[techType] = energyCost; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::TECx) ) {
                checkLimit(true);
                this->techCosts.energyCost[techType] = energyCost;
            } else {
                checkLimit(false);
                this->origTechnologyCosts.energyCost[techType] = energyCost;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::TECS) ) {
                checkLimit(false);
                this->origTechnologyCosts.energyCost[techType] = energyCost;
            } else {
                checkLimit(true);
                this->techCosts.energyCost[techType] = energyCost;
            }
            break;
    }
}

bool LiteScenario::useExpansionTechAvailability(Chk::UseExpSection useExp, Sc::Tech::Type techType) const
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return techType >= Sc::Tech::Type::Restoration || this->isHybridOrAbove();
        case Chk::UseExpSection::Yes: return true;
        case Chk::UseExpSection::No: return false;
        case Chk::UseExpSection::NoIfOrigAvailable: return this->hasSection(Chk::SectionName::PTEC) ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return this->hasSection(Chk::SectionName::PTEx) ? true : false;
    }
}

bool LiteScenario::techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? this->techAvailability.techAvailableForPlayer[playerIndex][techType] : this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType];
}

bool LiteScenario::techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? this->techAvailability.techResearchedForPlayer[playerIndex][techType] : this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType];
}

bool LiteScenario::techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    return expansion ? this->techAvailability.techAvailableByDefault[techType] : this->origTechnologyAvailability.techAvailableByDefault[techType];
}

bool LiteScenario::techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    return expansion ? this->techAvailability.techResearchedByDefault[techType] : this->origTechnologyAvailability.techResearchedByDefault[techType];
}

bool LiteScenario::playerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp) const
{
    bool expansion = useExpansionTechAvailability(useExp, techType);
    if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
    {
        throw std::out_of_range(std::string("TechType: ") + std::to_string((size_t)techType) +
            " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    }
    else if ( playerIndex >= Sc::Player::Total )
        throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");

    return expansion ? this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] : this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType];
}

void LiteScenario::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Available value = available ? Chk::Available::Yes : Chk::Available::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            checkLimit(true);
            this->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value; this->techAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                this->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                this->origTechnologyAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                this->techAvailability.techAvailableForPlayer[playerIndex][techType] = value;
            }
            break;
    }
}

void LiteScenario::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Researched value = researched ? Chk::Researched::Yes : Chk::Researched::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            checkLimit(true);
            this->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value; this->techAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                this->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                this->origTechnologyAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                this->techAvailability.techResearchedForPlayer[playerIndex][techType] = value;
            }
            break;
    }
}

void LiteScenario::setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Available value = available ? Chk::Available::Yes : Chk::Available::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            checkLimit(true);
            this->techAvailability.techAvailableByDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyAvailability.techAvailableByDefault[techType] = value; this->techAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyAvailability.techAvailableByDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                this->techAvailability.techAvailableByDefault[techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                this->origTechnologyAvailability.techAvailableByDefault[techType] = value;
            } else {
                checkLimit(true);
                this->techAvailability.techAvailableByDefault[techType] = value;
            }
            break;
    }
}

void LiteScenario::setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::Researched value = researched ? Chk::Researched::Yes : Chk::Researched::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            checkLimit(true);
            this->techAvailability.techResearchedByDefault[techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyAvailability.techResearchedByDefault[techType] = value; this->techAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyAvailability.techResearchedByDefault[techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                this->techAvailability.techResearchedByDefault[techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                this->origTechnologyAvailability.techResearchedByDefault[techType] = value;
            } else {
                checkLimit(true);
                this->techAvailability.techResearchedByDefault[techType] = value;
            }
            break;
    }
}

void LiteScenario::setPlayerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    auto checkLimit = [&techType, &playerIndex](bool expansion){
        if ( (expansion && techType >= Sc::Tech::TotalTypes) || (!expansion && techType >= Sc::Tech::TotalOriginalTypes) )
            throw std::out_of_range(std::string("TechType: ") + std::to_string(techType) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
        else if ( playerIndex >= Sc::Player::Total )
            throw std::out_of_range(std::string("PlayerIndex: ") + std::to_string(playerIndex) + " is out of range for the " + (expansion ? "PTEx" : "PTEC") + " section!");
    };
    Chk::UseDefault value = useDefault ? Chk::UseDefault::Yes : Chk::UseDefault::No;
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType >= Sc::Tech::Type::Restoration && this->version < Chk::Version::StarCraft_Hybrid )
                changeVersionTo(Chk::Version::StarCraft_Hybrid);
            if ( techType < Sc::Tech::TotalOriginalTypes && hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            checkLimit(true);
            this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            break;
        case Chk::UseExpSection::Both: checkLimit(false); this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::Yes: checkLimit(true); this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::No: checkLimit(false); this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value; break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( hasSection(Chk::SectionName::PTEx) ) {
                checkLimit(true);
                this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            } else {
                checkLimit(false);
                this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            }
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( hasSection(Chk::SectionName::PTEC) ) {
                checkLimit(false);
                this->origTechnologyAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            } else {
                checkLimit(true);
                this->techAvailability.playerUsesDefaultsForTech[playerIndex][techType] = value;
            }
            break;
    }
}

void LiteScenario::setTechsToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            this->origTechnologyCosts = Chk::TECS {};
            this->techCosts = Chk::TECx {};
            this->origTechnologyAvailability = Chk::PTEC {};
            this->techAvailability = Chk::PTEx {};
            break;
        case Chk::UseExpSection::Yes:
            this->techCosts = Chk::TECx {};
            this->techAvailability = Chk::PTEx {};
            break;
        case Chk::UseExpSection::No:
            this->origTechnologyCosts = Chk::TECS {};
            this->origTechnologyAvailability = Chk::PTEC {};
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( this->hasSection(Chk::SectionName::TECx) )
                this->techCosts = Chk::TECx {};
            else
                this->origTechnologyCosts = Chk::TECS {};

            if ( this->hasSection(Chk::SectionName::PTEx) )
                this->techAvailability = Chk::PTEx {};
            else
                this->origTechnologyAvailability = Chk::PTEC {};
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( this->hasSection(Chk::SectionName::TECS) )
                this->origTechnologyCosts = Chk::TECS {};
            else
                this->techCosts = Chk::TECx {};

            if ( this->hasSection(Chk::SectionName::PTEC) )
                this->origTechnologyAvailability = Chk::PTEC {};
            else
                this->techAvailability = Chk::PTEx {};
            break;
    }
}

void LiteScenario::appendUnitStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, u32 userMask) const
{
    u16 u16StringId = (u16)stringId;
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->origUnitSettings.nameStringId[i] == u16StringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::OriginalUnitSettings, i));
        }
    }

    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->unitSettings.nameStringId[i] == u16StringId )
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExpansionUnitSettings, i));
        }
    }
}

bool LiteScenario::unitStringUsed(size_t stringId, u32 userMask) const
{
    u16 u16StringId = (u16)stringId;
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->origUnitSettings.nameStringId[i] == u16StringId )
                return true;
        }
    }
    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->unitSettings.nameStringId[i] == u16StringId )
                return true;
        }
    }
    return false;
}

void LiteScenario::markUsedUnitStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->origUnitSettings.nameStringId[i] > 0 )
                stringIdUsed[this->origUnitSettings.nameStringId[i]] = true;
        }
    }
    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
    {
        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( this->unitSettings.nameStringId[i] > 0 )
                stringIdUsed[this->unitSettings.nameStringId[i]] = true;
        }
    }
}

void LiteScenario::remapUnitStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(this->origUnitSettings.nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            this->origUnitSettings.nameStringId[i] = found->second;
    }
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        auto found = stringIdRemappings.find(this->unitSettings.nameStringId[i]);
        if ( found != stringIdRemappings.end() )
            this->unitSettings.nameStringId[i] = found->second;
    }
}

void LiteScenario::deleteUnitString(size_t stringId)
{
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( this->origUnitSettings.nameStringId[i] == stringId )
            this->origUnitSettings.nameStringId[i] = 0;
    }
    for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
    {
        if ( this->unitSettings.nameStringId[i] == stringId )
            this->unitSettings.nameStringId[i] = 0;
    }
}

Chk::Cuwp LiteScenario::getCuwp(size_t cuwpIndex) const
{
    if ( cuwpIndex >= Sc::Unit::MaxCuwps )
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPRP section!");

    return this->createUnitProperties[cuwpIndex];
}

void LiteScenario::setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        this->createUnitProperties[cuwpIndex] = cuwp;
}

size_t LiteScenario::addCuwp(const Chk::Cuwp & cuwp, bool fixUsageBeforeAdding, size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( memcmp(&cuwp, &this->createUnitProperties[i], sizeof(Chk::Cuwp)) == 0 )
            return i; // Return existing CUWP index
    }
    if ( fixUsageBeforeAdding )
        fixCuwpUsage(excludedTriggerIndex, excludedTriggerActionIndex);

    size_t nextUnused = Sc::Unit::MaxCuwps;
    for ( size_t i = 0; i < Sc::Unit::MaxCuwps; i++ )
    {
        if ( this->createUnitPropertiesUsed[i] == Chk::CuwpUsed::No )
        {
            nextUnused = i;
            break;
        }
    }
    if ( nextUnused < Sc::Unit::MaxCuwps )
        this->createUnitProperties[nextUnused] = cuwp;

    return nextUnused;
}

void LiteScenario::fixCuwpUsage(size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    for ( size_t i=0; i<Sc::Unit::MaxCuwps; i++ )
        this->createUnitPropertiesUsed[i] = Chk::CuwpUsed::No;

    size_t numTriggers = this->numTriggers();
    for ( size_t i=0; i<numTriggers; ++i )
    {
        const auto & trigger = this->triggers[i];
        for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; ++actionIndex )
        {
            const auto & action = trigger.actions[actionIndex];
            if ( action.actionType == Chk::Action::Type::CreateUnitWithProperties && action.number < Sc::Unit::MaxCuwps && !(i == excludedTriggerIndex && actionIndex == excludedTriggerActionIndex) )
                this->createUnitPropertiesUsed[action.number] = Chk::CuwpUsed::Yes;
        }
    }
}

bool LiteScenario::cuwpUsed(size_t cuwpIndex) const
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        return this->createUnitPropertiesUsed[cuwpIndex] != Chk::CuwpUsed::No;
    else
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPUS section!");
}

void LiteScenario::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    if ( cuwpIndex < Sc::Unit::MaxCuwps )
        this->createUnitPropertiesUsed[cuwpIndex] = cuwpUsed ? Chk::CuwpUsed::Yes : Chk::CuwpUsed::No;
    else
        throw std::out_of_range(std::string("CuwpIndex: ") + std::to_string(cuwpIndex) + " is out of range for the UPUS section!");
}

size_t LiteScenario::numTriggers() const
{
    return this->triggers.size();
}

Chk::Trigger & LiteScenario::getTrigger(size_t triggerIndex)
{
    if ( triggerIndex < this->triggers.size() )
        return this->triggers[triggerIndex];
    else
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
}

const Chk::Trigger & LiteScenario::getTrigger(size_t triggerIndex) const
{
    if ( triggerIndex < this->triggers.size() )
        return this->triggers[triggerIndex];
    else
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
}

size_t LiteScenario::addTrigger(const Chk::Trigger & trigger)
{
    this->triggers.push_back(trigger);
    return this->triggers.size()-1;
}

void LiteScenario::insertTrigger(size_t triggerIndex, const Chk::Trigger & trigger)
{
    if ( triggerIndex < this->triggers.size() )
    {
        auto position = std::next(this->triggers.begin(), triggerIndex);
        this->triggers.insert(position, trigger);
    }
    else
        this->triggers.push_back(trigger);

    fixTriggerExtensions();
}

void LiteScenario::deleteTrigger(size_t triggerIndex)
{
    if ( triggerIndex < this->triggers.size() )
    {
        auto trigger = std::next(this->triggers.begin(), triggerIndex);
        this->removeTriggersExtension(triggerIndex);
        this->triggers.erase(trigger);
    }
    fixTriggerExtensions();
}

void LiteScenario::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    size_t triggerIndexMin = std::min(triggerIndexFrom, triggerIndexTo);
    size_t triggerIndexMax = std::max(triggerIndexFrom, triggerIndexTo);
    if ( triggerIndexMax < this->triggers.size() && triggerIndexFrom != triggerIndexTo )
    {
        if ( triggerIndexMax-triggerIndexMin == 1 ) // Move up or down by 1 using swap
            std::swap(this->triggers[triggerIndexMin], this->triggers[triggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto trigger = this->triggers[triggerIndexFrom];
            auto toErase = std::next(this->triggers.begin(), triggerIndexFrom);
            this->triggers.erase(toErase);
            if ( triggerIndexTo == this->triggers.size() )
                this->triggers.push_back(trigger);
            else
            {
                auto insertPosition = triggerIndexTo == 0 ? this->triggers.begin() : std::next(this->triggers.begin(), triggerIndexTo-1);
                this->triggers.insert(insertPosition, trigger);
            }
        }
    }
    fixTriggerExtensions();
}

std::vector<Chk::Trigger> LiteScenario::replaceTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & triggers)
{
    if ( beginIndex == 0 && endIndex == this->triggers.size() )
    {
        this->triggers.swap(triggers);
        fixTriggerExtensions();
        return triggers;
    }
    else if ( beginIndex < endIndex && endIndex <= this->triggers.size() )
    {
        auto begin = this->triggers.begin()+beginIndex;
        auto end = this->triggers.begin()+endIndex;
        std::vector<Chk::Trigger> replacedTriggers(this->triggers.begin()+beginIndex, this->triggers.begin()+endIndex);
        this->triggers.erase(begin, end);
        this->triggers.insert(this->triggers.begin()+beginIndex, triggers.begin(), triggers.end());
        fixTriggerExtensions();
        return replacedTriggers;
    }
    else
        throw std::out_of_range(std::string("Range [") + std::to_string(beginIndex) + ", " + std::to_string(endIndex) +
            ") is invalid for trigger list of size: " + std::to_string(triggers.size()));
}

bool LiteScenario::hasTriggerExtension(size_t triggerIndex) const
{
    auto & trigger = this->triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    return extendedTrigDataIndex < this->triggerExtensions.size();
}

Chk::ExtendedTrigData & LiteScenario::getTriggerExtension(size_t triggerIndex, bool addIfNotFound)
{
    if ( triggerIndex >= this->triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    auto & trigger = this->triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    if ( extendedTrigDataIndex < this->triggerExtensions.size() )
        return this->triggerExtensions[extendedTrigDataIndex];
    else if ( addIfNotFound )
    {
        std::set<u32> usedExtensionIndexes {};
        for ( const auto & trig : this->triggers )
        {
            u32 usedIndex = u32(trig.getExtendedDataIndex());
            if ( (usedIndex & Chk::UnusedExtendedTrigDataIndexCheck) != 0 ) // Some usable index
                usedExtensionIndexes.insert(usedIndex);
        }
        for ( size_t i=0; i<this->triggerExtensions.size(); i++ )
        {
            if ( (i & Chk::UnusedExtendedTrigDataIndexCheck) != 0 && usedExtensionIndexes.count(u32(i)) == 0 ) // If index is usable and unused
            {
                this->triggerExtensions[i] = Chk::ExtendedTrigData{};
                trigger.setExtendedDataIndex(this->triggerExtensions.size()-1);
                return this->triggerExtensions[i];
            }
        }

        while ( (this->triggerExtensions.size() & Chk::UnusedExtendedTrigDataIndexCheck) == 0 ) // While next index is unusable
            this->triggerExtensions.push_back(Chk::ExtendedTrigData{}); // Put a dummy value in this position

        this->triggerExtensions.push_back(Chk::ExtendedTrigData{}); // Put real/usable extension data in this position
        trigger.setExtendedDataIndex(this->triggerExtensions.size()-1);
        return this->triggerExtensions[this->triggerExtensions.size()-1];
    }
    else
        throw std::logic_error(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " did not have an extension and addIfNotFound was not set!");
}

const Chk::ExtendedTrigData & LiteScenario::getTriggerExtension(size_t triggerIndex) const
{
    if ( triggerIndex >= this->triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    auto & trigger = this->triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    if ( extendedTrigDataIndex < this->triggerExtensions.size() )
        return this->triggerExtensions[extendedTrigDataIndex];
    else
        throw std::logic_error(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " did not have an extension!");
}

void LiteScenario::removeTriggersExtension(size_t triggerIndex)
{
    if ( triggerIndex >= this->triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");

    auto & trigger = this->triggers[triggerIndex];
    size_t extendedTrigDataIndex = trigger.getExtendedDataIndex();
    trigger.clearExtendedDataIndex();
    deleteTriggerExtension(extendedTrigDataIndex);
}

void LiteScenario::deleteTriggerExtension(size_t triggerExtensionIndex)
{
    if ( triggerExtensionIndex < this->triggerExtensions.size() && triggerExtensionIndex != Chk::ExtendedTrigDataIndex::None )
    {
        size_t i = this->triggerExtensions.size();
        for ( ; i > 0 && (((i-1) & Chk::UnusedExtendedTrigDataIndexCheck) == 0 || i-1 >= this->triggerExtensions.size()); i-- );

        if ( i == 0 )
            this->triggerExtensions.clear();
        else if ( i < this->triggerExtensions.size() )
        {
            auto firstErased = std::next(this->triggerExtensions.begin(), i);
            this->triggerExtensions.erase(firstErased, this->triggerExtensions.end());
        }
    }
}

void LiteScenario::fixTriggerExtensions()
{
    std::set<size_t> usedExtendedTrigDataIndexes;
    size_t numTriggers = this->triggers.size();
    for ( size_t i=0; i<numTriggers; i++ )
    {
        Chk::Trigger & trigger = this->triggers[i];
        size_t extendedDataIndex = trigger.getExtendedDataIndex();
        if ( extendedDataIndex != Chk::ExtendedTrigDataIndex::None )
        {
            if ( extendedDataIndex >= this->triggerExtensions.size() )
                trigger.clearExtendedDataIndex();
            else if ( usedExtendedTrigDataIndexes.find(extendedDataIndex) == usedExtendedTrigDataIndexes.end() ) // Valid extension
            {
                this->triggerExtensions[extendedDataIndex].trigNum = (u32)i; // Ensure the trigNum is correct
                usedExtendedTrigDataIndexes.insert(extendedDataIndex);
            }
            else // Same extension used by multiple triggers
                trigger.clearExtendedDataIndex();
        }
    }

    size_t numTriggerExtensions = this->triggerExtensions.size();
    for ( size_t i=0; i<numTriggerExtensions; i++ )
    {
        const Chk::ExtendedTrigData & extension = this->triggerExtensions[i];
        if ( usedExtendedTrigDataIndexes.find(i) == usedExtendedTrigDataIndexes.end() ) // Extension exists, but no trigger uses it
        {
            if ( extension.trigNum != Chk::ExtendedTrigData::TrigNum::None ) // Refers to a trigger
            {
                if ( extension.trigNum < this->triggers.size() && // this trigger exists without an extension
                    this->triggers[extension.trigNum].getExtendedDataIndex() == Chk::ExtendedTrigDataIndex::None )
                {
                    this->triggers[extension.trigNum].setExtendedDataIndex(i); // Link up extension to the trigger
                }
                else // Trigger does not exist
                    this->deleteTriggerExtension(i); // Delete the extension
            }
            else if ( extension.trigNum == Chk::ExtendedTrigData::TrigNum::None ) // Does not refer to a trigger
                this->deleteTriggerExtension(i); // Delete the extension
        }
    }
}

size_t LiteScenario::getCommentStringId(size_t triggerIndex) const
{
    if ( triggerIndex >= this->triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
    else
        return this->triggers[triggerIndex].getComment();
}

size_t LiteScenario::getExtendedCommentStringId(size_t triggerIndex) const
{
    if ( triggerIndex >= this->triggers.size() )
        throw std::out_of_range(std::string("TriggerIndex: ") + std::to_string(triggerIndex) + " is out of range for the TRIG section!");
    else
    {
        const auto & trigger = this->triggers[triggerIndex];
        size_t extendedDataIndex = trigger.getExtendedDataIndex();
        if ( (extendedDataIndex & Chk::UnusedExtendedTrigDataIndexCheck) != 0 && extendedDataIndex < this->triggerExtensions.size() )
            return this->triggerExtensions[extendedDataIndex].commentStringId;
    }
    return Chk::StringId::NoString;
}

void LiteScenario::setExtendedCommentStringId(size_t triggerIndex, size_t stringId)
{
    Chk::ExtendedTrigData & extension = getTriggerExtension(triggerIndex, stringId != Chk::StringId::NoString);
    extension.commentStringId = (u32)stringId;
    if ( stringId == Chk::StringId::NoString && extension.isBlank() )
        removeTriggersExtension(triggerIndex);
}

size_t LiteScenario::getExtendedNotesStringId(size_t triggerIndex) const
{
    if ( this->hasTriggerExtension(triggerIndex) )
        return getTriggerExtension(triggerIndex).notesStringId;
    else
        return Chk::StringId::NoString;
}

void LiteScenario::setExtendedNotesStringId(size_t triggerIndex, size_t stringId)
{
    Chk::ExtendedTrigData & extension = getTriggerExtension(triggerIndex, stringId != Chk::StringId::NoString);
    extension.notesStringId = (u32)stringId;
    if ( stringId == Chk::StringId::NoString && extension.isBlank() )
        removeTriggersExtension(triggerIndex);
}

size_t LiteScenario::numBriefingTriggers() const
{
    return this->briefingTriggers.size();
}

Chk::Trigger & LiteScenario::getBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < this->briefingTriggers.size() )
        return this->briefingTriggers[briefingTriggerIndex];
    else
        throw std::out_of_range(std::string("BriefingTriggerIndex: ") + std::to_string(briefingTriggerIndex) + " is out of range for the MBRF section!");
}

const Chk::Trigger & LiteScenario::getBriefingTrigger(size_t briefingTriggerIndex) const
{
    if ( briefingTriggerIndex < this->briefingTriggers.size() )
        return this->briefingTriggers[briefingTriggerIndex];
    else
        throw std::out_of_range(std::string("BriefingTriggerIndex: ") + std::to_string(briefingTriggerIndex) + " is out of range for the MBRF section!");
}

size_t LiteScenario::addBriefingTrigger(const Chk::Trigger & briefingTrigger)
{
    this->briefingTriggers.push_back(briefingTrigger);
    return this->briefingTriggers.size()-1;
}

void LiteScenario::insertBriefingTrigger(size_t briefingTriggerIndex, const Chk::Trigger & briefingTrigger)
{
    
    if ( briefingTriggerIndex < this->briefingTriggers.size() )
    {
        auto position = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.insert(position, briefingTrigger);
    }
    else
        this->briefingTriggers.push_back(briefingTrigger);
}

void LiteScenario::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    if ( briefingTriggerIndex < briefingTriggers.size() )
    {
        auto briefingTrigger = std::next(briefingTriggers.begin(), briefingTriggerIndex);
        briefingTriggers.erase(briefingTrigger);
    }
}

void LiteScenario::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    size_t briefingTriggerIndexMin = std::min(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    size_t briefingTriggerIndexMax = std::max(briefingTriggerIndexFrom, briefingTriggerIndexTo);
    if ( briefingTriggerIndexMax < briefingTriggers.size() && briefingTriggerIndexFrom != briefingTriggerIndexTo )
    {
        if ( briefingTriggerIndexMax-briefingTriggerIndexMin == 1 && briefingTriggerIndexMax < briefingTriggers.size() ) // Move up or down by 1 using swap
            std::swap(briefingTriggers[briefingTriggerIndexMin], briefingTriggers[briefingTriggerIndexMax]);
        else // Move up or down by more than one, remove from present location, insert in the list at destination
        {
            auto briefingTrigger = briefingTriggers[briefingTriggerIndexFrom];
            auto toErase = std::next(briefingTriggers.begin(), briefingTriggerIndexFrom);
            briefingTriggers.erase(toErase);
            auto insertPosition = briefingTriggerIndexTo == 0 ? briefingTriggers.begin() : std::next(briefingTriggers.begin(), briefingTriggerIndexTo-1);
            briefingTriggers.insert(insertPosition, briefingTrigger);
        }
    }
}

std::vector<Chk::Trigger> LiteScenario::replaceBriefingTriggerRange(size_t beginIndex, size_t endIndex, std::vector<Chk::Trigger> & briefingTriggers)
{
    if ( beginIndex == 0 && endIndex == this->briefingTriggers.size() )
    {
        this->briefingTriggers.swap(briefingTriggers);
        return briefingTriggers;
    }
    else if ( beginIndex < endIndex && endIndex <= this->briefingTriggers.size() )
    {
        auto begin = this->briefingTriggers.begin()+beginIndex;
        auto end = this->briefingTriggers.begin()+endIndex;
        std::vector<Chk::Trigger> replacedBriefingTriggers(this->briefingTriggers.begin()+beginIndex, this->briefingTriggers.begin()+endIndex);
        this->briefingTriggers.erase(begin, end);
        this->briefingTriggers.insert(this->briefingTriggers.begin()+beginIndex, briefingTriggers.begin(), briefingTriggers.end());
        return replacedBriefingTriggers;
    }
    else
        throw std::out_of_range(std::string("Range [") + std::to_string(beginIndex) + ", " + std::to_string(endIndex) +
            ") is invalid for briefing trigger list of size: " + std::to_string(briefingTriggers.size()));
}

size_t LiteScenario::addSound(size_t stringId)
{
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( this->soundPaths[i] == Chk::StringId::UnusedSound )
        {
            this->soundPaths[i] = (u32)stringId;
            return i;
        }
    }
    return Chk::TotalSounds;
}

bool LiteScenario::stringIsSound(size_t stringId) const
{
    u32 u32StringId = (u32)stringId;
    for ( size_t i=0; i<Chk::TotalSounds; i++ )
    {
        if ( this->soundPaths[i] == u32StringId )
            return true;
    }
    return false;
}

size_t LiteScenario::getSoundStringId(size_t soundIndex) const
{
    if ( soundIndex < Chk::TotalSounds )
        return this->soundPaths[soundIndex];
    else
        throw std::out_of_range(std::string("SoundIndex: ") + std::to_string((u32)soundIndex) + " is out of range for the WAV section!");
}

void LiteScenario::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    if ( soundIndex < Chk::TotalSounds )
        this->soundPaths[soundIndex] = (u32)soundStringId;
}

bool LiteScenario::triggerSwitchUsed(size_t switchId) const
{
    for ( const auto & trigger : this->triggers )
    {
        if ( trigger.switchUsed(switchId) )
            return true;
    }
    return false;
}

bool LiteScenario::triggerLocationUsed(size_t locationId) const
{
    for ( const auto & trigger : this->triggers )
    {
        if ( trigger.locationUsed(locationId) )
            return true;
    }
    return false;
}

void LiteScenario::appendTriggerStrUsage(size_t stringId, std::vector<Chk::StringUser> & stringUsers, Chk::Scope storageScope, u32 userMask) const
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) != Chk::StringUserFlag::None )
        {
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( this->soundPaths[i] == stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Sound, i));
            }
        }
        if ( (userMask & Chk::StringUserFlag::Switch) != Chk::StringUserFlag::None )
        {
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->switchNames[i] == stringId )
                    stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::Switch, i));
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTrigger) != Chk::StringUserFlag::None )
        {
            size_t numTriggers = this->triggers.size();
            for ( size_t trigIndex=0; trigIndex<numTriggers; trigIndex++ )
            {
                const auto & trigger = this->triggers[trigIndex];
                for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction &&
                        trigger.actions[actionIndex].stringUsed(stringId, Chk::StringUserFlag::TriggerAction) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::TriggerAction, trigIndex, actionIndex));
                    }
                    if ( (userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound &&
                        trigger.actions[actionIndex].stringUsed(stringId, Chk::StringUserFlag::TriggerActionSound) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::TriggerActionSound, trigIndex, actionIndex));
                    }
                }
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) != Chk::StringUserFlag::None )
        {
            size_t numBriefingTriggers = briefingTriggers.size();
            for ( size_t briefingTrigIndex=0; briefingTrigIndex<numBriefingTriggers; briefingTrigIndex++ )
            {
                const auto & briefingTrigger = briefingTriggers[briefingTrigIndex];
                for ( size_t actionIndex=0; actionIndex<Chk::Trigger::MaxActions; actionIndex++ )
                {
                    if ( (userMask & Chk::StringUserFlag::BriefingTriggerAction) == Chk::StringUserFlag::BriefingTriggerAction &&
                        briefingTrigger.actions[actionIndex].briefingStringUsed(stringId, Chk::StringUserFlag::BriefingTriggerAction) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::BriefingTriggerAction, briefingTrigIndex, actionIndex));
                    }
                    if ( (userMask & Chk::StringUserFlag::BriefingTriggerActionSound) == Chk::StringUserFlag::BriefingTriggerActionSound &&
                        briefingTrigger.actions[actionIndex].briefingStringUsed(stringId, Chk::StringUserFlag::BriefingTriggerActionSound) )
                    {
                        stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::BriefingTriggerActionSound, briefingTrigIndex, actionIndex));
                    }
                }
            }
        }
    }
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) != Chk::StringUserFlag::None )
    {
        for ( const auto & extendedTrig : this->triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                extendedTrig.commentStringId == stringId) )
            {
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExtendedTriggerComment, extendedTrig.trigNum));
            }
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                extendedTrig.notesStringId == stringId))
            {
                stringUsers.push_back(Chk::StringUser(Chk::StringUserFlag::ExtendedTriggerNotes, extendedTrig.trigNum));
            }
        }
    }
}

bool LiteScenario::triggerStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound && this->stringIsSound(stringId) )
            return true;
        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            u32 u32StringId = (u32)stringId;
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->switchNames[i] == u32StringId )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
        {
            for ( const auto & trigger : this->triggers )
            {
                if ( trigger.stringUsed(stringId, userMask) )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
        {
            for ( const auto & briefingTrigger : briefingTriggers )
            {
                if ( briefingTrigger.briefingStringUsed(stringId, userMask) )
                    return true;
            }
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {
        for ( const auto & extendedTrig : this->triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                    extendedTrig.commentStringId == stringId) ||
                ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                    extendedTrig.notesStringId == stringId) )
            {
                return true;
            }
        }
    }
    return false;
}

bool LiteScenario::triggerGameStringUsed(size_t stringId, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
    {
        for ( const auto & trigger : this->triggers )
        {
            if ( trigger.gameStringUsed(stringId, userMask) )
                return true;
        }
    }
    if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
    {
        for ( auto briefingTrigger : briefingTriggers )
        {
            if ( briefingTrigger.briefingStringUsed(stringId, userMask) )
                return true;
        }
    }
    return false;
}

bool LiteScenario::triggerEditorStringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        {
            return stringIsSound(stringId);
        }
        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            u32 u32StringId = (u32)stringId;
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->switchNames[i] == u32StringId )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction )
        {
            for ( const auto & trigger : this->triggers )
            {
                if ( trigger.commentStringUsed(stringId) )
                    return true;
            }
        }
        if ( (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
        {
            for ( const auto & extendedTrig : this->triggerExtensions )
            {
                if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                        extendedTrig.commentStringId == stringId) ||
                    ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                        extendedTrig.notesStringId == stringId) )
                {
                    return true;
                }
            }
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {
        for ( const auto & extendedTrig : this->triggerExtensions )
        {
            if ( ((userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment &&
                    extendedTrig.commentStringId == stringId) ||
                ((userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes &&
                    extendedTrig.notesStringId == stringId) )
            {
                return true;
            }
        }
    }
    return false;
}

void LiteScenario::markUsedTriggerLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    for ( const auto & trigger : this->triggers )
        trigger.markUsedLocations(locationIdUsed);
}

void LiteScenario::markUsedTriggerStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        {
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( this->soundPaths[i] != Chk::StringId::UnusedSound && this->soundPaths[i] < Chk::MaxStrings )
                    stringIdUsed[this->soundPaths[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->switchNames[i] > 0 && this->switchNames[i] < Chk::MaxStrings )
                    stringIdUsed[this->switchNames[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
        {
            for ( const auto & trigger : this->triggers )
                trigger.markUsedStrings(stringIdUsed, userMask);
        }

        if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
        {            
            for ( const auto & briefingTrigger : this->briefingTriggers )
                briefingTrigger.markUsedBriefingStrings(stringIdUsed, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {        
        for ( const auto & extendedTrig : this->triggerExtensions )
        {
            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment && extendedTrig.commentStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig.commentStringId] = true;

            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes && extendedTrig.notesStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig.notesStringId] = true;
        }
    }
}

void LiteScenario::markUsedTriggerGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( (userMask & Chk::StringUserFlag::AnyTrigger) > 0 )
    {
        for ( const auto & trigger : this->triggers )
            trigger.markUsedGameStrings(stringIdUsed, userMask);
    }
    if ( (userMask & Chk::StringUserFlag::AnyBriefingTrigger) > 0 )
    {
        for ( const auto & briefingTrigger : briefingTriggers )
            briefingTrigger.markUsedBriefingStrings(stringIdUsed, userMask);
    }
}

void LiteScenario::markUsedTriggerEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope storageScope, u32 userMask) const
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        {
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( this->soundPaths[i] != Chk::StringId::UnusedSound && this->soundPaths[i] < Chk::MaxStrings )
                    stringIdUsed[this->soundPaths[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        {
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( this->switchNames[i] > 0 && this->switchNames[i] < Chk::MaxStrings )
                    stringIdUsed[this->switchNames[i]] = true;
            }
        }

        if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction )
        {
            for ( const auto & trigger : this->triggers )
                trigger.markUsedCommentStrings(stringIdUsed);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (userMask & Chk::StringUserFlag::AnyTriggerExtension) > 0 )
    {
        for ( const auto & extendedTrig : this->triggerExtensions )
        {
            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerComment) == Chk::StringUserFlag::ExtendedTriggerComment && extendedTrig.commentStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig.commentStringId] = true;

            if ( (userMask & Chk::StringUserFlag::ExtendedTriggerNotes) == Chk::StringUserFlag::ExtendedTriggerNotes && extendedTrig.notesStringId != Chk::StringId::NoString )
                stringIdUsed[extendedTrig.notesStringId] = true;
        }
    }
}

void LiteScenario::remapTriggerLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    for ( auto & trigger : this->triggers )
        trigger.remapLocationIds(locationIdRemappings);
}

void LiteScenario::remapTriggerStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            auto found = stringIdRemappings.find(this->soundPaths[i]);
            if ( found != stringIdRemappings.end() )
                this->soundPaths[i] = found->second;
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            auto found = stringIdRemappings.find(this->switchNames[i]);
            if ( found != stringIdRemappings.end() )
                this->switchNames[i] = found->second;
        }
        for ( auto & trigger : this->triggers )
            trigger.remapStringIds(stringIdRemappings);
        for ( auto & briefingTrigger : this->briefingTriggers )
            briefingTrigger.remapBriefingStringIds(stringIdRemappings);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        for ( auto & extendedTrig : this->triggerExtensions )
        {
            auto replacement = stringIdRemappings.find(extendedTrig.commentStringId);
            if ( replacement != stringIdRemappings.end() )
                extendedTrig.commentStringId = replacement->second;

            replacement = stringIdRemappings.find(extendedTrig.notesStringId);
            if ( replacement != stringIdRemappings.end() )
                extendedTrig.notesStringId = replacement->second;
        }
    }
}

void LiteScenario::deleteTriggerLocation(size_t locationId)
{
    for ( auto & trigger : this->triggers )
        trigger.deleteLocation(locationId);
}

void LiteScenario::deleteTriggerString(size_t stringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( this->soundPaths[i] == stringId )
                this->soundPaths[i] = 0;
        }
        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( this->switchNames[i] == stringId )
                this->switchNames[i] = 0;
        }
        for ( auto & trigger : this->triggers )
            trigger.deleteString(stringId);
        for ( auto & briefingTrigger : briefingTriggers )
            briefingTrigger.deleteString(stringId);
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        for ( auto & extendedTrig : this->triggerExtensions )
        {
            if ( extendedTrig.commentStringId == stringId )
                extendedTrig.commentStringId = Chk::StringId::NoString;

            if ( extendedTrig.notesStringId == stringId )
                extendedTrig.notesStringId = Chk::StringId::NoString;
        }
    }
}

void LiteScenario::addIsomUndo(Chk::IsomRect::Point point, Chk::IsomCache & cache)
{
    if ( !cache.undoMap[point.y*cache.isomWidth + point.x] ) // if undoMap entry doesn't already exist at this position...
    {
        Chk::IsomRectUndo isomRectUndo(Chk::IsomDiamond{point.x, point.y}, getIsomRect(point), Chk::IsomRect{});
        cache.undoMap[point.y*cache.isomWidth + point.x] = isomRectUndo; // add undoMap entry at position
        cache.addIsomUndo(isomRectUndo);
    }
}

bool LiteScenario::diamondNeedsUpdate(Chk::IsomDiamond isomDiamond) const
{
    return isInBounds(isomDiamond) &&
        !centralIsomValueModified(isomDiamond) &&
        getCentralIsomValue(isomDiamond) != 0;
}

void LiteScenario::setIsomValue(Chk::IsomRect::Point isomDiamond, Sc::Isom::Quadrant shapeQuadrant, uint16_t isomValue, bool undoable, Chk::IsomCache & cache)
{
    if ( isInBounds(isomDiamond) )
    {
        Chk::IsomRectUndo* isomUndo = nullptr;
        size_t isomRectIndex = isomDiamond.y*cache.isomWidth + size_t(isomDiamond.x);
        if ( undoable && isomRectIndex < cache.undoMap.size() )
        {
            addIsomUndo(isomDiamond, cache);
            isomUndo = cache.undoMap[isomRectIndex] ? &cache.undoMap[isomRectIndex].value() : nullptr;
        }

        Chk::IsomRect & rect = isomRectAt(isomDiamond);
        rect.set(shapeQuadrant, isomValue);
        rect.setModified(shapeQuadrant);
        cache.changedArea.expandToInclude(isomDiamond.x, isomDiamond.y);

        if ( isomUndo != nullptr ) // Update the undo if it was present prior to the changes
            isomUndo->setNewValue(rect);
    }
}

void LiteScenario::setDiamondIsomValues(Chk::IsomDiamond isomDiamond, uint16_t isomValue, bool undoable, Chk::IsomCache & cache)
{
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::TopLeft), Sc::Isom::Quadrant::TopLeft, isomValue, undoable, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::TopRight), Sc::Isom::Quadrant::TopRight, isomValue, undoable, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::BottomRight), Sc::Isom::Quadrant::BottomRight, isomValue, undoable, cache);
    setIsomValue(isomDiamond.getRectangleCoords(Sc::Isom::Quadrant::BottomLeft), Sc::Isom::Quadrant::BottomLeft, isomValue, undoable, cache);
}

void LiteScenario::loadNeighborInfo(Chk::IsomDiamond isomDiamond, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const
{
    for ( auto i : Chk::IsomDiamond::neighbors ) // Gather info about the four neighboring isom diamonds/isom shapes
    {
        Chk::IsomDiamond neighbor = isomDiamond.getNeighbor(i);
        if ( isInBounds(neighbor) )
        {
            uint16_t isomValue = getCentralIsomValue(neighbor);
            neighbors[i].modified = centralIsomValueModified(neighbor);
            neighbors[i].isomValue = isomValue;
            if ( isomValue < isomLinks.size() )
            {
                neighbors[i].linkId = isomLinks[isomValue].getLinkId(Sc::Isom::OppositeQuadrant(i));
                if ( neighbors[i].modified && isomLinks[isomValue].terrainType > neighbors.maxModifiedOfFour )
                    neighbors.maxModifiedOfFour = isomLinks[isomValue].terrainType;
            }
        }
    }
}

uint16_t LiteScenario::countNeighborMatches(const Sc::Isom::ShapeLinks & shapeLinks, IsomNeighbors & neighbors, Span<Sc::Isom::ShapeLinks> isomLinks) const
{
    auto terrainType = shapeLinks.terrainType;
    uint16_t totalMatches = 0;
    for ( auto quadrant : Sc::Isom::quadrants ) // For each quadrant in the shape (and each neighbor which overlaps with said quadrant)
    {
        const auto & neighborShape = isomLinks[neighbors[quadrant].isomValue];
        auto neighborTerrainType = neighborShape.terrainType;
        auto neighborLinkId = neighbors[quadrant].linkId;
        auto quadrantLinkId = shapeLinks.getLinkId(quadrant);

        if ( neighborLinkId == quadrantLinkId && (quadrantLinkId < Sc::Isom::LinkId::OnlyMatchSameType || terrainType == neighborTerrainType) )
            ++totalMatches;
        else if ( neighbors[quadrant].modified ) // There was no match with a neighbor that was already modified, so this isomValue can't be valid
            return uint16_t(0);
    }
    return totalMatches;
}

void LiteScenario::searchForBestMatch(uint16_t startingTerrainType, IsomNeighbors & neighbors, Chk::IsomCache & cache) const
{
    bool searchUntilHigherTerrainType = startingTerrainType == cache.terrainTypes.size()/2+1; // The final search always searches until end or higher types
    bool searchUntilEnd = startingTerrainType == 0; // If startingTerrainType is zero, the whole table after start must be searched

    uint16_t isomValue = cache.getTerrainTypeIsomValue(startingTerrainType);
    for ( ; isomValue < cache.isomLinks.size(); ++isomValue )
    {
        auto terrainType = cache.isomLinks[isomValue].terrainType;
        if ( !searchUntilEnd && terrainType != startingTerrainType && (!searchUntilHigherTerrainType || terrainType > startingTerrainType) )
            break; // Do not search the rest of the table

        auto matchCount = countNeighborMatches(cache.isomLinks[isomValue], neighbors, cache.isomLinks);
        if ( matchCount > neighbors.bestMatch.matchCount )
            neighbors.bestMatch = {isomValue, matchCount};
    }
}

std::optional<uint16_t> LiteScenario::findBestMatchIsomValue(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache) const
{
    IsomNeighbors neighbors {};
    loadNeighborInfo(isomDiamond, neighbors, cache.isomLinks);

    uint16_t prevIsomValue = getCentralIsomValue(isomDiamond);
    if ( prevIsomValue < cache.isomLinks.size() )
    {
        uint8_t prevTerrainType = cache.isomLinks[prevIsomValue].terrainType; // Y = maxOfFour, x = prevTerrainType
        uint16_t mappedTerrainType = cache.terrainTypeMap[size_t(neighbors.maxModifiedOfFour)*cache.terrainTypes.size() + size_t(prevTerrainType)];
        searchForBestMatch(mappedTerrainType, neighbors, cache);
    }
    searchForBestMatch(uint16_t(neighbors.maxModifiedOfFour), neighbors, cache);
    searchForBestMatch(uint16_t(cache.terrainTypes.size()/2 + 1), neighbors, cache);

    if ( neighbors.bestMatch.isomValue == prevIsomValue ) // This ISOM diamond was already the best possible value
        return std::nullopt;
    else
        return neighbors.bestMatch.isomValue;
}

void LiteScenario::radiallyUpdateTerrain(bool undoable, std::deque<Chk::IsomDiamond> & diamondsToUpdate, Chk::IsomCache & cache)
{
    while ( !diamondsToUpdate.empty() )
    {
        Chk::IsomDiamond isomDiamond = diamondsToUpdate.front();
        diamondsToUpdate.pop_front();
        if ( diamondNeedsUpdate(isomDiamond) && !getIsomRect(isomDiamond).isVisited() )
        {
            isomRectAt(isomDiamond).setVisited();
            cache.changedArea.expandToInclude(isomDiamond.x, isomDiamond.y);
            if ( auto bestMatch = findBestMatchIsomValue(isomDiamond, cache) )
            {
                if ( *bestMatch != 0 )
                    setDiamondIsomValues(isomDiamond, *bestMatch, undoable, cache);

                for ( auto i : Chk::IsomDiamond::neighbors )
                {
                    Chk::IsomDiamond neighbor = isomDiamond.getNeighbor(i);
                    if ( diamondNeedsUpdate(neighbor) )
                        diamondsToUpdate.push_back({neighbor.x, neighbor.y});
                }
            }
        }
    }
}
    
void LiteScenario::updateTileFromIsom(Chk::IsomDiamond isomDiamond, Chk::IsomCache & cache)
{
    if ( isomDiamond.x+1 >= cache.isomWidth || isomDiamond.y+1 >= cache.isomHeight )
        return;

    size_t leftTileX = 2*isomDiamond.x;
    size_t rightTileX = leftTileX+1;

    size_t totalConnections = cache.tileGroups.size();

    uint32_t isomRectHash = getIsomRect(isomDiamond).getHash(cache.isomLinks);
    auto foundPotentialGroups = cache.hashToTileGroup->find(isomRectHash);
    if ( foundPotentialGroups != cache.hashToTileGroup->end() )
    {
        const std::vector<uint16_t> & potentialGroups = foundPotentialGroups->second;
        uint16_t destTileGroup = potentialGroups[0];

        // Lookup the isom group for this row using the above rows stack-bottom connection
        if ( isomDiamond.y > 0 )
        {
            auto aboveTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, isomDiamond.y-1));
            if ( aboveTileGroup < cache.tileGroups.size() )
            {
                uint16_t tileGroupBottom = cache.tileGroups[aboveTileGroup].stackConnections.bottom;
                for ( size_t i=0; i<potentialGroups.size(); ++i )
                {
                    if ( cache.tileGroups[potentialGroups[i]].stackConnections.top == tileGroupBottom )
                    {
                        destTileGroup = potentialGroups[i];
                        break;
                    }
                }
            }
        }

        uint16_t destSubTile = cache.getRandomSubtile(destTileGroup) % 16;
        cache.setTileValue(leftTileX, isomDiamond.y, 16*destTileGroup + destSubTile);
        cache.setTileValue(rightTileX, isomDiamond.y, 16*(destTileGroup+1) + destSubTile);

        // Find the top row of the tile-group stack (note: this is a tad performance sensitive, consider pre-linking stacks)
        size_t stackTopY = isomDiamond.y;
        auto curr = Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY));
        for ( ; stackTopY > 0 && curr < totalConnections && cache.tileGroups[curr].stackConnections.top != 0; --stackTopY )
        {
            auto above = Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY-1));
            if ( above >= totalConnections || cache.tileGroups[curr].stackConnections.top != cache.tileGroups[above].stackConnections.bottom )
                break;

            curr = above;
        }

        cache.setTileValue(leftTileX, stackTopY, 16*Sc::Terrain::getTileGroup(getTileValue(leftTileX, stackTopY)) + destSubTile);
        cache.setTileValue(rightTileX, stackTopY, 16*Sc::Terrain::getTileGroup(getTileValue(rightTileX, stackTopY)) + destSubTile);

        // Set tile values for the rest of the stack
        auto tileHeight = getTileHeight();
        for ( size_t y=stackTopY+1; y<tileHeight; ++y )
        {
            auto tileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y-1));
            auto nextTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y));

            if ( tileGroup >= cache.tileGroups.size() || nextTileGroup >= cache.tileGroups.size() ||
                cache.tileGroups[tileGroup].stackConnections.bottom == 0 || cache.tileGroups[nextTileGroup].stackConnections.top == 0 )
            {
                break;
            }

            uint16_t bottomConnection = cache.tileGroups[tileGroup].stackConnections.bottom;
            uint16_t leftTileGroup = Sc::Terrain::getTileGroup(getTileValue(leftTileX, y));
            uint16_t rightTileGroup = Sc::Terrain::getTileGroup(getTileValue(rightTileX, y));
            if ( bottomConnection != cache.tileGroups[nextTileGroup].stackConnections.top )
            {
                isomRectHash = getIsomRect({isomDiamond.x, y}).getHash(cache.isomLinks);

                auto foundPotentialGroups = cache.hashToTileGroup->find(isomRectHash);
                if ( foundPotentialGroups != cache.hashToTileGroup->end() )
                {
                    const std::vector<uint16_t> & potentialGroups = foundPotentialGroups->second;
                    for ( size_t i=0; i<potentialGroups.size(); ++i )
                    {
                        if ( cache.tileGroups[potentialGroups[i]].stackConnections.top == bottomConnection )
                        {
                            leftTileGroup = potentialGroups[i];
                            rightTileGroup = leftTileGroup + 1;
                            break;
                        }
                    }
                }
            }

            cache.setTileValue(leftTileX, y, 16*leftTileGroup + destSubTile);
            cache.setTileValue(rightTileX, y, 16*rightTileGroup + destSubTile);
        }
    }
    else
    {
        cache.setTileValue(leftTileX, isomDiamond.y, 0);
        cache.setTileValue(rightTileX, isomDiamond.y, 0);
    }
}
