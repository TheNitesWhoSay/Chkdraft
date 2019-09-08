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


/*// Cached regular section pointers
type(nullptr), ver(nullptr), iver(nullptr), ive2(nullptr), vcod(nullptr), iown(nullptr), ownr(nullptr), era(nullptr),
dim(nullptr), side(nullptr), mtxm(nullptr), puni(nullptr), upgr(nullptr), ptec(nullptr), unit(nullptr), isom(nullptr),
tile(nullptr), dd2(nullptr), thg2(nullptr), mask(nullptr), str(nullptr), uprp(nullptr), upus(nullptr), mrgn(nullptr),
trig(nullptr), mbrf(nullptr), sprp(nullptr), forc(nullptr), wav(nullptr), unis(nullptr), upgs(nullptr), tecs(nullptr),
swnm(nullptr), colr(nullptr), pupx(nullptr), ptex(nullptr), unix(nullptr), upgx(nullptr), tecx(nullptr),

// Cached extended section pointers
kstr(nullptr)*/

Scenario::Scenario() : mapIsProtected(false), caching(false), tailLength(0)
{ // ctor body

}

Scenario::~Scenario()
{

}

Scenario* Scenario::scenario()
{
    return this;
}

bool Scenario::isExpansion()
{
    return versions.isExpansion();
}

bool Scenario::HasVerSection()
{
    return VER != nullptr;
}

bool Scenario::IsScOrig()
{
    return versions.isOriginal();
}

bool Scenario::IsHybrid()
{
    return versions.isHybrid();
}

bool Scenario::IsScExp()
{
    return versions.isExpansion();
}

void Scenario::ChangeToScOrig()
{
    versions.changeTo(Chk::Version::StarCraft_Original);
    /*TYPE->setType(Chk::Type::RAWS);
    VER->setVersion(Chk::Version::StarCraft_Original);
    IVER->setVersion(Chk::IVersion::Current);
    MRGN->sizeToScOriginal();*/
}

void Scenario::ChangeToHybrid()
{
    versions.changeTo(Chk::Version::StarCraft_Hybrid);
    /*TYPE->setType(Chk::Type::RAWS);
    VER->setVersion(Chk::Version::StarCraft_Hybrid);
    IVER->setVersion(Chk::IVersion::Current);
    MRGN->sizeToScHybridOrExpansion();*/
}

void Scenario::ChangeToScExp()
{
    versions.changeTo(Chk::Version::StarCraft_BroodWar);
    /*TYPE->setType(Chk::Type::RAWB);
    VER->setVersion(Chk::Version::StarCraft_BroodWar);
    IVER->setVersion(Chk::IVersion::Current);
    MRGN->sizeToScHybridOrExpansion();*/
}

u16 Scenario::GetMapTitleStrId()
{
    return (u16)strings.getScenarioNameStringId();
}

u16 Scenario::GetMapDescriptionStrId()
{
    return (u16)strings.getScenarioDescriptionStringId();
}

bool Scenario::getMapTitle(ChkdString &dest)
{
//    return strings.getScenarioName<ChkdString>(dest);
    /*dest = "";
    if ( SPRP != nullptr )
    {
        u16 stringNum = GetMapTitleStrId();
        if ( stringNum > 0 )
            return GetString(dest, (u32)stringNum);
    }
    dest = "Untitled Scenario";
    return false;*/
    return false;
}

bool Scenario::getMapDescription(ChkdString &dest)
{
//    return strings.getScenarioDescription<ChkdString>(dest);
    /*dest = "";
    if ( SPRP != nullptr )
    {
        u16 stringNum = GetMapDescriptionStrId();
        if ( stringNum > 0 )
            return GetString(dest, (u32)stringNum);
    }
    dest = "Destroy all enemy buildings.";
    return false;*/
    return false;
}

bool Scenario::SetMapTitle(ChkdString &newMapTitle)
{
//    strings.setScenarioName<ChkdString>(newMapTitle);
//    return true;
    /*u16* mapTitleString = nullptr;
    return SPRP->getPtr<u16>(mapTitleString, 0, 2) &&
        replaceString(newMapTitle, *mapTitleString, false, true);*/
    return false;
}

bool Scenario::SetMapDescription(ChkdString &newMapDescription)
{
//    strings.setScenarioDescription<ChkdString>(newMapDescription);
    return true;
    /*u16* mapDescriptionString = nullptr;
    return SPRP->getPtr<u16>(mapDescriptionString, 2, 2) &&
        replaceString(newMapDescription, *mapDescriptionString, false, true);*/
}

u16 Scenario::getWidth()
{
    return (u16)layers.getTileWidth();
}

u16 Scenario::getHeight()
{
    return (u16)layers.getTileHeight();
}

u16 Scenario::getTileset()
{
    return (u16)layers.getTileset();
}

u16 Scenario::numUnits()
{
    return (u16)layers.numUnits();
}

/*bool Scenario::getUnit(Chk::Unit* &unitRef, u16 index)
{
return UNIT->getPtr(unitRef, index*sizeof(Chk::Unit), sizeof(Chk::Unit));
}*/

bool Scenario::HasLocationSection()
{
    return layers.mrgn != nullptr;
}

/*bool Scenario::getLocationName(u16 locationIndex, RawString &str)
{
/*    u16 locationStringId = 0;
    if ( MRGN->get<u16>(locationStringId, 16 + sizeof(Chk::Location)*(u32)locationIndex) &&
        locationStringId != 0 && GetString(str, locationStringId) )
    {
        return true;
    }

    try
    {
        str = "Location " + std::to_string(locationIndex);
        return true;
    }
    catch ( std::exception ) {}
    return false;
}*/

/*bool Scenario::getLocationName(u16 locationIndex, ChkdString &str)
{
/*    u16 locationStringId = 0;
    if ( MRGN->get<u16>(locationStringId, 16 + sizeof(Chk::Location)*(u32)locationIndex) &&
        locationStringId != 0 && GetString(str, locationStringId) )
    {
        return true;
    }

    try
    {
        str = "Location " + std::to_string(locationIndex);
        return true;
    }
    catch ( std::exception ) {}
    return false;
}*/

/*bool Scenario::MoveLocation(u16 locationIndex, s32 xChange, s32 yChange)
{
/*    Chk::Location* location = nullptr;
    if ( getLocation(location, locationIndex) )
    {
        location->xc1 += xChange;
        location->xc2 += xChange;
        location->yc1 += yChange;
        location->yc2 += yChange;
        return true;
    }
    return false;
}*/

u32 Scenario::GetLocationFieldData(u16 locationIndex, Chk::Location::Field field)
{
/*    u32 pos = sizeof(Chk::Location)*(u32)locationIndex + (u32)locationFieldLoc[field];
    switch ( locationFieldSize[field] )
    {
    case 1: return MRGN->get<u8>(pos); break;
    case 2: return MRGN->get<u16>(pos); break;
    case 4: return MRGN->get<u32>(pos); break;
    }*/
    return 0;
}

bool Scenario::SetLocationFieldData(u16 locationIndex, Chk::Location::Field field, u32 data)
{
/*    u32 pos = sizeof(Chk::Location)*(u32)locationIndex + (u32)locationFieldLoc[field];
    switch ( locationFieldSize[field] )
    {
    case 1: return MRGN->replace<u8>(pos, u8(data)); break;
    case 2: return MRGN->replace<u16>(pos, u16(data)); break;
    case 4: return MRGN->replace<u32>(pos, data); break;
    }*/
    return false;
}

bool Scenario::insertLocation(u16 index, Chk::Location &location, RawString &name)
{
/*    if ( name.size() > 0 )
    {
        u32 stringNum;
        if ( addString(name, stringNum, false) )
        {
            location.stringNum = stringNum;
            return MRGN->replace<Chk::Location>(sizeof(Chk::Location)*(u32)index, location);
        }
    }
    location.stringNum = 0;
    return MRGN->replace<Chk::Location>(sizeof(Chk::Location)*(u32)index, location);*/
    return false;
}

bool Scenario::insertLocation(u16 index, Chk::Location &location, ChkdString &name)
{
/*    RawString rawLocationName;
    return ParseChkdStr(name, rawLocationName) &&
        insertLocation(index, location, rawLocationName);*/
    return false;
}

u8 Scenario::numLocations()
{
/*    u8 numLocs = 0;
    Chk::Location* locRef;
    for ( u16 i=0; i<256; i++ )
    {
        if ( getLocation(locRef, i) )
        {
            if ( locRef->elevation != 0 || locRef->stringNum != 0 ||
                locRef->xc1 != 0 || locRef->xc2 != 0 || locRef->yc1 != 0 || locRef->yc2 != 0 )
            {
                numLocs ++;
            }
        }
        else // No more locations in the section
            break;
    }
    return numLocs;*/
    return 0;
}

u16 Scenario::locationCapacity()
{
//    return u16(MRGN->size() / sizeof(Chk::Location));
    return 0;
}

bool Scenario::locationIsUsed(u16 locationIndex)
{
/*    Chk::Location* locRef;
    if ( getLocation(locRef, locationIndex) )
    {
        if ( locRef->elevation != 0 || locRef->stringNum != 0 ||
            locRef->xc1 != 0 || locRef->xc2 != 0 || locRef->yc1 != 0 || locRef->yc2 != 0 )
        {
            return true;
        }
    }
    return false;*/
    return false;
}

bool Scenario::HasTrigSection()
{
//    return TRIG != nullptr;
    return false;
}

bool Scenario::AddOrReplaceTrigSection(buffer & newTrigSection)
{
/*    triggers.ReplaceData(newTrigSection);
    return true;*/
    return false;
}

u32 Scenario::numTriggers()
{
//    return triggers.numTriggers();
    return 0;
}

/*std::shared_ptr<Chk::Trigger> Scenario::getTrigger(u32 index)
{
//    return triggers.getTrigger(index);
}*/

/*bool Scenario::getActiveComment(Trigger* trigger, RawString &comment)
{
/*    for ( u32 i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        Chk::Action action = trigger->actions[i];
        if ( action.action == (u8)Chk::Action::Type::Comment )
        {
            if ( (action.flags&(u8)Chk::Action::Flags::Disabled) != (u8)Chk::Action::Flags::Disabled )
            {
                if ( action.stringNum != 0 && GetString(comment, action.stringNum) )
                    return true;
                else
                {
                    try {
                        comment = " ";
                        return true;
                    } catch ( std::exception ) { return false; } // Catch bad_alloc and length_error
                }
            }
        }
    }
    return false;
}*/

bool Scenario::getBriefingTrigger(Chk::Trigger* &trigRef, u32 index)
{
//    return MBRF->getPtr(trigRef, index*sizeof(Chk::Trigger), sizeof(Chk::Trigger));
    return false;
}

bool Scenario::hasForceSection()
{
//    return FORC->exists();
    return false;
}

u32 Scenario::getForceStringNum(u8 index)
{
/*    if ( index < 4 )
        return FORC->get<u16>(8+2*index);
    else
        return 0;*/
    return 0;
}

bool Scenario::getForceString(ChkdString &str, u8 forceNum)
{
/*    u32 stringNum = getForceStringNum(forceNum);
    if ( stringNum != 0 )
        return GetString(str, stringNum);
    else
    {
        try
        {
            str = "Force " + std::to_string(forceNum + 1);
            return true;
        }
        catch ( std::exception ) { }
    }*/
    return false;
}

bool Scenario::getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av)
{
/*    if ( forceNum < 4 )
    {
        u8 forceProp;
        if ( FORC->get<u8>(forceProp, 16+forceNum) )
        {
            allied = ((forceProp&(u8)ForceFlags::Allied) == (u8)ForceFlags::Allied);
            vision = ((forceProp&(u8)ForceFlags::SharedVision) == (u8)ForceFlags::SharedVision);
            random = ((forceProp&(u8)ForceFlags::RandomizeStartLocation) == (u8)ForceFlags::RandomizeStartLocation);
            av     = ((forceProp&(u8)ForceFlags::AlliedVictory) == (u8)ForceFlags::AlliedVictory);
            return true;
        }
    }
    return false;*/
    return false;
}

bool Scenario::getPlayerForce(u8 playerNum, u8 &force)
{
/*    if ( playerNum < 8 )
        return FORC->get<u8>(force, playerNum);
    else*/
        return false;
}

bool Scenario::ReplaceUNISSection(buffer &newUNISSection)
{
//    return UNIS->takeAllData(newUNISSection);
    return false;
}

bool Scenario::ReplaceUNIxSection(buffer &newUNIxSection)
{
//    return UNIx->takeAllData(newUNIxSection);
    return false;
}

bool Scenario::ReplacePUNISection(buffer &newPUNISection)
{
//    return PUNI->takeAllData(newPUNISection);
    return false;
}

bool Scenario::getUnitStringNum(u16 unitId, u16 &stringNum)
{
/*    if ( isExpansion() )
        return UNIx->get<u16>(stringNum, unitId*2+(u32)UnitSettingsDataLoc::StringIds);
    else
        return UNIS->get<u16>(stringNum, unitId*2+(u32)UnitSettingsDataLoc::StringIds);*/
    return false;
}

u16 Scenario::strSectionCapacity()
{
//    return STR->get<u16>(0);
    return 0;
}

u32 Scenario::kstrSectionCapacity()
{
//    return KSTR->get<u32>(4);
    return 0;
}

u32 Scenario::stringCapacity()
{
//    return STR->get<u16>(0) + KSTR->get<u32>(4);
    return 0;
}

u32 Scenario::stringCapacity(bool extended)
{
/*    if ( extended )
        return KSTR->get<u32>(4);
    else
        return STR->get<u16>(0);*/
    return 0;
}

bool Scenario::hasStrSection(bool extended)
{
/*    if ( extended )
        return KSTR->exists();
    else
        return STR->exists();*/
    return false;
}

u32 Scenario::strBytesUsed()
{
//    return (u32)STR->size();
    return 0;
}

bool Scenario::GetString(RawString &dest, u32 stringNum)
{
/*    dest.clear();

    char* srcStr;
    size_t length;

    if ( GetStrInfo(srcStr, length, stringNum) )
    {
        try {
            dest.append(srcStr, length);
            return true;
        }
        catch ( std::exception ) { // Catch bad_alloc and length_error
            return false;
        }
    }
    else if ( stringNum == 0 )
    {
        dest.clear();
        return true;
    }

    return false;*/
    return false;
}

bool Scenario::GetString(EscString &dest, u32 stringNum)
{
/*    dest.clear();
    char* srcStr;
    size_t length;

    if ( GetStrInfo(srcStr, length, stringNum) )
    {
        return MakeEscStr(srcStr, length, dest);
    }
    else if ( stringNum == 0 )
    {
        try {
            dest = "";
            return true;
        }
        catch ( std::exception ) {
            return false;
        }
    }
    else
        return false;*/
    return false;
}

bool Scenario::GetString(ChkdString &dest, u32 stringNum)
{
/*    dest.clear();

    char* srcStr = nullptr;
    size_t length = 0;

    if ( GetStrInfo(srcStr, length, stringNum) )
    {
        return MakeChkdStr(srcStr, length, dest);
    }
    else if ( stringNum == 0 )
    {
        dest = "";
        return true;
    }

    return false;*/
    return false;
}

bool Scenario::GetExtendedString(RawString &dest, u32 extendedStringSectionIndex)
{
//    return GetString(dest, 65535 - extendedStringSectionIndex);
    return false;
}

bool Scenario::GetExtendedString(EscString &dest, u32 extendedStringSectionIndex)
{
//    return GetString(dest, 65535 - extendedStringSectionIndex);
    return false;
}

bool Scenario::GetExtendedString(ChkdString &dest, u32 extendedStringSectionIndex)
{
//    return GetString(dest, 65535 - extendedStringSectionIndex);
    return false;
}

u32 Scenario::NumNameableSwitches()
{
//    return u32(SWNM->size() / 4);
    return 0;
}

bool Scenario::getSwitchStrId(u8 switchId, u32 &stringId)
{
//    return SWNM->get<u32>(stringId, 4 * (u32)switchId);
    return false;
}

bool Scenario::getSwitchName(ChkdString &dest, u8 switchID)
{
/*    u32 stringNum = 0;
    if ( SWNM->get<u32>(stringNum, 4 * (u32)switchID) && stringNum != 0 )
        return GetString(dest, stringNum);
    else
        return false;*/
    return false;
}

bool Scenario::setSwitchName(ChkdString &newName, u8 switchId, bool extended)
{
/*    u32 stringNum = 0;
    if ( getSwitchStrId(switchId, stringNum) )
    {
        u32* stringToReplace = nullptr;
        if ( SWNM->getPtr(stringToReplace, 4*(u32)switchId, 4) )
            return replaceString<u32>(newName, *stringToReplace, extended, true);
    }
    return false;*/
    return false;
}

bool Scenario::hasSwitchSection()
{
//    return SWNM->exists();
    return false;
}

bool Scenario::switchHasName(u8 switchId)
{
/*    u32 stringId = 0;
    return getSwitchStrId(switchId, stringId) && stringId != 0;*/
    return false;
}

void Scenario::removeSwitchName(u8 switchId)
{
/*    u32 stringNum = 0;
    if ( getSwitchStrId(switchId, stringNum) && stringNum != 0 )
    {
        SWNM->replace<u32>(4 * (u32)switchId, 0);
        removeUnusedString(stringNum);
    }*/
}

