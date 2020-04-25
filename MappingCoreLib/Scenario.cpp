#include "Scenario.h"
#include "sha256.h"
#include "Math.h"
#include "../CommanderLib/Logger.h"
#include "Sections.h"
#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <set>

extern Logger logger;

template <typename SectionType>
std::shared_ptr<SectionType> GetSection(std::unordered_map<SectionName, Section> & sections, const SectionName & sectionName)
{
    auto found = sections.find(sectionName);
    if ( found != sections.end() )
        return std::dynamic_pointer_cast<SectionType>(found->second);
    else
        return nullptr;
}

Scenario::Scenario() :
    versions(), strings(), players(), layers(), properties(), triggers(),
    tailData({}), tailLength(0), mapIsProtected(false), jumpCompress(false)
{
    versions.layers = &layers;
    strings.versions = &versions;
    strings.players = &players;
    strings.layers = &layers;
    strings.properties = &properties;
    strings.triggers = &triggers;
    players.strings = &strings;
    layers.strings = &strings;
    layers.triggers = &triggers;
    properties.versions = &versions;
    properties.strings = &strings;
    triggers.strings = &strings;
    triggers.layers = &layers;
}

Scenario::Scenario(Sc::Terrain::Tileset tileset, u16 width, u16 height) :
    versions(true), strings(true), players(true), layers(tileset, width, height), properties(true), triggers(true),
    tailData({}), tailLength(0), mapIsProtected(false), jumpCompress(false)
{
    versions.layers = &layers;
    strings.versions = &versions;
    strings.players = &players;
    strings.layers = &layers;
    strings.properties = &properties;
    strings.triggers = &triggers;
    players.strings = &strings;
    layers.strings = &strings;
    layers.triggers = &triggers;
    properties.versions = &versions;
    properties.strings = &strings;
    triggers.strings = &strings;
    triggers.layers = &layers;

    if ( versions.isHybridOrAbove() )
        allSections.push_back(versions.type);
    
    allSections.push_back(versions.ver);

    if ( !versions.isHybridOrAbove() )
        allSections.push_back(versions.iver);

    allSections.push_back(versions.ive2);
    allSections.push_back(versions.vcod);
    allSections.push_back(players.iown);
    allSections.push_back(players.ownr);
    allSections.push_back(layers.era);
    allSections.push_back(layers.dim);
    allSections.push_back(players.side);
    allSections.push_back(layers.mtxm);
    allSections.push_back(properties.puni);
    allSections.push_back(properties.upgr);
    allSections.push_back(properties.ptec);
    allSections.push_back(layers.unit);
    allSections.push_back(layers.isom);
    allSections.push_back(layers.tile);
    allSections.push_back(layers.dd2);
    allSections.push_back(layers.thg2);
    allSections.push_back(layers.mask);
    allSections.push_back(strings.str);
    allSections.push_back(triggers.uprp);
    allSections.push_back(triggers.upus);
    allSections.push_back(layers.mrgn);
    allSections.push_back(triggers.trig);
    allSections.push_back(triggers.mbrf);
    allSections.push_back(strings.sprp);
    allSections.push_back(players.forc);
    allSections.push_back(triggers.wav);
    allSections.push_back(properties.unis);
    allSections.push_back(properties.upgs);
    allSections.push_back(properties.tecs);
    allSections.push_back(triggers.swnm);
    allSections.push_back(players.colr);
    allSections.push_back(properties.pupx);
    allSections.push_back(properties.ptex);
    allSections.push_back(properties.unix);
    allSections.push_back(properties.upgx);
    allSections.push_back(properties.tecx);
}

Scenario::~Scenario()
{

}

void Scenario::clear()
{
    strings.clear();
    triggers.clear();
    layers.clear();
    properties.clear();
    players.clear();
    versions.clear();

    allSections.clear();

    for ( size_t i=0; i<tailData.size(); i++ )
        tailData[i] = u8(0);

    tailLength = 0;
    
    jumpCompress = false;
    mapIsProtected = false;
}

bool Scenario::empty()
{
    return allSections.empty() && tailLength == 0 && versions.empty() && strings.empty() && players.empty() && layers.empty() && properties.empty() && triggers.empty();
}

bool Scenario::isProtected()
{
    return mapIsProtected;
}

bool Scenario::hasPassword()
{
    return tailLength == 7;
}

bool Scenario::isPassword(const std::string & password)
{
    if ( hasPassword() )
    {
        SHA256 sha256;
        std::string hashStr = sha256(password);
        if ( hashStr.length() >= 7 )
        {
            u64 eightHashBytes = std::stoull(hashStr.substr(0, 8), nullptr, 16);
            u8* hashBytes = (u8*)&eightHashBytes;

            for ( u8 i = 0; i < tailLength; i++ )
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

bool Scenario::setPassword(const std::string & oldPass, const std::string & newPass)
{
    if ( isPassword(oldPass) )
    {
        if ( newPass == "" )
        {
            for ( u8 i = 0; i < tailLength; i++ )
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
                for ( u8 i = 0; i < tailLength; i++ )
                    tailData[i] = hashBytes[i];

                return true;
            }
        }
    }
    return false;
}

bool Scenario::login(const std::string & password)
{
    if ( isPassword(password) )
    {
        mapIsProtected = false;
        return true;
    }
    return false;
}

bool Scenario::read(std::istream & is)
{
    clear();

    // First read contents of "is" to "chk", this will allow jumping backwards when reading chks with jump sections
    std::stringstream chk(std::ios_base::binary|std::ios_base::in|std::ios_base::out);
    chk << is.rdbuf();
    if ( !is.good() && !is.eof() )
    {
        logger.error("Unexpected failure reading scenario contents!");
        return false; // Read error on "is"
    }

    chk.seekg(std::ios_base::beg); // Move to start of chk
    std::multimap<SectionName, Section> parsedSections;
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
                Chk::SectionSize sizeRead = 0;
                Section section = nullptr;
                try {
                    section = ChkSection::read(parsedSections, sectionHeader, chk, sizeRead);
                } catch ( std::exception & e ) {
                    logger.error() << "Read of section " << ChkSection::getNameString(sectionHeader.name) << " failed with error: " << e.what() << std::endl;
                    section = nullptr;
                }
                if ( section != nullptr && (chk.good() || chk.eof()) )
                {
                    parsedSections.insert(std::pair<SectionName, Section>(sectionHeader.name, section));
                    allSections.push_back(section);

                    if ( sizeRead != sectionHeader.sizeInBytes ) // Undersized section
                        mapIsProtected = true;
                }
                else
                    return parsingFailed("Unexpected error reading chk section contents!");
            }
            else // if ( sectionHeader.sizeInBytes < 0 ) // Jump section
            {
                chk.seekg(sectionHeader.sizeInBytes, std::ios_base::cur);
                if ( !chk.good() )
                    return parsingFailed("Unexpected error processing chk jump section!");

                jumpCompress = true;
            }
        }
        else // if ( bytesRead < sizeof(Chk::SectionHeader) ) // Partial section header
        {
            for ( std::streamsize i=0; i<headerBytesRead; i++ )
                tailData[i] = ((u8*)&sectionHeader)[i];
            for ( size_t i=headerBytesRead; i<tailData.size(); i++ )
                tailData[i] = u8(0);

            tailLength = (u8)headerBytesRead;
            mapIsProtected = true;
        }
    }
    while ( chk.good() );

    // For all sections that are actually used by scenario, get the instance of the section that will be used
    std::unordered_map<SectionName, Section> finalSections;
    const std::vector<SectionName> & sectionNames = ChkSection::getNames();
    for ( const SectionName & sectionName : sectionNames )
    {
        LoadBehavior loadBehavior = ChkSection::getLoadBehavior(sectionName);
        auto sectionInstances = parsedSections.equal_range(sectionName);
        auto it = sectionInstances.first;
        if ( it != parsedSections.end() )
        {
            if ( loadBehavior == LoadBehavior::Standard ) // Last instance of a section is used
            {
                Section section = nullptr;
                for ( ; it != sectionInstances.second; ++it )
                    section = it->second;
                
                if ( section != nullptr )
                    finalSections.insert(std::pair<SectionName, Section>(sectionName, section));
            }
            else if ( loadBehavior == LoadBehavior::Append || loadBehavior == LoadBehavior::Override ) // First instance of a section is used
                finalSections.insert(std::pair<SectionName, Section>(sectionName, sectionInstances.first->second));
        }
    }
    
    versions.set(finalSections);
    strings.set(finalSections);
    players.set(finalSections);
    layers.set(finalSections);
    properties.set(finalSections);
    triggers.set(finalSections);

    if ( versions.ver == nullptr )
        return parsingFailed("Map was missing the VER section!");

    Chk::Version version = versions.ver->getVersion();

    // TODO: More validation
    return true;
}

StrSynchronizerPtr Scenario::getStrSynchronizer()
{
    return StrSynchronizerPtr(&strings, [](StrSynchronizer*){});
}

void Scenario::addSection(Section section)
{
    if ( section != nullptr )
    {
        for ( auto & existing : allSections )
        {
            if ( section == existing )
                return;
        }
        allSections.push_back(section);
    }
}

void Scenario::removeSection(const SectionName & sectionName)
{
    bool foundSection = false;
    do
    {
        foundSection = false;
        for ( auto it = allSections.begin(); it != allSections.end(); ++it )
        {
            if ( (*it)->getName() == sectionName )
            {
                allSections.erase(it);
                foundSection = true;
                break;
            }
        }
    }
    while ( foundSection );
}

bool Scenario::parsingFailed(const std::string & error)
{
    logger.error(error);
    clear();
    return false;
}

void Scenario::write(std::ostream & os)
{
    for ( auto & section : allSections )
        section->writeWithHeader(os, *this);
}

std::vector<u8> Scenario::serialize()
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

bool Scenario::deserialize(Chk::SerializedChk* data)
{
    if ( data->header.name == Chk::CHK )
    {
        Chk::Size size = data->header.sizeInBytes;
        std::stringstream chk(std::ios_base::in|std::ios_base::out|std::ios_base::binary);
        std::copy(&data->data[0], &data->data[size], std::ostream_iterator<u8>(chk));
    }
    return false;
}

void Scenario::updateSaveSections()
{
    if ( strings.hasExtendedStrings() )
    {
        addSection(strings.ostr);
        addSection(strings.kstr);
    }

    if ( triggers.ktrg != nullptr && !triggers.ktrg->empty() )
        addSection(triggers.ktrg);

    if ( triggers.ktgp != nullptr && !triggers.ktgp->empty() )
        addSection(triggers.ktgp);
}

bool Scenario::changeVersionTo(Chk::Version version, bool lockAnywhere, bool autoDefragmentLocations)
{
    if ( versions.changeTo(version, lockAnywhere, autoDefragmentLocations) )
    {
        if ( version < Chk::Version::StarCraft_BroodWar ) // Original or Hybrid: No COLR, include all original properties
        {
            if ( version < Chk::Version::StarCraft_Hybrid ) // Original: No TYPE, IVE2, or expansion properties
            {
                removeSection(SectionName::TYPE);
                removeSection(SectionName::PUPx);
                removeSection(SectionName::PTEx);
                removeSection(SectionName::UNIx);
                removeSection(SectionName::TECx);
            }
            removeSection(SectionName::COLR);
            addSection(versions.iver);
            addSection(properties.upgr);
            addSection(properties.ptec);
            addSection(properties.unis);
            addSection(properties.upgs);
            addSection(properties.tecs);
        }
        else // if ( version >= Chk::Version::StarCraft_BroodWar ) // Broodwar: No IVER or original properties, include COLR
        {
            removeSection(SectionName::IVER);
            removeSection(SectionName::UPGR);
            removeSection(SectionName::PTEC);
            removeSection(SectionName::UNIS);
            removeSection(SectionName::UPGS);
            removeSection(SectionName::TECS);
            addSection(players.colr);
        }
        
        if ( version >= Chk::Version::StarCraft_Hybrid ) // Hybrid or BroodWar: Include type, ive2, and all expansion properties
        {
            addSection(versions.type);
            addSection(properties.pupx);
            addSection(properties.ptex);
            addSection(properties.unix);
            addSection(properties.upgx);
            addSection(properties.tecx);
        }
        return true;
    }
    return false;
}

void Scenario::setTileset(Sc::Terrain::Tileset tileset)
{
    layers.setTileset(tileset);
}

Versions::Versions(bool useDefault) : layers(nullptr)
{
    if ( useDefault )
    {
        ver = VerSection::GetDefault(); // StarCraft version information
        type = TypeSection::GetDefault(ver->getVersion()); // Redundant versioning
        iver = IverSection::GetDefault(); // Redundant versioning
        ive2 = Ive2Section::GetDefault(); // Redundant versioning
        vcod = VcodSection::GetDefault(); // Validation
        if ( vcod == nullptr || ive2 == nullptr || iver == nullptr || type == nullptr || ver == nullptr )
        {
            throw ScenarioAllocationFailure(
                (vcod == nullptr ? ChkSection::getNameString(SectionName::VCOD) :
                (ive2 == nullptr ? ChkSection::getNameString(SectionName::IVE2) :
                (iver == nullptr ? ChkSection::getNameString(SectionName::IVER) :
                (type == nullptr ? ChkSection::getNameString(SectionName::TYPE) :
                    ChkSection::getNameString(SectionName::VER))))));
        }
    }
}

bool Versions::empty()
{
    return ver == nullptr && type == nullptr && iver == nullptr && ive2 == nullptr && vcod == nullptr;
}

Chk::Version Versions::getVersion()
{
    return ver->getVersion();
}

bool Versions::is(Chk::Version version)
{
    return ver->getVersion() == version;
}

bool Versions::isOriginal()
{
    return ver->isOriginal();
}

bool Versions::isHybrid()
{
    return ver->isHybrid();
}

bool Versions::isExpansion()
{
    return ver->isExpansion();
}

bool Versions::isHybridOrAbove()
{
    return ver->isHybridOrAbove();
}

