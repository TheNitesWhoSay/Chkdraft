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
    properties.versions = &versions;
    properties.strings = &strings;
    triggers.strings = &strings;
    triggers.layers = &layers;

    allSections.push_back(versions.type);
    allSections.push_back(versions.ver);

    if ( !versions.isExpansion() )
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

bool Scenario::isProtected()
{
//    return mapIsProtected;
    return false;
}

bool Scenario::Save(ChkVersion chkVersion, std::ostream &outputStream, ScenarioSaverPtr scenarioSaver)
{
/*    if ( isProtected() )
        return false;

    Strubgs
    if ( chkVersion == ChkVersion::Unchanged )
    {
        for ( auto &section : allSections )
            section->write(outputStream);

        if ( tailLength > 0 && tailLength < 8 )
            outputStream.write(tailData.data, tailLength);
    }
    else // chkVersion changed, update all potentially affected sections
    {
        Section newMrgn(new ChkSection(ChkSection::SectionName::MRGN));
        Section newType(new ChkSection(ChkSection::SectionName::TYPE));
        Section newVer(new ChkSection(ChkSection::SectionName::VER));
        Section newIver(new ChkSection(ChkSection::SectionName::IVER));
        Section newIve2(new ChkSection(ChkSection::SectionName::IVE2));

        bool removeIver = false;
        if ( !PrepareSaveSectionChanges(chkVersion, newMrgn, newType, newVer, newIver, newIve2, removeIver, scenarioSaver) )
            return false;

        std::unordered_set<ChkSection::SectionName> skipSections;
        std::unordered_map<Section, Section> sectionsToReplace;
        std::vector<Section> sectionsToAdd;
        std::vector<ChkSection::SectionName> sectionsToRemove;
        for ( auto section = allSections.rbegin(); section != allSections.rend(); ++section )
        {
            ChkSection::SectionName sectionName = (ChkSection::SectionName)(*section)->titleVal();
            if ( foundSectionNames.find(sectionName) == foundSectionNames.end() )
            {
                foundSectionNames.insert(sectionName);
                switch ( sectionName )
                {
                case ChkSection::SectionName::MRGN:
                    replacedSections.insert(std::pair<Section, Section>(*section, newMrgn)); foundSectionNames.insert(sectionName); break;
                case ChkSection::SectionName::TYPE: replacedSections.insert(std::pair<Section, Section>(*section, newType)); foundSectionNames.insert(sectionName); break;
                case ChkSection::SectionName::VER:
                    replacedSections.insert(std::pair<Section, Section>(*section, newVer)); foundSectionNames.insert(sectionName); break;
                case ChkSection::SectionName::IVE2:
                    if ( IVE2->exists() )
                        replacedSections.insert(std::pair<Section, Section>(*section, newIve2));
                    break;
                case ChkSection::SectionName::IVER:
                    if ( !removeIver && IVER->exists() )
                        replacedSections.insert(std::pair<Section, Section>(*section, newIver));
                    break;
                }
            }
        }

        size_t numSectionsToAdd = (MRGN->exists() ? 0 : 1) + (TYPE->exists() ? 0 : 1) + (VER->exists() ? 0 : 1) + (IVE2->exists() ? 0 : 1) +
            (removeIver || IVER->exists() ? 0 : 1);

        allSections.reserve(allSections.size() + numSectionsToAdd);

        bool wroteMRGN = false, wroteTYPE = false, wroteVER = false, wroteIVER = false, wroteIVE2 = false;
        for ( auto &section : allSections )
        {
            ChkSection::SectionName sectionName = (ChkSection::SectionName)section->titleVal;
            switch ( sectionName )
            {
            case ChkSection::SectionName::MRGN:
            case ChkSection::SectionName::TYPE:
            case ChkSection::SectionName::VER:
            case ChkSection::SectionName::IVER:
            case ChkSection::SectionName::IVE2:
                if ( section == lastSectionInstances.find(sectionName)->second )

                    break;
            default:
                section->write(outputStream, true);
                break;
            }
        }

        if ( tailLength > 0 && tailLength < 8 )
            outputStream.write((const char*)tailData, tailLength);

        // Save succeeded, now update the scenario with the saveSections
    }*/
    return false;
}

bool Scenario::ParseScenario(buffer &chk)
{
/*    caching = false;
    s64 chkSize = chk.size();

    s64 position = 0,
        nextPosition = 0;

    std::vector<Section> rawSections;
    bool parsingRawData = true;
    do
    {
        if ( position + 8 < chkSize ) // Valid section header
        {
            if ( ParseSection(chk, position, nextPosition, rawSections) )
                position = nextPosition;
            else // Severe data handling issue or out of memory
                return false;
        }
        else if ( position < chkSize ) // Partial section header
        {
            tailLength = (u8)(chkSize - position);
            if ( !chk.getArray<u8>(tailData, position, tailLength) )
                return false;

            mapIsProtected = true;
            parsingRawData = false;
        }
        else if ( position > chkSize ) // Oversized finish
        {
            mapIsProtected = true;
            parsingRawData = false;
        }
        else // Natural finish (position == chkSize)
            parsingRawData = false;

    } while ( parsingRawData );

    auto foundVer = std::find_if(rawSections.rbegin(), rawSections.rend(), [](const Section &section) { section->titleVal() == (u32)SectionName::VER && section->size() == 2; });
    bool hybridOrBroodWar = foundVer == rawSections.rend() || (*foundVer)->get<u16>(0) >= 63; // True if no valid VER section or file format version >= 63

    for ( Section section : rawSections )
    {
        if ( ValidateSection(section, hybridOrBroodWar) )
        {

        }
    }

    bool hybridOrBroodWar = VER->get<u16>(0) >= 63; // File format version at least 63
    std::find


        for ( Section &section : rawSections )
        {
            if ( section->titleVal() == (u32)SectionName::VER && )
        }

    // Pre-cache mergings/corrections
    correctMTXM();

    // Cache sections
    CacheSections();
    caching = true;
    */
    return true;
}

bool Scenario::ParseSection(buffer &chk, s64 position, s64 &nextPosition, std::vector<Section> &sections)
{
/*    u32 sectionName = 0;
    s32 sectionSize = 0;

    if ( chk.get<u32>(sectionName, position) &&    // Get sectionName/title
        chk.get<s32>(sectionSize, position + 4) ) // Get section size
    {
        nextPosition = position + s64(8) + s64(sectionSize);
    }
    else
        return false; // Unexpected read faliure

    if ( sectionSize >= 0 ) // Normal section
    {
        try
        {
            Section newSection(new ChkSection((ChkSection::SectionName)sectionName));
            if ( nextPosition >= position + s64(8) && // Check for addition overflow
                (nextPosition > chk.size() || // Either oversized
                    sectionSize == 0 || // Or empty
                    newSection->extract(chk, position+8, sectionSize)) ) // Or got the section data
            {
                sections.push_back(newSection);
                return true;
            }
        } catch ( std::exception &e ) {}
        return false; // Either failed to allocate section or the section data was valid but extraction failed
    }
    else if ( sectionSize < 0 ) // Jump section
    {
        return nextPosition < position + s64(8) && // Check for addition underflow
            nextPosition != 0;                  // Check for loop (right now it just checks for position to zero)
    }*/
    return true; // sectionSize == 0
}