bool Scenario::SwitchUsesExtendedName(u8 switchId)
{
/*    u32 stringId = 0;
    if ( getSwitchStrId(switchId, stringId) && stringId != 0 )
        return isExtendedString(stringId);

    return false;*/
    return false;
}

bool Scenario::ToggleUseExtendedSwitchName(u8 switchId)
{
/*    ChkdString switchName;
    u32 switchStringId = 0;
    if ( getSwitchStrId(switchId, switchStringId) &&
        stringExists(switchStringId) &&
        GetString(switchName, switchStringId) )
    {
        bool wasExtended = isExtendedString(switchStringId);
        removeSwitchName(switchId);
        u32 newSwitchStringId = 0;
        if ( addString(switchName, newSwitchStringId, !wasExtended) )
            return SWNM->replace<u32>(4 * (u32)switchId, newSwitchStringId);
    }
    return false;*/
    return false;
}

bool Scenario::getUnitName(RawString &dest, u16 unitID)
{
/*    buffer& settings = unitSettings();
    if ( unitID < 228 ) // Regular unit
    {
        if ( isExpansion() && UNIS->)
        if ( settings.get<u8>(unitID) == 0 && // Not default unit settings
            settings.get<u16>(unitID * 2 + (u32)UnitSettingsDataLoc::StringIds) > 0 ) // Not default string
        {
            u16 stringID = settings.get<u16>(2 * unitID + (u32)UnitSettingsDataLoc::StringIds);
            if ( GetString(dest, stringID) )
                return true;
        }

        try
        {
            dest = DefaultUnitDisplayNames[unitID];
            return true;
        }
        catch ( std::exception ) {}
    }
    else // Extended unit
    {
        try
        {
            dest = "Unit #" + std::to_string(unitID);
            return true;
        }
        catch ( std::exception ) {}
    }
    return false;*/
    return false;
}

bool Scenario::getUnitName(ChkdString &dest, u16 unitID)
{
/*    buffer& settings = unitSettings();
    if ( unitID < 228 ) // Regular unit
    {
        if ( settings.get<u8>(unitID) == 0 && // Not default unit settings
            settings.get<u16>(unitID*2+(u32)UnitSettingsDataLoc::StringIds) > 0 ) // Not default string
        {
            u16 stringID = settings.get<u16>(2*unitID+(u32)UnitSettingsDataLoc::StringIds);
            if ( GetString(dest, stringID) )
                return true;
        }

        try
        {
            dest = DefaultUnitDisplayNames[unitID];
            return true;
        }
        catch ( std::exception ) {}
    }
    else // Extended unit
    {
        char unitName[16];
        sprintf_s(unitName, 16, "Unit #%u", unitID);
        try
        {
            dest = unitName;
            return true;
        }
        catch ( std::exception ) {}
    }
    return false;*/
    return false;
}

bool Scenario::deleteUnit(u16 index)
{
//    return UNIT->del(index*sizeof(Chk::Unit), sizeof(Chk::Unit));
    return false;
}

/*bool Scenario::SwapUnits(u16 firstIndex, u16 secondIndex)
{
//    return UNIT->swap<Chk::Unit>(((u32)firstIndex)*sizeof(Chk::Unit), (secondIndex)*sizeof(Chk::Unit));
}*/

u32 Scenario::GetUnitFieldData(u16 unitIndex, Chk::Unit::Field field)
{
/*    Chk::Unit unit = getUnit(unitIndex);
    switch ( field )
    {
    case Chk::Unit::Field::ClassId: return unit.classId; break;
    case Chk::Unit::Field::Xc: return unit.xc; break;
    case Chk::Unit::Field::Yc: return unit.yc; break;
    case Chk::Unit::Field::Type: return (u16)unit.type; break;
    case Chk::Unit::Field::RelationFlags: return unit.relationFlags; break;
    case Chk::Unit::Field::ValidStateFlags: return unit.validStateFlags; break;
    case Chk::Unit::Field::ValidFieldFlags: return unit.validFieldFlags; break;
    case Chk::Unit::Field::Owner: return unit.owner; break;
    case Chk::Unit::Field::HitpointPercent: return unit.hitpointPercent; break;
    case Chk::Unit::Field::ShieldPercent: return unit.shieldPercent; break;
    case Chk::Unit::Field::EnergyPercent: return unit.energyPercent; break;
    case Chk::Unit::Field::ResourceAmount: return unit.resourceAmount; break;
    case Chk::Unit::Field::HangerAmount: return unit.hangerAmount; break;
    case Chk::Unit::Field::StateFlags: return unit.stateFlags; break;
    case Chk::Unit::Field::Unused: return unit.unused; break;
    case Chk::Unit::Field::RelationClassId: return unit.relationClassId; break;
    }*/
    return 0;
}

bool Scenario::SetUnitFieldData(u16 unitIndex, Chk::Unit::Field field, u32 data)
{
/*    Chk::Unit unit = getUnit(unitIndex);
    switch ( field )
    {
    case Chk::Unit::Field::ClassId: unit.classId = data; break;
    case Chk::Unit::Field::Xc: unit.xc = data; break;
    case Chk::Unit::Field::Yc: unit.yc = data; break;
    case Chk::Unit::Field::Type: unit.type = (Sc::Unit::Type)data; break;
    case Chk::Unit::Field::RelationFlags: unit.relationFlags = data; break;
    case Chk::Unit::Field::ValidStateFlags: unit.validStateFlags = data; break;
    case Chk::Unit::Field::ValidFieldFlags: unit.validFieldFlags = data; break;
    case Chk::Unit::Field::Owner: unit.owner = data; break;
    case Chk::Unit::Field::HitpointPercent: unit.hitpointPercent = data; break;
    case Chk::Unit::Field::ShieldPercent: unit.shieldPercent = data; break;
    case Chk::Unit::Field::EnergyPercent: unit.energyPercent = data; break;
    case Chk::Unit::Field::ResourceAmount: unit.resourceAmount = data; break;
    case Chk::Unit::Field::HangerAmount: unit.hangerAmount = data; break;
    case Chk::Unit::Field::StateFlags: unit.stateFlags = data; break;
    case Chk::Unit::Field::Unused: unit.unused = data; break;
    case Chk::Unit::Field::RelationClassId: unit.relationClassId = data; break;
    default: return false; break;
    }*/
    return true;
}

bool Scenario::SetUnitHitpoints(u16 unitIndex, u8 newHitpoints)
{
//    return UNIT->replace<u8>(17 + sizeof(Chk::Unit)*(u32)unitIndex, newHitpoints);
    return false;
}

bool Scenario::SetUnitEnergy(u16 unitIndex, u8 newEnergy)
{
//    return UNIT->replace<u8>(19 + sizeof(Chk::Unit)*(u32)unitIndex, newEnergy);
    return false;
}

bool Scenario::SetUnitShields(u16 unitIndex, u8 newShields)
{
//    return UNIT->replace<u8>(18 + sizeof(Chk::Unit)*(u32)unitIndex, newShields);
    return false;
}

bool Scenario::SetUnitResources(u16 unitIndex, u32 newResources)
{
//    return UNIT->replace<u32>(20 + sizeof(Chk::Unit)*(u32)unitIndex, newResources);
    return false;
}

bool Scenario::SetUnitHanger(u16 unitIndex, u16 newHanger)
{
//    return UNIT->replace<u16>(24 + sizeof(Chk::Unit)*(u32)unitIndex, newHanger);
    return false;
}

bool Scenario::SetUnitTypeId(u16 unitIndex, u16 newTypeId)
{
//    return UNIT->replace<u16>(8 + sizeof(Chk::Unit)*(u32)unitIndex, newTypeId);
    return false;
}

bool Scenario::SetUnitXc(u16 unitIndex, u16 newXc)
{
//    return UNIT->replace<u16>(4 + sizeof(Chk::Unit)*(u32)unitIndex, newXc);
    return false;
}

bool Scenario::SetUnitYc(u16 unitIndex, u16 newYc)
{
//    return UNIT->replace<u16>(6 + sizeof(Chk::Unit)*(u32)unitIndex, newYc);
    return false;
}

u16 Scenario::SpriteSectionCapacity()
{
//    return u16(THG2->size() / sizeof(Chk::Sprite));
    return 0;
}

bool Scenario::GetSpriteRef(Chk::Sprite* &spriteRef, u16 index)
{
//    return THG2->getPtr(spriteRef, sizeof(Chk::Sprite)*(u32)index, sizeof(Chk::Sprite));
    return false;
}

bool Scenario::stringIsUsed(u32 stringNum)
{
    // MRGN - location strings
/*    Chk::Location* loc;
    u32 numLocs = u32(MRGN->size()/sizeof(Chk::Location));
    for ( u32 i=0; i<numLocs; i++ )
    {
        if ( getLocation(loc, u8(i)) && loc->stringNum == stringNum )
            return true;
    }

    // TRIG - trigger strings
    Trigger* trig;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == stringNum )
                return true;
            if ( trig->actions[i].wavID == stringNum )
                return true;
        }

        trigNum ++;
    }

    // MBRF - briefing strings
    trigNum = 0;

    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == stringNum )
                return true;
            if ( trig->actions[i].wavID == stringNum )
                return true;
        }

        trigNum ++;
    }

    // SPRP - scenario property strings
    buffer& SPRP = this->SPRP();
    if ( SPRP.get<u16>(0) == stringNum ) // Scenario Name
        return true;
    if ( SPRP.get<u16>(2) == stringNum ) // Scenario Description
        return true;

    // FORC - force strings
    buffer& FORC = this->FORC();
    for ( int i=0; i<4; i++ )
    {
        if ( getForceStringNum(i) == stringNum )
            return true;
    }

    // WAV  - sound strings
    buffer& WAV = this->WAV();
    u32 numWavs = u32(WAV.size()/4);
    for ( u32 i=0; i<numWavs; i++ )
    {
        if ( WAV.get<u32>(i*4) == stringNum )
            return true;
    }

    // UNIS - unit settings strings (vanilla)
    buffer& UNIS = this->UNIS();
    for ( int i=0; i<228; i++ )
    {
        if ( UNIS.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == stringNum )
            return true;
    }

    // SWNM - switch strings
    buffer& SWNM = this->SWNM();
    for ( int i=0; i<256; i++ )
    {
        if ( SWNM.get<u32>(i*4) == stringNum )
            return true;
    }

    // UNIx - unit settings strings (brood war)
    buffer& UNIx = this->UNIx();
    for ( int i=0; i<228; i++ )
    {
        if ( UNIx.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == stringNum )
            return true;
    }
    */
    return false;
}

bool Scenario::isExtendedString(u32 stringNum)
{
/*    return stringNum >= strSectionCapacity() &&
        stringNum < 65536 &&
        KSTR->exists() &&
        (u32(65536-stringNum)) <= KSTR->get<u32>(4);*/
    return false;
}

bool Scenario::stringExists(u32 stringNum)
{
/*    return isExtendedString(stringNum) ||
        ( STR->exists() &&
            stringNum < strSectionCapacity() );*/
    return false;
}

bool Scenario::stringExists(const RawString &str, u32& stringNum)
{
/*    std::vector<StringTableNode> strList;
    addAllUsedStrings(strList, true, true);

    for ( auto &existingString : strList )
    {
        if ( existingString.string.compare(str) == 0 )
        {
            stringNum = existingString.stringNum;
            return true;
        }
    }
    */
    return false;
}

bool Scenario::stringExists(const ChkdString &str, u32 &stringNum)
{
/*    RawString rawStr;
    return ParseChkdStr(str, rawStr) && stringExists(rawStr, stringNum);*/
    return false;
}

bool Scenario::stringExists(const RawString &str, u32& stringNum, bool extended)
{
/*    std::vector<StringTableNode> strList;
    if ( extended )
        addAllUsedStrings(strList, false, true);
    else
        addAllUsedStrings(strList, true, false);

    for ( auto &existingString : strList )
    {
        if ( existingString.string.compare(str) == 0 )
        {
            stringNum = existingString.stringNum;
            return true;
        }
    }
    */
    return false;
}

/*bool Scenario::stringExists(const RawString &str, u32& stringNum, std::vector<StringTableNode> &strList)
{
    for ( auto &existingString : strList )
    {
        if ( existingString.string.compare(str) == 0 )
        {
            stringNum = existingString.stringNum;
            return true;
        }
    }
    return false;
}*/

bool Scenario::FindDifference(ChkdString &str, u32& stringNum)
{
/*    RawString rawSuppliedString, rawStringAtNum;
    if ( ParseChkdStr(str, rawSuppliedString) && GetString(rawStringAtNum, stringNum) )
    {
        if ( rawSuppliedString.compare(rawStringAtNum) == 0 )
            return false;
    }*/
    return true;
}

u32 Scenario::extendedToRegularStr(u32 stringNum)
{
    return 65536 - stringNum;
}

bool Scenario::stringUsedWithLocs(u32 stringNum)
{
/*    Chk::Location* loc;
    for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
    {
        if ( getLocation(loc, u8(i)) && ((u32)loc->stringNum) == stringNum )
            return true;
    }*/
    return true;
}

void Scenario::IncrementIfEqual(u32 lhs, u32 rhs, u32 &counter)
{
    if ( lhs == rhs )
        ++counter;
}

u32 Scenario::amountStringUsed(u32 stringNum)
{
/*    u32 amountStringUsed = 0;
    Chk::Location* loc; // MRGN - location strings
    for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
    {
        if ( getLocation(loc, u8(i)) )
            IncrementIfEqual(loc->stringNum, stringNum, amountStringUsed);
    }
    Trigger* trig; // TRIG - trigger strings
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, amountStringUsed);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, amountStringUsed);
        }
    }
    trigNum = 0; // MBRF - briefing strings
    while ( getBriefingTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, amountStringUsed);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, amountStringUsed);
        }
    }
    // SPRP - scenario property strings
    u16 strId = SPRP->get<u16>(0); // Scenario Name
    IncrementIfEqual(strId, stringNum, amountStringUsed);
    strId = SPRP->get<u16>(2); // Scenario Description
    IncrementIfEqual(strId, stringNum, amountStringUsed);
    for ( int i=0; i<4; i++ ) // FORC - force strings
        IncrementIfEqual(getForceStringNum(i), stringNum, amountStringUsed);
    for ( u32 i=0; i<WAV->size()/4; i++ ) // WAV  - sound strings
        IncrementIfEqual(WAV->get<u32>(i*4), stringNum, amountStringUsed);
    for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
        IncrementIfEqual(UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, amountStringUsed);
    for ( int i=0; i<256; i++ ) // SWNM - switch strings
        IncrementIfEqual(SWNM->get<u32>(i*4), stringNum, amountStringUsed);
    for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
        IncrementIfEqual(UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, amountStringUsed);
        
    return amountStringUsed;*/
    return 0;
}

void Scenario::getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches)
{
/*    locs = trigs = briefs = props = forces = wavs = units = switches = 0;
    Chk::Location* loc; // MRGN - location strings
    for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
    {
        if ( getLocation(loc, u8(i)) )
            IncrementIfEqual(loc->stringNum, stringNum, locs);
    }
    Trigger* trig; // TRIG - trigger strings
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, trigs);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, trigs);
        }
    }
    trigNum = 0; // MBRF - briefing strings
    while ( getBriefingTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, briefs);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, briefs);
        }
    }
    // SPRP - scenario property strings
    u16 strId = SPRP->get<u16>(0); // Scenario Name
    IncrementIfEqual(strId, stringNum, props);
    strId = SPRP->get<u16>(2); // Scenario Description
    IncrementIfEqual(strId, stringNum, props);
    for ( int i=0; i<4; i++ ) // FORC - force strings
        IncrementIfEqual(getForceStringNum(i), stringNum, forces);
    for ( u32 i=0; i<WAV->size()/4; i++ ) // WAV  - sound strings
        IncrementIfEqual(WAV->get<u32>(i*4), stringNum, wavs);
    for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
        IncrementIfEqual(UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, units);
    for ( int i=0; i<256; i++ ) // SWNM - switch strings
        IncrementIfEqual(SWNM->get<u32>(i*4), stringNum, switches);
    for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
        IncrementIfEqual(UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, units);*/
}

bool Scenario::GoodVCOD()
{
/*    Section defaultVCOD = VcodSection::GetDefault();

    if ( !VCOD->exists() ) // Map has no VCOD section
        return false;

    char* defaultData = (char*)defaultVCOD->getPtr(0),
        * inData = (char*)VCOD->getPtr(0);

    s64 defaultSize = defaultVCOD->size();
    if ( VCOD->size() != defaultSize ) // Size mismatch
        return false;

    for ( s64 pos = 0; pos < defaultSize; pos++ )
    {
        if ( defaultData[pos] != inData[pos] )
            return false;
    }
    return true;*/
    return false;
}

bool Scenario::isProtected()
{
//    return mapIsProtected;
    return false;
}

bool Scenario::getPlayerOwner(u8 player, u8& owner)
{
//    return OWNR->get<u8>(owner, (u32)player);
    return false;
}

bool Scenario::getPlayerRace(u8 player, u8& race)
{
//    return SIDE->get<u8>(race, (u32)player);
    return false;
}

bool Scenario::getPlayerColor(u8 player, u8& color)
{
//    return COLR->get<u8>(color, (u32)player);
    return false;
}