bool Versions::changeTo(Chk::Version version, bool lockAnywhere, bool autoDefragmentLocations)
{
    Chk::Version oldVersion = ver->getVersion();
    ver->setVersion(version);
    if ( version < Chk::Version::StarCraft_Hybrid )
    {
        if ( layers->trimLocationsToOriginal(lockAnywhere, autoDefragmentLocations) )
        {
            type->setType(Chk::Type::RAWS);
            iver->setVersion(Chk::IVersion::Current);
        }
        else
        {
            logger.error("Cannot save as original with over 64 locations in use!");
            return false;
        }
    }
    else if ( version < Chk::Version::StarCraft_BroodWar )
    {
        type->setType(Chk::Type::RAWS);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->expandToScHybridOrExpansion();
    }
    else // version >= Chk::Version::StarCraft_Broodwar
    {
        type->setType(Chk::Type::RAWB);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->expandToScHybridOrExpansion();
    }
    return true;
}

bool Versions::hasDefaultValidation()
{
    return vcod->isDefault();
}

void Versions::setToDefaultValidation()
{
    vcod->setToDefault();
}

void Versions::set(std::unordered_map<SectionName, Section> & sections)
{
    ver = GetSection<VerSection>(sections, SectionName::VER);
    type = GetSection<TypeSection>(sections, SectionName::TYPE);
    iver = GetSection<IverSection>(sections, SectionName::IVER);
    ive2 = GetSection<Ive2Section>(sections, SectionName::IVE2);

    vcod = GetSection<VcodSection>(sections, SectionName::VCOD);

    if ( ver != nullptr )
    {
        if ( type == nullptr )
            type = TypeSection::GetDefault(ver->getVersion());
        if ( iver == nullptr )
            iver = IverSection::GetDefault();
        if ( ive2 == nullptr )
            ive2 = Ive2Section::GetDefault();
    }
}

void Versions::clear()
{
    ver = nullptr;
    type = nullptr;
    iver = nullptr;
    ive2 = nullptr;

    vcod = nullptr;
}


Strings::Strings(bool useDefault) : versions(nullptr), players(nullptr), layers(nullptr), properties(nullptr), triggers(nullptr),
    StrSynchronizer(StrCompressFlag::DuplicateStringRecycling, StrCompressFlag::AllNonInterlacing)
{
    if ( useDefault )
    {
        sprp = SprpSection::GetDefault(); // Scenario name and description
        str = StrSection::GetDefault(); // StarCraft string data
        ostr = OstrSection::GetDefault(); // Overrides for all but trigger and briefing strings
        kstr = KstrSection::GetDefault(); // Editor only string data
        if ( str == nullptr || sprp == nullptr )
        {
            throw ScenarioAllocationFailure(
                str == nullptr ? ChkSection::getNameString(SectionName::STR) :
                ChkSection::getNameString(SectionName::SPRP));
        }
    }
}

bool Strings::empty()
{
    return sprp == nullptr && str == nullptr && ostr == nullptr && kstr == nullptr;
}

bool Strings::hasExtendedStrings()
{
    return ostr != nullptr && kstr != nullptr && !kstr->empty();
}

size_t Strings::getCapacity(Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        return str->getCapacity();
    else if ( storageScope == Chk::Scope::Editor )
        return kstr->getCapacity();
    else
        return 0;
}

bool Strings::stringUsed(size_t stringId, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask, bool ensureStored)
{
    if ( storageScope == Chk::Scope::Game && (str->stringStored(stringId) || !ensureStored) )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( usageScope == Chk::Scope::Editor )
                return layers->stringUsed(stringId, storageScope, userMask) || triggers->editorStringUsed(stringId, userMask);
            else if ( usageScope == Chk::Scope::Game )
            {
                return sprp->stringUsed(stringId, userMask) || players->stringUsed(stringId, userMask) ||
                    properties->stringUsed(stringId, userMask) || triggers->gameStringUsed(stringId, userMask);
            }
            else // if ( usageScope == Chk::Scope::Either )
            {
                return sprp->stringUsed(stringId, userMask) || players->stringUsed(stringId, userMask) ||
                    properties->stringUsed(stringId, userMask) || layers->stringUsed(stringId, storageScope, userMask) ||
                    triggers->stringUsed(stringId, userMask);
            }
        }
        else // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            return usageScope == Chk::Scope::Either && triggers->stringUsed(stringId, userMask) ||
                usageScope == Chk::Scope::Game && triggers->gameStringUsed(stringId, userMask) ||
                usageScope == Chk::Scope::Editor && triggers->editorStringUsed(stringId, userMask);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (kstr->stringStored(stringId) || !ensureStored) )
        return ostr->stringUsed(stringId, userMask);

    return false;
}

bool Strings::stringStored(size_t stringId, Chk::Scope storageScope)
{
    return (storageScope & Chk::Scope::Game) == Chk::Scope::Game && str->stringStored(stringId) ||
        (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor && kstr->stringStored(stringId); 
}

void Strings::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask)
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        bool markGameStrings = (usageScope & Chk::Scope::Game) == Chk::Scope::Game;
        bool markEditorStrings = (usageScope & Chk::Scope::Editor) == Chk::Scope::Editor;

        if ( markGameStrings )
        {
            sprp->markUsedStrings(stringIdUsed, userMask); // {SPRP, Game, u16}: Scenario Name and Scenario Description
            players->markUsedStrings(stringIdUsed, userMask); // {FORC, Game, u16}: Force Names
            properties->markUsedStrings(stringIdUsed, userMask); // {UNIS, Game, u16}: Unit Names (original); {UNIx, Game, u16}: Unit names (expansion)
            if ( markEditorStrings ) // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, ...
                triggers->markUsedStrings(stringIdUsed, userMask); // ... transmission text, next scenario, sound path, comment; {MBRF, Game, u32}: mission objectives, sound, text message
            else
                triggers->markUsedGameStrings(stringIdUsed, userMask); // {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, transmission text, next scenario, sound path
        }

        if ( markEditorStrings )
        {
            layers->markUsedStrings(stringIdUsed, userMask); // {MRGN, Editor, u16}: location name
            if ( !markGameStrings )
                triggers->markUsedEditorStrings(stringIdUsed, userMask); // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: comment
        }
    }
    
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
        ostr->markUsedStrings(stringIdUsed, userMask);
}

void Strings::markValidUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope, u32 userMask)
{
    markUsedStrings(stringIdUsed, usageScope, storageScope, userMask);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
            str->unmarkUnstoredStrings(stringIdUsed);
            break;
        case Chk::Scope::Editor:
            kstr->unmarkUnstoredStrings(stringIdUsed);
            break;
        case Chk::Scope::Either:
            {
                size_t limit = std::min(std::min((size_t)Chk::MaxStrings, str->getCapacity()), kstr->getCapacity());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && !str->stringStored(stringId) && !kstr->stringStored(stringId) )
                        stringIdUsed[stringId] = false;
                }

                if ( str->getCapacity() > kstr->getCapacity() )
                {
                    for ( ; stringId < str->getCapacity(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && !str->stringStored(stringId) )
                            stringIdUsed[stringId] = false;
                    }
                }
                else if ( kstr->getCapacity() > str->getCapacity() )
                {
                    for ( ; stringId < kstr->getCapacity(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && !kstr->stringStored(stringId) )
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

StrProp Strings::getProperties(size_t editorStringId)
{
    return kstr != nullptr ? kstr->getProperties(editorStringId) : StrProp();
}

void Strings::setProperties(size_t editorStringId, const StrProp & strProp)
{
    if ( kstr != nullptr )
        kstr->setProperties(editorStringId, strProp);
}

template <typename StringType>
std::shared_ptr<StringType> Strings::getString(size_t stringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame:
        {
            std::shared_ptr<StringType> editorResult = kstr->getString<StringType>(stringId);
            return editorResult != nullptr ? editorResult : str->getString<StringType>(stringId) ;
        }
        case Chk::Scope::Game: return str->getString<StringType>(stringId);
        case Chk::Scope::Editor: return kstr->getString<StringType>(stringId);
        case Chk::Scope::GameOverEditor:
        {
            std::shared_ptr<StringType> gameResult = str->getString<StringType>(stringId);
            return gameResult != nullptr ? gameResult : kstr->getString<StringType>(stringId) ;
        }
        default: return nullptr;
    }
}
template std::shared_ptr<RawString> Strings::getString<RawString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getString<EscString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getString<ChkdString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getString<SingleLineChkdString>(size_t stringId, Chk::Scope storageScope);

template <typename StringType>
size_t Strings::findString(const StringType & str, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: return this->str->findString<StringType>(str);
        case Chk::Scope::Editor: return kstr->findString<StringType>(str);
        case Chk::Scope::GameOverEditor:
        case Chk::Scope::Either:
            {
                size_t gameResult = this->str->findString<StringType>(str);
                return gameResult != Chk::StringId::NoString ? gameResult : kstr->findString<StringType>(str);
            }
        case Chk::Scope::EditorOverGame:
            {
                size_t editorResult = kstr->findString<StringType>(str);
                return editorResult != Chk::StringId::NoString ? editorResult : this->str->findString<StringType>(str);
            }
    }
    return (size_t)Chk::StringId::NoString;
}
template size_t Strings::findString<RawString>(const RawString & str, Chk::Scope storageScope);
template size_t Strings::findString<EscString>(const EscString & str, Chk::Scope storageScope);
template size_t Strings::findString<ChkdString>(const ChkdString & str, Chk::Scope storageScope);
template size_t Strings::findString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope);

void Strings::setCapacity(size_t stringCapacity, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
        str->setCapacity(stringCapacity, *this, autoDefragment);
    else if ( storageScope == Chk::Scope::Editor )
        kstr->setCapacity(stringCapacity, *this, autoDefragment);
}

template <typename StringType>
size_t Strings::addString(const StringType & str, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
        return this->str->addString<StringType>(str, *this, autoDefragment);
    else if ( storageScope == Chk::Scope::Editor )
        return kstr->addString<StringType>(str, *this, autoDefragment);

    return (size_t)Chk::StringId::NoString;
}
template size_t Strings::addString<RawString>(const RawString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<EscString>(const EscString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<ChkdString>(const ChkdString & str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<SingleLineChkdString>(const SingleLineChkdString & str, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::replaceString(size_t stringId, const StringType & str, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        this->str->replaceString<StringType>(stringId, str);
    else if ( storageScope == Chk::Scope::Editor )
        kstr->replaceString<StringType>(stringId, str);
}
template void Strings::replaceString<RawString>(size_t stringId, const RawString & str, Chk::Scope storageScope);
template void Strings::replaceString<EscString>(size_t stringId, const EscString & str, Chk::Scope storageScope);
template void Strings::replaceString<ChkdString>(size_t stringId, const ChkdString & str, Chk::Scope storageScope);
template void Strings::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString & str, Chk::Scope storageScope);

void Strings::deleteUnusedStrings(Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: str->deleteUnusedStrings(*this); break;
        case Chk::Scope::Editor: kstr->deleteUnusedStrings(*this); break;
        case Chk::Scope::Both: str->deleteUnusedStrings(*this); kstr->deleteUnusedStrings(*this); break;
    }
}

void Strings::deleteString(size_t stringId, Chk::Scope storageScope, bool deleteOnlyIfUnused)
{
    if ( (storageScope & Chk::Scope::Game) == Chk::Scope::Game )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Game) )
        {
            str->deleteString(stringId, deleteOnlyIfUnused, std::shared_ptr<StrSynchronizer>(this, [](StrSynchronizer*){}));

            sprp->deleteString(stringId);
            players->deleteString(stringId);
            properties->deleteString(stringId);
            layers->deleteString(stringId);
            triggers->deleteString(stringId);
        }
    }
    
    if ( (storageScope & Chk::Scope::Editor) == Chk::Scope::Editor )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
        {
            kstr->deleteString(stringId, deleteOnlyIfUnused, std::shared_ptr<StrSynchronizer>(this, [](StrSynchronizer*){}));

            ostr->deleteString(stringId);
        }
    }
}

void Strings::moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        str->moveString(stringIdFrom, stringIdTo, *this);
    else if ( storageScope == Chk::Scope::Editor )
        kstr->moveString(stringIdFrom, stringIdTo, *this);
}