bool Scenario::ToSingleBuffer(buffer& chk)
{
/*    chk.flush();
    if ( chk.setTitle("CHK ") )
    {
        for ( auto &section : allSections )
        {
            if ( !( chk.add<u32>(section->titleVal()) &&
                chk.add<u32>((u32)section->size()) &&
                ( section->size() == 0 || chk.addStr((const char*)section->getPtr(0), section->size()) ) ) )
            {
                return false;
            }
        }

        if ( tailLength > 0 && tailLength < 8 )
            chk.addStr((const char*)tailData, tailLength);

        return true;
    }
    else*/
        return false;
}

std::shared_ptr<void> Scenario::Serialize()
{
/*    buffer chk("CHK ");
    if ( ToSingleBuffer(chk) )
        return chk.serialize();
    else*/
        return nullptr;
}

bool Scenario::Deserialize(const void* data)
{
/*    buffer chk;
    if ( chk.deserialize(data) )
    {
        Flush();
        if ( ParseScenario(chk) )
            return true;
    }
    return false;*/
    return false;
}

bool Scenario::hasPassword()
{
//    return tailLength == 7;
    return false;
}

bool Scenario::isPassword(std::string &password)
{
/*    if ( hasPassword() )
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
    }
    else // No password
        return true;
        */
    return false;
}

bool Scenario::SetPassword(std::string &oldPass, std::string &newPass)
{
/*    if ( isPassword(oldPass) )
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
    }*/
    return false;
}

bool Scenario::Login(std::string &password)
{
    if ( isPassword(password) )
    {
        mapIsProtected = false;
        return true;
    }
    return false;
}

void Scenario::WriteFile(FILE* pFile)
{
/*    if ( !isProtected() )
    {
        for ( auto &section : allSections )
            section->write(pFile, true);

        if ( tailLength > 0 && tailLength < 8 )
            fwrite(tailData, tailLength, 1, pFile);
    }*/
}


Versions::Versions(bool useDefault)
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

void Versions::changeTo(Chk::Version version)
{
    Chk::Version oldVersion = ver->getVersion();
    ver->setVersion(version);
    if ( version < Chk::Version::StarCraft_Hybrid )
    {
        type->setType(Chk::Type::RAWS);
        iver->setVersion(Chk::IVersion::Current);
        layers->sizeLocationsToScOriginal();
    }
    else if ( version < Chk::Version::StarCraft_BroodWar )
    {
        type->setType(Chk::Type::RAWS);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->sizeLocationsToScHybridOrExpansion();
    }
    else // version >= Chk::Version::StarCraft_Broodwar
    {
        type->setType(Chk::Type::RAWB);
        iver->setVersion(Chk::IVersion::Current);
        ive2->setVersion(Chk::I2Version::StarCraft_1_04);
        layers->sizeLocationsToScHybridOrExpansion();
    }
}

bool Versions::hasDefaultValidation()
{
    return vcod->isDefault();
}

void Versions::setToDefaultValidation()
{
    vcod->setToDefault();
}


Strings::Strings(bool useDefault) :
    StrSynchronizer((u32)StrCompressFlag::DuplicateStringRecycling, (u32)StrCompressFlag::AllNonInterlacing)
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
        
        str->syncFromBuffer(*this);
        kstr->syncFromBuffer(*this);
    }
}

size_t Strings::getCapacity(Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
        return strings.size();
    else if ( storageScope == Chk::Scope::Editor )
        return kStrings.size();
    else
        return 0;
}

bool Strings::stringUsed(size_t stringId, Chk::Scope usageScope, Chk::Scope storageScope, bool ensureStored)
{
    if ( storageScope == Chk::Scope::Game && (stringId < strings.size() && strings[stringId] != nullptr || !ensureStored) )
    {
        if ( stringId < Chk::MaxStrings ) // 16 or 32-bit stringId
        {
            if ( usageScope == Chk::Scope::Editor )
                return layers->stringUsed(stringId) || triggers->editorStringUsed(stringId);
            else if ( usageScope == Chk::Scope::Game )
                return sprp->stringUsed(stringId) || players->stringUsed(stringId) || properties->stringUsed(stringId) || triggers->gameStringUsed(stringId);
            else // if ( usageScope == Chk::Scope::Either )
                return sprp->stringUsed(stringId) || players->stringUsed(stringId) || properties->stringUsed(stringId) || layers->stringUsed(stringId) || triggers->stringUsed(stringId);
        }
        else // stringId >= Chk::MaxStrings // 32-bit stringId
        {
            return usageScope == Chk::Scope::Either && triggers->stringUsed(stringId) ||
                usageScope == Chk::Scope::Game && triggers->gameStringUsed(stringId) ||
                usageScope == Chk::Scope::Editor && triggers->editorStringUsed(stringId);
        }
    }
    else if ( storageScope == Chk::Scope::Editor && (stringId < kStrings.size() && kStrings[stringId] != nullptr || !ensureStored) )
        return ostr->stringUsed(stringId);

    return false;
}

bool Strings::stringStored(size_t stringId, Chk::Scope storageScope)
{
    return ((u32)storageScope & (u32)Chk::Scope::Game) == (u32)Chk::Scope::Game && stringId < strings.size() && strings[stringId] != nullptr ||
        ((u32)storageScope & (u32)Chk::Scope::Editor) == (u32)Chk::Scope::Editor && stringId < kStrings.size() && kStrings[stringId] != nullptr; 
}

void Strings::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        bool markGameStrings = ((u32)usageScope & (u32)Chk::Scope::Game) == (u32)Chk::Scope::Game;
        bool markEditorStrings = ((u32)usageScope & (u32)Chk::Scope::Editor) == (u32)Chk::Scope::Editor;

        if ( markGameStrings )
        {
            sprp->markUsedStrings(stringIdUsed); // {SPRP, Game, u16}: Scenario Name and Scenario Description
            players->markUsedStrings(stringIdUsed); // {FORC, Game, u16}: Force Names
            properties->markUsedStrings(stringIdUsed); // {UNIS, Game, u16}: Unit Names (original); {UNIx, Game, u16}: Unit names (expansion)
            if ( markEditorStrings ) // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, ...
                triggers->markUsedStrings(stringIdUsed); // ... transmission text, next scenario, sound path, comment; {MBRF, Game, u32}: mission objectives, sound, text message
            else
                triggers->markUsedGameStrings(stringIdUsed); // {TRIG, Game&Editor, u32}: text message, mission objectives, leaderboard text, transmission text, next scenario, sound path
        }

        if ( markEditorStrings )
        {
            layers->markUsedStrings(stringIdUsed); // {MRGN, Editor, u16}: location name
            if ( !markGameStrings )
                triggers->markUsedEditorStrings(stringIdUsed); // {WAV, Editor, u32}: Sound Names; {SWNM, Editor, u32}: Switch Names; {TRIG, Game&Editor, u32}: comment
        }
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        ostr->markUsedStrings(stringIdUsed);
    }
}