bool Scenario::unitUsesDefaultSettings(u16 unitId)
{
//    return unitSettings->get<u8>((u32)unitId) == 1;
    return false;
}

bool Scenario::unitIsEnabled(u16 unitId)
{
//    return PUNI->get<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability) != 0;
    return false;
}

/*UnitEnabledState Scenario::getUnitEnabledState(u16 unitId, u8 player)
{
/*    if ( PUNI->get<u8>(unitId+player*228+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault) == 0 ) // Override
    {
        if ( PUNI->get<u8>((u32)unitId+228*(u32)player) == 0 )
            return UnitEnabledState::Disabled;
        else
            return UnitEnabledState::Enabled;
    }
    else
        return UnitEnabledState::Default;
    return UnitEnabledState::Default;
}*/

bool Scenario::hasUnitSettingsSection()
{
//    return unitSettings->exists();
    return false;
}

bool Scenario::getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints)
{
/*    u32 untrimmedHitpoints;
    if ( unitSettings->get<u32>(untrimmedHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        hitpoints = untrimmedHitpoints/256;
        return true;
    }
    else
        return false;*/
    return false;
}

bool Scenario::getUnitSettingsHitpointByte(u16 unitId, u8 &hitpointByte)
{
/*    u32 untrimmedHitpoints;
    if ( unitSettings->get<u32>(untrimmedHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        hitpointByte = untrimmedHitpoints%256;
        return true;
    }
    else
        return false;*/
    return false;
}

bool Scenario::getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints)
{
//    return unitSettings->get<u16>(shieldPoints, 2*(u32)unitId + (u32)UnitSettingsDataLoc::ShieldPoints);
    return false;
}

bool Scenario::getUnitSettingsArmor(u16 unitId, u8 &armor)
{
//    return unitSettings->get<u8>(armor, (u32)unitId+(u32)UnitSettingsDataLoc::Armor);
    return false;
}

bool Scenario::getUnitSettingsBuildTime(u16 unitId, u16 &buildTime)
{
//    return unitSettings->get<u16>(buildTime, 2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime);
    return false;
}

bool Scenario::getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost)
{
//    return unitSettings->get<u16>(mineralCost, 2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost);
    return false;
}

bool Scenario::getUnitSettingsGasCost(u16 unitId, u16 &gasCost)
{
//    return unitSettings->get<u16>(gasCost, 2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost);
    return false;
}

bool Scenario::getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage)
{
//    return unitSettings->get<u16>(baseDamage, 2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon);
    return false;
}

bool Scenario::getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage)
{
//    return unitSettings->get<u16>(bonusDamage, UnitSettingsDataLocBonusWeapon(isExpansion())+2*weaponId);
    return false;
}

bool Scenario::getUnisStringId(u16 unitId, u16 &stringId)
{
//    return UNIS->get<u16>(stringId, 2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds);
    return false;
}

bool Scenario::getUnixStringId(u16 unitId, u16 &stringId)
{
//    return UNIx->get<u16>(stringId, 2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds);
    return false;
}

bool Scenario::setUnisStringId(u16 unitId, u16 newStringId)
{
//    return UNIS->replace<u16>(2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds, newStringId);
    return false;
}

bool Scenario::setUnixStringId(u16 unitId, u16 newStringId)
{
//    return UNIx->replace<u16>(2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds, newStringId);
    return false;
}

bool Scenario::ReplaceUPGSSection(buffer &newUPGSSection)
{
//    return UPGS->takeAllData(newUPGSSection);
    return false;
}

bool Scenario::ReplaceUPGxSection(buffer &newUPGxSection)
{
//    return UPGx->takeAllData(newUPGxSection);
    return false;
}

bool Scenario::ReplaceUPGRSection(buffer &newUPGRSection)
{
//    return UPGR->takeAllData(newUPGRSection);
    return false;
}

bool Scenario::ReplacePUPxSection(buffer &newPUPxSection)
{
//    return PUPx->takeAllData(newPUPxSection);
    return false;
}

bool Scenario::upgradeUsesDefaultCosts(u8 upgradeId)
{
//    return upgradeSettings->get<u8>((u32)upgradeId) == 1;
    return false;
}

bool Scenario::getUpgradeMineralCost(u8 upgradeId, u16 &mineralCost)
{
/*    return upgradeSettings->get<u16>(mineralCost,
        UpgradeSettingsDataLocMineralCost(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeMineralFactor(u8 upgradeId, u16 &mineralFactor)
{
/*    return upgradeSettings->get<u16>(mineralFactor,
        UpgradeSettingsDataLocMineralFactor(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeGasCost(u8 upgradeId, u16 &gasCost)
{
/*    return upgradeSettings->get<u16>(gasCost,
        UpgradeSettingsDataLocGasCost(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeGasFactor(u8 upgradeId, u16 &gasFactor)
{
/*    return upgradeSettings->get<u16>(gasFactor,
        UpgradeSettingsDataLocGasFactor(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeTimeCost(u8 upgradeId, u16 &timeCost)
{
/*    return upgradeSettings->get<u16>(timeCost,
        UpgradeSettingsDataLocTimeCost(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeTimeFactor(u8 upgradeId, u16 &timeFactor)
{
/*    return upgradeSettings->get<u16>(timeFactor,
        UpgradeSettingsDataLocTimeFactor(isExpansion())+2*(u32)upgradeId);*/
    return false;
}

bool Scenario::getUpgradeDefaultStartLevel(u8 upgradeId, u8 &startLevel)
{
//    return upgradeRestrictions->get<u8>(startLevel, UpgradeSettingsDataLocDefaultStartLevel(isExpansion())+(u32)upgradeId);
    return false;
}

bool Scenario::getUpgradeDefaultMaxLevel(u8 upgradeId, u8 &maxLevel)
{
//    return upgradeRestrictions->get<u8>(maxLevel, UpgradeSettingsDataLocDefaultMaxLevel(isExpansion())+(u32)upgradeId);
    return false;
}

bool Scenario::playerUsesDefaultUpgradeLevels(u8 upgradeId, u8 player)
{
//    return upgradeRestrictions->get<u8>(UpgradeSettingsDataLocPlayerUsesDefault(isExpansion(), player)+(u32)upgradeId) == 1;
    return false;
}

bool Scenario::getUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 &startLevel)
{
//    return upgradeRestrictions->get<u8>(startLevel, UpgradeSettingsDataLocPlayerStartLevel(isExpansion(), player)+(u32)upgradeId);
    return false;
}

bool Scenario::getUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 &maxLevel)
{
//    return upgradeRestrictions->get<u8>(maxLevel, UpgradeSettingsDataLocPlayerMaxLevel(isExpansion(), player)+(u32)upgradeId);
    return false;
}

bool Scenario::ReplaceTECSSection(buffer &newTECSSection)
{
//    return TECS->takeAllData(newTECSSection);
    return false;
}

bool Scenario::ReplaceTECxSection(buffer &newTECxSection)
{
//    return TECx->takeAllData(newTECxSection);
    return false;
}

bool Scenario::ReplacePTECSection(buffer &newPTECSection)
{
//    return PTEC->takeAllData(newPTECSection);
    return false;
}

bool Scenario::ReplacePTExSection(buffer &newPTExSection)
{
//    return PTEx->takeAllData(newPTExSection);
    return false;
}

bool Scenario::techUsesDefaultCosts(u8 techId)
{
//    return techSettings->get<u8>((u32)techId) == 1;
    return false;
}

bool Scenario::getTechMineralCost(u8 techId, u16 &mineralCost)
{
//    return techSettings->get<u16>(mineralCost, TechSettingsDataLocMineralCost(isExpansion())+2*(u32)techId);
    return false;
}

bool Scenario::getTechGasCost(u8 techId, u16 &gasCost)
{
//    return techSettings->get<u16>(gasCost, TechSettingsDataLocGasCost(isExpansion())+2*(u32)techId);
    return false;
}

bool Scenario::getTechTimeCost(u8 techId, u16 &timeCost)
{
//    return techSettings->get<u16>(timeCost, TechSettingsDataLocTimeCost(isExpansion())+2*(u32)techId);
    return false;
}

bool Scenario::getTechEnergyCost(u8 techId, u16 &energyCost)
{
//    return techSettings->get<u16>(energyCost, TechSettingsDataLocEnergyCost(isExpansion())+2*(u32)techId);
    return false;
}

bool Scenario::techAvailableForPlayer(u8 techId, u8 player)
{
//    return techRestrictions->get<u8>(PlayerTechSettingsDataLocAvailableForPlayer(isExpansion(), player)+(u32)techId) == 1;
    return false;
}

bool Scenario::techResearchedForPlayer(u8 techId, u8 player)
{
//    return techRestrictions->get<u8>(PlayerTechSettingsDataLocResearchedForPlayer(isExpansion(), player)+(u32)techId) == 1;
    return false;
}

bool Scenario::techAvailableByDefault(u8 techId)
{
//    return techRestrictions->get<u8>(PlayerTechSettingsDataLocDefaultAvailability(isExpansion())+(u32)techId) == 1;
    return false;
}

bool Scenario::techResearchedByDefault(u8 techId)
{
//    return techRestrictions->get<u8>(PlayerTechSettingsDataLocDefaultReserached(isExpansion())+(u32)techId) == 1;
    return false;
}

bool Scenario::playerUsesDefaultTechSettings(u8 techId, u8 player)
{
//    return techRestrictions->get<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(isExpansion(), player)+(u32)techId) == 1;
    return false;
}

bool Scenario::setTileset(u16 newTileset)
{
//    return ERA->replace<u16>(0, newTileset);
    return false;
}

u16 Scenario::getTile(u16 xc, u16 yc)
{
//    return MTXM->get<u16>(2 * (u32)getWidth() * (u32)yc + 2 * (u32)xc);
    return 0;
}

u16 Scenario::getTileSectionTile(u16 xc, u16 yc)
{
//    return TILE->get<u16>(2 * (u32)getWidth() * (u32)yc + 2 * (u32)xc);
    return 0;
}

bool Scenario::getTile(u16 xc, u16 yc, u16 &tileValue)
{
//    return MTXM->get<u16>(tileValue, 2 * (u32)getWidth()*(u32)yc + 2 * (u32)xc);
    return false;
}

bool Scenario::getTileSectionTile(u16 xc, u16 yc, u16 &tileValue)
{
//    return TILE->get<u16>(tileValue, 2 * (u32)getWidth()*(u32)yc + 2 * (u32)xc);
    return false;
}

bool Scenario::setTile(u16 xc, u16 yc, u16 value)
{
//    TILE->replace<u16>(2 * (u32)getWidth() * (u32)yc + 2 * (u32)xc, value);
//    return MTXM->replace<u16>(2 * (u32)getWidth() * (u32)yc + 2 * (u32)xc, value);
    return false;
}

bool Scenario::setDimensions(u16 newWidth, u16 newHeight)
{
/*    u16 oldWidth = getWidth(), oldHeight = getHeight();
    if ( DIM->replace<u16>(0, newWidth) && DIM->replace<u16>(2, newHeight) )
    {
        buffer& mtxm = MTXM(),
            & tile = TILE(),
            & mask = MASK();

        if ( newWidth > oldWidth )
        {
            u16 additionalWidth = newWidth-oldWidth;
            for ( u16 y=0; y<oldHeight; y++ )
            {
                mtxm.insert<u16>((y*newWidth+oldWidth)*2, 0, additionalWidth);
                tile.insert<u16>((y*newWidth+oldWidth)*2, 0, additionalWidth);
                mask.insert<u8>(y*newWidth+oldWidth, 0, additionalWidth);
            }
        }
        else if ( newWidth < oldWidth )
        {
            u16 removedWidth = oldWidth-newWidth;
            for ( u16 y=0; y<oldHeight; y++ )
            {
                mtxm.delRange((newWidth*y+newWidth)*2, (newWidth*y+oldWidth)*2);
                tile.delRange((newWidth*y+newWidth)*2, (newWidth*y+oldWidth)*2);
                mask.delRange(y*newWidth+newWidth, newWidth*y+oldWidth);
            }
        }

        if ( newHeight > oldHeight )
        {
            mtxm.add<u16>(0, (newHeight-oldHeight)*newWidth);
            tile.add<u16>(0, (newHeight-oldHeight)*newWidth);
            mask.add<u8>(0, (newHeight-oldHeight)*newWidth);
        }
        else if ( newHeight < oldHeight )
        {
            mtxm.delRange(newWidth*newHeight*2, mtxm.size());
            tile.delRange(newWidth*newHeight*2, tile.size());
            mask.delRange(newWidth*newHeight, mask.size());
        }

        if ( (((u32)newWidth)/2+1)*(((u32)newHeight)+1)*8 > ISOM->size() )
            ISOM->add<u8>(0, ISOM->size()-((newWidth/2+1)*(newHeight+1)*8));
        else if ( (((u32)newWidth)/2+1)*(((u32)newHeight)+1)*8 < ISOM->size() )
            ISOM->delRange((newWidth/2+1)*(newHeight+1)*8, ISOM->size());

        return true;
    }*/
    return false;
}

bool Scenario::addUnit(u16 unitID, u8 owner, u16 xc, u16 yc, u16 stateFlags)
{
/*    if ( UNIT->size()%sizeof(Chk::Unit) != 0 )
    {
        if ( !UNIT->add<u8>(0, sizeof(Chk::Unit)-UNIT->size()%sizeof(Chk::Unit)) )
            return false;
    }

    Chk::Unit unit = { };
    unit.id = unitID;
    unit.owner = owner;
    unit.xc = xc;
    unit.yc = yc;
    unit.stateFlags = stateFlags;

    return UNIT->add<Chk::Unit&>(unit);*/
    return false;
}

/*bool Scenario::addUnit(Chk::Unit unit)
{
//    return UNIT->add<Chk::Unit>(unit);
}*/

bool Scenario::createLocation(s32 xc1, s32 yc1, s32 xc2, s32 yc2, u16& locationIndex)
{
/*    Chk::Location* curr;
    u16 unusedIndex, afterMaxLoc = u16(MRGN->size()/sizeof(Chk::Location));
    for ( unusedIndex=0; unusedIndex<=afterMaxLoc; unusedIndex++ )
    {
        if ( getLocation(curr, unusedIndex) &&
            curr->stringNum == 0 &&
            curr->elevation == 0 &&
            curr->xc1 == 0 &&
            curr->xc2 == 0 &&
            curr->yc1 == 0 &&
            curr->yc2 == 0 )
        {
            break;
        }
    }

    if ( unusedIndex == afterMaxLoc && MRGN->size()/sizeof(Chk::Location) >= 255 )
        return false;
    else if ( afterMaxLoc < 255 )
        MRGN->add<u8>(0, 20*(255-afterMaxLoc)); // Expand to 255 locations

    if ( getLocation(curr, unusedIndex) )
    {
        try {
            RawString str = "Location " + std::to_string(unusedIndex);
            u32 newStrNum = 0;
            if ( addString(str, newStrNum, false) )
                curr->stringNum = u16(newStrNum);
        } catch ( std::bad_alloc ) {
            curr->stringNum = 0;
        }

        curr->elevation = 0;
        curr->xc1 = xc1;
        curr->yc1 = yc1;
        curr->xc2 = xc2;
        curr->yc2 = yc2;
        locationIndex = unusedIndex;
        return true;
    }*/
    return false;
}

void Scenario::deleteLocation(u16 locationIndex)
{
/*    Chk::Location* locRef;
    if ( getLocation(locRef, locationIndex) )
    {
        locRef->elevation = 0;
        locRef->xc1 = 0;
        locRef->xc2 = 0;
        locRef->yc1 = 0;
        locRef->yc2 = 0;

        u16 stringNum = locRef->stringNum;
        if ( stringNum != 0 )
        {
            locRef->stringNum = 0;
            removeUnusedString(stringNum);
        }
    }*/
}

bool Scenario::removeUnusedString(u32 stringNum)
{
/*    if ( stringIsUsed(stringNum) || stringNum == 0 )
        return false;
    else if ( cleanStringTable(isExtendedString(stringNum)) )
        return true;
    else // Not enough memory for a full clean, just zero-out the string
        return ZeroOutString(stringNum);*/
    return false;
}