size_t Strings::rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo, bool autoDefragment)
{
    if ( changeStorageScopeTo == Chk::Scope::Editor && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, Chk::StringUserFlag::All, true) )
    {
        RawStringPtr toRescope = getString<RawString>(stringId, Chk::Scope::Game);
        size_t newStringId = addString<RawString>(*toRescope, Chk::Scope::Editor, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == sprp->getScenarioNameStringId() )
            {
                stringIdsReplaced.insert(ostr->getScenarioNameStringId());
                ostr->setScenarioNameStringId((u32)newStringId);
            }
            if ( stringId == sprp->getScenarioDescriptionStringId() )
            {
                stringIdsReplaced.insert(ostr->getScenarioDescriptionStringId());
                ostr->setScenarioDescriptionStringId((u32)newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == players->getForceStringId((Chk::Force)i) )
                {
                    stringIdsReplaced.insert(ostr->getForceNameStringId((Chk::Force)i));
                    ostr->setForceNameStringId((Chk::Force)i, (u32)newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::No) )
                {
                    stringIdsReplaced.insert(ostr->getUnitNameStringId((Sc::Unit::Type)i));
                    ostr->setUnitNameStringId((Sc::Unit::Type)i, (u32)newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::Yes) )
                {
                    stringIdsReplaced.insert(ostr->getExpUnitNameStringId((Sc::Unit::Type)i));
                    ostr->setExpUnitNameStringId((Sc::Unit::Type)i, (u32)newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == triggers->getSoundStringId(i) )
                {
                    stringIdsReplaced.insert(ostr->getSoundPathStringId(i));
                    ostr->setSoundPathStringId(i, (u32)newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == triggers->getSwitchNameStringId(i) )
                {
                    stringIdsReplaced.insert(ostr->getSwitchNameStringId(i));
                    ostr->setSwitchNameStringId(i, (u32)newStringId);
                }
            }
            size_t numLocations = layers->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == layers->getLocation(i)->stringId )
                {
                    stringIdsReplaced.insert(ostr->getLocationNameStringId(i));
                    ostr->setLocationNameStringId(i, (u32)newStringId);
                }
            }
            // TODO: When implementing TRIG and MBRF extended strings you'll have to implement string rescoping for triggers

            deleteString(stringId, Chk::Scope::Game, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Editor, true);
        }
    }
    else if ( changeStorageScopeTo == Chk::Scope::Game && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, Chk::StringUserFlag::All, true) )
    {
        RawStringPtr toRescope = getString<RawString>(stringId, Chk::Scope::Editor);
        size_t newStringId = addString<RawString>(*toRescope, Chk::Scope::Game, autoDefragment);
        if ( newStringId != 0 )
        {
            std::set<u32> stringIdsReplaced;
            if ( stringId == ostr->getScenarioNameStringId() )
            {
                stringIdsReplaced.insert((u32)sprp->getScenarioNameStringId());
                sprp->setScenarioNameStringId((u16)newStringId);
            }
            if ( stringId == ostr->getScenarioDescriptionStringId() )
            {
                stringIdsReplaced.insert((u32)sprp->getScenarioDescriptionStringId());
                sprp->setScenarioDescriptionStringId((u16)newStringId);
            }
            for ( size_t i=0; i<Chk::TotalForces; i++ )
            {
                if ( stringId == ostr->getForceNameStringId((Chk::Force)i) )
                {
                    stringIdsReplaced.insert((u32)players->getForceStringId((Chk::Force)i));
                    players->setForceStringId((Chk::Force)i, (u16)newStringId);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == ostr->getUnitNameStringId((Sc::Unit::Type)i) )
                {
                    stringIdsReplaced.insert((u32)properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::No));
                    properties->setUnitNameStringId((Sc::Unit::Type)i, newStringId, Chk::UseExpSection::No);
                }
            }
            for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
            {
                if ( stringId == ostr->getExpUnitNameStringId((Sc::Unit::Type)i) )
                {
                    stringIdsReplaced.insert((u32)properties->getUnitNameStringId((Sc::Unit::Type)i, Chk::UseExpSection::Yes));
                    properties->setUnitNameStringId((Sc::Unit::Type)i, newStringId, Chk::UseExpSection::Yes);
                }
            }
            for ( size_t i=0; i<Chk::TotalSounds; i++ )
            {
                if ( stringId == ostr->getSoundPathStringId(i) )
                {
                    stringIdsReplaced.insert((u32)triggers->getSoundStringId(i));
                    triggers->setSoundStringId(i, newStringId);
                }
            }
            for ( size_t i=0; i<Chk::TotalSwitches; i++ )
            {
                if ( stringId == ostr->getSwitchNameStringId(i) )
                {
                    stringIdsReplaced.insert((u32)triggers->getSwitchNameStringId(i));
                    triggers->setSwitchNameStringId(i, newStringId);
                }
            }
            size_t numLocations = layers->numLocations();
            for ( size_t i=1; i<=numLocations; i++ )
            {
                if ( stringId == ostr->getLocationNameStringId(i) )
                {
                    Chk::LocationPtr location = layers->getLocation(i);
                    stringIdsReplaced.insert(location->stringId);
                    location->stringId = (u16)newStringId;
                }
            }
            // TODO: When implementing TRIG and MBRF extended strings you'll have to implement string rescoping for triggers

            deleteString(stringId, Chk::Scope::Editor, false);
            for ( auto stringIdReplaced : stringIdsReplaced )
                deleteString(stringIdReplaced, Chk::Scope::Game, true);
        }
    }
    return 0;
}

std::vector<u8> & Strings::getTailData()
{
    return str->getTailData();
}

size_t Strings::getTailDataOffset()
{
    return str->getTailDataOffset(*this);
}

size_t Strings::getInitialTailDataOffset()
{
    return str->getInitialTailDataOffset();
}

size_t Strings::getBytePaddedTo()
{
    return str->getBytePaddedTo();
}

void Strings::setBytePaddedTo(size_t bytePaddedTo)
{
    str->setBytePaddedTo(bytePaddedTo);
}

size_t Strings::getScenarioNameStringId(Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getScenarioNameStringId() : sprp->getScenarioNameStringId();
}

size_t Strings::getScenarioDescriptionStringId(Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getScenarioDescriptionStringId() : sprp->getScenarioDescriptionStringId();
}

size_t Strings::getForceNameStringId(Chk::Force force, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getForceNameStringId(force) : players->getForceStringId(force);
}

size_t Strings::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        return properties->getUnitNameStringId(unitType, useExp);
    else if ( storageScope == Chk::Scope::Editor )
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::Yes: return ostr->getExpUnitNameStringId(unitType);
            case Chk::UseExpSection::No: return ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::YesIfAvailable: return ostr->getExpUnitNameStringId(unitType) != 0 ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType);
            case Chk::UseExpSection::NoIfOrigAvailable: return ostr->getUnitNameStringId(unitType) != 0 ? ostr->getUnitNameStringId(unitType) : ostr->getExpUnitNameStringId(unitType);
        }
    }
    return 0;
}

size_t Strings::getSoundPathStringId(size_t soundIndex, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getSoundPathStringId(soundIndex) : triggers->getSoundStringId(soundIndex);
}

size_t Strings::getSwitchNameStringId(size_t switchIndex, Chk::Scope storageScope)
{
    return storageScope == Chk::Scope::Editor ? ostr->getSwitchNameStringId(switchIndex) : triggers->getSwitchNameStringId(switchIndex);
}

size_t Strings::getLocationNameStringId(size_t locationId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        return ostr->getLocationNameStringId(locationId);
    else
    {
        Chk::LocationPtr location = layers->getLocation(locationId);
        return location != nullptr ? location->stringId : 0;
    }
}

void Strings::setScenarioNameStringId(size_t scenarioNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setScenarioNameStringId((u32)scenarioNameStringId);
    else
        sprp->setScenarioNameStringId((u16)scenarioNameStringId);
}

void Strings::setScenarioDescriptionStringId(size_t scenarioDescriptionStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setScenarioDescriptionStringId((u32)scenarioDescriptionStringId);
    else
        sprp->setScenarioDescriptionStringId((u16)scenarioDescriptionStringId);
}

void Strings::setForceNameStringId(Chk::Force force, size_t forceNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setForceNameStringId(force, (u32)forceNameStringId);
    else
        players->setForceStringId(force, (u16)forceNameStringId);
}

void Strings::setUnitNameStringId(Sc::Unit::Type unitType, size_t unitNameStringId, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        properties->setUnitNameStringId(unitType, unitNameStringId, useExp);
    else
    {
        switch ( useExp )
        {
            case Chk::UseExpSection::Auto:
            case Chk::UseExpSection::Both: ostr->setUnitNameStringId(unitType, (u32)unitNameStringId); ostr->setExpUnitNameStringId(unitType, (u32)unitNameStringId); break;
            case Chk::UseExpSection::YesIfAvailable:
            case Chk::UseExpSection::Yes: ostr->setExpUnitNameStringId(unitType, (u32)unitNameStringId); break;
            case Chk::UseExpSection::NoIfOrigAvailable:
            case Chk::UseExpSection::No: ostr->setUnitNameStringId(unitType, (u32)unitNameStringId); break;
        }
    }
}

void Strings::setSoundPathStringId(size_t soundIndex, size_t soundPathStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setSoundPathStringId(soundIndex, (u32)soundPathStringId);
    else
        triggers->setSoundStringId(soundIndex, (u32)soundPathStringId);
}

void Strings::setSwitchNameStringId(size_t switchIndex, size_t switchNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setSwitchNameStringId(switchIndex, (u32)switchNameStringId);
    else
        triggers->setSwitchNameStringId(switchIndex, (u32)switchNameStringId);
}