void Strings::markValidUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope usageScope, Chk::Scope storageScope)
{
    markUsedStrings(stringIdUsed, usageScope);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
            {
                size_t limit = std::min((size_t)Chk::MaxStrings, strings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
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
                size_t limit = std::min((size_t)Chk::MaxStrings, kStrings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && kStrings[stringId] == nullptr )
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
                size_t limit = std::min(std::min((size_t)Chk::MaxStrings, strings.size()), kStrings.size());
                size_t stringId = 1;
                for ( ; stringId < limit; stringId++ )
                {
                    if ( stringIdUsed[stringId] && strings[stringId] == nullptr && kStrings[stringId] == nullptr )
                        stringIdUsed[stringId] = false;
                }

                if ( strings.size() > kStrings.size() )
                {
                    for ( ; stringId < strings.size(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && strings[stringId] == nullptr )
                            stringIdUsed[stringId] = false;
                    }
                }
                else if ( kStrings.size() > strings.size() )
                {
                    for ( ; stringId < kStrings.size(); stringId++ )
                    {
                        if ( stringIdUsed[stringId] && kStrings[stringId] == nullptr )
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

template <typename StringType>
std::shared_ptr<StringType> Strings::getString(size_t stringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame: return stringId < kStrings.size() && kStrings[stringId] != nullptr ? convert<StringType>(kStrings[stringId]) : (stringId < strings.size() ? convert<StringType>(strings[stringId]) : nullptr);
        case Chk::Scope::Game: return stringId < strings.size() ? convert<StringType>(strings[stringId]) : nullptr;
        case Chk::Scope::Editor: return stringId < kStrings.size() ? convert<StringType>(kStrings[stringId]) : nullptr;
        case Chk::Scope::GameOverEditor: return stringId < strings.size() && strings[stringId] != nullptr ? convert<StringType>(strings[stringId]) : (stringId < kStrings.size() ? convert<StringType>(kStrings[stringId]) : nullptr);
        default: return nullptr;
    }
}
template std::shared_ptr<RawString> Strings::getString<RawString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getString<EscString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getString<ChkdString>(size_t stringId, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getString<SingleLineChkdString>(size_t stringId, Chk::Scope storageScope);

template <typename StringType>
size_t Strings::findString(const StringType &str, Chk::Scope storageScope)
{
    RawString rawStr;
    ConvertStr<StringType, RawString>(str, rawStr);
    switch ( storageScope )
    {
        case Chk::Scope::Game:
            {
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::Editor:
            {
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::GameOverEditor:
        case Chk::Scope::Either:
            {
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
        case Chk::Scope::EditorOverGame:
            {
                for ( size_t stringId=1; stringId<kStrings.size(); stringId++ )
                {
                    if ( kStrings[stringId] != nullptr && kStrings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
                for ( size_t stringId=1; stringId<strings.size(); stringId++ )
                {
                    if ( strings[stringId] != nullptr && strings[stringId]->compare<RawString>(rawStr) == 0 )
                        return stringId;
                }
            }
            break;
    }
    return (size_t)Chk::StringId::NoString;
}

void Strings::setCapacity(size_t stringCapacity, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game )
    {
        if ( stringCapacity > Chk::MaxStrings )
            throw MaximumStringsExceeded();

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
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragmentStr(false);
            else
                throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::STR), numValidUsedStrings, stringCapacity, autoDefragment);
        }
        
        while ( strings.size() < stringCapacity )
            strings.push_back(nullptr);

        while ( strings.size() > stringCapacity )
            strings.pop_back();
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        if ( stringCapacity > Chk::MaxKStrings )
            throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), stringCapacity, Chk::MaxKStrings);

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
            throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        else if ( highestValidUsedStringId > stringCapacity )
        {
            if ( autoDefragment && numValidUsedStrings <= stringCapacity )
                defragmentKstr(false);
            else
                throw InsufficientStringCapacity(ChkSection::getNameString(SectionName::KSTR), numValidUsedStrings, stringCapacity, autoDefragment);
        }

        while ( kStrings.size() < stringCapacity )
            kStrings.push_back(nullptr);

        while ( kStrings.size() > stringCapacity )
            kStrings.pop_back();
    }
}

template <typename StringType>
size_t Strings::addString(const StringType &str, Chk::Scope storageScope, bool autoDefragment)
{
    RawString rawString;
    ConvertStr<StringType, RawString>(str, rawString);

    size_t stringId = findString<StringType>(str, storageScope);
    if ( stringId != (size_t)Chk::StringId::NoString )
        return stringId; // String already exists, return the id

    if ( storageScope == Chk::Scope::Game )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, Chk::Scope::Game, true);
        
        if ( nextUnusedStringId >= strings.size() )
            setCapacity(nextUnusedStringId, Chk::Scope::Game, autoDefragment);
        else if ( nextUnusedStringId == 0 && storageScope == Chk::Scope::Game )
            throw MaximumStringsExceeded();

        strings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
        return nextUnusedStringId;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        size_t nextUnusedStringId = getNextUnusedStringId(stringIdUsed, Chk::Scope::Editor, true);

        if ( nextUnusedStringId >= kStrings.size() )
            setCapacity(nextUnusedStringId, Chk::Scope::Editor, autoDefragment);
        else if ( nextUnusedStringId == 0 )
            throw MaximumStringsExceeded();

        kStrings[nextUnusedStringId] = ScStrPtr(new ScStr(rawString));
        return nextUnusedStringId;
    }
    return (size_t)Chk::StringId::NoString;
}
template size_t Strings::addString<RawString>(const RawString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<EscString>(const EscString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<ChkdString>(const ChkdString &str, Chk::Scope storageScope, bool autoDefragment);
template size_t Strings::addString<SingleLineChkdString>(const SingleLineChkdString &str, Chk::Scope storageScope, bool autoDefragment);

bool Strings::addStrings(const std::vector<zzStringTableNode> stringsToAdd, Chk::Scope storageScope, bool autoDefragment)
{
    size_t highestStringId = 0;
    std::vector<zzStringTableNode> allocatedStrings; // Create the strings' final containers all before adding any, do not use the containers passed in as that would allow those strings to mutate without validation
    if ( storageScope == Chk::Scope::Game )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Game);
        for ( const zzStringTableNode &stringToAdd : stringsToAdd )
        {
            if ( stringToAdd.stringId > 0 || stringToAdd.stringId > Chk::MaxStrings || stringIdUsed[stringToAdd.stringId] ) // Check if stringId invalid or stringId already in use
                return false;
            else // Looks good, setup the final container for the string
            {
                zzStringTableNode stringAllocation;
                stringAllocation.scStr = ScStrPtr(new ScStr(stringToAdd.scStr->str));
                stringAllocation.stringId = stringToAdd.stringId;
                allocatedStrings.push_back(stringAllocation);
                if ( stringAllocation.stringId > highestStringId )
                    highestStringId = stringAllocation.stringId;
            }
        }

        if ( highestStringId > getCapacity(Chk::Scope::Game) ) // If the strings capacity needs to be adjusted, do so now
            setCapacity(highestStringId, Chk::Scope::Game, autoDefragment);

        for ( zzStringTableNode stringAllocation : allocatedStrings )
            strings[stringAllocation.stringId] = stringAllocation.scStr; // Copy the string into the strings array

        return true;
    }
    else if ( storageScope == Chk::Scope::Editor )
    {
        std::bitset<Chk::MaxStrings> stringIdUsed;
        markUsedStrings(stringIdUsed, Chk::Scope::Either, Chk::Scope::Editor);
        for ( const zzStringTableNode &stringToAdd : stringsToAdd )
        {
            if ( stringToAdd.stringId == 0 || stringToAdd.stringId > Chk::MaxStrings || stringIdUsed[stringToAdd.stringId] ) // Check if stringId invalid or stringId already in use
                return false;
            else // Looks good, setup the final container for the string
            {
                zzStringTableNode stringAllocation;
                stringAllocation.scStr = ScStrPtr(new ScStr(stringToAdd.scStr->str));
                stringAllocation.stringId = stringToAdd.stringId;
                allocatedStrings.push_back(stringAllocation);
                if ( stringAllocation.stringId > highestStringId )
                    highestStringId = stringAllocation.stringId;
            }
        }

        if ( highestStringId > getCapacity(Chk::Scope::Editor) ) // If the strings capacity needs to be adjusted, do so now
            setCapacity(highestStringId, Chk::Scope::Editor, autoDefragment);

        for ( zzStringTableNode stringAllocation : allocatedStrings )
            strings[stringAllocation.stringId] = stringAllocation.scStr; // Copy the string into the strings array

        return true;
    }
    return false;
}

template <typename StringType>
void Strings::replaceString(size_t stringId, const StringType &str, Chk::Scope storageScope)
{
    RawString rawString;
    ConvertStr<StringType, RawString>(str, rawString);

    if ( storageScope == Chk::Scope::Game && stringId < strings.size() )
        strings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
    else if ( storageScope == Chk::Scope::Editor && stringId < kStrings.size() )
        kStrings[stringId] = ScStrPtr(new ScStr(rawString, StrProp()));
}
template void Strings::replaceString<RawString>(size_t stringId, const RawString &str, Chk::Scope storageScope);
template void Strings::replaceString<EscString>(size_t stringId, const EscString &str, Chk::Scope storageScope);
template void Strings::replaceString<ChkdString>(size_t stringId, const ChkdString &str, Chk::Scope storageScope);
template void Strings::replaceString<SingleLineChkdString>(size_t stringId, const SingleLineChkdString &str, Chk::Scope storageScope);

void Strings::deleteUnusedStrings(Chk::Scope storageScope)
{
    std::bitset<65536> stringIdUsed;
    markUsedStrings(stringIdUsed, Chk::Scope::Game, storageScope);
    for ( size_t i=0; i<strings.size(); i++ )
    {
        if ( !stringIdUsed[i] && strings[i] != nullptr )
            strings[i] = nullptr;
    }

    stringIdUsed.reset();
    markUsedStrings(stringIdUsed, Chk::Scope::Editor, storageScope);
    for ( size_t i=0; i<kStrings.size(); i++ )
    {
        if ( !stringIdUsed[i] && kStrings[i] != nullptr )
            kStrings[i] = nullptr;
    }
}

void Strings::deleteString(size_t stringId, Chk::Scope storageScope, bool deleteOnlyIfUnused)
{
    if ( ((u32)storageScope & (u32)Chk::Scope::Game) == (u32)Chk::Scope::Game )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Game) )
        {
            if ( stringId < strings.size() )
                strings[stringId] = nullptr;
            
            sprp->deleteString(stringId);
            players->deleteString(stringId);
            properties->deleteString(stringId);
            layers->deleteString(stringId);
            triggers->deleteString(stringId);
        }
    }
    
    if ( ((u32)storageScope & (u32)Chk::Scope::Editor) == (u32)Chk::Scope::Editor )
    {
        if ( !deleteOnlyIfUnused || !stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, true) )
        {
            if ( stringId < kStrings.size() )
                kStrings[stringId] = nullptr;

            ostr->deleteString(stringId);
        }
    }
}