void Scenario::forceDeleteString(u32 stringNum)
{
/*    if ( stringNum == 0 )
        return;

    // MRGN - location strings
    Chk::Location* loc;
    u32 numLocs = u32(MRGN->size()/sizeof(Chk::Location));
    for ( u32 i=0; i<numLocs; i++ )
    {
        if ( getLocation(loc, u8(i)) && loc->stringNum == stringNum )
            loc->stringNum = 0;
    }

    // TRIG - trigger strings
    Trigger* trig;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == stringNum )
                trig->actions[i].stringNum = 0;
            if ( trig->actions[i].wavID == stringNum )
                trig->actions[i].wavID = 0;
        }

        trigNum ++;
    }

    // MBRF - briefing strings
    trigNum = 0;

    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == stringNum )
                trig->actions[i].stringNum = 0;
            if ( trig->actions[i].wavID == stringNum )
                trig->actions[i].wavID = 0;
        }

        trigNum ++;
    }

    // SPRP - scenario property strings
    buffer& SPRP = this->SPRP();
    if ( SPRP.get<u16>(0) == stringNum ) // Scenario Name
        SPRP.replace<u16>(0, 0);
    if ( SPRP.get<u16>(2) == stringNum ) // Scenario Description
        SPRP.replace<u16>(2, 0);

    // FORC - force strings
    buffer& FORC = this->FORC();
    for ( int i=0; i<4; i++ )
    {
        if ( getForceStringNum(i) == stringNum )
            FORC.replace<u16>(8+2*i, 0);
    }

    // WAV  - sound strings
    buffer& WAV = this->WAV();
    u32 numWavs = u32(WAV.size()/4);
    for ( u32 i=0; i<numWavs; i++ )
    {
        if ( WAV.get<u32>(i*4) == stringNum )
            WAV.replace<u32>(i*4, 0);
    }

    // UNIS - unit settings strings (vanilla)
    buffer& UNIS = this->UNIS();
    for ( int i=0; i<228; i++ )
    {
        if ( UNIS.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == stringNum )
            UNIS.replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, 0);
    }

    // SWNM - switch strings
    buffer& SWNM = this->SWNM();
    for ( int i=0; i<256; i++ )
    {
        if ( SWNM.get<u32>(i*4) == stringNum )
            SWNM.replace<u32>(i*4, 0);
    }

    // UNIx - unit settings strings (brood war)
    buffer& UNIx = this->UNIx();
    for ( int i=0; i<228; i++ )
    {
        if ( UNIx.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == stringNum )
            UNIx.replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, 0);
    }

    if ( !cleanStringTable(isExtendedString(stringNum)) ) // Scrub string clean from table
        ZeroOutString(stringNum); // Not enough memory for a full clean at this time, just zero-out the string*/
}

template <typename numType> // Strings can, and can only be u16 or u32
bool Scenario::addString(const RawString &str, numType &stringNum, bool extended)
{
/*    u32 newStringNum;
    if ( stringExists(str, newStringNum, extended) )
    {
        stringNum = newStringNum;
        return true;
    }
    else if ( cleanStringTable(extended) )
    {
        StringUsageTable stringTable;
        if ( stringTable.populateTable(this, extended) )
        {
            if ( stringTable.useNext(stringNum) ) // Get an unused entry if possible
            {
                // Check for a need to make a new entry
                if ( extended && (u32)stringNum > KSTR->get<u32>(4) )
                {
                    u32 numStrs = KSTR->get<u32>(4);

                    // Add a new string offset after all the others and a prop struct after all the others
                    if ( !(KSTR->insert<u32>(8+4*numStrs, 0) && KSTR->insert<u32>(12+8*numStrs, 0)) )
                        return false;

                    for ( u32 i=1; i<=numStrs; i++ ) // Increment every string offset by 8 to accommodate the new header info
                        KSTR->replace<u32>(4+4*i, KSTR->get<u32>(4+4*i)+8);

                    numStrs ++;
                    KSTR->replace<u32>(4, numStrs);
                }
                else if ( !extended && (u32)stringNum > STR->get<u16>(0) )
                {
                    if ( STR->size() < 65534 )
                    {
                        u16 numStrs = STR->get<u16>(0);
                        for ( u16 i = 1; i <= numStrs; i++ ) // Increment every string offset by 2 to accommodate the new header info
                            STR->replace<u16>(2*i, STR->get<u16>(2*i)+2);

                        // Add a new string offset after all the others
                        if ( !STR->insert<u16>(2+2*numStrs, 0) )
                            return false;

                        numStrs ++;
                        STR->replace<u16>(0, numStrs);
                    }
                    else
                    {
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                        return false;
                    }
                }

                if ( extended )
                {
                    u32 strOffset = (u32)KSTR->size();

                    if ( KSTR->addStr(str.c_str(), str.size()) )
                    {
                        if ( str[str.size()-1] != '\0' )
                        {
                            if ( !KSTR->add('\0') )
                                return false;
                        }
                        KSTR->replace<u32>(4+4*(u32(stringNum)), strOffset);
                        stringNum = (65536 - (u32)stringNum);
                        return true;
                    }
                }
                else
                {
                    if ( STR->size() < 65536 )
                    {
                        u16 strOffset = u16(STR->size());
                        if ( STR->addStr(str.c_str(), str.size()) )
                        {
                            if ( str[str.size()-1] != '\0' )
                            {
                                if ( !STR->add('\0') )
                                    return false;
                            }
                            STR->replace<u16>(2*(u32(stringNum)), strOffset);
                            return true;
                        }
                    }
                    else
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                }
            }
        }
    }
    */
    return false;
}
template bool Scenario::addString<u16>(const RawString &str, u16 &stringNum, bool extended);
template bool Scenario::addString<u32>(const RawString &str, u32 &stringNum, bool extended);

template <typename numType> // Strings can, and can only be u16 or u32
bool Scenario::addString(const ChkdString &str, numType &stringNum, bool extended)
{
/*    RawString rawString;
    return ParseChkdStr(str, rawString) &&
        addString<numType>(rawString, stringNum, extended);*/
    return false;
}
template bool Scenario::addString<u16>(const ChkdString &str, u16 &stringNum, bool extended);
template bool Scenario::addString<u32>(const ChkdString &str, u32 &stringNum, bool extended);

/*bool Scenario::addNonExistentString(RawString &str, u32& stringNum, bool extended, std::vector<StringTableNode> &strList)
{
    if ( cleanStringTable(extended, strList) )
    {
        StringUsageTable stringTable;
        if ( stringTable.populateTable(this, extended) )
        {
            if ( stringTable.useNext(stringNum) ) // Get an unused entry if possible
            {
                // Check for a need to make a new entry
                if ( extended && stringNum > KSTR->get<u32>(4) )
                {
                    u32 numStrs = KSTR->get<u32>(4);

                    // Add a new string offset after all the others and a prop struct after all the others
                    if ( !(KSTR->insert<u32>(8+4*numStrs, 0) && KSTR->insert<u32>(12+8*numStrs, 0)) )
                        return false;

                    for ( u32 i=1; i<=numStrs; i++ ) // Increment every string offset by 8 to accommodate the new header info
                        KSTR->replace<u32>(4+4*i, KSTR->get<u32>(4+4*i)+8);

                    numStrs ++;
                    KSTR->replace<u32>(4, numStrs);
                }
                else if ( !extended && stringNum > STR->get<u16>(0) )
                {
                    if ( STR->size() < 65534 )
                    {
                        u16 numStrs = STR->get<u16>(0);
                        for ( u16 i=1; i<= numStrs; i++ ) // Increment every string offset by 2 to accommodate the new header info
                            STR->replace<u16>(2*i, STR->get<u16>(2*i)+2);

                        // Add a new string offset after all the others
                        if ( !STR->insert<u16>(2+2*numStrs, 0) )
                            return false;

                        numStrs ++;
                        STR->replace<u16>(0, numStrs);
                    }
                    else
                    {
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                        return false;
                    }
                }

                if ( extended )
                {
                    u32 strOffset = u32(KSTR->size());

                    if ( KSTR->addStr(str.c_str(), str.size()) )
                    {
                        if ( str[str.size()-1] != '\0' )
                        {
                            if ( !KSTR->add('\0') )
                                return false;
                        }
                        KSTR->replace<u32>(4+4*stringNum, strOffset);
                        stringNum = 65536 - stringNum;
                        return true;
                    }
                }
                else
                {
                    if ( STR->size() < 65536 )
                    {
                        u16 strOffset = u16(STR->size());
                        if ( STR->addStr(str.c_str(), str.size()) )
                        {
                            if ( str[str.size()-1] != '\0' )
                            {
                                if ( !STR->add('\0') )
                                    return false;
                            }
                            STR->replace<u16>(2*stringNum, strOffset);
                            return true;
                        }
                    }
                    else
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                }
            }
        }
    }
    return false;
}*/

template <typename numType>
bool Scenario::replaceString(RawString &newString, numType& stringNum, bool extended, bool safeReplace)
{
/*    u32 newStringNum, oldStringNum = (u32)stringNum;

    if ( stringNum != 0 && amountStringUsed(stringNum) == 1 )
    {
        RawString testEqual;
        if ( GetString(testEqual, oldStringNum) && testEqual.compare(newString) == 0 )
            return false;
        else
            return editString<numType>(newString, stringNum, extended, safeReplace);
    }
    else
    {
        std::vector<StringTableNode> strList;
        bool populatedList = addAllUsedStrings(strList, !extended, extended);

        if ( populatedList && stringExists(newString, newStringNum, strList) )
        {
            stringNum = (numType)newStringNum;
            removeUnusedString(oldStringNum);
            return true;
        }
        else if ( populatedList && addNonExistentString(newString, newStringNum, extended, strList) )
        {
            stringNum = (numType)newStringNum;
            removeUnusedString(oldStringNum);
            return true;
        }
        else // String addition failed, try preserving strings, removing the old string, then adding
        {
            strList.clear();
            if ( extended && !KSTR->exists() ) // Addition of the first KSTR() failed above, nothing more can be done
                return false;

            // Get a pointer to the correct string section
            buffer* strings;
            if ( extended )
                strings = &KSTR();
            else
                strings = &STR();

            // Attempt to create a backup string section, or at least a backup string (if safeReplace is not set)
            buffer stringSectionBackup;
            RawString stringBackup;

            bool didBackupSection = strings->exists() && stringSectionBackup.addStr((const char*)strings->getPtr(0), strings->size()),
                didBackupString = false,
                wasExtended = false;

            if ( !didBackupSection && !safeReplace )
            {
                didBackupString = GetString(stringBackup, stringNum);
                wasExtended = isExtendedString(stringNum);
            }

            // Check if deleting the original can be safely done or safeReplace is not set
            if ( didBackupSection || !safeReplace )
            {
                u32 backupStringNum = stringNum;

                // Delete the original string
                stringNum = 0;
                removeUnusedString(oldStringNum);

                if ( addString(newString, newStringNum, extended) ) // Try adding the new string
                {
                    stringNum = (numType)newStringNum;
                    return true;
                }
                else if ( didBackupSection ) // String addition failed, load backup
                {
                    if ( extended )
                        stringSectionBackup.setTitle((u32)SectionName::KSTR);
                    else
                        stringSectionBackup.setTitle((u32)SectionName::STR);

                    strings->takeAllData(stringSectionBackup);
                    stringNum = backupStringNum;
                }
                else if ( didBackupString ) // String addition failed and full section could not be backed up
                {
                    if ( addString(stringBackup, newStringNum, wasExtended) ) // Attempt to re-insert the original string
                        stringNum = (numType)newStringNum;
                }
            }
        }
    }*/
    return false;
}
template bool Scenario::replaceString<u16>(RawString &newString, u16& stringNum, bool extended, bool safeReplace);
template bool Scenario::replaceString<u32>(RawString &newString, u32& stringNum, bool extended, bool safeReplace);

template <typename numType>
bool Scenario::replaceString(ChkdString &newString, numType &stringNum, bool extended, bool safeReplace)
{
/*    RawString rawString;
    return ParseChkdStr(newString, rawString) &&
        replaceString<numType>(rawString, stringNum, extended, safeReplace);*/
    return false;
}
template bool Scenario::replaceString<u16>(ChkdString &newString, u16& stringNum, bool extended, bool safeReplace);
template bool Scenario::replaceString<u32>(ChkdString &newString, u32& stringNum, bool extended, bool safeReplace);

template <typename numType>
bool Scenario::editString(RawString &newString, numType stringNum, bool extended, bool safeEdit)
{
/*    RawString testEqual;
    if ( stringNum == 0 || (GetString(testEqual, stringNum) && testEqual.compare(newString) == 0) )
        return false;
    else if ( stringNum != 0 )
        testEqual.clear();

    std::vector<StringTableNode> strList;
    bool populatedList = addAllUsedStrings(strList, !extended, extended);

    u32 newStringNum = 0;
    if ( populatedList && stringExists(newString, newStringNum, strList) )
    {
        replaceStringNum(stringNum, newStringNum);
        return true;
    }
    else
    {
        if ( populatedList )
        {
            for ( auto &existingString : strList )
            {
                if ( existingString.stringNum == stringNum )
                {
                    existingString.string = newString; // Replace the old string with the new string
                    return rebuildStringTable(strList, extended); // Attempt rebuilding string table
                }
            }
        }
    }*/
    return false;
}
template bool Scenario::editString<u16>(RawString &newString, u16 stringNum, bool extended, bool safeEdit);
template bool Scenario::editString<u32>(RawString &newString, u32 stringNum, bool extended, bool safeEdit);

template <typename numType>
bool Scenario::editString(ChkdString &newString, numType stringNum, bool extended, bool safeEdit)
{
/*    RawString rawString;
    return ParseChkdStr(newString, rawString) &&
        editString(rawString, stringNum, extended, safeEdit);*/
    return false;
}
template bool Scenario::editString<u16>(ChkdString &newString, u16 stringNum, bool extended, bool safeEdit);
template bool Scenario::editString<u32>(ChkdString &newString, u32 stringNum, bool extended, bool safeEdit);

void Scenario::replaceStringNum(u32 toReplace, u32 replacement)
{
/*    Chk::Location* loc;
    for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
    {
        if ( getLocation(loc, u8(i)) )
        {
            if ( loc->stringNum == toReplace )
                loc->stringNum = (u16)replacement;
        }
    }

    Trigger* trig;
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == toReplace )
                trig->actions[i].stringNum = replacement;
            if ( trig->actions[i].wavID == toReplace )
                trig->actions[i].wavID = replacement;
        }

        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            if ( trig->actions[i].stringNum == toReplace )
                trig->actions[i].stringNum = replacement;
            if ( trig->actions[i].wavID == toReplace )
                trig->actions[i].wavID = replacement;
        }

        trigNum ++;
    }

    if ( SPRP->get<u16>(0) == toReplace ) // Scenario Name
        SPRP->replace<u16>(0, (u16)replacement);
    if ( SPRP->get<u16>(2) == toReplace ) // Scenario Description
        SPRP->replace<u16>(2, (u16)replacement);

    for ( int i=0; i<4; i++ )
    {
        if ( getForceStringNum(i) == toReplace )
            FORC->replace<u16>(8+2*i, (u16)replacement);
    }

    for ( u32 i=0; i<WAV->size()/4; i++ )
    {
        if ( WAV->get<u32>(i*4) == toReplace )
            WAV->replace<u32>(i*4, replacement);
    }

    for ( int i=0; i<228; i++ )
    {
        if ( UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == toReplace )
            UNIS->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)replacement);
        if ( UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == toReplace )
            UNIx->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)replacement);
    }

    for ( int i=0; i<256; i++ )
    {
        if ( SWNM->get<u32>(i*4) == toReplace )
            SWNM->replace<u32>(i*4, replacement);
    }*/
}

void Scenario::removeMetaStrings()
{

}

bool Scenario::cleanStringTable(bool extendedTable)
{
 /*   std::vector<StringTableNode> strList;

    if ( extendedTable )
        return addAllUsedStrings(strList, false, true) && rebuildStringTable(strList, extendedTable);
    else
        return addAllUsedStrings(strList, true, false) && rebuildStringTable(strList, extendedTable);*/
    return false;
}

/*bool Scenario::cleanStringTable(bool extendedTable, std::vector<StringTableNode> &strList)
{
    return rebuildStringTable(strList, extendedTable);
}*/

bool Scenario::removeDuplicateStrings()
{
/*    std::vector<StringTableNode> strList;

    if ( addAllUsedStrings(strList, true, true) )
    {
        for ( auto it = strList.begin(); it != strList.end(); it ++ ) // Do all but the last string as it's covered by the rest
        {
            for ( auto tail = it; tail != strList.end(); tail ++ )
            {
                if ( it->stringNum != tail->stringNum && it->string.compare(tail->string) == 0 )
                {
                    if ( it->isExtended && !tail->isExtended ) // Give non-extended precedence
                    {
                        replaceStringNum(it->stringNum, tail->stringNum);
                        removeUnusedString(it->stringNum);
                    }
                    else // Give 'it' precedence
                    {
                        replaceStringNum(tail->stringNum, it->stringNum);
                        removeUnusedString(tail->stringNum);
                    }
                }
            }
        }
        cleanStringTable(true);
        return cleanStringTable(false);
    }
    else*/
        return false;
}