void Strings::setLocationNameStringId(size_t locationId, size_t locationNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setLocationNameStringId(locationId, (u32)locationNameStringId);
    else
    {
        auto location = layers->getLocation(locationId);
        if ( location != nullptr )
            location->stringId = (u16)locationNameStringId;
    }
}

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::shared_ptr<StringType> Strings::getString(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Game: return getString<StringType>(gameStringId, Chk::Scope::Game);
        case Chk::Scope::Editor: return getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::GameOverEditor: return gameStringId != 0 ? getString<StringType>(gameStringId, Chk::Scope::Game) : getString<StringType>(editorStringId, Chk::Scope::Editor);
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return editorStringId != 0 ? getString<StringType>(editorStringId, Chk::Scope::Editor) : getString<StringType>(gameStringId, Chk::Scope::Game);
    }
    return nullptr;
}
template std::shared_ptr<RawString> Strings::getString<RawString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getString<EscString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getString<ChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getString<SingleLineChkdString>(size_t gameStringId, size_t editorStringId, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getScenarioName(Chk::Scope storageScope)
{
    return getString<StringType>(sprp->getScenarioNameStringId(), ostr->getScenarioNameStringId(), storageScope);
}
template std::shared_ptr<RawString> Strings::getScenarioName<RawString>(Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getScenarioName<EscString>(Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getScenarioName<ChkdString>(Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getScenarioName<SingleLineChkdString>(Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getScenarioDescription(Chk::Scope storageScope)
{
    return getString<StringType>(sprp->getScenarioDescriptionStringId(), ostr->getScenarioDescriptionStringId(), storageScope);
}
template std::shared_ptr<RawString> Strings::getScenarioDescription<RawString>(Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getScenarioDescription<EscString>(Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getScenarioDescription<ChkdString>(Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getScenarioDescription<SingleLineChkdString>(Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getForceName(Chk::Force force, Chk::Scope storageScope)
{
    return getString<StringType>(players->getForceStringId(force), ostr->getForceNameStringId(force), storageScope);
}
template std::shared_ptr<RawString> Strings::getForceName<RawString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getForceName<EscString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getForceName<ChkdString>(Chk::Force force, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getForceName<SingleLineChkdString>(Chk::Force force, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getUnitName(Sc::Unit::Type unitType, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    std::shared_ptr<StringType> mapUnitName = unitType < Sc::Unit::TotalTypes ? getString<StringType>(
        properties->getUnitNameStringId(unitType, useExp),
        properties->useExpansionUnitSettings(useExp) ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType),
        storageScope) : nullptr;

    if ( mapUnitName != nullptr )
        return mapUnitName;
    else if ( unitType < Sc::Unit::TotalTypes )
        return std::shared_ptr<StringType>(new StringType(Sc::Unit::defaultDisplayNames[unitType]));
    else
        return std::shared_ptr<StringType>(new StringType("ID:" + std::to_string(unitType)));
}
template std::shared_ptr<RawString> Strings::getUnitName<RawString>(Sc::Unit::Type unitType, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getUnitName<EscString>(Sc::Unit::Type unitType, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getUnitName<ChkdString>(Sc::Unit::Type unitType, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, bool defaultIfNull, Chk::UseExpSection useExp, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getSoundPath(size_t soundIndex, Chk::Scope storageScope)
{
    return getString<StringType>(triggers->getSoundStringId(soundIndex), ostr->getSoundPathStringId(soundIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getSoundPath<RawString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getSoundPath<EscString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getSoundPath<ChkdString>(size_t soundIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getSoundPath<SingleLineChkdString>(size_t soundIndex, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getSwitchName(size_t switchIndex, Chk::Scope storageScope)
{
    return getString<StringType>(triggers->getSwitchNameStringId(switchIndex), ostr->getSwitchNameStringId(switchIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getSwitchName<RawString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getSwitchName<EscString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getSwitchName<ChkdString>(size_t switchIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getSwitchName<SingleLineChkdString>(size_t switchIndex, Chk::Scope storageScope);

template <typename StringType>
std::shared_ptr<StringType> Strings::getLocationName(size_t locationId, Chk::Scope storageScope)
{
    return getString<StringType>((locationId > 0 && locationId <= layers->numLocations() ? layers->getLocation(locationId)->stringId : 0), ostr->getLocationNameStringId(locationId), storageScope);
}
template std::shared_ptr<RawString> Strings::getLocationName<RawString>(size_t locationId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getLocationName<EscString>(size_t locationId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getLocationName<ChkdString>(size_t locationId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getLocationName<SingleLineChkdString>(size_t locationId, Chk::Scope storageScope);

template <typename StringType>
void Strings::setScenarioName(const StringType & scenarioNameString, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioNameString, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                sprp->setScenarioNameStringId((u16)newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setScenarioNameStringId((u32)newStringId);
        }
    }
}
template void Strings::setScenarioName<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setScenarioDescription(const StringType & scenarioDescription, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor )
    {
        size_t newStringId = addString<StringType>(scenarioDescription, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                sprp->setScenarioDescriptionStringId((u16)newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setScenarioDescriptionStringId((u32)newStringId);
        }
    }
}
template void Strings::setScenarioDescription<RawString>(const RawString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<EscString>(const EscString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<ChkdString>(const ChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<SingleLineChkdString>(const SingleLineChkdString & scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setForceName(Chk::Force force, const StringType & forceName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (u32)force < Chk::TotalForces )
    {
        size_t newStringId = addString<StringType>(forceName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                players->setForceStringId(force, (u16)newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setForceNameStringId(force, (u32)newStringId);
        }
    }
}
template void Strings::setForceName<RawString>(Chk::Force force, const RawString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<EscString>(Chk::Force force, const EscString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<ChkdString>(Chk::Force force, const ChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<SingleLineChkdString>(Chk::Force force, const SingleLineChkdString & forceName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setUnitName(Sc::Unit::Type unitType, const StringType & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && unitType < Sc::Unit::TotalTypes )
    {
        size_t newStringId = addString<StringType>(unitName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                properties->setUnitNameStringId(unitType, newStringId, useExp);
            else if ( storageScope == Chk::Scope::Editor )
            {
                switch ( useExp )
                {
                    case Chk::UseExpSection::Auto: versions->isHybridOrAbove() ? ostr->setExpUnitNameStringId(unitType, (u32)newStringId) : ostr->setUnitNameStringId(unitType, (u32)newStringId); break;
                    case Chk::UseExpSection::Yes: ostr->setExpUnitNameStringId(unitType, (u32)newStringId); break;
                    case Chk::UseExpSection::No: ostr->setUnitNameStringId(unitType, (u32)newStringId); break;
                    default: ostr->setUnitNameStringId(unitType, (u32)newStringId); ostr->setExpUnitNameStringId(unitType, (u32)newStringId); break;
                }
            }
        }
    }
}
template void Strings::setUnitName<RawString>(Sc::Unit::Type unitType, const RawString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<EscString>(Sc::Unit::Type unitType, const EscString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<ChkdString>(Sc::Unit::Type unitType, const ChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const SingleLineChkdString & unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSoundPath(size_t soundIndex, const StringType & soundPath, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && soundIndex < Chk::TotalSounds )
    {
        size_t newStringId = addString<StringType>(soundPath, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                triggers->setSoundStringId(soundIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setSoundPathStringId(soundIndex, (u32)newStringId);
        }
    }
}
template void Strings::setSoundPath<RawString>(size_t soundIndex, const RawString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<EscString>(size_t soundIndex, const EscString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<ChkdString>(size_t soundIndex, const ChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<SingleLineChkdString>(size_t soundIndex, const SingleLineChkdString & soundPath, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSwitchName(size_t switchIndex, const StringType & switchName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && switchIndex < Chk::TotalSwitches )
    {
        size_t newStringId = addString<StringType>(switchName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                triggers->setSwitchNameStringId(switchIndex, newStringId);
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setSwitchNameStringId(switchIndex, (u32)newStringId);
        }
    }
}
template void Strings::setSwitchName<RawString>(size_t switchIndex, const RawString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<EscString>(size_t switchIndex, const EscString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<ChkdString>(size_t switchIndex, const ChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<SingleLineChkdString>(size_t switchIndex, const SingleLineChkdString & switchName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setLocationName(size_t locationId, const StringType & locationName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && locationId > 0 && locationId <= layers->numLocations() )
    {
        size_t newStringId = addString<StringType>(locationName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                layers->getLocation(locationId)->stringId = (u16)newStringId;
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setLocationNameStringId(locationId, (u32)newStringId);
        }
    }
}
template void Strings::setLocationName<RawString>(size_t locationId, const RawString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<EscString>(size_t locationId, const EscString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<ChkdString>(size_t locationId, const ChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<SingleLineChkdString>(size_t locationId, const SingleLineChkdString & locationName, Chk::Scope storageScope, bool autoDefragment);

void Strings::syncStringsToBytes(std::deque<ScStrPtr> & strings, std::vector<u8> & stringBytes,
    StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
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
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, maxStrings);
        
    size_t sizeAndOffsetSpaceAndNulSpace = sizeof(u16) + sizeof(u16)*numStrings + 1;
    size_t sectionSize = sizeAndOffsetSpaceAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] != nullptr )
            sectionSize += strings[i]->length();
    }

    constexpr size_t maxStandardSize = u16_max;
    if ( sectionSize > maxStandardSize )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::STR), sectionSize-sizeAndOffsetSpaceAndNulSpace, maxStandardSize);

    stringBytes.assign(sizeof(u16)+sizeof(u16)*numStrings, u8(0));
    (u16 &)stringBytes[0] = (u16)numStrings;
    u16 initialNulOffset = u16(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] == nullptr )
            (u16 &)stringBytes[sizeof(u16)*i] = initialNulOffset;
        else
        {
            (u16 &)stringBytes[sizeof(u16)*i] = u16(stringBytes.size());
            stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+(strings[i]->length()+1));
        }
    }
}

void Strings::syncKstringsToBytes(std::deque<ScStrPtr> & strings, std::vector<u8> & stringBytes,
    StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
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
    size_t numStrings = strings.size() > 0 ? strings.size()-1 : 0; // Exclude string at index 0
    if ( numStrings > maxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), numStrings, maxStrings);

    size_t stringPropertiesStart = 2*sizeof(u32)+2*numStrings;
    size_t versionAndSizeAndOffsetAndStringPropertiesAndNulSpace = 2*sizeof(u32) + 2*sizeof(u32)*numStrings + 1;
    size_t sectionSize = versionAndSizeAndOffsetAndStringPropertiesAndNulSpace;
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] != nullptr )
            sectionSize += strings[i]->length();
    }

    constexpr size_t maxStandardSize = s32_max;
    if ( sectionSize > maxStandardSize )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::KSTR), sectionSize-versionAndSizeAndOffsetAndStringPropertiesAndNulSpace, maxStandardSize);

    stringBytes.assign(2*sizeof(u32)+2*sizeof(u32)*numStrings, u8(0));
    (u32 &)stringBytes[0] = 2;
    (u32 &)stringBytes[sizeof(u32)] = (u32)numStrings;
    u32 initialNulOffset = u32(stringBytes.size());
    stringBytes.push_back(u8('\0')); // Add initial NUL character
    for ( size_t i=1; i<=numStrings; i++ )
    {
        if ( strings[i] == nullptr )
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = initialNulOffset;
        else
        {
            auto prop = strings[i]->properties();
            (u32 &)stringBytes[stringPropertiesStart+sizeof(u32)*i] = (u32 &)Chk::StringProperties(prop.red, prop.green, prop.blue, prop.isUsed, prop.hasPriority, prop.isBold, prop.isUnderlined, prop.isItalics, prop.size);
            (u32 &)stringBytes[sizeof(u32)+sizeof(u32)*i] = u32(stringBytes.size());
            stringBytes.insert(stringBytes.end(), strings[i]->str, strings[i]->str+strings[i]->length()+1);
        }
    }
}

const std::vector<u32> Strings::compressionFlagsProgression = {
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


Strings::StringBackup Strings::backup()
{
    return { (str == nullptr ? nullptr : str->backup()) };
}

void Strings::restore(StringBackup & backup)
{
    if ( str != nullptr )
        str->restore(backup.strBackup);
}

void Strings::remapStringIds(const std::map<u32, u32> & stringIdRemappings, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        sprp->remapStringIds(stringIdRemappings);
        players->remapStringIds(stringIdRemappings);
        properties->remapStringIds(stringIdRemappings);
        layers->remapStringIds(stringIdRemappings);
        triggers->remapStringIds(stringIdRemappings);
    }
    else if ( storageScope == Chk::Scope::Editor )
        ostr->remapStringIds(stringIdRemappings);
}

void Strings::set(std::unordered_map<SectionName, Section> & sections)
{
    sprp = GetSection<SprpSection>(sections, SectionName::SPRP);
    str = GetSection<StrSection>(sections, SectionName::STR);
    ostr = GetSection<OstrSection>(sections, SectionName::OSTR);
    kstr = GetSection<KstrSection>(sections, SectionName::KSTR);

    if ( str == nullptr )
        str = StrSection::GetDefault(true);
    if ( ostr == nullptr )
        ostr = OstrSection::GetDefault();
    if ( kstr == nullptr )
        kstr = KstrSection::GetDefault();
}

void Strings::clear()
{
    sprp = nullptr;
    str = nullptr;
    ostr = nullptr;
    kstr = nullptr;
}


Players::Players(bool useDefault) : strings(nullptr)
{
    if ( useDefault )
    {
        side = SideSection::GetDefault(); // Races
        colr = ColrSection::GetDefault(); // Player colors
        forc = ForcSection::GetDefault(); // Forces
        ownr = OwnrSection::GetDefault(); // Slot owners
        iown = IownSection::GetDefault(); // Redundant slot owners
        if ( iown == nullptr || ownr == nullptr || forc == nullptr || colr == nullptr || side == nullptr )
        {
            throw ScenarioAllocationFailure(
                iown == nullptr ? ChkSection::getNameString(SectionName::IOWN) :
                (ownr == nullptr ? ChkSection::getNameString(SectionName::OWNR) :
                (forc == nullptr ? ChkSection::getNameString(SectionName::FORC) :
                (colr == nullptr ? ChkSection::getNameString(SectionName::COLR) :
                ChkSection::getNameString(SectionName::SIDE)))));
        }
    }
}

bool Players::empty()
{
    return side == nullptr && colr == nullptr && forc == nullptr && ownr == nullptr && iown == nullptr;
}

Sc::Player::SlotType Players::getSlotType(size_t slotIndex, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: return ownr->getSlotType(slotIndex);
        case Chk::Scope::Editor: return iown->getSlotType(slotIndex);
        case Chk::Scope::EditorOverGame: return iown != nullptr ? iown->getSlotType(slotIndex) : ownr->getSlotType(slotIndex);
        default: return ownr != nullptr ? ownr->getSlotType(slotIndex) : iown->getSlotType(slotIndex);
    }
    return Sc::Player::SlotType::Inactive;
}

void Players::setSlotType(size_t slotIndex, Sc::Player::SlotType slotType, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: ownr->setSlotType(slotIndex, slotType); break;
        case Chk::Scope::Editor: iown->setSlotType(slotIndex, slotType); break;
        default: ownr->setSlotType(slotIndex, slotType); iown->setSlotType(slotIndex, slotType); break;
    }
}

Chk::Race Players::getPlayerRace(size_t playerIndex)
{
    return side->getPlayerRace(playerIndex);
}

void Players::setPlayerRace(size_t playerIndex, Chk::Race race)
{
    side->setPlayerRace(playerIndex, race);
}

Chk::PlayerColor Players::getPlayerColor(size_t slotIndex)
{
    return colr->getPlayerColor(slotIndex);
}

void Players::setPlayerColor(size_t slotIndex, Chk::PlayerColor color)
{
    colr->setPlayerColor(slotIndex, color);
}

Chk::Force Players::getPlayerForce(size_t slotIndex)
{
    return forc->getPlayerForce(slotIndex);
}

size_t Players::getForceStringId(Chk::Force force)
{
    return forc->getForceStringId(force);
}

u8 Players::getForceFlags(Chk::Force force)
{
    return forc->getForceFlags(force);
}

void Players::setPlayerForce(size_t slotIndex, Chk::Force force)
{
    forc->setPlayerForce(slotIndex, force);
}

void Players::setForceStringId(Chk::Force force, u16 forceStringId)
{
    forc->setForceStringId(force, forceStringId);
}

void Players::setForceFlags(Chk::Force force, u8 forceFlags)
{
    forc->setForceFlags(force, forceFlags);
}

bool Players::stringUsed(size_t stringId, u32 userMask)
{
    return (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force && forc->stringUsed(stringId);
}

void Players::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Force) == Chk::StringUserFlag::Force )
        forc->markUsedStrings(stringIdUsed);
}

void Players::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    forc->remapStringIds(stringIdRemappings);
}

void Players::deleteString(size_t stringId)
{
    forc->deleteString(stringId);
}

void Players::set(std::unordered_map<SectionName, Section> & sections)
{
    side = GetSection<SideSection>(sections, SectionName::SIDE);
    colr = GetSection<ColrSection>(sections, SectionName::COLR);
    forc = GetSection<ForcSection>(sections, SectionName::FORC);
    ownr = GetSection<OwnrSection>(sections, SectionName::OWNR);

    iown = GetSection<IownSection>(sections, SectionName::IOWN);

    if ( colr == nullptr )
        colr = ColrSection::GetDefault();
    if ( iown == nullptr )
        iown = IownSection::GetDefault();
}

void Players::clear()
{
    side = nullptr;
    colr = nullptr;
    forc = nullptr;
    ownr = nullptr;

    iown = nullptr;
}


Terrain::Terrain()
{

}

Terrain::Terrain(Sc::Terrain::Tileset tileset, u16 width, u16 height)
{
    era = EraSection::GetDefault(tileset); // Tileset
    dim = DimSection::GetDefault(width, height); // Dimensions
    mtxm = MtxmSection::GetDefault(width, height); // Real terrain data
    tile = TileSection::GetDefault(width, height); // Intermediate terrain data
    isom = IsomSection::GetDefault(width, height); // Isometric terrain data
    if ( isom == nullptr || tile == nullptr || mtxm == nullptr || dim == nullptr || era == nullptr )
    {
        throw ScenarioAllocationFailure(
            isom == nullptr ? ChkSection::getNameString(SectionName::ISOM) :
            (tile == nullptr ? ChkSection::getNameString(SectionName::TILE) :
            (mtxm == nullptr ? ChkSection::getNameString(SectionName::MTXM) :
            (dim == nullptr ? ChkSection::getNameString(SectionName::DIM) :
            ChkSection::getNameString(SectionName::ERA)))));
    }
}

bool Terrain::empty()
{
    return era == nullptr && dim == nullptr && mtxm == nullptr && tile == nullptr && isom == nullptr;
}

Sc::Terrain::Tileset Terrain::getTileset()
{
    return era->getTileset();
}

void Terrain::setTileset(Sc::Terrain::Tileset tileset)
{
    era->setTileset(tileset);
}

size_t Terrain::getTileWidth()
{
    return dim->getTileWidth();
}

size_t Terrain::getTileHeight()
{
    return dim->getTileHeight();
}

size_t Terrain::getPixelWidth()
{
    return dim->getPixelWidth();
}

size_t Terrain::getPixelHeight()
{
    return dim->getPixelHeight();
}

void Terrain::setTileWidth(u16 newTileWidth, s32 leftEdge)
{
    u16 tileWidth = (u16)dim->getTileWidth();
    u16 tileHeight = (u16)dim->getTileHeight();
    isom->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    tile->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    mtxm->setDimensions(newTileWidth, tileHeight, tileWidth, tileHeight, leftEdge, 0);
    dim->setTileWidth(tileWidth);
}

void Terrain::setTileHeight(u16 newTileHeight, s32 topEdge)
{
    u16 tileWidth = (u16)dim->getTileWidth();
    u16 tileHeight = (u16)dim->getTileHeight();
    isom->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    tile->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    mtxm->setDimensions(tileWidth, newTileHeight, tileWidth, tileHeight, 0, topEdge);
    dim->setTileHeight(tileHeight);
}

void Terrain::setDimensions(u16 newTileWidth, u16 newTileHeight, s32 leftEdge, s32 topEdge)
{
    u16 tileWidth = (u16)dim->getTileWidth();
    u16 tileHeight = (u16)dim->getTileHeight();
    isom->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    tile->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    mtxm->setDimensions(newTileWidth, newTileHeight, tileWidth, tileHeight, leftEdge, topEdge);
    dim->setDimensions(newTileWidth, newTileHeight);
}

u16 Terrain::getTile(size_t tileXc, size_t tileYc, Chk::Scope scope)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileYc*tileWidth + tileXc;
    switch ( scope )
    {
        case Chk::Scope::Game: return mtxm->getTile(tileIndex);
        case Chk::Scope::Editor: return tile->getTile(tileIndex);
        case Chk::Scope::EditorOverGame: return tile != nullptr ? tile->getTile(tileIndex) : mtxm->getTile(tileIndex);
        default: return mtxm != nullptr ? mtxm->getTile(tileIndex) : tile->getTile(tileIndex);
    }
    return 0;
}

inline u16 Terrain::getTilePx(size_t pixelXc, size_t pixelYc, Chk::Scope scope)
{
    return getTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, scope);
}

void Terrain::setTile(size_t tileXc, size_t tileYc, u16 tileValue, Chk::Scope scope)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileYc*tileWidth + tileXc;
    switch ( scope )
    {
        case Chk::Scope::Game: mtxm->setTile(tileIndex, tileValue); break;
        case Chk::Scope::Editor: tile->setTile(tileIndex, tileValue); break;
        default: mtxm->setTile(tileIndex, tileValue); tile->setTile(tileIndex, tileValue); break;
    }
}

inline void Terrain::setTilePx(size_t pixelXc, size_t pixelYc, u16 tileValue, Chk::Scope scope)
{
    setTile(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, tileValue, scope);
}

Chk::IsomEntry & Terrain::getIsomEntry(size_t isomIndex)
{
    return isom->getIsomEntry(isomIndex);
}

void Terrain::set(std::unordered_map<SectionName, Section> & sections)
{
    era = GetSection<EraSection>(sections, SectionName::ERA);
    dim = GetSection<DimSection>(sections, SectionName::DIM);
    mtxm = GetSection<MtxmSection>(sections, SectionName::MTXM);
    tile = GetSection<TileSection>(sections, SectionName::TILE);

    isom = GetSection<IsomSection>(sections, SectionName::ISOM);

    if ( dim != nullptr )
    {
        if ( tile == nullptr )
            tile = TileSection::GetDefault((u16)dim->getTileWidth(), (u16)dim->getTileHeight());
        if ( isom == nullptr )
            isom = IsomSection::GetDefault((u16)dim->getTileWidth(), (u16)dim->getTileHeight());
    }
}

void Terrain::clear()
{
    era = nullptr;
    dim = nullptr;
    mtxm = nullptr;
    tile = nullptr;

    isom = nullptr;
}


Layers::Layers() : Terrain(), strings(nullptr)
{

}

Layers::Layers(Sc::Terrain::Tileset tileset, u16 width, u16 height) : Terrain(tileset, width, height), strings(nullptr)
{
    mask = MaskSection::GetDefault(width, height); // Fog of war
    thg2 = Thg2Section::GetDefault(); // Sprites
    dd2 = Dd2Section::GetDefault(); // Doodads
    unit = UnitSection::GetDefault(); // Units
    mrgn = MrgnSection::GetDefault(width, height); // Locations
    if ( mrgn == nullptr || unit == nullptr || dd2 == nullptr || thg2 == nullptr || mask == nullptr )
    {
        throw ScenarioAllocationFailure(
            mrgn == nullptr ? ChkSection::getNameString(SectionName::MRGN) :
            (unit == nullptr ? ChkSection::getNameString(SectionName::UNIT) :
            (dd2 == nullptr ? ChkSection::getNameString(SectionName::DD2) :
            (thg2 == nullptr ? ChkSection::getNameString(SectionName::THG2) :
            ChkSection::getNameString(SectionName::MASK)))));
    }
}

bool Layers::empty()
{
    return Terrain::empty() && mask == nullptr && thg2 == nullptr && dd2 == nullptr && unit == nullptr & mrgn == nullptr;
}

void Layers::setTileWidth(u16 tileWidth, u16 sizeValidationFlags, s32 leftEdge)
{
    Terrain::setTileWidth(tileWidth, leftEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::setTileHeight(u16 tileHeight, u16 sizeValidationFlags, s32 topEdge)
{
    Terrain::setTileHeight(tileHeight, topEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::setDimensions(u16 tileWidth, u16 tileHeight, u16 sizeValidationFlags, s32 leftEdge, s32 topEdge)
{
    Terrain::setDimensions(tileWidth, tileHeight, leftEdge, topEdge);
    validateSizes(sizeValidationFlags);
}

void Layers::validateSizes(u16 sizeValidationFlags)
{
    bool updateAnywhereIfAlreadyStandard = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhereIfAlreadyStandard) == SizeValidationFlag::UpdateAnywhereIfAlreadyStandard;
    bool updateAnywhere = (sizeValidationFlags & SizeValidationFlag::UpdateAnywhere) == SizeValidationFlag::UpdateAnywhere;
    if ( (!updateAnywhereIfAlreadyStandard && updateAnywhere) || (updateAnywhereIfAlreadyStandard && anywhereIsStandardDimensions()) )
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

u8 Layers::getFog(size_t tileXc, size_t tileYc)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileWidth*tileYc + tileXc;
    return mask->getFog(tileIndex);
}

u8 Layers::getFogPx(size_t pixelXc, size_t pixelYc)
{
    return getFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile);
}

void Layers::setFog(size_t tileXc, size_t tileYc, u8 fogOfWarPlayers)
{
    size_t tileWidth = dim->getTileWidth();
    size_t tileIndex = tileWidth*tileYc + tileXc;
    mask->setFog(tileIndex, fogOfWarPlayers);
}

void Layers::setFogPx(size_t pixelXc, size_t pixelYc, u8 fogOfWarPlayers)
{
    setFog(pixelXc / Sc::Terrain::PixelsPerTile, pixelYc / Sc::Terrain::PixelsPerTile, fogOfWarPlayers);
}

size_t Layers::numSprites()
{
    return thg2->numSprites();
}

std::shared_ptr<Chk::Sprite> Layers::getSprite(size_t spriteIndex)
{
    return thg2->getSprite(spriteIndex);
}

size_t Layers::addSprite(std::shared_ptr<Chk::Sprite> sprite)
{
    return thg2->addSprite(sprite);
}

void Layers::insertSprite(size_t spriteIndex, std::shared_ptr<Chk::Sprite> sprite)
{
    thg2->insertSprite(spriteIndex, sprite);
}

void Layers::deleteSprite(size_t spriteIndex)
{
    thg2->deleteSprite(spriteIndex);
}

void Layers::moveSprite(size_t spriteIndexFrom, size_t spriteIndexTo)
{
    thg2->moveSprite(spriteIndexFrom, spriteIndexTo);
}

void Layers::updateOutOfBoundsSprites()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numSprites = thg2->numSprites();
    for ( size_t i=0; i<numSprites; i++ )
    {
        std::shared_ptr<Chk::Sprite> sprite = thg2->getSprite(i);

        if ( sprite->xc >= pixelWidth )
            sprite->xc = u16(pixelWidth-1);

        if ( sprite->yc >= pixelHeight )
            sprite->yc = u16(pixelHeight-1);
    }
}

void Layers::removeOutOfBoundsSprites()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numSprites = thg2->numSprites();
    for ( size_t i = numSprites-1; i < numSprites; i-- )
    {
        std::shared_ptr<Chk::Sprite> sprite = thg2->getSprite(i);
        if ( sprite->xc >= pixelWidth || sprite->yc >= pixelHeight )
            thg2->deleteSprite(i);
    }
}

size_t Layers::numDoodads()
{
    return dd2->numDoodads();
}

std::shared_ptr<Chk::Doodad> Layers::getDoodad(size_t doodadIndex)
{
    return dd2->getDoodad(doodadIndex);
}

size_t Layers::addDoodad(std::shared_ptr<Chk::Doodad> doodad)
{
    return dd2->addDoodad(doodad);
}

void Layers::insertDoodad(size_t doodadIndex, std::shared_ptr<Chk::Doodad> doodad)
{
    return dd2->insertDoodad(doodadIndex, doodad);
}

void Layers::deleteDoodad(size_t doodadIndex)
{
    return dd2->deleteDoodad(doodadIndex);
}

void Layers::moveDoodad(size_t doodadIndexFrom, size_t doodadIndexTo)
{
    return dd2->moveDoodad(doodadIndexFrom, doodadIndexTo);
}

void Layers::removeOutOfBoundsDoodads()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numDoodads = dd2->numDoodads();
    for ( size_t i = numDoodads-1; i < numDoodads; i-- )
    {
        std::shared_ptr<Chk::Doodad> doodad = dd2->getDoodad(i);
        if ( doodad->xc >= pixelWidth || doodad->yc >= pixelHeight )
            dd2->deleteDoodad(i);
    }
}

size_t Layers::numUnits()
{
    return unit->numUnits();
}

std::shared_ptr<Chk::Unit> Layers::getUnit(size_t unitIndex)
{
    return unit->getUnit(unitIndex);
}

size_t Layers::addUnit(std::shared_ptr<Chk::Unit> unit)
{
    return this->unit->addUnit(unit);
}

void Layers::insertUnit(size_t unitIndex, std::shared_ptr<Chk::Unit> unit)
{
    return this->unit->insertUnit(unitIndex, unit);
}

void Layers::deleteUnit(size_t unitIndex)
{
    return unit->deleteUnit(unitIndex);
}

void Layers::moveUnit(size_t unitIndexFrom, size_t unitIndexTo)
{
    return unit->moveUnit(unitIndexFrom, unitIndexTo);
}

void Layers::updateOutOfBoundsUnits()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numUnits = unit->numUnits();
    for ( size_t i=0; i<numUnits; i++ )
    {
        std::shared_ptr<Chk::Unit> currUnit = unit->getUnit(i);

        if ( currUnit->xc >= pixelWidth )
            currUnit->xc = u16(pixelWidth-1);

        if ( currUnit->yc >= pixelHeight )
            currUnit->yc = u16(pixelHeight-1);
    }
}

void Layers::removeOutOfBoundsUnits()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numUnits = unit->numUnits();
    for ( size_t i = numUnits-1; i < numUnits; i-- )
    {
        std::shared_ptr<Chk::Unit> currUnit = unit->getUnit(i);
        if ( currUnit->xc >= pixelWidth || currUnit->yc >= pixelHeight )
            unit->deleteUnit(i);
    }
}

size_t Layers::numLocations()
{
    return mrgn->numLocations();
}

std::shared_ptr<Chk::Location> Layers::getLocation(size_t locationId)
{
    return mrgn->getLocation(locationId);
}

size_t Layers::addLocation(std::shared_ptr<Chk::Location> location)
{
    return mrgn->addLocation(location);
}

void Layers::replaceLocation(size_t locationId, std::shared_ptr<Chk::Location> location)
{
    mrgn->replaceLocation(locationId, location);
}

void Layers::deleteLocation(size_t locationId, bool deleteOnlyIfUnused)
{
    if ( !deleteOnlyIfUnused || !triggers->locationUsed(locationId) )
        mrgn->deleteLocation(locationId);
}

bool Layers::moveLocation(size_t locationIdFrom, size_t locationIdTo, bool lockAnywhere)
{
    return mrgn->moveLocation(locationIdFrom, locationIdTo, lockAnywhere);
}

bool Layers::isBlank(size_t locationId)
{
    return mrgn->isBlank(locationId);
}

void Layers::downsizeOutOfBoundsLocations()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numLocations = mrgn->numLocations();
    for ( size_t i=1; i<=numLocations; i++ )
    {
        std::shared_ptr<Chk::Location> location = mrgn->getLocation(i);

        if ( location->left >= pixelWidth )
            location->left = u32(pixelWidth-1);

        if ( location->right >= pixelWidth )
            location->right = u32(pixelWidth-1);

        if ( location->top >= pixelHeight )
            location->top = u32(pixelHeight-1);

        if ( location->bottom >= pixelHeight )
            location->bottom = u32(pixelHeight-1);
    }
}

bool Layers::locationsFitOriginal(bool lockAnywhere, bool autoDefragment)
{
    return mrgn->locationsFitOriginal(*triggers, lockAnywhere, autoDefragment);
}

bool Layers::trimLocationsToOriginal(bool lockAnywhere, bool autoDefragment)
{
    return mrgn->trimToOriginal(*triggers, lockAnywhere, autoDefragment);
}

void Layers::expandToScHybridOrExpansion()
{
    mrgn->expandToScHybridOrExpansion();
}

bool Layers::anywhereIsStandardDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation(Chk::LocationId::Anywhere);
    return anywhere != nullptr && anywhere->left == 0 && anywhere->top == 0 && anywhere->right == dim->getPixelWidth() && anywhere->bottom == dim->getPixelHeight();
}

void Layers::matchAnywhereToDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation(Chk::LocationId::Anywhere);
    if ( anywhere != nullptr )
    {
        anywhere->left = 0;
        anywhere->top = 0;
        anywhere->right = (u32)dim->getPixelWidth();
        anywhere->bottom = (u32)dim->getPixelHeight();
    }
}

bool Layers::stringUsed(size_t stringId, Chk::Scope storageScope, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
    {
        switch ( storageScope )
        {
            case Chk::Scope::Either:
            case Chk::Scope::EditorOverGame:
            case Chk::Scope::GameOverEditor: return mrgn->stringUsed(stringId) || strings->ostr->stringUsed(stringId, Chk::StringUserFlag::Location);
            case Chk::Scope::Game: return mrgn->stringUsed(stringId);
            case Chk::Scope::Editor: return strings->ostr->stringUsed(stringId, Chk::StringUserFlag::Location);
            default: return false;
        }
    }
    return false;
}

void Layers::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Location) == Chk::StringUserFlag::Location )
        mrgn->markUsedStrings(stringIdUsed);
}

void Layers::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    mrgn->remapStringIds(stringIdRemappings);
}

void Layers::deleteString(size_t stringId)
{
    mrgn->deleteString(stringId);
}

void Layers::set(std::unordered_map<SectionName, Section> & sections)
{
    Terrain::set(sections);
    mask = GetSection<MaskSection>(sections, SectionName::MASK);
    thg2 = GetSection<Thg2Section>(sections, SectionName::THG2);
    dd2 = GetSection<Dd2Section>(sections, SectionName::DD2);
    unit = GetSection<UnitSection>(sections, SectionName::UNIT);

    mrgn = GetSection<MrgnSection>(sections, SectionName::MRGN);

    if ( Terrain::dim != nullptr )
    {
        if ( mask == nullptr )
            mask = MaskSection::GetDefault((u16)dim->getTileWidth(), (u16)dim->getTileHeight());
        if ( mrgn == nullptr )
            mrgn = MrgnSection::GetDefault((u16)dim->getTileWidth(), (u16)dim->getTileHeight());
    }
    if ( dd2 == nullptr )
        dd2 = Dd2Section::GetDefault();
}

void Layers::clear()
{
    Terrain::clear();
    mask = nullptr;
    thg2 = nullptr;
    dd2 = nullptr;
    unit = nullptr;

    mrgn = nullptr;
}


Properties::Properties(bool useDefault) : versions(nullptr), strings(nullptr)
{
    if ( useDefault )
    {
        unis = UnisSection::GetDefault(); // Unit settings
        unix = UnixSection::GetDefault(); // Expansion Unit Settings
        puni = PuniSection::GetDefault(); // Unit availability
        upgs = UpgsSection::GetDefault(); // Upgrade costs
        upgx = UpgxSection::GetDefault(); // Expansion upgrade costs
        upgr = UpgrSection::GetDefault(); // Upgrade leveling
        pupx = PupxSection::GetDefault(); // Expansion upgrade leveling
        tecs = TecsSection::GetDefault(); // Technology costs
        tecx = TecxSection::GetDefault(); // Expansion technology costs
        ptec = PtecSection::GetDefault(); // Technology availability
        ptex = PtexSection::GetDefault(); // Expansion technology availability
        if ( unis == nullptr || unix == nullptr || puni == nullptr || upgs == nullptr || upgx == nullptr || upgr == nullptr || pupx == nullptr ||
            tecs == nullptr || tecx == nullptr || ptec == nullptr || ptex == nullptr )
        {
            throw ScenarioAllocationFailure(
                unis == nullptr ? ChkSection::getNameString(SectionName::UNIS) :
                (unix == nullptr ? ChkSection::getNameString(SectionName::UNIx) :
                (puni == nullptr ? ChkSection::getNameString(SectionName::PUNI) :
                (upgs == nullptr ? ChkSection::getNameString(SectionName::UPGS) :
                (upgx == nullptr ? ChkSection::getNameString(SectionName::UPGx) :
                (upgr == nullptr ? ChkSection::getNameString(SectionName::UPGR) :
                (pupx == nullptr ? ChkSection::getNameString(SectionName::PUPx) :
                (tecs == nullptr ? ChkSection::getNameString(SectionName::TECS) :
                (tecx == nullptr ? ChkSection::getNameString(SectionName::TECx) :
                (ptec == nullptr ? ChkSection::getNameString(SectionName::PTEC) :
                ChkSection::getNameString(SectionName::PTEx)))))))))));
        }
    }
}

bool Properties::empty()
{
    return unis == nullptr && unix == nullptr && puni == nullptr && upgs == nullptr && upgx == nullptr &&
        upgr == nullptr && pupx == nullptr && tecs == nullptr && tecx == nullptr & ptec == nullptr && ptex == nullptr;
}

bool Properties::useExpansionUnitSettings(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return unis != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable: 
        default: return unix != nullptr ? true : false;
    }
    return true;
}

bool Properties::unitUsesDefaultSettings(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->unitUsesDefaultSettings(unitType) : unis->unitUsesDefaultSettings(unitType);
}

u32 Properties::getUnitHitpoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitHitpoints(unitType) : unis->getUnitHitpoints(unitType);
}

u16 Properties::getUnitShieldPoints(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitShieldPoints(unitType) : unis->getUnitShieldPoints(unitType);
}

u8 Properties::getUnitArmorLevel(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitArmorLevel(unitType) : unis->getUnitArmorLevel(unitType);
}

u16 Properties::getUnitBuildTime(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitBuildTime(unitType) : unis->getUnitBuildTime(unitType);
}

u16 Properties::getUnitMineralCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitMineralCost(unitType) : unis->getUnitMineralCost(unitType);
}

u16 Properties::getUnitGasCost(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitGasCost(unitType) : unis->getUnitGasCost(unitType);
}

size_t Properties::getUnitNameStringId(Sc::Unit::Type unitType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getUnitNameStringId(unitType) : unis->getUnitNameStringId(unitType);
}

u16 Properties::getWeaponBaseDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getWeaponBaseDamage(weaponType) : unis->getWeaponBaseDamage(weaponType);
}

u16 Properties::getWeaponUpgradeDamage(Sc::Weapon::Type weaponType, Chk::UseExpSection useExp)
{
    return useExpansionUnitSettings(useExp) ? unix->getWeaponUpgradeDamage(weaponType) : unis->getWeaponUpgradeDamage(weaponType);
}

void Properties::setUnitUsesDefaultSettings(Sc::Unit::Type unitType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitUsesDefaultSettings(unitType, useDefault); unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::Yes: unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::No: unis->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitUsesDefaultSettings(unitType, useDefault) : unis->setUnitUsesDefaultSettings(unitType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitUsesDefaultSettings(unitType, useDefault) : unix->setUnitUsesDefaultSettings(unitType, useDefault); break;
    }
}

void Properties::setUnitHitpoints(Sc::Unit::Type unitType, u32 hitpoints, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitHitpoints(unitType, hitpoints); unix->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::Yes: unix->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::No: unis->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitHitpoints(unitType, hitpoints) : unis->setUnitHitpoints(unitType, hitpoints); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitHitpoints(unitType, hitpoints) : unix->setUnitHitpoints(unitType, hitpoints); break;
    }
}

void Properties::setUnitShieldPoints(Sc::Unit::Type unitType, u16 shieldPoints, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitShieldPoints(unitType, shieldPoints); unix->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::Yes: unix->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::No: unis->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitShieldPoints(unitType, shieldPoints) : unis->setUnitShieldPoints(unitType, shieldPoints); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitShieldPoints(unitType, shieldPoints) : unix->setUnitShieldPoints(unitType, shieldPoints); break;
    }
}

void Properties::setUnitArmorLevel(Sc::Unit::Type unitType, u8 armorLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitArmorLevel(unitType, armorLevel); unix->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::Yes: unix->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::No: unis->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitArmorLevel(unitType, armorLevel) : unis->setUnitArmorLevel(unitType, armorLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitArmorLevel(unitType, armorLevel) : unix->setUnitArmorLevel(unitType, armorLevel); break;
    }
}

void Properties::setUnitBuildTime(Sc::Unit::Type unitType, u16 buildTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitBuildTime(unitType, buildTime); unix->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::Yes: unix->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::No: unis->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitBuildTime(unitType, buildTime) : unis->setUnitBuildTime(unitType, buildTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitBuildTime(unitType, buildTime) : unix->setUnitBuildTime(unitType, buildTime); break;
    }
}

void Properties::setUnitMineralCost(Sc::Unit::Type unitType, u16 mineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitMineralCost(unitType, mineralCost); unix->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::Yes: unix->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::No: unis->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitMineralCost(unitType, mineralCost) : unis->setUnitMineralCost(unitType, mineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitMineralCost(unitType, mineralCost) : unix->setUnitMineralCost(unitType, mineralCost); break;
    }
}

void Properties::setUnitGasCost(Sc::Unit::Type unitType, u16 gasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitGasCost(unitType, gasCost); unix->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::Yes: unix->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::No: unis->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitGasCost(unitType, gasCost) : unis->setUnitGasCost(unitType, gasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitGasCost(unitType, gasCost) : unix->setUnitGasCost(unitType, gasCost); break;
    }
}

void Properties::setUnitNameStringId(Sc::Unit::Type unitType, size_t nameStringId, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: unis->setUnitNameStringId(unitType, (u16)nameStringId); unix->setUnitNameStringId(unitType, (u16)nameStringId); break;
        case Chk::UseExpSection::Yes: unix->setUnitNameStringId(unitType, (u16)nameStringId); break;
        case Chk::UseExpSection::No: unis->setUnitNameStringId(unitType, (u16)nameStringId); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setUnitNameStringId(unitType, (u16)nameStringId) : unis->setUnitNameStringId(unitType, (u16)nameStringId); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setUnitNameStringId(unitType, (u16)nameStringId) : unix->setUnitNameStringId(unitType, (u16)nameStringId); break;
    }
}

void Properties::setWeaponBaseDamage(Sc::Weapon::Type weaponType, u16 baseDamage, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType < Sc::Weapon::TotalOriginal && unis != nullptr )
                unis->setWeaponBaseDamage(weaponType, baseDamage);
            if ( unix != nullptr )
                unix->setWeaponBaseDamage(weaponType, baseDamage);
            break;
        case Chk::UseExpSection::Both: unis->setWeaponBaseDamage(weaponType, baseDamage); unix->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::Yes: unix->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::No: unis->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setWeaponBaseDamage(weaponType, baseDamage) : unis->setWeaponBaseDamage(weaponType, baseDamage); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setWeaponBaseDamage(weaponType, baseDamage) : unix->setWeaponBaseDamage(weaponType, baseDamage); break;
    }
}

void Properties::setWeaponUpgradeDamage(Sc::Weapon::Type weaponType, u16 upgradeDamage, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( weaponType < Sc::Weapon::TotalOriginal && unis != nullptr )
                unis->setWeaponUpgradeDamage(weaponType, upgradeDamage);
            if ( unix != nullptr )
                unix->setWeaponUpgradeDamage(weaponType, upgradeDamage);
            break;
        case Chk::UseExpSection::Both: unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::Yes: unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::No: unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::YesIfAvailable: unix != nullptr ? unix->setWeaponUpgradeDamage(weaponType, upgradeDamage) : unis->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
        case Chk::UseExpSection::NoIfOrigAvailable: unis != nullptr ? unis->setWeaponUpgradeDamage(weaponType, upgradeDamage) : unix->setWeaponUpgradeDamage(weaponType, upgradeDamage); break;
    }
}