void Strings::moveString(size_t stringIdFrom, size_t stringIdTo, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Game )
    {
        size_t stringIdMin = std::min(stringIdFrom, stringIdTo);
        size_t stringIdMax = std::max(stringIdFrom, stringIdTo);
        if ( stringIdMin > 0 && stringIdMax <= strings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Game);
            ScStrPtr selected = strings[stringIdFrom];
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
                            ScStrPtr highestString = strings[stringId-1];
                            strings[stringId-1] = nullptr;
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
                            ScStrPtr lowestString = strings[stringId+1];
                            strings[stringId+1] = nullptr;
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
        if ( stringIdMin > 0 && stringIdMax <= kStrings.size() && stringIdFrom != stringIdTo )
        {
            std::bitset<Chk::MaxStrings> stringIdUsed;
            markUsedStrings(stringIdUsed, Chk::Scope::Editor);
            ScStrPtr selected = kStrings[stringIdFrom];
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
                            ScStrPtr highestString = kStrings[stringId-1];
                            kStrings[stringId-1] = nullptr;
                            stringIdUsed[stringId-1] = false;
                            kStrings[stringId] = highestString;
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
                            ScStrPtr lowestString = kStrings[stringId+1];
                            kStrings[stringId+1] = nullptr;
                            stringIdUsed[stringId+1] = false;
                            kStrings[stringId] = lowestString;
                            stringIdUsed[stringId] = true;
                            stringIdRemappings.insert(std::pair<u32, u32>(u32(stringId+1), (u32)stringId));
                            break;
                        }
                    }
                }
            }
            kStrings[stringIdTo] = selected;
            stringIdRemappings.insert(std::pair<u32, u32>((u32)stringIdFrom, (u32)stringIdTo));
            remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        }
    }
}

size_t Strings::rescopeString(size_t stringId, Chk::Scope changeStorageScopeTo, bool autoDefragment)
{
    if ( changeStorageScopeTo == Chk::Scope::Editor && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Game, true) )
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
            for ( size_t i=0; i<numLocations; i++ )
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
    else if ( changeStorageScopeTo == Chk::Scope::Game && stringUsed(stringId, Chk::Scope::Either, Chk::Scope::Editor, true) )
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
            for ( size_t i=0; i<numLocations; i++ )
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