bool Scenario::compressStringTable(bool extendedTable, bool recycleSubStrings)
{
/*    if ( recycleSubStrings )
        return false; // Unimplemented

    removeDuplicateStrings();

    std::vector<StringTableNode> strList;

    StringUsageTable strUsage;
    if ( strUsage.populateTable(this, extendedTable) && addAllUsedStrings(strList, !extendedTable, extendedTable) )
    {
        u32 firstEmpty, lastFragmented;
        while ( strUsage.popFragmentedString(firstEmpty, lastFragmented) )
        {
            if ( ( extendedTable && KSTR->swap<u32>(4+4*lastFragmented, 4+4*firstEmpty) ) ||
                ( !extendedTable && STR->swap<u16>(lastFragmented*2, firstEmpty*2) ) )
            {
                Chk::Location* loc;
                for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
                {
                    if ( getLocation(loc, u8(i)) && loc->stringNum == lastFragmented )
                        loc->stringNum = (u16)firstEmpty;
                }

                Trigger* trig;
                int trigNum = 0;
                while ( getTrigger(trig, trigNum) )
                {
                    for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
                    {
                        if ( trig->actions[i].stringNum == lastFragmented )
                            trig->actions[i].stringNum = firstEmpty;
                        if ( trig->actions[i].wavID == lastFragmented )
                            trig->actions[i].wavID = firstEmpty;
                    }

                    trigNum ++;
                }

                trigNum = 0;
                while ( getBriefingTrigger(trig, trigNum) )
                {
                    for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
                    {
                        if ( trig->actions[i].stringNum == lastFragmented )
                            trig->actions[i].stringNum = firstEmpty;
                        if ( trig->actions[i].wavID == lastFragmented )
                            trig->actions[i].wavID = firstEmpty;
                    }

                    trigNum ++;
                }

                if ( (u32)SPRP->get<u16>(0) == lastFragmented ) // Scenario Name
                    SPRP->replace<u16>(0, (u16)firstEmpty);

                if ( (u32)SPRP->get<u16>(2) == lastFragmented ) // Scenario Description
                    SPRP->replace<u16>(2, (u16)firstEmpty);

                for ( int i=0; i<4; i++ )
                {
                    if ( getForceStringNum(i) == lastFragmented )
                        FORC->replace<u16>(8+2*i, (u16)firstEmpty);
                }

                for ( u32 i=0; i<WAV->size()/4; i++ )
                {
                    if ( WAV->get<u32>(i*4) == lastFragmented )
                        WAV->replace<u32>(i*4, firstEmpty);
                }

                buffer &unitSettings = this->unitSettings();
                for ( int i=0; i<228; i++ )
                {
                    if ( unitSettings.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == lastFragmented )
                        unitSettings.replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)firstEmpty);
                }

                for ( int i=0; i<256; i++ )
                {
                    if ( SWNM->get<u32>(i*4) == lastFragmented )
                        SWNM->replace<u32>(i*4, firstEmpty);
                }
            }
        }

        StringUsageTable altStrUsage;
        if ( altStrUsage.populateTable(this, extendedTable) )
        {
            u32 lastUsed = altStrUsage.lastUsedString();
            if ( lastUsed <= 1024 && STR->get<u16>(0) > 1024 )
                STR->replace<u16>(0, 1024);
        }

        std::vector<StringTableNode> newStrList;
        return addAllUsedStrings(newStrList, !extendedTable, extendedTable) && rebuildStringTable(newStrList, extendedTable);
    }
    */
    return false;
}

bool Scenario::repairStringTable(bool extendedTable)
{
/*    u32 stringNum = 0;

    Chk::Location* loc;
    for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
    {
        if ( getLocation(loc, u8(i)) )
        {
            stringNum = loc->stringNum;
            if ( loc->xc1 != 0 || loc->xc2 != 0 || loc->yc1 != 0 || loc->yc2 != 0 || loc->elevation != 0 )
            {
                if ( RepairString(stringNum, false) )
                    loc->stringNum = (u16)stringNum;
            }
            else if ( stringNum != 0 ) // String for a non-existant location
            {
                loc->stringNum = 0;
                removeUnusedString(loc->stringNum);
            }
        }
    }

    Trigger* trig;
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            stringNum = trig->actions[i].stringNum;
            Chk::Action::Type action = (Chk::Action::Type)trig->actions[i].action;
            if ( action == Chk::Action::Type::Comment || action == Chk::Action::Type::::DisplayTextMessage || action == Chk::Action::Type::::LeaderboardCtrlAtLoc ||
                action == Chk::Action::Type::::LeaderboardCtrl || action == Chk::Action::Type::::LeaderboardKills || action == Chk::Action::Type::::LeaderboardPoints ||
                action == Chk::Action::Type::::LeaderboardResources || action == Chk::Action::Type::::LeaderboardGoalCtrlAtLoc ||
                action == Chk::Action::Type::::LeaderboardGoalCtrl || action == Chk::Action::Type::::LeaderboardGoalKills ||
                action == Chk::Action::Type::::LeaderboardGoalPoints || action == Chk::Action::Type::::LeaderboardGoalResources ||
                action == Chk::Action::Type::::SetMissionObjectives || action == Chk::Action::Type::::SetNextScenario || action == Chk::Action::Type::::Transmission )
            {
                if ( RepairString(stringNum, false) )
                    trig->actions[i].stringNum = stringNum;
            }
            else if ( stringNum != 0 ) // Shouldn't have a string
            {
                trig->actions[i].stringNum = 0;
                removeUnusedString(stringNum);
            }

            stringNum = trig->actions[i].wavID;
            action = (Chk::Action::Type)trig->actions[i].action;
            if ( action == Chk::Action::Type::PlayWav || action == Chk::Action::Type::Transmission )
            {
                if ( RepairString(stringNum, false) )
                    trig->actions[i].wavID = stringNum;
            }
            else if ( stringNum != 0 ) // Shouldn't have a wav string
            {
                trig->actions[i].wavID = 0;
                removeUnusedString(stringNum);
            }
        }
        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            stringNum = trig->actions[i].stringNum;
            u8 action = trig->actions[i].action;
            if ( action == 3 || action == 4 || action == 8 )
            {
                if ( RepairString(stringNum, false) )
                    trig->actions[i].stringNum = stringNum;
            }
            else if ( stringNum != 0 ) // Shouldn't have a string
            {
                trig->actions[i].stringNum = 0;
                removeUnusedString(stringNum);
            }

            stringNum = trig->actions[i].wavID;
            action = trig->actions[i].action;
            if ( action == 2 || action == 8 )
            {
                if ( RepairString(stringNum, false) )
                    trig->actions[i].wavID = stringNum;
            }
            else if ( stringNum != 0 ) // Shouldn't have a wav string
            {
                trig->actions[i].wavID = 0;
                removeUnusedString(stringNum);
            }
        }
        trigNum ++;
    }

    stringNum = SPRP->get<u16>(0); // Scenario Name
    if ( RepairString(stringNum, false) )
        SPRP->replace<u16>(0, (u16)stringNum);

    stringNum = SPRP->get<u16>(2); // Scenario Description
    if ( RepairString(stringNum, false) )
        SPRP->replace<u16>(0, (u16)stringNum);

    for ( int i=0; i<4; i++ )
    {
        stringNum = getForceStringNum(i);
        if ( RepairString(stringNum, false) )
            FORC->replace<u16>(8+2*i, (u16)stringNum);
    }

    for ( u32 i=0; i<WAV->size()/4; i++ )
    {
        RawString str;
        stringNum = WAV->get<u32>(i*4);
        // if stringNum is nonzero and does not have valid contents...
        if ( stringNum != 0 && !( stringExists(stringNum) && GetString(str, stringNum) && str.size() > 0 ) )
        {
            WAV->replace<u32>(i*4, 0);
            removeUnusedString(stringNum);
        }
    }

    buffer &unitSettings = this->unitSettings();
    bool expansionUnitSettings = ( (&unitSettings) == (&UNIx()) );
    if ( expansionUnitSettings )
    {
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            if ( stringNum != 0 && RepairString(stringNum, false) )
                UNIx->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)stringNum);
        }
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            UNIS->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds));
        }
    }
    else
    {
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            if ( stringNum != 0 && RepairString(stringNum, false) )
                UNIS->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)stringNum);
        }
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            UNIx->replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds));
        }
    }

    for ( int i=0; i<256; i++ )
    {
        stringNum = SWNM->get<u32>(i*4);
        if ( stringNum != 0 && RepairString(stringNum, false) )
            SWNM->replace<u32>(i*4, stringNum);
    }

    return compressStringTable(extendedTable, false);*/
    return false;
}

/*bool Scenario::addAllUsedStrings(std::vector<StringTableNode>& strList, bool includeStandard, bool includeExtended)
{
    std::hash<std::string> strHash;
    std::unordered_multimap<size_t, StringTableNode> stringSearchTable;
    strList.reserve(strList.size()+stringCapacity());
    StringTableNode node;
    int numMatching = 0;
    size_t hash = 0;
    u32 standardNumStrings = strSectionCapacity();
    u32 extendedNumStrings = kstrSectionCapacity();

#define AddStrIfOverZero(index)                                                     \
        if ( index > 0 &&                                                               \
             (                                                                          \
               ( includeStandard && (u32(index)) <= standardNumStrings ) ||             \
               ( includeExtended && (u32(65536-index)) <= extendedNumStrings )          \
             )                                                                          \
           )                                                                            \
        {                                                                               \
            if ( GetString(node.string, index) ) {                                      \
                node.stringNum = index;                                                 \
                if ( !strIsInHashTable(node.string, strHash, stringSearchTable) )       \
                {                                                                       \
                    strList.push_back(node); /* add if the string isn't in the list */  \
/*                    stringSearchTable.insert( std::pair<size_t, StringTableNode>(       \
                        strHash(node.string), node) );                                  \
                }                                                                       \
            }                                                                           \
        }

    try
    {
        Chk::Location* loc;
        for ( u32 i=0; i<MRGN->size()/sizeof(Chk::Location); i++ )
        {
            if ( getLocation(loc, u8(i)) )
                AddStrIfOverZero(loc->stringNum);
        }

        Trigger* trig;
        int trigNum = 0;
        while ( getTrigger(trig, trigNum) )
        {
            for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
            {
                AddStrIfOverZero( trig->actions[i].stringNum );
                AddStrIfOverZero( trig->actions[i].wavID );
            }

            trigNum ++;
        }

        trigNum = 0;
        while ( getBriefingTrigger(trig, trigNum) )
        {
            for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
            {
                AddStrIfOverZero( trig->actions[i].stringNum );
                AddStrIfOverZero( trig->actions[i].wavID );
            }

            trigNum ++;
        }

        AddStrIfOverZero( SPRP->get<u16>(0) ); // Scenario Name
        AddStrIfOverZero( SPRP->get<u16>(2) ); // Scenario Description

        for ( int i=0; i<4; i++ )
            AddStrIfOverZero( getForceStringNum(i) );

        for ( u32 i=0; i<WAV->size()/4; i++ )
            AddStrIfOverZero( WAV->get<u32>(i*4) );

        buffer &unitSettings = this->unitSettings();
        for ( int i=0; i<228; i++ )
        {
            AddStrIfOverZero( UNIS->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
            AddStrIfOverZero( UNIx->get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
        }

        for ( int i=0; i<256; i++ )
            AddStrIfOverZero( SWNM->get<u32>(i*4) );

        strList.shrink_to_fit();
        return true;
    }
    catch ( std::bad_alloc )
    {
        return false;
    }
    return false;
}*/

/*bool Scenario::rebuildStringTable(std::vector<StringTableNode> &strList, bool extendedTable)
{
    if ( extendedTable )
    {
        for ( auto &str : strList )
        {
            if ( str.stringNum >= strSectionCapacity() )
                str.stringNum = 65536 - str.stringNum;
        }
    }

    // Sort string list from lowest to highest index
    std::sort(strList.begin(), strList.end(), CompareStrTblNode);

    // Decide what the max strings should be
    u32 prevNumStrings;
    if ( extendedTable )
        prevNumStrings = kstrSectionCapacity();
    else
        prevNumStrings = strSectionCapacity();

    u32 numStrs = 0;
    if ( strList.size() > 0 )
        numStrs = strList.back->stringNum+1;

    if ( numStrs < prevNumStrings ) // Ensure there are at least as many entries as before
        numStrs = prevNumStrings;
    if ( !extendedTable && numStrs < 1024 ) // Ensure there are at least 1024 string entries, unless it's an extended table
        numStrs = 1024;
    else if ( numStrs == 0 )
        numStrs = 1;

    // Create a new string section
    Section newStrSection(new ChkSection(ChkSection::SectionName::STR));
    s64 strPortionOffset = 0;
    if ( extendedTable )
    {
        newStrSection->setTitle((u32)ChkSection::SectionName::KSTR);
        if ( !( newStrSection->add<u32>(2) && newStrSection->add<u32>(numStrs) ) )
            return false; // Out of memory
        strPortionOffset = 8 + 8*numStrs;
    }
    else
    {
        newStrSection->setTitle((u32)ChkSection::SectionName::STR);
        if ( !newStrSection->add<u16>(u16(numStrs)) )
            return false; // Out of memory
        strPortionOffset = 2 + 2*numStrs;
    }

    buffer strPortion("STRp");

    if ( !extendedTable && strPortionOffset+strPortion.size() > 65535 )
    {
        CHKD_ERR("Structural STR limits exceeded, header information for %u strings took up all the room.", numStrs);
        return false; // No room for the initial NUL character
    }

    if ( !strPortion.add<u8>(0) ) // Add initial NUL character
        return false; // Out of memory

                      // Add the string offsets while you build the string portion of the table
    if ( !buildStringTables(strList, extendedTable, newStrSection, strPortion, strPortionOffset, numStrs, false) )
    {
        s64 lengthToSave = extendedTable ? 8 : 2;
        newStrSection->del(lengthToSave, newStrSection->size()-lengthToSave);
        strPortion.del(1, strPortion.size() - 1);
        if ( !buildStringTables(strList, extendedTable, newStrSection, strPortion, strPortionOffset, numStrs, true) )
            return false;
        else
            CHKD_SHOUT("Regular STR section cannot hold the result. Sub-strings have been recycled, \
                these may not work with other editors, reduce the number/size of strings in your map to get back to normal.");
    }

    if ( extendedTable )
    {
        if ( !newStrSection->add<u32>(0, numStrs) ) // Add space for prop stuct
            return false;

        for ( auto &str : strList ) // Fill in prop struct if it's used
        {
            if ( str.propStruct != 0 )
                newStrSection->replace<u32>(4+4*numStrs+4*str.stringNum, str.propStruct);
        }

        if ( KSTR->exists() )
        {
            if ( newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) )
                return KSTR->takeAllData(*newStrSection);
            else
                return false; // Out of memory
        }
        else // Map didn't have an extended string section
        {
            return newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) &&
                AddSection(newStrSection);   
        }
    }
    else
    {
        if ( STR->exists() ) // Map had a string section
        {
            if ( newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) )
                return STR->takeAllData(*newStrSection);
            else
                return false; // Out of memory
        }
        else // Map didn't have a string section
        {
            return newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) &&
                AddSection(newStrSection);
        }
    }
    return false;
}*/

/*bool Scenario::buildStringTables(std::vector<StringTableNode> &strList, bool extendedTable,
    Section &offsets, buffer &strPortion, s64 strPortionOffset, u32 numStrs, bool recycleSubStrings)
{
/*    u32 strId = 1;
    if ( recycleSubStrings )
    {
        std::vector<SubStringPtr> subStrings = SubString::GetSubStrings(strList);

        for ( auto &subStr : subStrings )
        {
            const StringTableNode& str = *(subStr->GetString());
            while ( strId < str.stringNum ) // Add any unused's till the next string, point them to the 'NUL' string
            {
                if ( (extendedTable && offsets->add<u32>((u32)strPortionOffset)) || // Add string offset (extended)
                    (!extendedTable && offsets->add<u16>((u16)strPortionOffset)) ) // Add string offset (regular)
                {
                    strId++;
                }
                else
                    return false; // Out of memory
            }

            if ( !extendedTable && strPortionOffset + strPortion.size() > 65535 )
            {
                CHKD_ERR("Structural STR limits exceeded, header information for %u string took up %u bytes.", numStrs, 2 + numStrs * 2);
                return false; // No room for the next string
            }

            SubStringPtr parent = subStr->GetParent();
            if ( parent != nullptr ) // This string is a sub-string of another
            {
                // Add space for the offset, have it set later
                if ( (extendedTable && offsets->add<u32>(0)) ||  // Add string offset (extended)
                    (!extendedTable && offsets->add<u16>(0)) )  // Add string offset (regular)
                {
                    strId++;
                }
                else
                    return false; // Out of memory
            }
            else // This string is not a sub-string of another
            {
                subStr->userData = (void*)(strPortion.size() + strPortionOffset);

                if ( ((extendedTable && offsets->add<u32>(u32(strPortion.size() + strPortionOffset))) || // Add string offset (extended)
                    (!extendedTable && offsets->add<u16>(u16(strPortion.size() + strPortionOffset)))) && // Add string offset (regular)
                    strPortion.addStr(str.string.c_str(), str.string.size() + 1) ) // Add the string + its NUL char
                {
                    strId++;
                }
                else
                    return false; // Out of memory
            }
        }

        for ( auto &subStr : subStrings )
        {
            const StringTableNode& str = *(subStr->GetString());

            SubStringPtr parent = subStr->GetParent();
            if ( parent != nullptr ) // This string is a sub-string of another
            {
                s64 parentStart = (s64)parent->userData;
                s64 childStart = parentStart + (parent->GetString()->string.size() - str.string.size());

                if ( (extendedTable && offsets->replace<u32>(4+4*(s64)str.stringNum, (u32)childStart)) || // Add string offset (extended)
                    (!extendedTable && offsets->replace<u16>(2*(s64)str.stringNum, (u16)childStart)) ) // Add string offset (regular)
                {

                }
                else
                {
                    CHKD_ERR("Failed to add string header #%u, out of memory.", subStr->GetString()->stringNum);
                    return false; // Out of memory
                }
            }
        }
    }
    else // Not recycling sub-strings
    {
        for ( auto &str : strList )
        {
            while ( strId < str.stringNum ) // Add any unused's till the next string, point them to the 'NUL' string
            {
                if ( (extendedTable && offsets->add<u32>((u32)strPortionOffset)) || // Add string offset (extended)
                    (!extendedTable && offsets->add<u16>((u16)strPortionOffset)) ) // Add string offset (regular)
                {
                    strId++;
                }
                else
                    return false; // Out of memory
            }

            if ( !extendedTable && strPortionOffset + strPortion.size() > 65535 )
            {
                CHKD_ERR("Structural STR limits exceeded, header information for %u string took up %u bytes.", numStrs, 2 + numStrs * 2);
                return false; // No room for the next string
            }

            if ( ((extendedTable && offsets->add<u32>(u32(strPortion.size() + strPortionOffset))) || // Add string offset (extended)
                (!extendedTable && offsets->add<u16>(u16(strPortion.size() + strPortionOffset)))) && // Add string offset (regular)
                strPortion.addStr(str.string.c_str(), str.string.size() + 1) ) // Add the string + its NUL char
            {
                strId++;
            }
            else
                return false; // Out of memory
        }
    }

    // Add any unused's that come after the last string, point them to the 'NUL' string
    while ( strId <= numStrs )
    {
        if ( (extendedTable && offsets->add<u32>((u32)strPortionOffset)) || // Add string offset (extended)
            (!extendedTable && offsets->add<u16>((u16)strPortionOffset)) ) // Add string offset (regular)
        {
            strId++;
        }
        else
            return false; // Out of memory
    }
    return true;
}*/

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