bool Properties::isUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex)
{
    return puni->isUnitBuildable(unitType, playerIndex);
}

bool Properties::isUnitDefaultBuildable(Sc::Unit::Type unitType)
{
    return puni->isUnitDefaultBuildable(unitType);
}

bool Properties::playerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex)
{
    return puni->playerUsesDefault(unitType, playerIndex);
}

void Properties::setUnitBuildable(Sc::Unit::Type unitType, size_t playerIndex, bool buildable)
{
    puni->setUnitBuildable(unitType, playerIndex, buildable);
}

void Properties::setUnitDefaultBuildable(Sc::Unit::Type unitType, bool buildable)
{
    puni->setUnitDefaultBuildable(unitType, buildable);
}

void Properties::setPlayerUsesDefaultUnitBuildability(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    puni->setPlayerUsesDefault(unitType, playerIndex, useDefault);
}

void Properties::setUnitsToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            *unis = *UnisSection::GetDefault();
            *unix = *UnixSection::GetDefault();
            *puni = *PuniSection::GetDefault();
            break;
        case Chk::UseExpSection::Yes:
            *unix = *UnixSection::GetDefault();
            *puni = *PuniSection::GetDefault();
            break;
        case Chk::UseExpSection::No:
            *unis = *UnisSection::GetDefault();
            *puni = *PuniSection::GetDefault();
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( unix != nullptr )
                *unix = *UnixSection::GetDefault();
            else
                *unis = *UnisSection::GetDefault();
            
            *puni = *PuniSection::GetDefault();
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( unis != nullptr )
                *unis = *UnisSection::GetDefault();
            else
                *unix = *UnixSection::GetDefault();
            
            *puni = *PuniSection::GetDefault();
            break;
    }
}