size_t Strings::getLocationNameStringId(size_t locationIndex, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        return ostr->getLocationNameStringId(locationIndex);
    else
    {
        Chk::LocationPtr location = layers->getLocation(locationIndex);
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

void Strings::setLocationNameStringId(size_t locationIndex, size_t locationNameStringId, Chk::Scope storageScope)
{
    if ( storageScope == Chk::Scope::Editor )
        ostr->setLocationNameStringId(locationIndex, (u32)locationNameStringId);
    else
    {
        auto location = layers->getLocation(locationIndex);
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
    std::shared_ptr<StringType> mapUnitName = getString<StringType>(
        properties->getUnitNameStringId(unitType, useExp),
        properties->useExpansionUnitSettings(useExp) ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType),
        storageScope);

    if ( mapUnitName != nullptr )
        return mapUnitName;
    else if ( unitType < Sc::Unit::TotalTypes )
        return std::shared_ptr<StringType>(new StringType(Sc::Unit::defaultDisplayNames[unitType]));
    else
        return std::shared_ptr<StringType>(new StringType("UnitID: " + std::to_string(unitType)));
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
std::shared_ptr<StringType> Strings::getLocationName(size_t locationIndex, Chk::Scope storageScope)
{
    return getString<StringType>((locationIndex < layers->numLocations() ? layers->getLocation(locationIndex)->stringId : 0), ostr->getLocationNameStringId(locationIndex), storageScope);
}
template std::shared_ptr<RawString> Strings::getLocationName<RawString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getLocationName<EscString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getLocationName<ChkdString>(size_t locationIndex, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getLocationName<SingleLineChkdString>(size_t locationIndex, Chk::Scope storageScope);

template <typename StringType>
void Strings::setScenarioName(const StringType &scenarioNameString, Chk::Scope storageScope, bool autoDefragment)
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
template void Strings::setScenarioName<RawString>(const RawString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<EscString>(const EscString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<ChkdString>(const ChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioName<SingleLineChkdString>(const SingleLineChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setScenarioDescription(const StringType &scenarioDescription, Chk::Scope storageScope, bool autoDefragment)
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
template void Strings::setScenarioDescription<RawString>(const RawString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<EscString>(const EscString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<ChkdString>(const ChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setScenarioDescription<SingleLineChkdString>(const SingleLineChkdString &scenarioNameString, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setForceName(Chk::Force force, const StringType &forceName, Chk::Scope storageScope, bool autoDefragment)
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
template void Strings::setForceName<RawString>(Chk::Force force, const RawString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<EscString>(Chk::Force force, const EscString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<ChkdString>(Chk::Force force, const ChkdString &forceName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setForceName<SingleLineChkdString>(Chk::Force force, const SingleLineChkdString &forceName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setUnitName(Sc::Unit::Type unitType, const StringType &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment)
{
    if ( (storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor) && (size_t)unitType < Sc::Unit::TotalTypes )
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
template void Strings::setUnitName<RawString>(Sc::Unit::Type unitType, const RawString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<EscString>(Sc::Unit::Type unitType, const EscString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<ChkdString>(Sc::Unit::Type unitType, const ChkdString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, const SingleLineChkdString &unitName, Chk::UseExpSection useExp, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSoundPath(size_t soundIndex, const StringType &soundPath, Chk::Scope storageScope, bool autoDefragment)
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
template void Strings::setSoundPath<RawString>(size_t soundIndex, const RawString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<EscString>(size_t soundIndex, const EscString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<ChkdString>(size_t soundIndex, const ChkdString &soundPath, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSoundPath<SingleLineChkdString>(size_t soundIndex, const SingleLineChkdString &soundPath, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setSwitchName(size_t switchIndex, const StringType &switchName, Chk::Scope storageScope, bool autoDefragment)
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
template void Strings::setSwitchName<RawString>(size_t switchIndex, const RawString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<EscString>(size_t switchIndex, const EscString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<ChkdString>(size_t switchIndex, const ChkdString &switchName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setSwitchName<SingleLineChkdString>(size_t switchIndex, const SingleLineChkdString &switchName, Chk::Scope storageScope, bool autoDefragment);

template <typename StringType>
void Strings::setLocationName(size_t locationIndex, const StringType &locationName, Chk::Scope storageScope, bool autoDefragment)
{
    if ( storageScope == Chk::Scope::Game || storageScope == Chk::Scope::Editor && locationIndex < layers->numLocations() )
    {
        size_t newStringId = addString<StringType>(locationName, storageScope, autoDefragment);
        if ( newStringId != (size_t)Chk::StringId::NoString )
        {
            if ( storageScope == Chk::Scope::Game )
                layers->getLocation(locationIndex)->stringId = (u16)newStringId;
            else if ( storageScope == Chk::Scope::Editor )
                ostr->setLocationNameStringId(locationIndex, (u32)newStringId);
        }
    }
}
template void Strings::setLocationName<RawString>(size_t locationIndex, const RawString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<EscString>(size_t locationIndex, const EscString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<ChkdString>(size_t locationIndex, const ChkdString &locationName, Chk::Scope storageScope, bool autoDefragment);
template void Strings::setLocationName<SingleLineChkdString>(size_t locationIndex, const SingleLineChkdString &locationName, Chk::Scope storageScope, bool autoDefragment);

bool Strings::checkFit(StrCompressionElevatorPtr compressionElevator)
{
    size_t numStrings = strings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : strings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u16_max - 3;
    size_t totalOffsetSpace = 2*numStrings;
    
    return totalOffsetSpace + totalCharacterSpace <= totalBytesAvailable;
}

void Strings::synchronizeToStrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the basic, staredit standard, STR section format, and not allowing section sizes over 65536

        u16 numStrings;
        u16 stringOffsets[numStrings]; // Offset of the start of the string within the section
        void[] stringData; // Character data, first comes initial NUL character... then all strings, in order, each NUL terminated
    */

    size_t numStrings = strings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : strings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u16_max - 3; // u16_max for the maximum size of string offsets minus three (two bytes for numStrings, one for NUL char)

    if ( totalCharacterSpace > totalBytesAvailable )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::STR), totalCharacterSpace, totalBytesAvailable);

    size_t totalOffsetSpace = 2 * numStrings;
    if ( totalOffsetSpace + totalCharacterSpace > totalBytesAvailable )
        throw MaximumOffsetAndCharsExceeded(ChkSection::getNameString(SectionName::STR), numStrings, totalCharacterSpace, totalBytesAvailable);

    buffer newStringSection;
    buffer characterData;
    u16 characterDataStart = 2 + (u16)totalOffsetSpace;
    newStringSection.add<u16>((u16)numStrings); // Add numStrings
    characterData.add<s8>(0); // Add initial NUL character=
    for ( auto string : strings )
    {
        size_t stringLength = string != nullptr ? string->length() : 0;
        if ( stringLength == 0 )
            newStringSection.add<u16>(characterDataStart); // Point this string to initial NUL character
        else // stringLength > 0
        {
            newStringSection.add<u16>(u16(characterDataStart + characterData.size())); // Set the offset this string will be at
            characterData.addStr(string->str, stringLength+1); // Add the string plus its NUL terminator
        }
    }
    newStringSection.addStr((char*)characterData.getPtr(0), characterData.size());
    characterData.flush();
    rawData.takeAllData(newStringSection);
}

void Strings::synchronizeFromStrBuffer(const buffer &rawData)
{
    size_t sectionSize = (size_t)rawData.size();
    size_t numStrings = sectionSize > 2 ? (size_t)rawData.get<u16>(0) : (sectionSize == 1 ? (u16)rawData.get<u8>(0) : 0);
    size_t highestStringWithValidOffset = std::min(numStrings, sectionSize/2);
    strings.clear();
    strings.push_back(nullptr); // Fill the non-existant 0th stringId

    size_t stringNum = 1;
    for ( ; stringNum <= highestStringWithValidOffset; ++stringNum ) // Process all strings with valid offsets
    {
        size_t offsetPos = 2*stringNum;
        u16 stringOffset = rawData.get<u16>(offsetPos);
        loadString(strings, rawData, stringOffset, sectionSize);
    }
    if ( highestStringWithValidOffset < numStrings ) // Some offsets aren't within bounds
    {
        if ( sectionSize % 2 > 0 ) // Can read one byte of an offset
        {
            stringNum ++;
            u16 stringOffset = (u16)rawData.get<u8>(sectionSize-1);
            loadString(strings, rawData, stringOffset, sectionSize);
        }
        for ( ; stringNum <= numStrings; ++stringNum ) // Any remaining strings are fully out of bounds
            strings.push_back(nullptr);
    }
}

void Strings::synchronizeToKstrBuffer(buffer &rawData, StrCompressionElevatorPtr compressionElevator, u32 requestedCompressionFlags, u32 allowedCompressionFlags)
{
    /**
        Uses the standard KSTR format
        
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        StringProperties[numStrings] stringProperties; // String properties
        void[] stringData; // List of strings, each null terminated
    */

    size_t numStrings = kStrings.size();
    if ( numStrings > Chk::MaxStrings )
        throw MaximumStringsExceeded(ChkSection::getNameString(SectionName::KSTR), kStrings.size(), Chk::MaxStrings);

    size_t totalCharacterSpace = 0;
    for ( auto string : kStrings )
    {
        if ( string != nullptr )
            totalCharacterSpace += string->length() + 1;
    }

    constexpr size_t totalBytesAvailable = u32_max - 9; // u32_max for the maximum size of string offsets minus 9 (8 for versions and numStrings, one for NUL char)

    if ( totalCharacterSpace > totalBytesAvailable )
        throw MaximumCharactersExceeded(ChkSection::getNameString(SectionName::KSTR), totalCharacterSpace, totalBytesAvailable);

    size_t totalOffsetSpace = 2 * numStrings;
    if ( totalOffsetSpace + totalCharacterSpace > totalBytesAvailable )
        throw MaximumOffsetAndCharsExceeded(ChkSection::getNameString(SectionName::KSTR), numStrings, totalCharacterSpace, totalBytesAvailable);

    buffer newStringSection;
    buffer characterData;
    u32 characterDataStart = 2 + (u32)totalOffsetSpace;
    newStringSection.add<u32>((u32)numStrings); // Add numStrings
    characterData.add<s8>(0); // Add initial NUL character
    for ( auto string : kStrings )
    {
        size_t stringLength = string != nullptr ? string->length() : 0;
        if ( stringLength == 0 )
            newStringSection.add<u32>(characterDataStart); // Point this string to initial NUL character
        else // stringLength > 0
        {
            newStringSection.add<u32>(u32(characterDataStart + characterData.size())); // Set the offset this string will be at
            characterData.addStr(string->str, stringLength+1); // Add the string plus its NUL terminator
        }
    }
    newStringSection.addStr((char*)characterData.getPtr(0), characterData.size());
    characterData.flush();
    rawData.takeAllData(newStringSection);
}

void Strings::synchronizeFromKstrBuffer(const buffer &rawData)
{
    size_t sectionSize = (size_t)rawData.size();
    size_t numStrings = sectionSize > 4 ? (size_t)rawData.get<u32>(0) : 0;
    size_t highestStringWithValidOffset = std::min(numStrings, (sectionSize-4)/4);
    kStrings.clear();
    kStrings.push_back(nullptr); // Fill the non-existant 0th stringId

    size_t stringNum = 1;
    for ( ; stringNum <= highestStringWithValidOffset; ++stringNum ) // Process all strings with valid offsets
    {
        size_t offsetPos = 4+4*stringNum;
        u32 stringOffset = rawData.get<u32>(offsetPos);
        loadString(kStrings, rawData, stringOffset, sectionSize);
    }
    if ( highestStringWithValidOffset < numStrings ) // Some offsets aren't within bounds
    {
        if ( sectionSize % 2 > 0 ) // Can read one byte of an offset
        {
            stringNum ++;
            u16 stringOffset = (u16)rawData.get<u8>(sectionSize-1);
            loadString(kStrings, rawData, stringOffset, sectionSize);
        }
        for ( ; stringNum <= numStrings; ++stringNum ) // Any remaining strings are fully out of bounds
            kStrings.push_back(nullptr);
    }
}

const std::vector<u32> Strings::compressionFlagsProgression = {
    (u32)StrCompressFlag::None,
    (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick | (u32)StrCompressFlag::DuplicateStringRecycling,
    (u32)StrCompressFlag::SubStringRecycling,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking,
    (u32)StrCompressFlag::SubStringRecycling | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::ReverseStacking | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OffsetInterlacing,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::OffsetInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SpareShuffledInterlacing,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::SizeBytesRecycling | (u32)StrCompressFlag::LastStringTrick,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing,
    (u32)StrCompressFlag::SubShuffledInterlacing | (u32)StrCompressFlag::SpareShuffledInterlacing | (u32)StrCompressFlag::OrderShuffledInterlacing
        | (u32)StrCompressFlag::SizeBytesRecycling
};

void Strings::loadString(std::deque<ScStrPtr> &stringContainer, const buffer &rawData, const u16 &stringOffset, const size_t &sectionSize)
{
    if ( (size_t)stringOffset < sectionSize )
    {
        s64 nextNulPos = 0;
        if ( rawData.getNext('\0', (s64)stringOffset, nextNulPos) ) // Has NUL terminator
        {
            if ( nextNulPos > stringOffset ) // Regular string
                stringContainer.push_back(ScStrPtr(new ScStr(rawData.getString((s64)stringOffset, nextNulPos-stringOffset))));
            else // Zero length string
                stringContainer.push_back(nullptr);
        }
        else if ( sectionSize > stringOffset ) // String ends where section ends
            stringContainer.push_back(ScStrPtr(new ScStr(rawData.getString((s64)stringOffset, sectionSize-stringOffset))));
        else // Character data would be out of bounds
            stringContainer.push_back(nullptr);
    }
}

size_t Strings::getNextUnusedStringId(std::bitset<Chk::MaxStrings> &stringIdUsed, Chk::Scope storageScope, bool checkBeyondScopedCapacity, size_t firstChecked)
{
    if ( ((u32)storageScope & (u32)Chk::Scope::Game) == (u32)Chk::Scope::Game )
    {
        size_t limit = checkBeyondScopedCapacity ? Chk::MaxStrings : getCapacity(Chk::Scope::Game);
        for ( size_t i=firstChecked; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
                return i;
        }
    }
    if ( ((u32)storageScope & (u32)Chk::Scope::Editor) == (u32)Chk::Scope::Editor )
    {
        size_t limit = checkBeyondScopedCapacity ? Chk::MaxStrings : getCapacity(Chk::Scope::Editor);
        for ( size_t i=firstChecked; i<limit; i++ )
        {
            if ( !stringIdUsed[i] )
                return i;
        }
    }
    return 0;
}

void Strings::resolveParantage()
{
    for ( auto string = strings.begin(); string != strings.end(); ++string )
    {
        auto otherString = string;
        ++otherString;
        for ( ; otherString != strings.end(); ++otherString )
            ScStr::adopt(*string, *otherString);
    }
}

void Strings::resolveParantage(ScStrPtr string)
{
    if ( string != nullptr )
    {
        string->disown();
        for ( auto it = strings.begin(); it != strings.end(); ++it )
        {
            ScStrPtr otherString = *it;
            if ( otherString != nullptr && otherString != string )
                ScStr::adopt(string, otherString);
        }
    }
}

bool Strings::defragmentStr(bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = strings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
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
    return false;
}

bool Strings::defragmentKstr(bool matchCapacityToUsage)
{
    size_t nextCandidateStringId = 0;
    size_t numStrings = kStrings.size();
    std::map<u32, u32> stringIdRemappings;
    for ( size_t i=0; i<numStrings; i++ )
    {
        if ( strings[i] == nullptr )
        {
            for ( size_t j = std::max(i+1, nextCandidateStringId); j < numStrings; j++ )
            {
                if ( strings[j] != nullptr )
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
        remapStringIds(stringIdRemappings, Chk::Scope::Editor);
        return true;
    }
    return false;
}

void Strings::replaceStringId(size_t oldStringId, size_t newStringId)
{
    Chk::SPRP & sprp = this->sprp->asStruct();
    Chk::FORC & forc = players->forc->asStruct();
    Chk::UNIS & unis = properties->unis->asStruct();
    Chk::UNIx & unix = properties->unix->asStruct();
    Chk::WAV & wav = triggers->wav->asStruct();
    Chk::SWNM & swnm = triggers->swnm->asStruct();

    if ( oldStringId != newStringId )
    {
        if ( (size_t)sprp.scenarioNameStringId == oldStringId )
            sprp.scenarioNameStringId = (u16)newStringId;

        if ( (size_t)sprp.scenarioDescriptionStringId == oldStringId )
            sprp.scenarioDescriptionStringId = (u16)newStringId;

        for ( size_t i=0; i<Chk::TotalForces; i++ )
        {
            if ( (size_t)forc.forceString[i] == oldStringId )
                forc.forceString[i] = (u16)newStringId;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( (size_t)unis.nameStringId[i] == oldStringId )
                unis.nameStringId[i] = (u16)newStringId;
        }

        for ( size_t i=0; i<Sc::Unit::TotalTypes; i++ )
        {
            if ( (size_t)unix.nameStringId[i] == oldStringId )
                unix.nameStringId[i] = (u16)newStringId;
        }

        for ( size_t i=0; i<Chk::TotalSounds; i++ )
        {
            if ( (size_t)wav.soundPathStringId[i] == oldStringId )
                wav.soundPathStringId[i] = (u32)newStringId;
        }

        for ( size_t i=0; i<Chk::TotalSwitches; i++ )
        {
            if ( (size_t)swnm.switchName[i] == oldStringId )
                swnm.switchName[i] = (u32)newStringId;
        }

        for ( size_t i=0; i<layers->numLocations(); i++ )
        {
            if ( (size_t)layers->getLocation(i)->stringId == oldStringId )
                layers->getLocation(i)->stringId = (u16)newStringId;
        }

        for ( size_t triggerIndex=0; triggerIndex<triggers->numTriggers(); triggerIndex++ )
        {
            std::shared_ptr<Chk::Trigger> trigger = triggers->getTrigger(triggerIndex);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & action = trigger->actions[actionIndex];
                if ( (size_t)action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::actionUsesStringArg[(size_t)action.actionType] && (size_t)action.stringId == oldStringId )
                        action.stringId = (u32)newStringId;
                    if ( Chk::Action::actionUsesSoundArg[(size_t)action.actionType] && (size_t)action.soundStringId == oldStringId )
                        action.soundStringId = (u32)newStringId;
                }
            }
        }

        for ( size_t briefingTriggerIndex=0; briefingTriggerIndex<triggers->numBriefingTriggers(); briefingTriggerIndex++ )
        {
            std::shared_ptr<Chk::Trigger> briefingTrigger = triggers->getBriefingTrigger(briefingTriggerIndex);
            for ( size_t actionIndex = 0; actionIndex < Chk::Trigger::MaxActions; actionIndex++ )
            {
                Chk::Action & action = briefingTrigger->actions[actionIndex];
                if ( (size_t)action.actionType < Chk::Action::NumActionTypes )
                {
                    if ( Chk::Action::briefingActionUsesStringArg[(size_t)action.actionType] && (size_t)action.stringId == oldStringId )
                        action.stringId = (u32)newStringId;
                    if ( Chk::Action::briefingActionUsesSoundArg[(size_t)action.actionType] && (size_t)action.soundStringId == oldStringId )
                        action.soundStringId = (u32)newStringId;
                }
            }
        }
    }
}

void Strings::remapStringIds(std::map<u32, u32> stringIdRemappings, Chk::Scope storageScope)
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

template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkString (Editor <01>Style)
std::shared_ptr<StringType> Strings::convert(ScStrPtr string)
{
    if ( string == nullptr )
        return nullptr;
    else
    {
        RawString rawString(string->str);
        std::shared_ptr<StringType> outString = std::shared_ptr<StringType>(new StringType());
        ConvertStr<RawString, StringType>(rawString, *outString);
        return outString;
    }
}
template std::shared_ptr<RawString> Strings::convert<RawString>(ScStrPtr string);
template std::shared_ptr<EscString> Strings::convert<EscString>(ScStrPtr string);
template std::shared_ptr<ChkdString> Strings::convert<ChkdString>(ScStrPtr string);
template std::shared_ptr<SingleLineChkdString> Strings::convert<SingleLineChkdString>(ScStrPtr string);

Players::Players(bool useDefault)
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

bool Players::stringUsed(size_t stringId)
{
    return forc->stringUsed(stringId);
}

void Players::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    forc->markUsedStrings(stringIdUsed);
}

void Players::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    forc->remapStringIds(stringIdRemappings);
}

void Players::deleteString(size_t stringId)
{
    forc->deleteString(stringId);
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

std::shared_ptr<Chk::IsomEntry> Terrain::getIsomEntry(size_t isomIndex)
{
    return isom->getIsomEntry(isomIndex);
}

Layers::Layers() : Terrain()
{

}

Layers::Layers(Sc::Terrain::Tileset tileset, u16 width, u16 height) : Terrain(tileset, width, height)
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
    bool updateAnywhereIfAlreadyStandard = (sizeValidationFlags & (u16)SizeValidationFlag::UpdateAnywhereIfAlreadyStandard) == (u16)SizeValidationFlag::UpdateAnywhereIfAlreadyStandard;
    bool updateAnywhere = (sizeValidationFlags & (u16)SizeValidationFlag::UpdateAnywhere) == (u16)SizeValidationFlag::UpdateAnywhere;
    if ( (!updateAnywhereIfAlreadyStandard && updateAnywhere) || (updateAnywhereIfAlreadyStandard && anywhereIsStandardDimensions()) )
        matchAnywhereToDimensions();

    if ( (sizeValidationFlags & (u16)SizeValidationFlag::UpdateOutOfBoundsLocations) == (u16)SizeValidationFlag::UpdateOutOfBoundsLocations )
        downsizeOutOfBoundsLocations();

    if ( (sizeValidationFlags & (u16)SizeValidationFlag::RemoveOutOfBoundsDoodads) == (u16)SizeValidationFlag::RemoveOutOfBoundsDoodads )
        removeOutOfBoundsDoodads();

    if ( (sizeValidationFlags & (u16)SizeValidationFlag::UpdateOutOfBoundsUnits) == (u16)SizeValidationFlag::UpdateOutOfBoundsUnits )
        updateOutOfBoundsUnits();
    else if ( (sizeValidationFlags & (u16)SizeValidationFlag::RemoveOutOfBoundsUnits) == (u16)SizeValidationFlag::RemoveOutOfBoundsUnits )
        removeOutOfBoundsUnits();

    if ( (sizeValidationFlags & (u16)SizeValidationFlag::UpdateOutOfBoundsSprites) == (u16)SizeValidationFlag::UpdateOutOfBoundsSprites )
        updateOutOfBoundsSprites();
    else if ( (sizeValidationFlags & (u16)SizeValidationFlag::RemoveOutOfBoundsSprites) == (u16)SizeValidationFlag::RemoveOutOfBoundsSprites )
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

void Layers::sizeLocationsToScOriginal()
{
    mrgn->sizeToScOriginal();
}

void Layers::sizeLocationsToScHybridOrExpansion()
{
    mrgn->sizeToScHybridOrExpansion();
}

std::shared_ptr<Chk::Location> Layers::getLocation(size_t locationIndex)
{
    return mrgn->getLocation(locationIndex);
}

size_t Layers::addLocation(std::shared_ptr<Chk::Location> location)
{
    return mrgn->addLocation(location);
}

void Layers::insertLocation(size_t locationIndex, std::shared_ptr<Chk::Location> location)
{
    return mrgn->insertLocation(locationIndex, location);
}

void Layers::deleteLocation(size_t locationIndex)
{
    Chk::LocationPtr location = mrgn->getLocation(locationIndex);
    size_t deletedLocationNameStringId = location != nullptr ? location->stringId : Chk::StringId::NoString;
    mrgn->deleteLocation(locationIndex);
    strings->deleteString(deletedLocationNameStringId);
}

void Layers::moveLocation(size_t locationIndexFrom, size_t locationIndexTo)
{
    mrgn->moveLocation(locationIndexFrom, locationIndexTo);
}

void Layers::downsizeOutOfBoundsLocations()
{
    size_t pixelWidth = dim->getPixelWidth();
    size_t pixelHeight = dim->getPixelHeight();
    size_t numLocations = mrgn->numLocations();
    for ( size_t i=0; i<numLocations; i++ )
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

bool Layers::anywhereIsStandardDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation((size_t)Chk::LocationId::Anywhere);
    return anywhere != nullptr && anywhere->left == 0 && anywhere->top == 0 && anywhere->right == dim->getPixelWidth() && anywhere->bottom == dim->getPixelHeight();
}

void Layers::matchAnywhereToDimensions()
{
    std::shared_ptr<Chk::Location> anywhere = mrgn->getLocation((size_t)Chk::LocationId::Anywhere);
    if ( anywhere != nullptr )
    {
        anywhere->left = 0;
        anywhere->top = 0;
        anywhere->right = (u32)dim->getPixelWidth();
        anywhere->bottom = (u32)dim->getPixelHeight();
    }
}

bool Layers::stringUsed(size_t stringId, Chk::Scope storageScope)
{
    switch ( storageScope )
    {
        case Chk::Scope::Either:
        case Chk::Scope::EditorOverGame:
        case Chk::Scope::GameOverEditor: return mrgn->stringUsed(stringId) || strings->ostr->stringUsed(stringId);
        case Chk::Scope::Game: return mrgn->stringUsed(stringId);
        case Chk::Scope::Editor: return strings->ostr->stringUsed(stringId);
        default: return false;
    }
}

void Layers::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    mrgn->markUsedStrings(stringIdUsed);
}

void Layers::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    mrgn->remapStringIds(stringIdRemappings);
}

void Layers::deleteString(size_t stringId)
{
    mrgn->deleteString(stringId);
}

Properties::Properties(bool useDefault)
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
        case Chk::UseExpSection::Both: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); pupx->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel) : upgr->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel) : pupx->setMaxUpgradeLevel(upgradeType, playerIndex, (u8)maxUpgradeLevel); break;
    }
}

void Properties::setStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t playerIndex, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); pupx->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel) : upgr->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel) : pupx->setStartUpgradeLevel(upgradeType, playerIndex, (u8)startUpgradeLevel); break;
    }
}

void Properties::setDefaultMaxUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t maxUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); pupx->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable: pupx != nullptr ? pupx->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel) : upgr->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgr != nullptr ? upgr->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel) : pupx->setDefaultMaxUpgradeLevel(upgradeType, (u8)maxUpgradeLevel); break;
    }
}

void Properties::setDefaultStartUpgradeLevel(Sc::Upgrade::Type upgradeType, size_t startUpgradeLevel, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgr->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); pupx->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::Yes: pupx->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::No: upgr->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::YesIfAvailable:
            pupx != nullptr ? pupx->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel) : upgr->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); break;
        case Chk::UseExpSection::NoIfOrigAvailable:
            upgr != nullptr ? upgr->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel) : pupx->setDefaultStartUpgradeLevel(upgradeType, (u8)startUpgradeLevel); break;
    }
}

void Properties::setPlayerUsesDefaultUpgradeLeveling(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
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

bool Properties::stringUsed(size_t stringId)
{
    return unis->stringUsed(stringId) || unix->stringUsed(stringId);
}

void Properties::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    unis->markUsedStrings(stringIdUsed);
    unix->markUsedStrings(stringIdUsed);
}

void Properties::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    unis->remapStringIds(stringIdRemappings);
    unix->remapStringIds(stringIdRemappings);
}

void Properties::deleteString(size_t stringId)
{
    unis->deleteString(stringId);
    unix->deleteString(stringId);
}

Triggers::Triggers(bool useDefault)
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

Chk::Cuwp Triggers::getCuwp(size_t cuwpIndex)
{
    return uprp->getCuwp(cuwpIndex);
}

void Triggers::setCuwp(size_t cuwpIndex, const Chk::Cuwp &cuwp)
{
    return uprp->setCuwp(cuwpIndex, cuwp);
}

size_t Triggers::addCuwp(const Chk::Cuwp &cuwp, bool fixUsageBeforeAdding)
{
    size_t found = uprp->findCuwp(cuwp);
    if ( found < Sc::Unit::MaxCuwps )
        return found;
    else
    {
        if ( fixUsageBeforeAdding )
            fixCuwpUsage();

        size_t nextUnused = upus->getNextUnusedCuwpIndex();
        if ( nextUnused < Sc::Unit::MaxCuwps )
            uprp->setCuwp(nextUnused, cuwp);

        return nextUnused;
    }
}

void Triggers::fixCuwpUsage()
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
            if ( action.actionType == Chk::Action::Type::CreateUnitWithProperties && action.number < Sc::Unit::MaxCuwps )
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

bool Triggers::stringUsed(size_t stringId)
{
    return wav->stringUsed(stringId) || swnm->stringUsed(stringId) || trig->stringUsed(stringId) || mbrf->stringUsed(stringId);
}

bool Triggers::gameStringUsed(size_t stringId)
{
    return trig->gameStringUsed(stringId) || mbrf->stringUsed(stringId);
}

bool Triggers::editorStringUsed(size_t stringId)
{
    return wav->stringUsed(stringId) || swnm->stringUsed(stringId) || trig->commentStringUsed(stringId);
}

void Triggers::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    wav->markUsedStrings(stringIdUsed);
    swnm->markUsedStrings(stringIdUsed);
    trig->markUsedStrings(stringIdUsed);
    mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    trig->markUsedGameStrings(stringIdUsed);
    mbrf->markUsedStrings(stringIdUsed);
}

void Triggers::markUsedEditorStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    wav->markUsedStrings(stringIdUsed);
    swnm->markUsedStrings(stringIdUsed);
    trig->markUsedCommentStrings(stringIdUsed);
}

void Triggers::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    wav->remapStringIds(stringIdRemappings);
    swnm->remapStringIds(stringIdRemappings);
    trig->remapStringIds(stringIdRemappings);
    mbrf->remapStringIds(stringIdRemappings);
}

void Triggers::deleteString(size_t stringId)
{
    wav->deleteString(stringId);
    swnm->deleteString(stringId);
    trig->deleteString(stringId);
    mbrf->deleteString(stringId);
}