/*bool Scenario::PrepareSaveSectionChanges(ChkVersion chkVersion, std::unordered_map<Section, Section> &sectionsToReplace,
    std::vector<Section> sectionsToAdd, std::vector<ChkSection::SectionName> sectionsToRemove, ScenarioSaverPtr scenarioSaver)
{
/*    Section newMrgn = nullptr;
    Section currMrgn = FindLast(SectionName::MRGN);
    s64 expectedMrgnSize = chkVersion == ChkVersion::StarCraft ? 1280 : 5100;
    if ( currMrgn == nullptr || currMrgn->size() != expectedMrgnSize )
    {
        Section newMrgn = currMrgn == nullptr ? Get_MRGN(getWidth(), getHeight()) : currMrgn->makeCopy(expectedMrgnSize);
        if ( newMrgn == nullptr )
        {
            logger.error("Failed to allocate new location section for saving!");
            return false;
        }
        else if ( currMrgn == nullptr )
            sectionsToAdd.push_back(newMrgn);
        else // if ( currMrgn->size() != expectedMrgnSize )
            sectionsToReplace.insert(std::pair<Section, Section>(currMrgn, newMrgn));
    }



    else if ( )
    {

    }
    if ( currMrgn == nullptr || currMrgn->size() != expectedMrgnSize )
    {
        if ( currMrgn != nullptr && newMrgn != nullptr && newMrgn->copyFrom( )
        {
            logger.error("Failed to prepare locations for saving!");
            return false;
        }
        newMrgn->add<u8>(0, expectedMrgnSize);
        sectionsToAdd.push_back(newMrgn);
    }
    else if ( )
    {
        if ( MRGN->size() > expectedMrgnSize )
        {
            if ( scenarioSaver->confirmRemoveLocations(*this) )
            {
                Section newMrgn = Section(new buffer((u32)SectionName::MRGN));
            }
        }
        if ( newMrgn->size() == 1280 )
            preparedLocations = true;
        else if ( newMrgn->size() < 1280 )
            preparedLocations = newMrgn->trimTo(1280);
        else if ( newMrgn->size() > 1280 ) // Need to remove locations
        {
            if ( scenarioSaver == nullptr )
                preparedLocations = newMrgn->trimTo(1280);
            else
                preparedLocations = scenarioSaver->confirmRemoveLocations(*this) && newMrgn->trimTo(1280);
        }
    }
    else
    {
    }

    if ( !preparedLocations )
    {
        logger.error("Failed to prepare locations for saving!");
        return false;
    }

    removeIver = chkVersion == ChkVersion::BroodWar;
    bool preparedVersions =
        chkVersion == ChkVersion::StarCraft && newType->overwrite("RAWS", 4) && newVer->overwrite(";\0", 2) && newIver->overwrite("\12\0", 2) && newIve2->overwrite("\13\0", 2) ||
        chkVersion == ChkVersion::Hybrid && newType->overwrite("RAWS", 4) && newVer->overwrite("?\0", 2) && newIver->overwrite("\12\0", 2) && newIve2->overwrite("\13\0", 2) ||
        chkVersion == ChkVersion::BroodWar && newType->overwrite("RAWB", 4) && newVer->overwrite("Í\0", 2) && newIve2->overwrite("\13\0", 2);

    if ( !preparedVersions )
    {
        logger.error("Failed to prepare version information for saving!");
        return false;
    }
    
    return true;
}*/

void Scenario::correctMTXM()
{
/*    bool didCorrection;

    do
    {
        didCorrection = false;

        Section lastInstance(nullptr);

        for ( auto &section : allSections ) // Cycle through all sections
        {
            ChkSection::SectionName sectionName = (ChkSection::SectionName)section->titleVal();
            if ( sectionName == ChkSection::SectionName::MTXM )
            {
                if ( lastInstance != nullptr && lastInstance->size() == getWidth()*getHeight()*2 && section->size() != getWidth()*getHeight()*2 )
                    // A second MTXM section was found
                    // The second MTXM section is not long enough to hold all the tiles, but the first is
                    // However, starcraft will still read in all the tiles it can from this section first
                    // Correct by merging the second section into the first section, then deleting the second section
                {
                    char* data = (char*)section->getPtr(0);
                    lastInstance->replaceStr(0, data, section->size());
                    RemoveSection(section);
                    didCorrection = true;
                    break; // curr section was removed, must break out of this loop
                }
                lastInstance = section;
            }
        }
    } while ( didCorrection );*/
}

bool Scenario::setPlayerOwner(u8 player, u8 newOwner)
{
/*    IOWN->replace<u8>((u32)player, newOwner); // This section is not required by starcraft and shouldn't affect function success
    return OWNR->replace<u8>((u32)player, newOwner);*/
    return false;
}

bool Scenario::setPlayerRace(u8 player, u8 newRace)
{
//    return SIDE->replace<u8>((u32)player, newRace);
    return false;
}

bool Scenario::setPlayerColor(u8 player, u8 newColor)
{
//    return COLR->replace<u8>((u32)player, newColor);
    return false;
}

bool Scenario::setPlayerForce(u8 player, u8 newForce)
{
/*    if ( player < 8 )
        return FORC->replace<u8>(player, newForce);
    else
        return false;*/
    return false;
}

bool Scenario::setForceAllied(u8 forceNum, bool allied)
{
/*    if ( forceNum < 4 )
        return FORC->setBit(16+forceNum, 1, allied);
    else
        return false;*/
    return false;
}

bool Scenario::setForceVision(u8 forceNum, bool sharedVision)
{
/*    if ( forceNum < 4 )
        return FORC->setBit(16+forceNum, 3, sharedVision);
    else
        return false;*/
    return false;
}

bool Scenario::setForceRandom(u8 forceNum, bool randomStartLocs)
{
/*    if ( forceNum < 4 )
        return FORC->setBit(16+forceNum, 0, randomStartLocs);
    else
        return false;*/
    return false;
}

bool Scenario::setForceAv(u8 forceNum, bool alliedVictory)
{
/*    if ( forceNum < 4 )
        return FORC->setBit(16+forceNum, 2, alliedVictory);
    else
        return false;*/
    return false;
}

bool Scenario::setForceName(u8 forceNum, ChkdString &newName)
{
/*    u16* mapForceString = nullptr;
    return FORC->getPtr<u16>(mapForceString, 8 + 2 * (u32)forceNum, 2) &&
        replaceString(newName, *mapForceString, false, true);*/
    return false;
}

bool Scenario::setUnitUseDefaults(u16 unitID, bool useDefaults)
{
/*    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = UNIx->replace<u8>((u32)unitID, newValue);
    setNormal = UNIS->replace<u8>((u32)unitID, newValue);

    return (isExpansion() && setExp) || (!isExpansion() && setNormal);*/
    return false;
}

bool Scenario::setUnitEnabled(u16 unitId, bool enabled)
{
/*    if ( enabled )
        return PUNI->replace<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability, 1);
    else
        return PUNI->replace<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability, 0);*/
    return false;
}

/*bool Scenario::setUnitEnabledState(u16 unitId, u8 player, UnitEnabledState unitEnabledState)
{
/*    if ( unitEnabledState == UnitEnabledState::Default )
    {
        if ( PUNI->replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 1) ) // Make player use default for this unit
        {
            PUNI->replace<u8>((u32)unitId+228*(u32)player, 0); // Clear player's unit enabled state (for compression, not necessary)
            return true;
        }
    }
    else if ( unitEnabledState == UnitEnabledState::Enabled )
    {
        return PUNI->replace<u8>((u32)unitId+228*(u32)player, 1) && // Set player's unit enabled state to enabled
            PUNI->replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 0); // Make player override this unit's default
    }
    else if ( unitEnabledState == UnitEnabledState::Disabled )
    {
        return PUNI->replace<u8>((u32)unitId+228*(u32)player, 0) && // Set player's unit enabled state to disabed
            PUNI->replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 0); // Make player override this unit's default
    }
    return false;
}*/

bool Scenario::setUnitSettingsCompleteHitpoints(u16 unitId, u32 completeHitpoints)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
        replacedUNIx = UNIx->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsHitpoints(u16 unitId, u32 hitpoints)
{
/*    u32 completeHitpoints;
    if ( unitSettings->get<u32>(completeHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        u32 hitpointsByte = completeHitpoints%256;
        completeHitpoints = hitpoints*256+(u32)hitpointsByte;

        bool expansion = isExpansion(),
            replacedUNIS = UNIS->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
            replacedUNIx = UNIx->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);
        return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
    }
    else*/
        return false;
}

bool Scenario::setUnitSettingsHitpointByte(u16 unitId, u8 hitpointByte)
{
/*    u32 completeHitpoints;
    if ( unitSettings->get<u32>(completeHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        u32 hitpoints = completeHitpoints/256;
        completeHitpoints = 256*hitpoints+(u32)hitpointByte;

        bool expansion = isExpansion(),
            replacedUNIS = UNIS->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
            replacedUNIx = UNIx->replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);
        return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
    }
    else*/
        return false;
}

bool Scenario::setUnitSettingsShieldPoints(u16 unitId, u16 shieldPoints)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::ShieldPoints, shieldPoints),
        replacedUNIx = UNIx->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::ShieldPoints, shieldPoints);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsArmor(u16 unitId, u8 armor)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u8>((u32)unitId+(u32)UnitSettingsDataLoc::Armor, armor),
        replacedUNIx = UNIx->replace<u8>((u32)unitId+(u32)UnitSettingsDataLoc::Armor, armor);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsBuildTime(u16 unitId, u16 buildTime)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime, buildTime),
        replacedUNIx = UNIx->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime, buildTime);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsMineralCost(u16 unitId, u16 mineralCost)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost, mineralCost),
        replacedUNIx = UNIx->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost, mineralCost);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsGasCost(u16 unitId, u16 gasCost)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost, gasCost),
        replacedUNIx = UNIx->replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost, gasCost);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsBaseWeapon(u32 weaponId, u16 baseDamage)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon, baseDamage),
        replacedUNIx = UNIx->replace<u16>(2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon, baseDamage);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitSettingsBonusWeapon(u32 weaponId, u16 bonusDamage)
{
/*    bool expansion = isExpansion(),
        replacedUNIS = UNIS->replace<u16>(UnitSettingsDataLocBonusWeapon(false)+2*weaponId, bonusDamage),
        replacedUNIx = UNIx->replace<u16>(UnitSettingsDataLocBonusWeapon(true)+2*weaponId, bonusDamage);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );*/
    return false;
}

bool Scenario::setUnitName(u16 unitId, RawString &unitName)
{
/*    u16* originalNameString = nullptr;
    u16* expansionNameString = nullptr;
    bool gotOrig = UNIx->getPtr<u16>(originalNameString, 2 * unitId + (u32)UnitSettingsDataLoc::StringIds, 2);
    bool gotExp = UNIS->getPtr<u16>(expansionNameString, 2 * unitId + (u32)UnitSettingsDataLoc::StringIds, 2);
    bool replacedOrig = false, replacedExp = false;
    gotOrig ? replacedOrig = replaceString(unitName, *originalNameString, false, true) : replacedOrig = false;
    gotExp ? replacedExp = replaceString(unitName, *expansionNameString, false, true) : replacedExp = false;
    return (isExpansion() && replacedExp) || (!isExpansion() && replacedOrig);*/
    return false;
}


bool Scenario::setUnitName(u16 unitId, ChkdString &unitName)
{
/*    RawString rawUnitName;
    if ( ParseChkdStr(unitName, rawUnitName) )
        return setUnitName(unitId, rawUnitName);
    else
        return false;*/
    return false;
}

bool Scenario::setUpgradeUseDefaults(u8 upgradeNum, bool useDefaults)
{
/*    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = UPGx->replace<u8>((u32)upgradeNum, newValue);
    setNormal = UPGS->replace<u8>((u32)upgradeNum, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setUpgradeMineralCost(u8 upgradeId, u16 mineralCost)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocMineralCost(false)+2*(u32)upgradeId, mineralCost),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocMineralCost(true)+2*(u32)upgradeId, mineralCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradeMineralFactor(u8 upgradeId, u16 mineralFactor)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocMineralFactor(false)+2*(u32)upgradeId, mineralFactor),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocMineralFactor(true)+2*(u32)upgradeId, mineralFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradeGasCost(u8 upgradeId, u16 gasCost)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocGasCost(false)+2*(u32)upgradeId, gasCost),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocGasCost(true)+2*(u32)upgradeId, gasCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradeGasFactor(u8 upgradeId, u16 gasFactor)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocGasFactor(false)+2*(u32)upgradeId, gasFactor),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocGasFactor(true)+2*(u32)upgradeId, gasFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradeTimeCost(u8 upgradeId, u16 timeCost)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocTimeCost(false)+2*(u32)upgradeId, timeCost),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocTimeCost(true)+2*(u32)upgradeId, timeCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradeTimeFactor(u8 upgradeId, u16 timeFactor)
{
/*    bool expansion = isExpansion(),
        replacedUPGS = UPGS->replace<u16>(UpgradeSettingsDataLocTimeFactor(false)+2*(u32)upgradeId, timeFactor),
        replacedUPGx = UPGx->replace<u16>(UpgradeSettingsDataLocTimeFactor(true)+2*(u32)upgradeId, timeFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );*/
    return false;
}

bool Scenario::setUpgradePlayerDefaults(u8 upgradeId, u8 player, bool usesDefaultLevels)
{
/*    u8 newValue = 0;
    if ( usesDefaultLevels )
        newValue = 1;

    bool expansion = isExpansion(),
        replacedUPGR = UPGR->replace<u8>(UpgradeSettingsDataLocPlayerUsesDefault(false, player)+(u32)upgradeId, newValue),
        replacedPUPx = PUPx->replace<u8>(UpgradeSettingsDataLocPlayerUsesDefault(true, player)+(u32)upgradeId, newValue);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );*/
    return false;
}

bool Scenario::setUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 newStartLevel)
{
/*    bool expansion = isExpansion(),
        replacedUPGR = UPGR->replace<u8>(UpgradeSettingsDataLocPlayerStartLevel(false, player)+(u32)upgradeId, newStartLevel),
        replacedPUPx = PUPx->replace<u8>(UpgradeSettingsDataLocPlayerStartLevel(true, player)+(u32)upgradeId, newStartLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );*/
    return false;
}

bool Scenario::setUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 newMaxLevel)
{
/*    bool expansion = isExpansion(),
        replacedUPGR = UPGR->replace<u8>(UpgradeSettingsDataLocPlayerMaxLevel(false, player)+(u32)upgradeId, newMaxLevel),
        replacedPUPx = PUPx->replace<u8>(UpgradeSettingsDataLocPlayerMaxLevel(true, player)+(u32)upgradeId, newMaxLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );*/
    return false;
}

bool Scenario::setUpgradeDefaultStartLevel(u8 upgradeId, u8 newStartLevel)
{
/*    bool expansion = isExpansion(),
        replacedUPGR = UPGR->replace<u8>(UpgradeSettingsDataLocDefaultStartLevel(false)+(u32)upgradeId, newStartLevel),
        replacedPUPx = PUPx->replace<u8>(UpgradeSettingsDataLocDefaultStartLevel(true)+(u32)upgradeId, newStartLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );*/
    return false;
}

bool Scenario::setUpgradeDefaultMaxLevel(u8 upgradeId, u8 newMaxLevel)
{
/*    bool expansion = isExpansion(),
        replacedUPGR = UPGR->replace<u8>(UpgradeSettingsDataLocDefaultMaxLevel(false)+(u32)upgradeId, newMaxLevel),
        replacedPUPx = PUPx->replace<u8>(UpgradeSettingsDataLocDefaultMaxLevel(true)+(u32)upgradeId, newMaxLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );*/
    return false;
}