bool Properties::useExpansionUpgradeCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return upgs != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return upgx != nullptr ? true : false;
    }
}

bool Properties::upgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->upgradeUsesDefaultCosts(upgradeType) : upgs->upgradeUsesDefaultCosts(upgradeType);
}

u16 Properties::getUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseMineralCost(upgradeType) : upgs->getBaseMineralCost(upgradeType);
}

u16 Properties::getUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getMineralCostFactor(upgradeType) : upgs->getMineralCostFactor(upgradeType);
}

u16 Properties::getUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseGasCost(upgradeType) : upgs->getBaseGasCost(upgradeType);
}

u16 Properties::getUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getGasCostFactor(upgradeType) : upgs->getGasCostFactor(upgradeType);
}

u16 Properties::getUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseResearchTime(upgradeType) : upgs->getBaseResearchTime(upgradeType);
}

u16 Properties::getUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getResearchTimeFactor(upgradeType) : upgs->getResearchTimeFactor(upgradeType);
}

void Properties::setUpgradeUsesDefaultCosts(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setUpgradeUsesDefaultCosts(upgradeType, useDefault);
            if ( upgx != nullptr )
                upgx->setUpgradeUsesDefaultCosts(upgradeType, useDefault);
            break;
        case Chk::UseExpSection::Both: upgs->setUpgradeUsesDefaultCosts(upgradeType, useDefault); upgx->setUpgradeUsesDefaultCosts(upgradeType, useDefault); break;
        case Chk::UseExpSection::Yes: upgx->setUpgradeUsesDefaultCosts(upgradeType, useDefault); break;
        case Chk::UseExpSection::No: upgs->setUpgradeUsesDefaultCosts(upgradeType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setUpgradeUsesDefaultCosts(upgradeType, useDefault) : upgs->setUpgradeUsesDefaultCosts(upgradeType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setUpgradeUsesDefaultCosts(upgradeType, useDefault) : upgx->setUpgradeUsesDefaultCosts(upgradeType, useDefault); break;
    }
}

void Properties::setUpgradeBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setBaseMineralCost(upgradeType, baseMineralCost);
            if ( upgx != nullptr )
                upgx->setBaseMineralCost(upgradeType, baseMineralCost);
            break;
        case Chk::UseExpSection::Both: upgs->setBaseMineralCost(upgradeType, baseMineralCost); upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::Yes: upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::No: upgs->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseMineralCost(upgradeType, baseMineralCost) : upgs->setBaseMineralCost(upgradeType, baseMineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseMineralCost(upgradeType, baseMineralCost) : upgx->setBaseMineralCost(upgradeType, baseMineralCost); break;
    }
}

void Properties::setUpgradeMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setMineralCostFactor(upgradeType, mineralCostFactor);
            if ( upgx != nullptr )
                upgx->setMineralCostFactor(upgradeType, mineralCostFactor);
            break;
        case Chk::UseExpSection::Both: upgs->setMineralCostFactor(upgradeType, mineralCostFactor); upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::Yes: upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::No: upgs->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setMineralCostFactor(upgradeType, mineralCostFactor) : upgs->setMineralCostFactor(upgradeType, mineralCostFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setMineralCostFactor(upgradeType, mineralCostFactor) : upgx->setMineralCostFactor(upgradeType, mineralCostFactor); break;
    }
}

void Properties::setUpgradeBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setBaseGasCost(upgradeType, baseGasCost);
            if ( upgx != nullptr )
                upgx->setBaseGasCost(upgradeType, baseGasCost);
            break;
        case Chk::UseExpSection::Both: upgs->setBaseGasCost(upgradeType, baseGasCost); upgx->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::Yes: upgx->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::No: upgs->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseGasCost(upgradeType, baseGasCost) : upgs->setBaseGasCost(upgradeType, baseGasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseGasCost(upgradeType, baseGasCost) : upgx->setBaseGasCost(upgradeType, baseGasCost); break;
    }
}

void Properties::setUpgradeGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setGasCostFactor(upgradeType, gasCostFactor);
            if ( upgx != nullptr )
                upgx->setGasCostFactor(upgradeType, gasCostFactor);
            break;
        case Chk::UseExpSection::Both: upgs->setGasCostFactor(upgradeType, gasCostFactor); upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::Yes: upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::No: upgs->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setGasCostFactor(upgradeType, gasCostFactor) : upgs->setGasCostFactor(upgradeType, gasCostFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setGasCostFactor(upgradeType, gasCostFactor) : upgx->setGasCostFactor(upgradeType, gasCostFactor); break;
    }
}

void Properties::setUpgradeBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setBaseResearchTime(upgradeType, baseResearchTime);
            if ( upgx != nullptr )
                upgx->setBaseResearchTime(upgradeType, baseResearchTime);
            break;
        case Chk::UseExpSection::Both: upgs->setBaseResearchTime(upgradeType, baseResearchTime); upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::Yes: upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::No: upgs->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setBaseResearchTime(upgradeType, baseResearchTime) : upgs->setBaseResearchTime(upgradeType, baseResearchTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setBaseResearchTime(upgradeType, baseResearchTime) : upgx->setBaseResearchTime(upgradeType, baseResearchTime); break;
    }
}

void Properties::setUpgradeResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgs != nullptr )
                upgs->setResearchTimeFactor(upgradeType, researchTimeFactor);
            if ( upgx != nullptr )
                upgx->setResearchTimeFactor(upgradeType, researchTimeFactor);
            break;
        case Chk::UseExpSection::Both: upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::Yes: upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::No: upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setResearchTimeFactor(upgradeType, researchTimeFactor) : upgs->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setResearchTimeFactor(upgradeType, researchTimeFactor) : upgx->setResearchTimeFactor(upgradeType, researchTimeFactor); break;
    }
}

bool Properties::useExpansionUpgradeLeveling(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return upgr != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return pupx != nullptr ? true : false;
    }
}

size_t Properties::getMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getMaxUpgradeLevel(upgradeType, playerIndex) : upgr->getMaxUpgradeLevel(upgradeType, playerIndex);
}

size_t Properties::getStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getStartUpgradeLevel(upgradeType, playerIndex) : upgr->getStartUpgradeLevel(upgradeType, playerIndex);
}

size_t Properties::getDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getDefaultMaxUpgradeLevel(upgradeType) : upgr->getDefaultMaxUpgradeLevel(upgradeType);
}

size_t Properties::getDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->getDefaultStartUpgradeLevel(upgradeType) : upgr->getDefaultStartUpgradeLevel(upgradeType);
}

bool Properties::playerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeLeveling(useExp) ? pupx->playerUsesDefault(upgradeType, playerIndex) : upgr->playerUsesDefault(upgradeType, playerIndex);
}

void Properties::setMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgr != nullptr )
                upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel);
            if ( pupx != nullptr )
                pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel) : upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel) : pupx->setMaxUpgradeLevel(upgradeType, playerIndex, maxUpgradeLevel); break;
    }
}

void Properties::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgr != nullptr )
                upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel);
            if ( pupx != nullptr )
                pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel) : upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel) : pupx->setStartUpgradeLevel(upgradeType, playerIndex, startUpgradeLevel); break;
    }
}

void Properties::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgr != nullptr )
                upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel);
            if ( pupx != nullptr )
                pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel) : upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel) : pupx->setDefaultMaxUpgradeLevel(upgradeType, maxUpgradeLevel); break;
    }
}

void Properties::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgr != nullptr )
                upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel);
            if ( pupx != nullptr )
                pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel);
            break;
        case Chk::UseExpSection::Both: upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel) : upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel) : pupx->setDefaultStartUpgradeLevel(upgradeType, startUpgradeLevel); break;
    }
}

void Properties::setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( upgradeType < Sc::Upgrade::TotalOriginalTypes && upgr != nullptr )
                upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault);
            if ( pupx != nullptr )
                pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault);
            break;
        case Chk::UseExpSection::Both: upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::Yes: pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::No: upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault) : upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setPlayerUsesDefault(upgradeType, playerIndex, useDefault) : pupx->setPlayerUsesDefault(upgradeType, playerIndex, useDefault); break;
    }
}

void Properties::setUpgradesToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            *upgs = *UpgsSection::GetDefault();
            *upgx = *UpgxSection::GetDefault();
            *upgr = *UpgrSection::GetDefault();
            *pupx = *PupxSection::GetDefault();
            break;
        case Chk::UseExpSection::Yes:
            *upgx = *UpgxSection::GetDefault();
            *pupx = *PupxSection::GetDefault();
            break;
        case Chk::UseExpSection::No:
            *upgs = *UpgsSection::GetDefault();
            *upgr = *UpgrSection::GetDefault();
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( upgx != nullptr )
                *upgx = *UpgxSection::GetDefault();
            else
                *upgs = *UpgsSection::GetDefault();

            if ( pupx != nullptr )
                *pupx = *PupxSection::GetDefault();
            else
                *upgr = *UpgrSection::GetDefault();
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( upgs != nullptr )
                *upgs = *UpgsSection::GetDefault();
            else
                *upgx = *UpgxSection::GetDefault();

            if ( upgr != nullptr )
                *upgr = *UpgrSection::GetDefault();
            else
                *pupx = *PupxSection::GetDefault();
            break;
    }
}

bool Properties::useExpansionTechCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return tecs != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return tecx != nullptr ? true : false;
    }
}

bool Properties::techUsesDefaultSettings(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->techUsesDefaultSettings(techType) : tecs->techUsesDefaultSettings(techType);
}

u16 Properties::getTechMineralCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechMineralCost(techType) : tecs->getTechMineralCost(techType);
}

u16 Properties::getTechGasCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechGasCost(techType) : tecs->getTechGasCost(techType);
}

u16 Properties::getTechResearchTime(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechResearchTime(techType) : tecs->getTechResearchTime(techType);
}

u16 Properties::getTechEnergyCost(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechCosts(useExp) ? tecx->getTechEnergyCost(techType) : tecs->getTechEnergyCost(techType);
}

void Properties::setTechUsesDefaultSettings(Sc::Tech::Type techType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && tecs != nullptr )
                tecs->setTechUsesDefaultSettings(techType, useDefault);
            if ( tecx != nullptr )
                tecx->setTechUsesDefaultSettings(techType, useDefault);
            break;
        case Chk::UseExpSection::Both: tecs->setTechUsesDefaultSettings(techType, useDefault); tecx->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::Yes: tecx->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::No: tecs->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechUsesDefaultSettings(techType, useDefault) : tecs->setTechUsesDefaultSettings(techType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechUsesDefaultSettings(techType, useDefault) : tecx->setTechUsesDefaultSettings(techType, useDefault); break;
    }
}

void Properties::setTechMineralCost(Sc::Tech::Type techType, u16 mineralCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && tecs != nullptr )
                tecs->setTechMineralCost(techType, mineralCost);
            if ( tecx != nullptr )
                tecx->setTechMineralCost(techType, mineralCost);
            break;
        case Chk::UseExpSection::Both: tecs->setTechMineralCost(techType, mineralCost); tecx->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::No: tecs->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechMineralCost(techType, mineralCost) : tecs->setTechMineralCost(techType, mineralCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechMineralCost(techType, mineralCost) : tecx->setTechMineralCost(techType, mineralCost); break;
    }
}

void Properties::setTechGasCost(Sc::Tech::Type techType, u16 gasCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && tecs != nullptr )
                tecs->setTechGasCost(techType, gasCost);
            if ( tecx != nullptr )
                tecx->setTechGasCost(techType, gasCost);
            break;
        case Chk::UseExpSection::Both: tecs->setTechGasCost(techType, gasCost); tecx->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::No: tecs->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechGasCost(techType, gasCost) : tecs->setTechGasCost(techType, gasCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechGasCost(techType, gasCost) : tecx->setTechGasCost(techType, gasCost); break;
    }
}

void Properties::setTechResearchTime(Sc::Tech::Type techType, u16 researchTime, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && tecs != nullptr )
                tecs->setTechResearchTime(techType, researchTime);
            if ( tecx != nullptr )
                tecx->setTechResearchTime(techType, researchTime);
            break;
        case Chk::UseExpSection::Both: tecs->setTechResearchTime(techType, researchTime); tecx->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::Yes: tecx->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::No: tecs->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechResearchTime(techType, researchTime) : tecs->setTechResearchTime(techType, researchTime); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechResearchTime(techType, researchTime) : tecx->setTechResearchTime(techType, researchTime); break;
    }
}

void Properties::setTechEnergyCost(Sc::Tech::Type techType, u16 energyCost, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && tecs != nullptr )
                tecs->setTechEnergyCost(techType, energyCost);
            if ( tecx != nullptr )
                tecx->setTechEnergyCost(techType, energyCost);
            break;
        case Chk::UseExpSection::Both: tecs->setTechEnergyCost(techType, energyCost); tecx->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::Yes: tecx->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::No: tecs->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::YesIfAvailable: tecx != nullptr ? tecx->setTechEnergyCost(techType, energyCost) : tecs->setTechEnergyCost(techType, energyCost); break;
        case Chk::UseExpSection::NoIfOrigAvailable: tecs != nullptr ? tecs->setTechEnergyCost(techType, energyCost) : tecx->setTechEnergyCost(techType, energyCost); break;
    }
}

bool Properties::useExpansionTechAvailability(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions->isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return ptec != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return ptex != nullptr ? true : false;
    }
}

bool Properties::techAvailable(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techAvailable(techType, playerIndex) : ptec->techAvailable(techType, playerIndex);
}

bool Properties::techResearched(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techResearched(techType, playerIndex) : ptec->techResearched(techType, playerIndex);
}

bool Properties::techDefaultAvailable(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techDefaultAvailable(techType) : ptec->techDefaultAvailable(techType);
}

bool Properties::techDefaultResearched(Sc::Tech::Type techType, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->techDefaultResearched(techType) : ptec->techDefaultResearched(techType);
}

bool Properties::playerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
{
    return useExpansionTechAvailability(useExp) ? ptex->playerUsesDefault(techType, playerIndex) : ptec->playerUsesDefault(techType, playerIndex);
}

void Properties::setTechAvailable(Sc::Tech::Type techType, size_t playerIndex, bool available, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && ptec != nullptr )
                ptec->setTechAvailable(techType, playerIndex, available);
            if ( ptex != nullptr )
                ptex->setTechAvailable(techType, playerIndex, available);
            break;
        case Chk::UseExpSection::Both: ptec->setTechAvailable(techType, playerIndex, available); ptex->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::Yes: ptex->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::No: ptec->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setTechAvailable(techType, playerIndex, available) : ptec->setTechAvailable(techType, playerIndex, available); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setTechAvailable(techType, playerIndex, available) : ptex->setTechAvailable(techType, playerIndex, available); break;
    }
}

void Properties::setTechResearched(Sc::Tech::Type techType, size_t playerIndex, bool researched, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && ptec != nullptr )
                ptec->setTechResearched(techType, playerIndex, researched);
            if ( ptex != nullptr )
                ptex->setTechResearched(techType, playerIndex, researched);
            break;
        case Chk::UseExpSection::Both: ptec->setTechResearched(techType, playerIndex, researched); ptex->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::Yes: ptex->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::No: ptec->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setTechResearched(techType, playerIndex, researched) : ptec->setTechResearched(techType, playerIndex, researched); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setTechResearched(techType, playerIndex, researched) : ptex->setTechResearched(techType, playerIndex, researched); break;
    }
}

void Properties::setDefaultTechAvailable(Sc::Tech::Type techType, bool available, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && ptec != nullptr )
                ptec->setDefaultTechAvailable(techType, available);
            if ( ptex != nullptr )
                ptex->setDefaultTechAvailable(techType, available);
            break;
        case Chk::UseExpSection::Both: ptec->setDefaultTechAvailable(techType, available); ptex->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::Yes: ptex->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::No: ptec->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setDefaultTechAvailable(techType, available) : ptec->setDefaultTechAvailable(techType, available); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setDefaultTechAvailable(techType, available) : ptex->setDefaultTechAvailable(techType, available); break;
    }
}

void Properties::setDefaultTechResearched(Sc::Tech::Type techType, bool researched, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && ptec != nullptr )
                ptec->setDefaultTechResearched(techType, researched);
            if ( ptex != nullptr )
                ptex->setDefaultTechResearched(techType, researched);
            break;
        case Chk::UseExpSection::Both: ptec->setDefaultTechResearched(techType, researched); ptex->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::Yes: ptex->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::No: ptec->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setDefaultTechResearched(techType, researched) : ptec->setDefaultTechResearched(techType, researched); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setDefaultTechResearched(techType, researched) : ptex->setDefaultTechResearched(techType, researched); break;
    }
}

void Properties::setPlayerUsesDefaultTechSettings(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
            if ( techType < Sc::Tech::TotalOriginalTypes && ptec != nullptr )
                ptec->setPlayerUsesDefault(techType, playerIndex, useDefault);
            if ( ptex != nullptr )
                ptex->setPlayerUsesDefault(techType, playerIndex, useDefault);
            break;
        case Chk::UseExpSection::Both: ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::Yes: ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::No: ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: ptex != nullptr ? ptex->setPlayerUsesDefault(techType, playerIndex, useDefault) : ptec->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: ptec != nullptr ? ptec->setPlayerUsesDefault(techType, playerIndex, useDefault) : ptex->setPlayerUsesDefault(techType, playerIndex, useDefault); break;
    }
}