bool Scenario::setTechUseDefaults(u8 techNum, bool useDefaults)
{
/*    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = TECx->replace<u8>((u32)techNum, newValue);
    setNormal = TECS->replace<u8>((u32)techNum, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setTechMineralCost(u8 techId, u16 newMineralCost)
{
/*    bool expansion = isExpansion(),
        replacedTECS = TECS->replace<u16>(TechSettingsDataLocMineralCost(false)+2*(u32)techId, newMineralCost),
        replacedTECx = TECx->replace<u16>(TechSettingsDataLocMineralCost(true)+2*(u32)techId, newMineralCost);

    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );*/
    return false;
}

bool Scenario::setTechGasCost(u8 techId, u16 newGasCost)
{
/*    bool expansion = isExpansion(),
        replacedTECS = TECS->replace<u16>(TechSettingsDataLocGasCost(false)+2*(u32)techId, newGasCost),
        replacedTECx = TECx->replace<u16>(TechSettingsDataLocGasCost(true)+2*(u32)techId, newGasCost);

    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );*/
    return false;
}

bool Scenario::setTechTimeCost(u8 techId, u16 newTimeCost)
{
/*    bool expansion = isExpansion(),
        replacedTECS = TECS->replace<u16>(TechSettingsDataLocTimeCost(false)+2*(u32)techId, newTimeCost),
        replacedTECx = TECx->replace<u16>(TechSettingsDataLocTimeCost(true)+2*(u32)techId, newTimeCost);

    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );*/
    return false;
}

bool Scenario::setTechEnergyCost(u8 techId, u16 newEnergyCost)
{
/*    bool expansion = isExpansion(),
        replacedTECS = TECS->replace<u16>(TechSettingsDataLocEnergyCost(false)+2*(u32)techId, newEnergyCost),
        replacedTECx = TECx->replace<u16>(TechSettingsDataLocEnergyCost(true)+2*(u32)techId, newEnergyCost);

    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );*/
    return false;
}

bool Scenario::setTechAvailableForPlayer(u8 techId, u8 player, bool availableForPlayer)
{
/*    u8 newValue = 0;
    if ( availableForPlayer )
        newValue = 1;

    bool setExp = PTEx->replace<u8>(PlayerTechSettingsDataLocAvailableForPlayer(true, player)+(u32)techId, newValue),
        setNormal = PTEC->replace<u8>(PlayerTechSettingsDataLocAvailableForPlayer(false, player)+(u32)techId, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setTechResearchedForPlayer(u8 techId, u8 player, bool researchedForPlayer)
{
/*    u8 newValue = 0;
    if ( researchedForPlayer )
        newValue = 1;

    bool setExp = PTEx->replace<u8>(PlayerTechSettingsDataLocResearchedForPlayer(true, player)+(u32)techId, newValue),
        setNormal = PTEC->replace<u8>(PlayerTechSettingsDataLocResearchedForPlayer(false, player)+(u32)techId, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setTechDefaultAvailability(u8 techId, bool availableByDefault)
{
/*    u8 newValue = 0;
    if ( availableByDefault )
        newValue = 1;

    bool setExp = PTEx->replace<u8>(PlayerTechSettingsDataLocDefaultAvailability(true)+(u32)techId, newValue),
        setNormal = PTEC->replace<u8>(PlayerTechSettingsDataLocDefaultAvailability(false)+(u32)techId, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setTechDefaultResearched(u8 techId, bool researchedByDefault)
{
/*    u8 newValue = 0;
    if ( researchedByDefault )
        newValue = 1;

    bool setExp = PTEx->replace<u8>(PlayerTechSettingsDataLocDefaultReserached(true)+(u32)techId, newValue),
        setNormal = PTEC->replace<u8>(PlayerTechSettingsDataLocDefaultReserached(false)+(u32)techId, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

bool Scenario::setPlayerUsesDefaultTechSettings(u8 techId, u8 player, bool playerUsesDefaultSettings)
{
/*    u8 newValue = 0;
    if ( playerUsesDefaultSettings )
        newValue = 1;

    bool setExp = PTEx->replace<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(true, player)+(u32)techId, newValue),
        setNormal = PTEC->replace<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(false, player)+(u32)techId, newValue);

    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );*/
    return false;
}

/*bool Scenario::addTrigger(Trigger &trigger)
{
//    return TRIG->add<Trigger&>(trigger);
}

bool Scenario::insertTrigger(u32 triggerId, Trigger &trigger)
{
//    return TRIG->insert<Trigger&>(triggerId*sizeof(Chk::Trigger), trigger);
}*/

bool Scenario::deleteTrigger(u32 triggerId)
{
//    return TRIG->del(triggerId*sizeof(Chk::Trigger), sizeof(Chk::Trigger));
    return false;
}

bool Scenario::copyTrigger(u32 triggerId)
{
/*    Trigger trig;
    return TRIG->get<Trigger>(trig, triggerId*sizeof(Chk::Trigger)) &&
        TRIG->insert<Trigger&>(triggerId*sizeof(Chk::Trigger)+sizeof(Chk::Trigger), trig);*/
    return false;
}

bool Scenario::moveTriggerUp(u32 triggerId)
{
/*    return triggerId > 0 && // Not already first trigger
        TRIG->swap<Trigger>(triggerId*sizeof(Chk::Trigger), (triggerId-1)*sizeof(Chk::Trigger));*/
    return false;
}

bool Scenario::moveTriggerDown(u32 triggerId)
{
/*    return triggerId < ((TRIG->size()/sizeof(Chk::Trigger))-1) && // Not already the last trigger
        TRIG->swap<Trigger>(triggerId*sizeof(Chk::Trigger), (triggerId+1)*sizeof(Chk::Trigger));*/
    return false;
}

bool Scenario::moveTrigger(u32 triggerId, u32 destId)
{
/*    if ( destId == triggerId-1 )
        return moveTriggerUp(triggerId);
    else if ( destId == triggerId+1 )
        return moveTriggerDown(triggerId);
    else
    {
        Trigger trigger;
        if ( triggerId != destId && // Not equivilant ids
            destId < (TRIG->size()/sizeof(Chk::Trigger)) && // Destination less than num trigs
            TRIG->get<Trigger>(trigger, triggerId*sizeof(Chk::Trigger)) && // Get contents of trigger
            TRIG->del<Trigger>(triggerId*sizeof(Chk::Trigger)) ) // Delete trigger from old position
        {
            if ( TRIG->insert<Trigger&>(destId*sizeof(Chk::Trigger), trigger) ) // Attempt to insert at new position
                return true;
            else
                TRIG->insert<Trigger&>(triggerId*sizeof(Chk::Trigger), trigger); // Attempt to recover old position
        }
    }*/
    return false;
}

TrigSectionPtr Scenario::GetTrigSection()
{
//    return TRIG;
    return nullptr;
}

bool Scenario::GetCuwp(u8 cuwpIndex, Chk::Cuwp &outPropStruct)
{
//    return UPRP->get<Chk::Cuwp>(outPropStruct, (u32)cuwpIndex*sizeof(Chk::Cuwp));
    return false;
}

bool Scenario::AddCuwp(Chk::Cuwp &propStruct, u8 &outCuwpIndex)
{
/*    for ( u8 i = 0; i < 64; i++ )
    {
        if ( !IsCuwpUsed(i) )
        {
            if ( UPRP->replace<Chk::Cuwp>((u32)i*sizeof(Chk::Cuwp), propStruct) && UPUS->replace<u8>(i, 1) )
            {
                outCuwpIndex = i;
                return true;
            }
            else
                return false;
        }
    }*/
    return false;
}

bool Scenario::ReplaceCuwp(Chk::Cuwp &propStruct, u8 cuwpIndex)
{
//    return UPRP->replace<Chk::Cuwp>((u32)cuwpIndex*sizeof(Chk::Cuwp), propStruct);
    return false;
}

bool Scenario::IsCuwpUsed(u8 cuwpIndex)
{
//    return UPUS->get<u8>(cuwpIndex) != 0;
    return false;
}

bool Scenario::SetCuwpUsed(u8 cuwpIndex, bool isUsed)
{
/*    if ( isUsed )
        return UPUS->replace<u8>(cuwpIndex, 1);
    else
        return UPUS->replace<u8>(cuwpIndex, 0);*/
    return false;
}

bool Scenario::FindCuwp(const Chk::Cuwp &cuwpToFind, u8 &outCuwpIndex)
{
/*    u8 cuwpCapacity = (u8)CuwpCapacity();
    Chk::Cuwp cuwp = {};
    for ( u8 i = 0; i < cuwpCapacity; i++ )
    {
        if ( UPRP->get<Chk::Cuwp>(cuwp, sizeof(Chk::Cuwp)*(u32)i) &&
            memcmp(&cuwpToFind, &cuwp, sizeof(Chk::Cuwp)) == 0 )
        {
            outCuwpIndex = i;
            return true;
        }
    }*/
    return false;
}

int Scenario::CuwpCapacity()
{
    return 64;
}

int Scenario::NumUsedCuwps()
{
/*    int numUsedCuwps = 0;
    for ( u8 i = 0; i < 64; i++ )
    {
        if ( IsCuwpUsed(i) )
            numUsedCuwps++;
    }
    return numUsedCuwps;*/
    return 0;
}

u32 Scenario::WavSectionCapacity()
{
//    return u32(WAV->size() / 4);
    return 0;
}

bool Scenario::GetWav(u16 wavIndex, u32 &outStringId)
{
//    return WAV->get<u32>(outStringId, (u32)wavIndex * 4) && outStringId > 0;
    return false;
}

bool Scenario::GetWavString(u16 wavIndex, RawString &outString)
{
/*    u32 stringId = 0;
    return WAV->get<u32>(stringId, (u32)wavIndex * 4) && stringId > 0 && GetString(outString, stringId);*/
    return false;
}

bool Scenario::AddWav(u32 stringId)
{
/*    if ( IsStringUsedWithWavs(stringId) )
        return true; // No need to add a duplicate entry

    for ( u32 i = 0; i < 512; i++ )
    {
        if ( !WavHasString(i) && WAV->replace<u32>(i * 4, stringId) )
            return true;
    }*/
    return false;
}

bool Scenario::AddWav(const RawString &wavMpqPath)
{
/*    u32 stringId = 0;
    if ( addString(wavMpqPath, stringId, false) )
    {
        if ( AddWav(stringId) )
        {
            return true;
        }
    }*/
    return false;
}

bool Scenario::RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed)
{
/*    u32 wavStringId = 0;
    if ( WAV->get<u32>(wavStringId, (u32)wavIndex * 4) && wavStringId > 0 )
    {
        if ( !removeIfUsed && MapUsesWavString(wavStringId) )
            return false;

        bool success = WAV->replace<u32>((u32)wavIndex * 4, 0);
        ZeroWavUsers(wavStringId);
        removeUnusedString(wavStringId);
        return success;
    }*/
    return false;
}

bool Scenario::RemoveWavByStringId(u32 wavStringId, bool removeIfUsed)
{
/*    if ( !removeIfUsed && MapUsesWavString(wavStringId) )
        return false;

    if ( wavStringId > 0 )
    {
        bool success = true;
        for ( u32 i = 0; i < 512; i++ )
        {
            u32 stringId = 0;
            if ( WAV->get<u32>(stringId, i * 4) && stringId == wavStringId )
            {
                if ( !WAV->replace<u32>((u32)i * 4, 0) )
                    success = false;
            }
        }
        ZeroWavUsers(wavStringId);
        removeUnusedString(wavStringId);
        return success;
    }*/
    return false;
}

bool Scenario::WavHasString(u16 wavIndex)
{
/*    u32 stringId = 0;
    return WAV->get<u32>(stringId, (u32)wavIndex * 4) && stringId > 0;*/
    return false;
}

bool Scenario::IsStringUsedWithWavs(u32 stringId)
{
/*    for ( u32 i = 0; i < 512; i++ )
    {
        if ( stringId == WAV->get<u32>(i * 4) )
            return true;
    }*/
    return false;
}

bool Scenario::GetWavs(std::map<u32/*stringId*/, u16/*wavIndex*/> &wavMap, bool includePureStringWavs)
{
/*    try
    {
        for ( u16 i = 0; i < 512; i++ )
        {
            u32 stringId = WAV->get<u32>((u32)i * 4);
            if ( stringId > 0 )
                wavMap.insert(std::pair<u32, u16>(stringId, i));
        }

        if ( includePureStringWavs )
        {
            Trigger* trig = nullptr;
            int trigNum = 0;

            while ( getTrigger(trig, trigNum) )
            {
                for ( u8 i=0; i<Chk::Trigger::MaxActions; i++ )
                {
                    Action &action = trig->action(i);
                    if ( action.wavID != 0 )
                    {
                        if ( wavMap.find(action.wavID) == wavMap.end() )
                            wavMap.insert(std::pair<u32, u16>(action.wavID, u16_max));
                    }
                }
                trigNum ++;
            }

            trigNum = 0;
            while ( getBriefingTrigger(trig, trigNum) )
            {
                for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
                {
                    Action &action = trig->action(i);
                    if ( action.wavID != 0 )
                    {
                        if ( wavMap.find(action.wavID) == wavMap.end() )
                            wavMap.insert(std::pair<u32, u16>(action.wavID, u16_max));
                    }
                }
                trigNum ++;
            }
        }

        return true;
    }
    catch ( std::exception )
    {
        wavMap.clear();
    }*/
    return false;
}

bool Scenario::MapUsesWavString(u32 wavStringId)
{
/*    Trigger* trig = nullptr;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( u8 i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringId )
                return true;
        }
        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringId )
                return true;
        }
        trigNum ++;
    }
    */
    return false;
}

void Scenario::ZeroWavUsers(u32 wavStringId)
{
/*    Trigger* trig = nullptr;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( u8 i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringId )
                action.wavID = 0;
        }
        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<Chk::Trigger::MaxActions; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringId )
                action.wavID = 0;
        }
        trigNum ++;
    }*/
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

/*const Section Scenario::r(ChkSection::SectionId sectionId)
{
/*    if ( split[(u32)sectionId] )
        return createVirtual(sectionId);
    else
        return sections[(u32)sectionId];
}*/

/*Section Scenario::w(SectionId sectionId)
{
/*    if ( split[(u32)sectionId] )
        return combine(sectionId);
    else
        return sections[(u32)sectionId];
}*/

/*const Section createVirtual(ChkSection::SectionId sectionId)
{
//    Section section(new ChkSection(ChkSection::getName(sectionId)));
}*/

/*Section combine(ChkSection::SectionId sectionId)
{

}*/


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

bool Scenario::CreateNew(u16 width, u16 height, Sc::Terrain::Tileset tileset, u32 terrain, u32 triggers)
{
/*    Flush();

    era = EraSection::GetDefault(tileset);
    dim = DimSection::GetDefault(width, height);
    mtxm = MtxmSection::GetDefault(width, height);
    isom = IsomSection::GetDefault(width, height);
    tile = TileSection::GetDefault(width, height);
    mask = MaskSection::GetDefault(width, height);
    mrgn = MrgnSection::GetDefault(width, height);

    type = TypeSection::GetDefault(); ver  = VerSection::GetDefault (); iver = IverSection::GetDefault(); ive2 = Ive2Section::GetDefault();
    vcod = VcodSection::GetDefault(); iown = IownSection::GetDefault(); ownr = OwnrSection::GetDefault(); side = SideSection::GetDefault();
    puni = PuniSection::GetDefault(); upgr = UpgrSection::GetDefault(); ptec = PtecSection::GetDefault(); unit = UnitSection::GetDefault();
    dd2  = Dd2Section::GetDefault (); thg2 = Thg2Section::GetDefault(); str  = StrSection::GetDefault (); uprp = UprpSection::GetDefault();
    upus = UpusSection::GetDefault(); trig = TrigSection::GetDefault(); mbrf = MbrfSection::GetDefault(); sprp = SprpSection::GetDefault();
    forc = ForcSection::GetDefault(); wav  = WavSection::GetDefault (); unis = UnisSection::GetDefault(); upgs = UpgsSection::GetDefault();
    tecs = TecsSection::GetDefault(); swnm = SwnmSection::GetDefault(); colr = ColrSection::GetDefault(); pupx = PupxSection::GetDefault();
    ptex = PtexSection::GetDefault(); unix = UnixSection::GetDefault(); upgx = UpgxSection::GetDefault(); tecx = TecxSection::GetDefault();

    if ( type != nullptr && ver != nullptr && iver != nullptr && ive2 != nullptr && vcod != nullptr && iown != nullptr && ownr != nullptr && era != nullptr &&
        dim != nullptr && side != nullptr && mtxm != nullptr && puni != nullptr && upgr != nullptr && ptec != nullptr && unit != nullptr && isom != nullptr &&
        tile != nullptr && dd2 != nullptr && thg2 != nullptr && mask != nullptr && str != nullptr && uprp != nullptr && upus != nullptr && mrgn != nullptr &&
        trig != nullptr && mbrf != nullptr && sprp != nullptr && forc != nullptr && wav != nullptr && unis != nullptr && upgs != nullptr && tecs != nullptr &&
        swnm != nullptr && colr != nullptr && pupx != nullptr && ptex != nullptr && unix != nullptr && upgx != nullptr && tecx != nullptr )
    {
        try {
            allSections = {
                type, ver, iver, ive2, vcod, iown, ownr, era,
                dim, side, mtxm, puni, upgr, ptec, unit, isom,
                tile, dd2, thg2, mask, str, uprp, upus, mrgn,
                trig, mbrf, sprp, forc, wav, unis, upgs, tecs,
                swnm, colr, pupx, ptex, unix, upgx, tecx
            };

            caching = true;
            return true;

        } catch ( std::exception &e ) {}
    }
    Flush();
    logger.error << "Failed to allocate new Scenario!" << std::endl;*/
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

/*Section Scenario::FindLast(ChkSection::SectionName sectionName)
{
/*    for ( auto section = allSections.rbegin(); section != allSections.rend(); ++section )
    {
        if ( (*section)->titleVal() == (u32)sectionName )
            return *section;
    }
    return nullptr;
}*/

/*bool Scenario::RemoveSection(ChkSection::SectionName sectionName)
{
/*    bool erased = false;
    auto section = allSections.begin();
    while ( section != allSections.end() )
    {
        if ( (*section)->titleVal() == (u32)sectionName )
        {
            section = allSections.erase(section);
            erased = true;
        }
        else
            ++section;
    }
    return erased;
}*/

bool Scenario::RemoveSection(Section sectionToRemove)
{
/*    auto sectionFound = std::find(allSections.begin(), allSections.end(), sectionToRemove);
    if ( sectionFound != allSections.end() )
    {
        allSections.erase(sectionFound);
        return true;
    }
    return false;*/
    return false;
}

bool Scenario::AddSection(Section sectionToAdd)
{
/*    try {
        allSections.push_back(sectionToAdd);
        CacheSections();
        return true;
    }
    catch ( std::exception ) { }*/
    return false;
}

/*Section Scenario::AddSection(ChkSection::SectionName sectionName)
{
/*    try {
        Section newSection(new ChkSection(sectionName));
        allSections.push_back(newSection);
        CacheSections();
        return newSection;
    }
    catch ( std::bad_alloc ) { } // Out of memory
    return nullptr;
}*/


bool Scenario::AddOrReplaceSection(Section section)
{
/*    section->getName();
    Section lastInstance = FindLast(section->getName());
    auto section = allSections.rbegin();
    while ( section != allSections.rend() )
    {

        ++section;
    }*/
    return false;
}

void Scenario::Flush()
{
/*    allSections.clear();
    CacheSections();
    mapIsProtected = false;*/
}

void Scenario::CacheSections()
{
/*    type = FindLast(ChkSection::SectionName::TYPE); ver = FindLast(ChkSection::SectionName::VER);
    iver = FindLast(ChkSection::SectionName::IVER); ive2 = FindLast(ChkSection::SectionName::IVE2);
    vcod = FindLast(ChkSection::SectionName::VCOD); iown = FindLast(ChkSection::SectionName::IOWN);
    ownr = FindLast(ChkSection::SectionName::OWNR); era = FindLast(ChkSection::SectionName::ERA);
    dim = FindLast(ChkSection::SectionName::DIM); side = FindLast(ChkSection::SectionName::SIDE);
    mtxm = FindLast(ChkSection::SectionName::MTXM); puni = FindLast(ChkSection::SectionName::PUNI);
    upgr = FindLast(ChkSection::SectionName::UPGR); ptec = FindLast(ChkSection::SectionName::PTEC);
    unit = FindLast(ChkSection::SectionName::UNIT); isom = FindLast(ChkSection::SectionName::ISOM);
    tile = FindLast(ChkSection::SectionName::TILE); dd2 = FindLast(ChkSection::SectionName::DD2);
    thg2 = FindLast(ChkSection::SectionName::THG2); mask = FindLast(ChkSection::SectionName::MASK);
    str = FindLast(ChkSection::SectionName::STR); uprp = FindLast(ChkSection::SectionName::UPRP);
    upus = FindLast(ChkSection::SectionName::UPUS); mrgn = FindLast(ChkSection::SectionName::MRGN);
    trig = FindLast(ChkSection::SectionName::TRIG); mbrf = FindLast(ChkSection::SectionName::MBRF);
    sprp = FindLast(ChkSection::SectionName::SPRP); forc = FindLast(ChkSection::SectionName::FORC);
    wav = FindLast(ChkSection::SectionName::WAV); unis = FindLast(ChkSection::SectionName::UNIS);
    upgs = FindLast(ChkSection::SectionName::UPGS); tecs = FindLast(ChkSection::SectionName::TECS);
    swnm = FindLast(ChkSection::SectionName::SWNM); colr = FindLast(ChkSection::SectionName::COLR);
    pupx = FindLast(ChkSection::SectionName::PUPx); ptex = FindLast(ChkSection::SectionName::PTEx);
    unix = FindLast(ChkSection::SectionName::UNIx); upgx = FindLast(ChkSection::SectionName::UPGx);
    tecx = FindLast(ChkSection::SectionName::TECx);

    kstr = FindLast(ChkSection::SectionName::KSTR);*/
}

// Standard Sections
/*buffer& Scenario::TYPE() { return *type; } buffer& Scenario::VER() { return *ver; }
buffer& Scenario::IVER() { return *iver; } buffer& Scenario::IVE2() { return *ive2; }
buffer& Scenario::VCOD() { return *vcod; } buffer& Scenario::IOWN() { return *iown; }
buffer& Scenario::OWNR() { return *ownr; } buffer& Scenario::ERA () { return *era; }
buffer& Scenario::DIM () { return *dim; } buffer& Scenario::SIDE() { return *side; }
buffer& Scenario::MTXM() { return *mtxm; } buffer& Scenario::PUNI() { return *puni; }
buffer& Scenario::UPGR() { return *upgr; } buffer& Scenario::PTEC() { return *ptec; }
buffer& Scenario::UNIT() { return *unit; } buffer& Scenario::ISOM() { return *isom; }
buffer& Scenario::TILE() { return *tile; } buffer& Scenario::DD2 () { return *dd2; }
buffer& Scenario::THG2() { return *thg2; } buffer& Scenario::MASK() { return *mask; }
buffer& Scenario::STR () { return *str; } buffer& Scenario::UPRP() { return *uprp; }
buffer& Scenario::UPUS() { return *upus; } buffer& Scenario::MRGN() { return *mrgn; }
buffer& Scenario::TRIG() { return *trig; } buffer& Scenario::MBRF() { return *mbrf; }
buffer& Scenario::SPRP() { return *sprp; } buffer& Scenario::FORC() { return *forc; }
buffer& Scenario::WAV () { return *wav; } buffer& Scenario::UNIS() { return *unis; }
buffer& Scenario::UPGS() { return *upgs; } buffer& Scenario::TECS() { return *tecs; }
buffer& Scenario::SWNM() { return *swnm; } buffer& Scenario::COLR() { return *colr; }
buffer& Scenario::PUPx() { return *pupx; } buffer& Scenario::PTEx() { return *ptex; }
buffer& Scenario::UNIx() { return *unix; } buffer& Scenario::UPGx() { return *upgx; }
buffer& Scenario::TECx() { return *tecx; }

// Extended Sections
buffer& Scenario::KSTR() { return *kstr; }*/

bool Scenario::GetStrInfo(char* &ptr, size_t &length, u32 stringNum)
{
/*    buffer* strings = &STR();
    u32 numStrings = u32(strings->get<u16>(0));
    bool extended = false;

    if ( stringNum >= numStrings ) // Might be an extended string
    {
        if ( !KSTR->exists() )
            return false;

        strings = &KSTR();
        if ( strings->get<u32>(0) != 2 ) // Invalid version or KSTR size is insufficient to get a long
            return false;

        stringNum = 65536-stringNum;
        numStrings = strings->get<u32>(4);
        if ( stringNum > numStrings )
            return false;

        extended = true;
    }

    if ( strings->exists() && stringNum != 0 )
    {
        s64 start = 0, end = 0;

        if ( extended )
            start = strings->get<u32>(4+4*stringNum);
        else
            start = strings->get<u16>(2*stringNum);

        if ( !strings->getNext('\0', start, end) ) // Next NUL char marks the end of the string
            end = strings->size(); // Just end it where section ends

        length = (size_t)(end-start);
        ptr = (char*)strings->getPtr(start);
        return true;
    }
    else*/
        return false;
}

bool Scenario::ZeroOutString(u32 stringNum)
{
/*    char* str;
    size_t length;
    if ( GetStrInfo(str, length, stringNum) )
    {
        for ( size_t i=0; i<length; i++ ) // Zero-out characters
            str[i] = '\0';

        u32 numRegularStrings = (u32)strSectionCapacity();
        if ( stringNum < strSectionCapacity() )
        {
            if ( STR->size() > numRegularStrings*2+2 ) // If you can get to start of string data
                return STR->replace<u16>(2*stringNum, numRegularStrings*2+2); // Set string offset to start of string data (NUL)
            else
                return STR->replace<u16>(2*stringNum, 0); // Otherwise just zero it
        }
        else if ( KSTR->exists() )
        {
            u32 numExtendedStrings = KSTR->get<u32>(4);
            if ( KSTR->size() > numExtendedStrings*8+8 ) // If you can get to start of string data
                return KSTR->replace<u32>(stringNum*4+4, numExtendedStrings*8+8); // Set string offset to start of extended string data (NUL)
            else
                return KSTR->replace<u32>(stringNum*4+4, 0); // Otherwise just zero it
        }
    }*/
    return false;
}

bool Scenario::RepairString(u32& stringNum, bool extended)
{
/*    RawString str;
    if ( stringExists(stringNum) && GetString(str, stringNum) && str.size() > 0 )
        return false; // No Repair
    else
    {
        if ( addString(RawString("strRepair"), stringNum, extended) )
            return true;
        else
            return false;
    }*/
    return false;
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


Strings::Strings() :
    StrSynchronizer((u32)StrCompressFlag::DuplicateStringRecycling, (u32)StrCompressFlag::AllNonInterlacing)
{

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
            case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType);
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
std::shared_ptr<StringType> Strings::getUnitName(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope)
{
    return getString<StringType>(
        properties->getUnitNameStringId(unitType, useExp),
        properties->useExpansionUnitSettings(useExp) ? ostr->getExpUnitNameStringId(unitType) : ostr->getUnitNameStringId(unitType),
        storageScope);
}
template std::shared_ptr<RawString> Strings::getUnitName<RawString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<EscString> Strings::getUnitName<EscString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<ChkdString> Strings::getUnitName<ChkdString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);
template std::shared_ptr<SingleLineChkdString> Strings::getUnitName<SingleLineChkdString>(Sc::Unit::Type unitType, Chk::UseExpSection useExp, Chk::Scope storageScope);

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
                    case Chk::UseExpSection::Auto: versions.isHybridOrAbove() ? ostr->setExpUnitNameStringId(unitType, (u32)newStringId) : ostr->setUnitNameStringId(unitType, (u32)newStringId); break;
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

void Strings::synchronzieFromStrBuffer(const buffer &rawData)
{
    size_t sectionSize = (size_t)rawData.size();
    size_t numStrings = sectionSize > 2 ? (size_t)rawData.get<u16>(0) : (sectionSize == 1 ? (u16)rawData.get<u8>(0) : 0);
    size_t highestStringWithValidOffset = std::min(numStrings, sectionSize/2);
    strings.clear();

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

void Strings::synchronzieFromKstrBuffer(const buffer &rawData)
{
    size_t sectionSize = (size_t)rawData.size();
    size_t numStrings = sectionSize > 4 ? (size_t)rawData.get<u32>(0) : 0;
    size_t highestStringWithValidOffset = std::min(numStrings, (sectionSize-4)/4);
    kStrings.clear();

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

Sc::Player::SlotOwner Players::getSlotOwner(size_t slotIndex, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: return ownr->getSlotOwner(slotIndex);
        case Chk::Scope::Editor: return iown->getSlotOwner(slotIndex);
        case Chk::Scope::EditorOverGame: return iown != nullptr ? iown->getSlotOwner(slotIndex) : ownr->getSlotOwner(slotIndex);
        default: return ownr != nullptr ? ownr->getSlotOwner(slotIndex) : iown->getSlotOwner(slotIndex);
    }
    return Sc::Player::SlotOwner::Inactive;
}

void Players::setSlotOwner(size_t slotIndex, Sc::Player::SlotOwner owner, Chk::Scope scope)
{
    switch ( scope )
    {
        case Chk::Scope::Game: ownr->setSlotOwner(slotIndex, owner); break;
        case Chk::Scope::Editor: iown->setSlotOwner(slotIndex, owner); break;
        default: ownr->setSlotOwner(slotIndex, owner); iown->setSlotOwner(slotIndex, owner); break;
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

Chk::PlayerColor Players::getPlayerColor(Sc::Player::SlotOwner slotOwner)
{
    return colr->getPlayerColor(slotOwner);
}

void Players::setPlayerColor(Sc::Player::SlotOwner slotOwner, Chk::PlayerColor color)
{
    colr->setPlayerColor(slotOwner, color);
}

Chk::Force Players::getPlayerForce(Sc::Player::SlotOwner slotOwner)
{
    return forc->getPlayerForce(slotOwner);
}

size_t Players::getForceStringId(Chk::Force force)
{
    return forc->getForceStringId(force);
}

u8 Players::getForceFlags(Chk::Force force)
{
    return forc->getForceFlags(force);
}

void Players::setPlayerForce(Sc::Player::SlotOwner slotOwner, Chk::Force force)
{
    forc->setPlayerForce(slotOwner, force);
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
    mrgn->deleteLocation(locationIndex);
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

bool Layers::stringUsed(size_t stringId)
{
    return mrgn->stringUsed(stringId);
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

bool Properties::useExpansionUnitSettings(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
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

bool Properties::playerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex)
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

void Properties::setPlayerUsesDefault(Sc::Unit::Type unitType, size_t playerIndex, bool useDefault)
{
    puni->setPlayerUsesDefault(unitType, playerIndex, useDefault);
}

bool Properties::useExpansionUpgradeCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
        case Chk::UseExpSection::Yes: true;
        case Chk::UseExpSection::No: false;
        case Chk::UseExpSection::NoIfOrigAvailable: return upgs != nullptr ? false : true;
        case Chk::UseExpSection::YesIfAvailable:
        default: return upgx != nullptr ? true : false;
    }
}

bool Properties::upgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->upgradeUsesDefaultSettings(upgradeType) : upgs->upgradeUsesDefaultSettings(upgradeType);
}

u16 Properties::getBaseMineralCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseMineralCost(upgradeType) : upgs->getBaseMineralCost(upgradeType);
}

u16 Properties::getMineralCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getMineralCostFactor(upgradeType) : upgs->getMineralCostFactor(upgradeType);
}

u16 Properties::getBaseGasCost(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseGasCost(upgradeType) : upgs->getBaseGasCost(upgradeType);
}

u16 Properties::getGasCostFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getGasCostFactor(upgradeType) : upgs->getGasCostFactor(upgradeType);
}

u16 Properties::getBaseResearchTime(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getBaseResearchTime(upgradeType) : upgs->getBaseResearchTime(upgradeType);
}

u16 Properties::getResearchTimeFactor(Sc::Upgrade::Type upgradeType, Chk::UseExpSection useExp)
{
    return useExpansionUpgradeCosts(useExp) ? upgx->getResearchTimeFactor(upgradeType) : upgs->getResearchTimeFactor(upgradeType);
}

void Properties::setUpgradeUsesDefaultSettings(Sc::Upgrade::Type upgradeType, bool useDefault, Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto:
        case Chk::UseExpSection::Both: upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::Yes: upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::No: upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::YesIfAvailable: upgx != nullptr ? upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault) : upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
        case Chk::UseExpSection::NoIfOrigAvailable: upgs != nullptr ? upgs->setUpgradeUsesDefaultSettings(upgradeType, useDefault) : upgx->setUpgradeUsesDefaultSettings(upgradeType, useDefault); break;
    }
}

void Properties::setBaseMineralCost(Sc::Upgrade::Type upgradeType, u16 baseMineralCost, Chk::UseExpSection useExp)
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

void Properties::setMineralCostFactor(Sc::Upgrade::Type upgradeType, u16 mineralCostFactor, Chk::UseExpSection useExp)
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

void Properties::setBaseGasCost(Sc::Upgrade::Type upgradeType, u16 baseGasCost, Chk::UseExpSection useExp)
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

void Properties::setGasCostFactor(Sc::Upgrade::Type upgradeType, u16 gasCostFactor, Chk::UseExpSection useExp)
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

void Properties::setBaseResearchTime(Sc::Upgrade::Type upgradeType, u16 baseResearchTime, Chk::UseExpSection useExp)
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

void Properties::setResearchTimeFactor(Sc::Upgrade::Type upgradeType, u16 researchTimeFactor, Chk::UseExpSection useExp)
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
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
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

bool Properties::playerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, Chk::UseExpSection useExp)
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

void Properties::setPlayerUsesDefault(Sc::Upgrade::Type upgradeType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
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

bool Properties::useExpansionTechCosts(Chk::UseExpSection useExp)
{
    switch ( useExp )
    {
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
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
        case Chk::UseExpSection::Auto: return versions.isHybridOrAbove() ? true : false;
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

bool Properties::playerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, Chk::UseExpSection useExp)
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

void Properties::setPlayerUsesDefault(Sc::Tech::Type techType, size_t playerIndex, bool useDefault, Chk::UseExpSection useExp)
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