void Properties::setTechsToDefault(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both:
            *tecs = *TecsSection::GetDefault();
            *tecx = *TecxSection::GetDefault();
            *ptec = *PtecSection::GetDefault();
            *ptex = *PtexSection::GetDefault();
            break;
        case Chk::UseExpSection::Yes:
            *tecx = *TecxSection::GetDefault();
            *ptex = *PtexSection::GetDefault();
            break;
        case Chk::UseExpSection::No:
            *tecs = *TecsSection::GetDefault();
            *ptec = *PtecSection::GetDefault();
            break;
        case Chk::UseExpSection::YesIfAvailable:
            if ( tecx != nullptr )
                *tecx = *TecxSection::GetDefault();
            else
                *tecs = *TecsSection::GetDefault();

            if ( ptex != nullptr )
                *ptex = *PtexSection::GetDefault();
            else
                *ptec = *PtecSection::GetDefault();
            break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            if ( tecs != nullptr )
                *tecs = *TecsSection::GetDefault();
            else
                *tecx = *TecxSection::GetDefault();

            if ( ptec != nullptr )
                *ptec = *PtecSection::GetDefault();
            else
                *ptex = *PtexSection::GetDefault();
            break;
    }
}

bool Properties::stringUsed(size_t stringId, u32 userMask)
{
    return ((userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings && unis->stringUsed(stringId)) ||
        ((userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings && unix->stringUsed(stringId));
}

void Properties::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::OriginalUnitSettings) == Chk::StringUserFlag::OriginalUnitSettings )
        unis->markUsedStrings(stringIdUsed);
    
    if ( (userMask & Chk::StringUserFlag::ExpansionUnitSettings) == Chk::StringUserFlag::ExpansionUnitSettings )
        unix->markUsedStrings(stringIdUsed);
}

void Properties::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    unis->remapStringIds(stringIdRemappings);
    unix->remapStringIds(stringIdRemappings);
}

void Properties::deleteString(size_t stringId)
{
    unis->deleteString(stringId);
    unix->deleteString(stringId);
}

void Properties::set(std::unordered_map<SectionName, Section> & sections)
{
    unis = GetSection<UnisSection>(sections, SectionName::UNIS);
    unix = GetSection<UnixSection>(sections, SectionName::UNIx);
    puni = GetSection<PuniSection>(sections, SectionName::PUNI);
    upgs = GetSection<UpgsSection>(sections, SectionName::UPGS);
    
    upgx = GetSection<UpgxSection>(sections, SectionName::UPGx);
    upgr = GetSection<UpgrSection>(sections, SectionName::UPGR);
    pupx = GetSection<PupxSection>(sections, SectionName::PUPx);
    tecs = GetSection<TecsSection>(sections, SectionName::TECS);
    
    tecx = GetSection<TecxSection>(sections, SectionName::TECx);
    ptec = GetSection<PtecSection>(sections, SectionName::PTEC);
    ptex = GetSection<PtexSection>(sections, SectionName::PTEx);

    if ( unis == nullptr )
        unis = UnisSection::GetDefault();
    if ( unix == nullptr )
        unix = UnixSection::GetDefault();
    if ( puni == nullptr )
        puni = PuniSection::GetDefault();
    if ( upgs == nullptr )
        upgs = UpgsSection::GetDefault();

    if ( upgx == nullptr )
        upgx = UpgxSection::GetDefault();
    if ( upgr == nullptr )
        upgr = UpgrSection::GetDefault();
    if ( pupx == nullptr )
        pupx = PupxSection::GetDefault();
    if ( tecs == nullptr )
        tecs = TecsSection::GetDefault();

    if ( tecx == nullptr )
        tecx = TecxSection::GetDefault();
    if ( ptec == nullptr )
        ptec = PtecSection::GetDefault();
    if ( ptex == nullptr )
        ptex = PtexSection::GetDefault();
}

void Properties::clear()
{
    unis = nullptr;
    unix = nullptr;
    puni = nullptr;
    upgs = nullptr;

    upgx = nullptr;
    upgr = nullptr;
    pupx = nullptr;
    tecs = nullptr;

    tecx = nullptr;
    ptec = nullptr;
    ptex = nullptr;
}


Triggers::Triggers(bool useDefault) : LocationSynchronizer(), strings(nullptr), layers(nullptr)
{
    if ( useDefault )
    {
        uprp = UprpSection::GetDefault(); // CUWP - Create unit with properties properties
        upus = UpusSection::GetDefault(); // CUWP usage
        trig = TrigSection::GetDefault(); // Triggers
        mbrf = MbrfSection::GetDefault(); // Mission briefing triggers
        swnm = SwnmSection::GetDefault(); // Switch names
        wav = WavSection::GetDefault(); // Sound names
        if ( wav == nullptr || swnm == nullptr || mbrf == nullptr || trig == nullptr || upus == nullptr || uprp == nullptr )
        {
            throw ScenarioAllocationFailure(
                wav == nullptr ? ChkSection::getNameString(SectionName::WAV) :
                (swnm == nullptr ? ChkSection::getNameString(SectionName::SWNM) :
                (mbrf == nullptr ? ChkSection::getNameString(SectionName::MBRF) :
                (trig == nullptr ? ChkSection::getNameString(SectionName::TRIG) :
                (upus == nullptr ? ChkSection::getNameString(SectionName::UPUS) :
                ChkSection::getNameString(SectionName::UPRP))))));
        }
    }
}

bool Triggers::empty()
{
    return uprp == nullptr && upus == nullptr && trig == nullptr && mbrf == nullptr && swnm == nullptr && wav == nullptr;
}

Chk::Cuwp Triggers::getCuwp(size_t cuwpIndex)
{
    return uprp->getCuwp(cuwpIndex);
}

void Triggers::setCuwp(size_t cuwpIndex, const Chk::Cuwp & cuwp)
{
    return uprp->setCuwp(cuwpIndex, cuwp);
}

size_t Triggers::addCuwp(const Chk::Cuwp & cuwp, bool fixUsageBeforeAdding, size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    size_t found = uprp->findCuwp(cuwp);
    if ( found < Sc::Unit::MaxCuwps )
        return found;
    else
    {
        if ( fixUsageBeforeAdding )
            fixCuwpUsage(excludedTriggerIndex, excludedTriggerActionIndex);

        size_t nextUnused = upus->getNextUnusedCuwpIndex();
        if ( nextUnused < Sc::Unit::MaxCuwps )
            uprp->setCuwp(nextUnused, cuwp);

        return nextUnused;
    }
}

void Triggers::fixCuwpUsage(size_t excludedTriggerIndex, size_t excludedTriggerActionIndex)
{
    for ( size_t i=0; i<Sc::Unit::MaxCuwps; i++ )
        upus->setCuwpUsed(i, false);

    size_t numTriggers = trig->numTriggers();
    for ( size_t triggerIndex=0; triggerIndex<numTriggers; triggerIndex++ )
    {
        Chk::TriggerPtr trigger = trig->getTrigger(triggerIndex);
        for ( size_t actionIndex=0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
        {
            Chk::Action & action = trigger->action(actionIndex);
            if ( action.actionType == Chk::Action::Type::CreateUnitWithProperties && action.number < Sc::Unit::MaxCuwps && !(triggerIndex == excludedTriggerIndex && actionIndex == excludedTriggerActionIndex) )
                upus->setCuwpUsed(action.number, true);
        }
    }
}

bool Triggers::cuwpUsed(size_t cuwpIndex)
{
    return upus->cuwpUsed(cuwpIndex);
}

void Triggers::setCuwpUsed(size_t cuwpIndex, bool cuwpUsed)
{
    upus->setCuwpUsed(cuwpIndex, cuwpUsed);
}

size_t Triggers::numTriggers()
{
    return trig->numTriggers();
}

std::shared_ptr<Chk::Trigger> Triggers::getTrigger(size_t triggerIndex)
{
    return trig->getTrigger(triggerIndex);
}

size_t Triggers::addTrigger(std::shared_ptr<Chk::Trigger> trigger)
{
    return trig->addTrigger(trigger);
}

void Triggers::insertTrigger(size_t triggerIndex, std::shared_ptr<Chk::Trigger> trigger)
{
    trig->insertTrigger(triggerIndex, trigger);
}

void Triggers::deleteTrigger(size_t triggerIndex)
{
    trig->deleteTrigger(triggerIndex);
}

void Triggers::moveTrigger(size_t triggerIndexFrom, size_t triggerIndexTo)
{
    trig->moveTrigger(triggerIndexFrom, triggerIndexTo);
}

std::deque<Chk::TriggerPtr> Triggers::replaceRange(size_t beginIndex, size_t endIndex, std::deque<Chk::TriggerPtr> & triggers)
{
    return trig->replaceRange(beginIndex, endIndex, triggers);
}

size_t Triggers::numBriefingTriggers()
{
    return mbrf->numBriefingTriggers();
}

std::shared_ptr<Chk::Trigger> Triggers::getBriefingTrigger(size_t briefingTriggerIndex)
{
    return mbrf->getBriefingTrigger(briefingTriggerIndex);
}

size_t Triggers::addBriefingTrigger(std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    return mbrf->addBriefingTrigger(briefingTrigger);
}

void Triggers::insertBriefingTrigger(size_t briefingTriggerIndex, std::shared_ptr<Chk::Trigger> briefingTrigger)
{
    mbrf->insertBriefingTrigger(briefingTriggerIndex, briefingTrigger);
}

void Triggers::deleteBriefingTrigger(size_t briefingTriggerIndex)
{
    mbrf->deleteBriefingTrigger(briefingTriggerIndex);
}

void Triggers::moveBriefingTrigger(size_t briefingTriggerIndexFrom, size_t briefingTriggerIndexTo)
{
    mbrf->moveBriefingTrigger(briefingTriggerIndexFrom, briefingTriggerIndexTo);
}

size_t Triggers::getSwitchNameStringId(size_t switchIndex)
{
    return swnm->getSwitchNameStringId(switchIndex);
}

void Triggers::setSwitchNameStringId(size_t switchIndex, size_t stringId)
{
    swnm->setSwitchNameStringId(switchIndex, stringId);
}

size_t Triggers::addSound(size_t stringId)
{
    return wav->addSound(stringId);
}

bool Triggers::stringIsSound(size_t stringId)
{
    return wav->stringIsSound(stringId);
}

size_t Triggers::getSoundStringId(size_t soundIndex)
{
    return wav->getSoundStringId(soundIndex);
}

void Triggers::setSoundStringId(size_t soundIndex, size_t soundStringId)
{
    wav->setSoundStringId(soundIndex, soundStringId);
}

bool Triggers::locationUsed(size_t locationId)
{
    return trig->locationUsed(locationId);
}

bool Triggers::stringUsed(size_t stringId, u32 userMask)
{
    return (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound && wav->stringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch && swnm->stringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger && trig->stringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::BriefingTrigger) == Chk::StringUserFlag::BriefingTrigger && mbrf->stringUsed(stringId);
}

bool Triggers::gameStringUsed(size_t stringId, u32 userMask)
{
    return (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger && trig->gameStringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::BriefingTrigger) == Chk::StringUserFlag::BriefingTrigger && mbrf->stringUsed(stringId);
}

bool Triggers::editorStringUsed(size_t stringId, u32 userMask)
{
    return (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound && wav->stringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch && swnm->stringUsed(stringId) ||
        (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger && trig->commentStringUsed(stringId);
}

void Triggers::markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed)
{
    trig->markUsedLocations(locationIdUsed);
}

void Triggers::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        wav->markUsedStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        swnm->markUsedStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger )
        trig->markUsedStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::BriefingTrigger) == Chk::StringUserFlag::BriefingTrigger )
        mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger )
        trig->markUsedGameStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::BriefingTrigger) == Chk::StringUserFlag::BriefingTrigger )
        mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedEditorStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask)
{
    if ( (userMask & Chk::StringUserFlag::Sound) == Chk::StringUserFlag::Sound )
        wav->markUsedStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::Switch) == Chk::StringUserFlag::Switch )
        swnm->markUsedStrings(stringIdUsed);

    if ( (userMask & Chk::StringUserFlag::Trigger) == Chk::StringUserFlag::Trigger )
        trig->markUsedCommentStrings(stringIdUsed);
}

void Triggers::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    trig->remapLocationIds(locationIdRemappings);
}

void Triggers::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    wav->remapStringIds(stringIdRemappings);
    swnm->remapStringIds(stringIdRemappings);
    trig->remapStringIds(stringIdRemappings);
    mbrf->remapStringIds(stringIdRemappings);
}

void Triggers::deleteLocation(size_t locationId)
{
    trig->deleteLocation(locationId);
}

void Triggers::deleteString(size_t stringId)
{
    wav->deleteString(stringId);
    swnm->deleteString(stringId);
    trig->deleteString(stringId);
    mbrf->deleteString(stringId);
}

void Triggers::set(std::unordered_map<SectionName, Section> & sections)
{
    uprp = GetSection<UprpSection>(sections, SectionName::UPRP);
    upus = GetSection<UpusSection>(sections, SectionName::UPUS);
    trig = GetSection<TrigSection>(sections, SectionName::TRIG);
    mbrf = GetSection<MbrfSection>(sections, SectionName::MBRF);
    
    swnm = GetSection<SwnmSection>(sections, SectionName::SWNM);
    wav = GetSection<WavSection>(sections, SectionName::WAV);

    if ( uprp == nullptr )
        uprp = UprpSection::GetDefault();
    if ( upus == nullptr )
        upus = UpusSection::GetDefault();
    if ( trig == nullptr )
        trig = TrigSection::GetDefault();
    if ( mbrf == nullptr )
        mbrf = MbrfSection::GetDefault();
    if ( swnm == nullptr )
        swnm = SwnmSection::GetDefault();
    if ( wav == nullptr )
        wav = WavSection::GetDefault();
}

void Triggers::clear()
{
    uprp = nullptr;
    upus = nullptr;
    trig = nullptr;
    mbrf = nullptr;

    swnm = nullptr;
    wav = nullptr;
}
