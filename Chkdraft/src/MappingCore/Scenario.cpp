#include "Scenario.h"
#include "DefaultCHK.h"
#include "StringUsage.h"
#include "sha256.h"
#include "Math.h"
#include "SubStringMap.h"
#include <algorithm>
#include <cstdio>
#include <exception>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>

Scenario::Scenario() : mapIsProtected(false), caching(false), tailLength(0),

// Cached regular section pointers
type(nullptr), ver(nullptr), iver(nullptr), ive2(nullptr), vcod(nullptr), iown(nullptr), ownr(nullptr), era(nullptr),
dim(nullptr), side(nullptr), mtxm(nullptr), puni(nullptr), upgr(nullptr), ptec(nullptr), unit(nullptr), isom(nullptr),
tile(nullptr), dd2(nullptr), thg2(nullptr), mask(nullptr), str(nullptr), uprp(nullptr), upus(nullptr), mrgn(nullptr),
trig(nullptr), mbrf(nullptr), sprp(nullptr), forc(nullptr), wav(nullptr), unis(nullptr), upgs(nullptr), tecs(nullptr),
swnm(nullptr), colr(nullptr), pupx(nullptr), ptex(nullptr), unix(nullptr), upgx(nullptr), tecx(nullptr),

// Cached extended section pointers
kstr(nullptr)

{ // ctor body

}

Scenario::~Scenario()
{
    sections.clear();
}

Scenario* Scenario::scenario()
{
    return this;
}

bool Scenario::isExpansion()
{
    return VER().get<u16>(0) >= 63;
}

buffer& Scenario::unitSettings()
{
    if ( isExpansion() && UNIx().exists() )
        return UNIx();
    else
        return UNIS();
}

buffer& Scenario::upgradeSettings()
{
    if ( isExpansion() && this->UPGx().exists() )
        return this->UPGx();
    else
        return this->UPGS();
}

buffer& Scenario::upgradeRestrictions()
{
    if ( isExpansion() && this->PUPx().exists() )
        return this->PUPx();
    else
        return this->UPGR();
}

buffer& Scenario::techSettings()
{
    if ( isExpansion() && this->TECx().exists() )
        return this->TECx();
    else
        return this->TECS();
}

buffer& Scenario::techRestrictions()
{
    if ( isExpansion() && this->PTEx().exists() )
        return this->PTEx();
    else
        return this->PTEC();
}

bool Scenario::HasVerSection()
{
    return VER().exists();
}

bool Scenario::IsScOrig()
{
    return VER().get<u16>(0) < 63;
}

bool Scenario::IsHybrid()
{
    return VER().get<u16>(0) >= 63 && VER().get<u16>(0) < 205;
}

bool Scenario::IsScExp()
{
    return VER().get<u16>(0) >= 205;
}

bool Scenario::ChangeToScOrig()
{
    return TYPE().overwrite("RAWS", 4) &&
        VER().overwrite(";\0", 2) &&
        IVER().overwrite("\12\0", 2) &&
        IVE2().overwrite("\13\0", 2) &&
        (MRGN().size() <= 1280 || MRGN().delRange(1280, MRGN().size()));
}

bool Scenario::ChangeToHybrid()
{
    return TYPE().overwrite("RAWS", 4) &&
        VER().overwrite("?\0", 2) &&
        IVER().overwrite("\12\0", 2) &&
        IVE2().overwrite("\13\0", 2) &&
        (MRGN().size() >= 5100 || MRGN().add<u8>(0, 5100 - MRGN().size()));
}

bool Scenario::ChangeToScExp()
{
    return TYPE().overwrite("RAWB", 4) &&
        VER().overwrite("Í\0", 2) &&
        RemoveSection(SectionId::IVER) &&
        IVE2().overwrite("\13\0", 2) &&
        (MRGN().size() >= 5100 || MRGN().add<u8>(0, 5100 - MRGN().size()));
}

u16 Scenario::GetMapTitleStrIndex()
{
    return SPRP().get<u16>(0);
}

u16 Scenario::GetMapDescriptionStrIndex()
{
    return SPRP().get<u16>(2);
}

bool Scenario::getMapTitle(ChkdString &dest)
{
    dest = "";
    if ( SPRP().exists() )
    {
        u16 stringNum = 0;
        if ( SPRP().get<u16>(stringNum, 0) )
            return GetString(dest, (u32)stringNum);
    }
    dest = "Untitled Scenario";
    return false;
}

bool Scenario::getMapDescription(ChkdString &dest)
{
    dest = "";
    if ( SPRP().exists() )
    {
        u16 stringNum = 0;
        if ( SPRP().get<u16>(stringNum, 2) )
            return GetString(dest, (u32)stringNum);
    }
    dest = "Destroy all enemy buildings.";
    return false;
}

bool Scenario::SetMapTitle(ChkdString &newMapTitle)
{
    u16* mapTitleString = nullptr;
    return SPRP().getPtr<u16>(mapTitleString, 0, 2) &&
        replaceString(newMapTitle, *mapTitleString, false, true);
}

bool Scenario::SetMapDescription(ChkdString &newMapDescription)
{
    u16* mapDescriptionString = nullptr;
    return SPRP().getPtr<u16>(mapDescriptionString, 2, 2) &&
        replaceString(newMapDescription, *mapDescriptionString, false, true);
}

u16 Scenario::XSize()
{
    return DIM().get<u16>(0);
}

u16 Scenario::YSize()
{
    return DIM().get<u16>(2);
}

u16 Scenario::getTileset()
{
    if ( this != nullptr )
        return ERA().get<u16>(0);

    return 0;
}

u16 Scenario::numUnits()
{
    return u16(UNIT().size()/UNIT_STRUCT_SIZE);
}

ChkUnit Scenario::getUnit(u16 index)
{
    ChkUnit* unitPtr;
    if ( UNIT().getPtr<ChkUnit>(unitPtr, UNIT_STRUCT_SIZE*(u32)index, (u32)UNIT_STRUCT_SIZE) )
        return *unitPtr;
    else
    {
        ChkUnit blankUnit = {};
        return blankUnit;
    }
}

bool Scenario::ReplaceUnit(u16 index, ChkUnit newUnit)
{
    return UNIT().replace<ChkUnit>(UNIT_STRUCT_SIZE*(u32)index, newUnit);
}

bool Scenario::insertUnit(u16 index, ChkUnit &unit)
{
    if ( index == numUnits() )
        return UNIT().add<ChkUnit>(unit);
    else
        return UNIT().insert<ChkUnit>(UNIT_STRUCT_SIZE*(u32)index, unit);
}

/*bool Scenario::getUnit(ChkUnit* &unitRef, u16 index)
{
    return UNIT().getPtr(unitRef, index*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);
}*/

bool Scenario::HasLocationSection()
{
    return MRGN().exists();
}

bool Scenario::getLocation(ChkLocation* &locRef, u16 index)
{
    return MRGN().getPtr(locRef, (u32(index))*CHK_LOCATION_SIZE, CHK_LOCATION_SIZE);
}

ChkLocation Scenario::getLocation(u16 locationIndex)
{
    ChkLocation* locationPtr;
    if ( MRGN().getPtr<ChkLocation>(locationPtr, CHK_LOCATION_SIZE*(u32)locationIndex, (u32)CHK_LOCATION_SIZE) )
        return *locationPtr;
    else
    {
        ChkLocation blankLocation = {};
        return blankLocation;
    }
}

bool Scenario::getLocationName(u16 locationIndex, RawString &str)
{
    u16 locationStringIndex = 0;
    if ( MRGN().get<u16>(locationStringIndex, 16 + CHK_LOCATION_SIZE*(u32)locationIndex) &&
        locationStringIndex != 0 && GetString(str, locationStringIndex) )
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
}

bool Scenario::getLocationName(u16 locationIndex, ChkdString &str)
{
    u16 locationStringIndex = 0;
    if ( MRGN().get<u16>(locationStringIndex, 16 + CHK_LOCATION_SIZE*(u32)locationIndex) &&
        locationStringIndex != 0 && GetString(str, locationStringIndex) )
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
}

bool Scenario::MoveLocation(u16 locationIndex, s32 xChange, s32 yChange)
{
    ChkLocation* location = nullptr;
    if ( getLocation(location, locationIndex) )
    {
        location->xc1 += xChange;
        location->xc2 += xChange;
        location->yc1 += yChange;
        location->yc2 += yChange;
        return true;
    }
    return false;
}

u32 Scenario::GetLocationFieldData(u16 locationIndex, u8 field)
{
    u32 pos = CHK_LOCATION_SIZE*(u32)locationIndex + (u32)locationFieldLoc[field];
    switch ( locationFieldSize[field] )
    {
        case 1: return MRGN().get<u8>(pos); break;
        case 2: return MRGN().get<u16>(pos); break;
        case 4: return MRGN().get<u32>(pos); break;
    }
    return 0;
}

bool Scenario::SetLocationFieldData(u16 locationIndex, u8 field, u32 data)
{
    u32 pos = CHK_LOCATION_SIZE*(u32)locationIndex + (u32)locationFieldLoc[field];
    switch ( locationFieldSize[field] )
    {
        case 1: return MRGN().replace<u8>(pos, u8(data)); break;
        case 2: return MRGN().replace<u16>(pos, u16(data)); break;
        case 4: return MRGN().replace<u32>(pos, data); break;
    }
    return false;
}

bool Scenario::insertLocation(u16 index, ChkLocation &location, RawString &name)
{
    if ( name.size() > 0 )
    {
        u32 stringNum;
        if ( addString(name, stringNum, false) )
        {
            location.stringNum = stringNum;
            return MRGN().replace<ChkLocation>(CHK_LOCATION_SIZE*(u32)index, location);
        }
    }
    location.stringNum = 0;
    return MRGN().replace<ChkLocation>(CHK_LOCATION_SIZE*(u32)index, location);
}

bool Scenario::insertLocation(u16 index, ChkLocation &location, ChkdString &name)
{
    RawString rawLocationName;
    return ParseChkdStr(name, rawLocationName) &&
        insertLocation(index, location, rawLocationName);
}

u8 Scenario::numLocations()
{
    u8 numLocs = 0;
    ChkLocation* locRef;
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
    return numLocs;
}

u16 Scenario::locationCapacity()
{
    return MRGN().size() / CHK_LOCATION_SIZE;
}

bool Scenario::locationIsUsed(u16 locationIndex)
{
    ChkLocation* locRef;
    if ( getLocation(locRef, locationIndex) )
    {
        if ( locRef->elevation != 0 || locRef->stringNum != 0 ||
             locRef->xc1 != 0 || locRef->xc2 != 0 || locRef->yc1 != 0 || locRef->yc2 != 0 )
        {
            return true;
        }
    }
    return false;
}

bool Scenario::HasTrigSection()
{
    return TRIG().exists();
}

bool Scenario::ReplaceTrigSection(Section newTrigSection)
{
    return TRIG().takeAllData(*newTrigSection);
}

u32 Scenario::numTriggers()
{
    return u32(TRIG().size()/TRIG_STRUCT_SIZE);
}

bool Scenario::getTrigger(Trigger* &trigRef, u32 index)
{
    return TRIG().getPtr(trigRef, index*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE);
}

bool Scenario::getActiveComment(Trigger* trigger, RawString &comment)
{
    for ( u32 i=0; i<NUM_TRIG_ACTIONS; i++ )
    {
        Action action = trigger->actions[i];
        if ( action.action == (u8)ActionId::Comment )
        {
            if ( (action.flags&(u8)Action::Flags::Disabled) != (u8)Action::Flags::Disabled )
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
}

bool Scenario::getBriefingTrigger(Trigger* &trigRef, u32 index)
{
    return MBRF().getPtr(trigRef, index*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE);
}

bool Scenario::hasForceSection()
{
    return FORC().exists();
}

u32 Scenario::getForceStringNum(u8 index)
{
    if ( index < 4 )
        return FORC().get<u16>(8+2*index);
    else
        return 0;
}

bool Scenario::getForceString(ChkdString &str, u8 forceNum)
{
    u32 stringNum = getForceStringNum(forceNum);
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
    }
    return false;
}

bool Scenario::getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av)
{
    if ( forceNum < 4 )
    {
        u8 forceProp;
        if ( FORC().get<u8>(forceProp, 16+forceNum) )
        {
            allied = ((forceProp&(u8)ForceFlags::Allied) == (u8)ForceFlags::Allied);
            vision = ((forceProp&(u8)ForceFlags::SharedVision) == (u8)ForceFlags::SharedVision);
            random = ((forceProp&(u8)ForceFlags::RandomizeStartLocation) == (u8)ForceFlags::RandomizeStartLocation);
            av     = ((forceProp&(u8)ForceFlags::AlliedVictory) == (u8)ForceFlags::AlliedVictory);
            return true;
        }
    }
    return false;
}

bool Scenario::getPlayerForce(u8 playerNum, u8 &force)
{
    if ( playerNum < 8 )
        return FORC().get<u8>(force, playerNum);
    else
        return false;
}

bool Scenario::ReplaceUNISSection(buffer &newUNISSection)
{
    return UNIS().takeAllData(newUNISSection);
}

bool Scenario::ReplaceUNIxSection(buffer &newUNIxSection)
{
    return UNIx().takeAllData(newUNIxSection);
}

bool Scenario::ReplacePUNISection(buffer &newPUNISection)
{
    return PUNI().takeAllData(newPUNISection);
}

bool Scenario::getUnitStringNum(u16 unitId, u16 &stringNum)
{
    if ( isExpansion() )
        return UNIx().get<u16>(stringNum, unitId*2+(u32)UnitSettingsDataLoc::StringIds);
    else
        return UNIS().get<u16>(stringNum, unitId*2+(u32)UnitSettingsDataLoc::StringIds);
}

u16 Scenario::strSectionCapacity()
{
    return STR().get<u16>(0);
}

u32 Scenario::kstrSectionCapacity()
{
    return KSTR().get<u32>(4);
}

u32 Scenario::stringCapacity()
{
    return STR().get<u16>(0) + KSTR().get<u32>(4);
}

u32 Scenario::stringCapacity(bool extended)
{
    if ( extended )
        return KSTR().get<u32>(4);
    else
        return STR().get<u16>(0);
}

bool Scenario::hasStrSection(bool extended)
{
    if ( extended )
        return KSTR().exists();
    else
        return STR().exists();
}

u32 Scenario::strBytesUsed()
{
    return STR().size();
}

bool Scenario::GetString(RawString &dest, u32 stringNum)
{
    dest.clear();

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

    return false;
}

bool Scenario::GetString(EscString &dest, u32 stringNum)
{
    dest.clear();
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
        return false;
}

bool Scenario::GetString(ChkdString &dest, u32 stringNum)
{
    dest.clear();

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

    return false;
}

bool Scenario::GetExtendedString(RawString &dest, u32 extendedStringSectionIndex)
{
    return GetString(dest, 65535 - extendedStringSectionIndex);
}

bool Scenario::GetExtendedString(EscString &dest, u32 extendedStringSectionIndex)
{
    return GetString(dest, 65535 - extendedStringSectionIndex);
}

bool Scenario::GetExtendedString(ChkdString &dest, u32 extendedStringSectionIndex)
{
    return GetString(dest, 65535 - extendedStringSectionIndex);
}

u32 Scenario::NumNameableSwitches()
{
    return SWNM().size() / 4;
}

bool Scenario::getSwitchStrId(u8 switchId, u32 &stringId)
{
    return SWNM().get<u32>(stringId, 4 * (u32)switchId);
}

bool Scenario::getSwitchName(ChkdString &dest, u8 switchID)
{
    buffer &SWNM = this->SWNM();
    u32 stringNum = 0;
    if ( SWNM.get<u32>(stringNum, 4 * (u32)switchID) && stringNum != 0 )
        return GetString(dest, stringNum);
    else
        return false;
}

bool Scenario::setSwitchName(ChkdString &newName, u8 switchId, bool extended)
{
    u32 stringNum = 0;
    if ( getSwitchStrId(switchId, stringNum) )
    {
        u32* stringToReplace = nullptr;
        if ( SWNM().getPtr(stringToReplace, 4*(u32)switchId, 4) )
            return replaceString<u32>(newName, *stringToReplace, extended, true);
    }
    return false;
}

bool Scenario::hasSwitchSection()
{
    return SWNM().exists();
}

bool Scenario::switchHasName(u8 switchId)
{
    u32 stringId = 0;
    return getSwitchStrId(switchId, stringId) && stringId != 0;
}

void Scenario::removeSwitchName(u8 switchId)
{
    u32 stringNum = 0;
    if ( getSwitchStrId(switchId, stringNum) && stringNum != 0 )
    {
        SWNM().replace<u32>(4 * (u32)switchId, 0);
        removeUnusedString(stringNum);
    }
}

bool Scenario::SwitchUsesExtendedName(u8 switchId)
{
    u32 stringId = 0;
    if ( getSwitchStrId(switchId, stringId) && stringId != 0 )
        return isExtendedString(stringId);
    
    return false;
}

bool Scenario::ToggleUseExtendedSwitchName(u8 switchId)
{
    ChkdString switchName;
    u32 switchStringId = 0;
    if ( getSwitchStrId(switchId, switchStringId) &&
         stringExists(switchStringId) &&
         GetString(switchName, switchStringId) )
    {
        bool wasExtended = isExtendedString(switchStringId);
        removeSwitchName(switchId);
        u32 newSwitchStringId = 0;
        if ( addString(switchName, newSwitchStringId, !wasExtended) )
            return SWNM().replace<u32>(4 * (u32)switchId, newSwitchStringId);
    }
    return false;
}

bool Scenario::getUnitName(RawString &dest, u16 unitID)
{
    buffer& settings = unitSettings();
    if ( unitID < 228 ) // Regular unit
    {
        if ( settings.get<u8>(unitID) == 0 && // Not default unit settings
            settings.get<u16>(unitID * 2 + (u32)UnitSettingsDataLoc::StringIds) > 0 ) // Not default string
        {
            u16 stringID = settings.get<u16>(2 * unitID + (u32)UnitSettingsDataLoc::StringIds);
            if ( GetString(dest, stringID) )
                return true;
        }

        try
        {
            dest = DefaultUnitDisplayName[unitID];
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
    return false;
}

bool Scenario::getUnitName(ChkdString &dest, u16 unitID)
{
    buffer& settings = unitSettings();
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
            dest = DefaultUnitDisplayName[unitID];
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
    return false;
}

bool Scenario::deleteUnit(u16 index)
{
    return UNIT().del(index*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);
}

bool Scenario::SwapUnits(u16 firstIndex, u16 secondIndex)
{
    return UNIT().swap<ChkUnit>(((u32)firstIndex)*UNIT_STRUCT_SIZE, (secondIndex)*UNIT_STRUCT_SIZE);
}

u32 Scenario::GetUnitFieldData(u16 unitIndex, ChkUnitField field)
{
    ChkUnit unit = getUnit(unitIndex);
    switch ( field )
    {
        case ChkUnitField::Serial: return unit.serial; break;
        case ChkUnitField::Xc: return unit.xc; break;
        case ChkUnitField::Yc: return unit.yc; break;
        case ChkUnitField::Id: return unit.id; break;
        case ChkUnitField::LinkType: return unit.linkType; break;
        case ChkUnitField::Special: return unit.special; break;
        case ChkUnitField::ValidFlags: return unit.validFlags; break;
        case ChkUnitField::Owner: return unit.owner; break;
        case ChkUnitField::Hitpoints: return unit.hitpoints; break;
        case ChkUnitField::Shields: return unit.shields; break;
        case ChkUnitField::Energy: return unit.energy; break;
        case ChkUnitField::Resources: return unit.resources; break;
        case ChkUnitField::Hanger: return unit.hanger; break;
        case ChkUnitField::StateFlags: return unit.stateFlags; break;
        case ChkUnitField::Unused: return unit.unused; break;
        case ChkUnitField::Link: return unit.link; break;
    }
    return 0;
}

bool Scenario::SetUnitFieldData(u16 unitIndex, ChkUnitField field, u32 data)
{
    ChkUnit unit = getUnit(unitIndex);
    switch ( field )
    {
        case ChkUnitField::Serial: unit.serial = data; break;
        case ChkUnitField::Xc: unit.xc = data; break;
        case ChkUnitField::Yc: unit.yc = data; break;
        case ChkUnitField::Id: unit.id = data; break;
        case ChkUnitField::LinkType: unit.linkType = data; break;
        case ChkUnitField::Special: unit.special = data; break;
        case ChkUnitField::ValidFlags: unit.validFlags = data; break;
        case ChkUnitField::Owner: unit.owner = data; break;
        case ChkUnitField::Hitpoints: unit.hitpoints = data; break;
        case ChkUnitField::Shields: unit.shields = data; break;
        case ChkUnitField::Energy: unit.energy = data; break;
        case ChkUnitField::Resources: unit.resources = data; break;
        case ChkUnitField::Hanger: unit.hanger = data; break;
        case ChkUnitField::StateFlags: unit.stateFlags = data; break;
        case ChkUnitField::Unused: unit.unused = data; break;
        case ChkUnitField::Link: unit.link = data; break;
        default: return false; break;
    }
    return true;
}

bool Scenario::SetUnitHitpoints(u16 unitIndex, u8 newHitpoints)
{
    return UNIT().replace<u8>(17 + UNIT_STRUCT_SIZE*(u32)unitIndex, newHitpoints);
}

bool Scenario::SetUnitEnergy(u16 unitIndex, u8 newEnergy)
{
    return UNIT().replace<u8>(19 + UNIT_STRUCT_SIZE*(u32)unitIndex, newEnergy);
}

bool Scenario::SetUnitShields(u16 unitIndex, u8 newShields)
{
    return UNIT().replace<u8>(18 + UNIT_STRUCT_SIZE*(u32)unitIndex, newShields);
}

bool Scenario::SetUnitResources(u16 unitIndex, u32 newResources)
{
    return UNIT().replace<u32>(20 + UNIT_STRUCT_SIZE*(u32)unitIndex, newResources);
}

bool Scenario::SetUnitHanger(u16 unitIndex, u16 newHanger)
{
    return UNIT().replace<u16>(24 + UNIT_STRUCT_SIZE*(u32)unitIndex, newHanger);
}

bool Scenario::SetUnitTypeId(u16 unitIndex, u16 newTypeId)
{
    return UNIT().replace<u16>(8 + UNIT_STRUCT_SIZE*(u32)unitIndex, newTypeId);
}

bool Scenario::SetUnitXc(u16 unitIndex, u16 newXc)
{
    return UNIT().replace<u16>(4 + UNIT_STRUCT_SIZE*(u32)unitIndex, newXc);
}

bool Scenario::SetUnitYc(u16 unitIndex, u16 newYc)
{
    return UNIT().replace<u16>(6 + UNIT_STRUCT_SIZE*(u32)unitIndex, newYc);
}

u16 Scenario::SpriteSectionCapacity()
{
    return THG2().size() / SPRITE_STRUCT_SIZE;
}

bool Scenario::GetSpriteRef(ChkSprite* &spriteRef, u16 index)
{
    return THG2().getPtr(spriteRef, SPRITE_STRUCT_SIZE*(u32)index, SPRITE_STRUCT_SIZE);
}

bool Scenario::stringIsUsed(u32 stringNum)
{
    // MRGN - location strings
    ChkLocation* loc;
    u32 numLocs = MRGN().size()/CHK_LOCATION_SIZE;
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
    u32 numWavs = WAV.size()/4;
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

    return false;
}

bool Scenario::isExtendedString(u32 stringNum)
{
    return stringNum >= strSectionCapacity() &&
           stringNum < 65536 &&
           KSTR().exists() &&
           (u32(65536-stringNum)) <= KSTR().get<u32>(4);
}

bool Scenario::stringExists(u32 stringNum)
{
    return isExtendedString(stringNum) ||
           ( STR().exists() &&
             stringNum < strSectionCapacity() );
}

bool Scenario::stringExists(const RawString &str, u32& stringNum)
{
    std::vector<StringTableNode> strList;
    addAllUsedStrings(strList, true, true);

    for ( auto &existingString : strList )
    {
        if ( existingString.string.compare(str) == 0 )
        {
            stringNum = existingString.stringNum;
            return true;
        }
    }

    return false;
}

bool Scenario::stringExists(const ChkdString &str, u32 &stringNum)
{
    RawString rawStr;
    return ParseChkdStr(str, rawStr) && stringExists(rawStr, stringNum);
}

bool Scenario::stringExists(const RawString &str, u32& stringNum, bool extended)
{
    std::vector<StringTableNode> strList;
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

    return false;
}

bool Scenario::stringExists(const RawString &str, u32& stringNum, std::vector<StringTableNode> &strList)
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
}

bool Scenario::FindDifference(ChkdString &str, u32& stringNum)
{
    RawString rawSuppliedString, rawStringAtNum;
    if ( ParseChkdStr(str, rawSuppliedString) && GetString(rawStringAtNum, stringNum) )
    {
        if ( rawSuppliedString.compare(rawStringAtNum) == 0 )
            return false;
    }
    return true;
}

u32 Scenario::extendedToRegularStr(u32 stringNum)
{
    return 65536 - stringNum;
}

bool Scenario::stringUsedWithLocs(u32 stringNum)
{
    ChkLocation* loc;
    for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
    {
        if ( getLocation(loc, u8(i)) && ((u32)loc->stringNum) == stringNum )
            return true;
    }
    return true;
}

void Scenario::IncrementIfEqual(u32 lhs, u32 rhs, u32 &counter)
{
    if ( lhs == rhs )
        ++counter;
}

u32 Scenario::amountStringUsed(u32 stringNum)
{
    u32 amountStringUsed = 0;
    ChkLocation* loc; // MRGN - location strings
    for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
    {
        if ( getLocation(loc, u8(i)) )
            IncrementIfEqual(loc->stringNum, stringNum, amountStringUsed);
    }
    Trigger* trig; // TRIG - trigger strings
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, amountStringUsed);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, amountStringUsed);
        }
    }
    trigNum = 0; // MBRF - briefing strings
    while ( getBriefingTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, amountStringUsed);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, amountStringUsed);
        }
    }
    // SPRP - scenario property strings
    u16 strIndex = SPRP().get<u16>(0); // Scenario Name
    IncrementIfEqual(strIndex, stringNum, amountStringUsed);
    strIndex = SPRP().get<u16>(2); // Scenario Description
    IncrementIfEqual(strIndex, stringNum, amountStringUsed);
    for ( int i=0; i<4; i++ ) // FORC - force strings
        IncrementIfEqual(getForceStringNum(i), stringNum, amountStringUsed);
    for ( u32 i=0; i<WAV().size()/4; i++ ) // WAV  - sound strings
        IncrementIfEqual(WAV().get<u32>(i*4), stringNum, amountStringUsed);
    for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
        IncrementIfEqual(UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, amountStringUsed);
    for ( int i=0; i<256; i++ ) // SWNM - switch strings
        IncrementIfEqual(SWNM().get<u32>(i*4), stringNum, amountStringUsed);
    for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
        IncrementIfEqual(UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, amountStringUsed);

    return amountStringUsed;
}

void Scenario::getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches)
{
    locs = trigs = briefs = props = forces = wavs = units = switches = 0;
    ChkLocation* loc; // MRGN - location strings
    for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
    {
        if ( getLocation(loc, u8(i)) )
            IncrementIfEqual(loc->stringNum, stringNum, locs);
    }
    Trigger* trig; // TRIG - trigger strings
    int trigNum = 0;
    while ( getTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, trigs);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, trigs);
        }
    }
    trigNum = 0; // MBRF - briefing strings
    while ( getBriefingTrigger(trig, trigNum) )
    {
        trigNum ++;
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            IncrementIfEqual(trig->actions[i].stringNum, stringNum, briefs);
            IncrementIfEqual(trig->actions[i].wavID, stringNum, briefs);
        }
    }
    // SPRP - scenario property strings
    u16 strIndex = SPRP().get<u16>(0); // Scenario Name
    IncrementIfEqual(strIndex, stringNum, props);
    strIndex = SPRP().get<u16>(2); // Scenario Description
    IncrementIfEqual(strIndex, stringNum, props);
    for ( int i=0; i<4; i++ ) // FORC - force strings
        IncrementIfEqual(getForceStringNum(i), stringNum, forces);
    for ( u32 i=0; i<WAV().size()/4; i++ ) // WAV  - sound strings
        IncrementIfEqual(WAV().get<u32>(i*4), stringNum, wavs);
    for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
        IncrementIfEqual(UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, units);
    for ( int i=0; i<256; i++ ) // SWNM - switch strings
        IncrementIfEqual(SWNM().get<u32>(i*4), stringNum, switches);
    for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
        IncrementIfEqual(UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds), stringNum, units);
}

bool Scenario::GoodVCOD()
{
    buffer defaultVCOD("vcod");
    Get_VCOD(defaultVCOD);

    if ( !VCOD().exists() ) // Map has no VCOD section
        return false;

    char* defaultData = (char*)defaultVCOD.getPtr(8),
        * inData = (char*)VCOD().getPtr(0);

    u32 defaultSize = defaultVCOD.size()-8;
    if ( VCOD().size() != defaultSize ) // Size mismatch
        return false;
    
    for ( u32 pos = 0; pos < defaultSize; pos++ )
    {
        if ( defaultData[pos] != inData[pos] )
            return false;
    }
    return true;
}

bool Scenario::isProtected()
{
    return mapIsProtected;
}

bool Scenario::getPlayerOwner(u8 player, u8& owner)
{
    return OWNR().get<u8>(owner, (u32)player);
}

bool Scenario::getPlayerRace(u8 player, u8& race)
{
    return SIDE().get<u8>(race, (u32)player);
}

bool Scenario::getPlayerColor(u8 player, u8& color)
{
    return COLR().get<u8>(color, (u32)player);
}

bool Scenario::unitUsesDefaultSettings(u16 unitId)
{
    return unitSettings().get<u8>((u32)unitId) == 1;
}

bool Scenario::unitIsEnabled(u16 unitId)
{
    return PUNI().get<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability) != 0;
}

UnitEnabledState Scenario::getUnitEnabledState(u16 unitId, u8 player)
{
    if ( PUNI().get<u8>(unitId+player*228+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault) == 0 ) // Override
    {
        if ( PUNI().get<u8>((u32)unitId+228*(u32)player) == 0 )
            return UnitEnabledState::Disabled;
        else
            return UnitEnabledState::Enabled;
    }
    else
        return UnitEnabledState::Default;
}

bool Scenario::hasUnitSettingsSection()
{
    return unitSettings().exists();
}

bool Scenario::getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints)
{
    u32 untrimmedHitpoints;
    if ( unitSettings().get<u32>(untrimmedHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        hitpoints = untrimmedHitpoints/256;
        return true;
    }
    else
        return false;
}

bool Scenario::getUnitSettingsHitpointByte(u16 unitId, u8 &hitpointByte)
{
    u32 untrimmedHitpoints;
    if ( unitSettings().get<u32>(untrimmedHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        hitpointByte = untrimmedHitpoints%256;
        return true;
    }
    else
        return false;
}

bool Scenario::getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints)
{
    return unitSettings().get<u16>(shieldPoints, 2*(u32)unitId + (u32)UnitSettingsDataLoc::ShieldPoints);
}

bool Scenario::getUnitSettingsArmor(u16 unitId, u8 &armor)
{
    return unitSettings().get<u8>(armor, (u32)unitId+(u32)UnitSettingsDataLoc::Armor);
}

bool Scenario::getUnitSettingsBuildTime(u16 unitId, u16 &buildTime)
{
    return unitSettings().get<u16>(buildTime, 2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime);
}

bool Scenario::getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost)
{
    return unitSettings().get<u16>(mineralCost, 2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost);
}

bool Scenario::getUnitSettingsGasCost(u16 unitId, u16 &gasCost)
{
    return unitSettings().get<u16>(gasCost, 2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost);
}

bool Scenario::getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage)
{
    return unitSettings().get<u16>(baseDamage, 2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon);
}

bool Scenario::getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage)
{
    return unitSettings().get<u16>(bonusDamage, UnitSettingsDataLocBonusWeapon(isExpansion())+2*weaponId);
}

bool Scenario::getUnisStringId(u16 unitId, u16 &stringId)
{
    return UNIS().get<u16>(stringId, 2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds);
}

bool Scenario::getUnixStringId(u16 unitId, u16 &stringId)
{
    return UNIx().get<u16>(stringId, 2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds);
}

bool Scenario::setUnisStringId(u16 unitId, u16 newStringId)
{
    return UNIS().replace<u16>(2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds, newStringId);
}

bool Scenario::setUnixStringId(u16 unitId, u16 newStringId)
{
    return UNIx().replace<u16>(2 * (u32)unitId + (u32)UnitSettingsDataLoc::StringIds, newStringId);
}

bool Scenario::ReplaceUPGSSection(buffer &newUPGSSection)
{
    return UPGS().takeAllData(newUPGSSection);
}

bool Scenario::ReplaceUPGxSection(buffer &newUPGxSection)
{
    return UPGx().takeAllData(newUPGxSection);
}

bool Scenario::ReplaceUPGRSection(buffer &newUPGRSection)
{
    return UPGR().takeAllData(newUPGRSection);
}

bool Scenario::ReplacePUPxSection(buffer &newPUPxSection)
{
    return PUPx().takeAllData(newPUPxSection);
}

bool Scenario::upgradeUsesDefaultCosts(u8 upgradeId)
{
    return upgradeSettings().get<u8>((u32)upgradeId) == 1;
}

bool Scenario::getUpgradeMineralCost(u8 upgradeId, u16 &mineralCost)
{
    return upgradeSettings().get<u16>(mineralCost,
        UpgradeSettingsDataLocMineralCost(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeMineralFactor(u8 upgradeId, u16 &mineralFactor)
{
    return upgradeSettings().get<u16>(mineralFactor,
        UpgradeSettingsDataLocMineralFactor(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeGasCost(u8 upgradeId, u16 &gasCost)
{
    return upgradeSettings().get<u16>(gasCost,
        UpgradeSettingsDataLocGasCost(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeGasFactor(u8 upgradeId, u16 &gasFactor)
{
    return upgradeSettings().get<u16>(gasFactor,
        UpgradeSettingsDataLocGasFactor(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeTimeCost(u8 upgradeId, u16 &timeCost)
{
    return upgradeSettings().get<u16>(timeCost,
        UpgradeSettingsDataLocTimeCost(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeTimeFactor(u8 upgradeId, u16 &timeFactor)
{
    return upgradeSettings().get<u16>(timeFactor,
        UpgradeSettingsDataLocTimeFactor(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeDefaultStartLevel(u8 upgradeId, u8 &startLevel)
{
    return upgradeRestrictions().get<u8>(startLevel, UpgradeSettingsDataLocDefaultStartLevel(isExpansion())+(u32)upgradeId);
}

bool Scenario::getUpgradeDefaultMaxLevel(u8 upgradeId, u8 &maxLevel)
{
    return upgradeRestrictions().get<u8>(maxLevel, UpgradeSettingsDataLocDefaultMaxLevel(isExpansion())+(u32)upgradeId);
}

bool Scenario::playerUsesDefaultUpgradeLevels(u8 upgradeId, u8 player)
{
    return upgradeRestrictions().get<u8>(UpgradeSettingsDataLocPlayerUsesDefault(isExpansion(), player)+(u32)upgradeId) == 1;
}

bool Scenario::getUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 &startLevel)
{
    return upgradeRestrictions().get<u8>(startLevel, UpgradeSettingsDataLocPlayerStartLevel(isExpansion(), player)+(u32)upgradeId);
}

bool Scenario::getUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 &maxLevel)
{
    return upgradeRestrictions().get<u8>(maxLevel, UpgradeSettingsDataLocPlayerMaxLevel(isExpansion(), player)+(u32)upgradeId);
}

bool Scenario::ReplaceTECSSection(buffer &newTECSSection)
{
    return TECS().takeAllData(newTECSSection);
}

bool Scenario::ReplaceTECxSection(buffer &newTECxSection)
{
    return TECx().takeAllData(newTECxSection);
}

bool Scenario::ReplacePTECSection(buffer &newPTECSection)
{
    return PTEC().takeAllData(newPTECSection);
}

bool Scenario::ReplacePTExSection(buffer &newPTExSection)
{
    return PTEx().takeAllData(newPTExSection);
}

bool Scenario::techUsesDefaultCosts(u8 techId)
{
    return techSettings().get<u8>((u32)techId) == 1;
}

bool Scenario::getTechMineralCost(u8 techId, u16 &mineralCost)
{
    return techSettings().get<u16>(mineralCost, TechSettingsDataLocMineralCost(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechGasCost(u8 techId, u16 &gasCost)
{
    return techSettings().get<u16>(gasCost, TechSettingsDataLocGasCost(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechTimeCost(u8 techId, u16 &timeCost)
{
    return techSettings().get<u16>(timeCost, TechSettingsDataLocTimeCost(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechEnergyCost(u8 techId, u16 &energyCost)
{
    return techSettings().get<u16>(energyCost, TechSettingsDataLocEnergyCost(isExpansion())+2*(u32)techId);
}

bool Scenario::techAvailableForPlayer(u8 techId, u8 player)
{
    return techRestrictions().get<u8>(PlayerTechSettingsDataLocAvailableForPlayer(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::techResearchedForPlayer(u8 techId, u8 player)
{
    return techRestrictions().get<u8>(PlayerTechSettingsDataLocResearchedForPlayer(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::techAvailableByDefault(u8 techId)
{
    return techRestrictions().get<u8>(PlayerTechSettingsDataLocDefaultAvailability(isExpansion())+(u32)techId) == 1;
}

bool Scenario::techResearchedByDefault(u8 techId)
{
    return techRestrictions().get<u8>(PlayerTechSettingsDataLocDefaultReserached(isExpansion())+(u32)techId) == 1;
}

bool Scenario::playerUsesDefaultTechSettings(u8 techId, u8 player)
{
    return techRestrictions().get<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::setTileset(u16 newTileset)
{
    return ERA().replace<u16>(0, newTileset);
}

u16 Scenario::getTile(u16 xc, u16 yc)
{
    return MTXM().get<u16>(2 * (u32)XSize() * (u32)yc + 2 * (u32)xc);
}

u16 Scenario::getTileSectionTile(u16 xc, u16 yc)
{
    return TILE().get<u16>(2 * (u32)XSize() * (u32)yc + 2 * (u32)xc);
}

bool Scenario::getTile(u16 xc, u16 yc, u16 &tileValue)
{
    return MTXM().get<u16>(tileValue, 2 * (u32)XSize()*(u32)yc + 2 * (u32)xc);
}

bool Scenario::getTileSectionTile(u16 xc, u16 yc, u16 &tileValue)
{
    return TILE().get<u16>(tileValue, 2 * (u32)XSize()*(u32)yc + 2 * (u32)xc);
}

bool Scenario::setTile(u16 xc, u16 yc, u16 value)
{
    TILE().replace<u16>(2 * (u32)XSize() * (u32)yc + 2 * (u32)xc, value);
    return MTXM().replace<u16>(2 * (u32)XSize() * (u32)yc + 2 * (u32)xc, value);
}

bool Scenario::setDimensions(u16 newWidth, u16 newHeight)
{
    u16 oldWidth = XSize(), oldHeight = YSize();
    if ( DIM().replace<u16>(0, newWidth) && DIM().replace<u16>(2, newHeight) )
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

        if ( (((u32)newWidth)/2+1)*(((u32)newHeight)+1)*8 > ISOM().size() )
            ISOM().add<u8>(0, ISOM().size()-((newWidth/2+1)*(newHeight+1)*8));
        else if ( (((u32)newWidth)/2+1)*(((u32)newHeight)+1)*8 < ISOM().size() )
            ISOM().delRange((newWidth/2+1)*(newHeight+1)*8, ISOM().size());

        return true;
    }
    return false;
}

bool Scenario::addUnit(u16 unitID, u8 owner, u16 xc, u16 yc, u16 stateFlags)
{
    if ( UNIT().size()%UNIT_STRUCT_SIZE != 0 )
    {
        if ( !UNIT().add<u8>(0, UNIT_STRUCT_SIZE-UNIT().size()%UNIT_STRUCT_SIZE) )
            return false;
    }

    ChkUnit unit = { };
    unit.id = unitID;
    unit.owner = owner;
    unit.xc = xc;
    unit.yc = yc;
    unit.stateFlags = stateFlags;

    return UNIT().add<ChkUnit&>(unit);
}

bool Scenario::addUnit(ChkUnit unit)
{
    return UNIT().add<ChkUnit&>(unit);
}

bool Scenario::createLocation(s32 xc1, s32 yc1, s32 xc2, s32 yc2, u16& locationIndex)
{
    ChkLocation* curr;
    u16 unusedIndex, afterMaxLoc = u16(MRGN().size()/CHK_LOCATION_SIZE);
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

    if ( unusedIndex == afterMaxLoc && MRGN().size()/CHK_LOCATION_SIZE >= 255 )
        return false;
    else if ( afterMaxLoc < 255 )
        MRGN().add<u8>(0, 20*(255-afterMaxLoc)); // Expand to 255 locations

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
    }
    return false;
}

void Scenario::deleteLocation(u16 locationIndex)
{
    ChkLocation* locRef;
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
    }
}

bool Scenario::removeUnusedString(u32 stringNum)
{
    if ( stringIsUsed(stringNum) || stringNum == 0 )
        return false;
    else if ( cleanStringTable(isExtendedString(stringNum)) )
        return true;
    else // Not enough memory for a full clean, just zero-out the string
        return ZeroOutString(stringNum);
}

void Scenario::forceDeleteString(u32 stringNum)
{
    if ( stringNum == 0 )
        return;

    // MRGN - location strings
    ChkLocation* loc;
    u32 numLocs = MRGN().size()/CHK_LOCATION_SIZE;
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
    u32 numWavs = WAV.size()/4;
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
        ZeroOutString(stringNum); // Not enough memory for a full clean at this time, just zero-out the string
}

template <typename numType> // Strings can, and can only be u16 or u32
bool Scenario::addString(const RawString &str, numType &stringNum, bool extended)
{
    u32 newStringNum;
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
                if ( extended && (u32)stringNum > KSTR().get<u32>(4) )
                {
                    u32 numStrs = KSTR().get<u32>(4);

                    // Add a new string offset after all the others and a prop struct after all the others
                    if ( !(KSTR().insert<u32>(8+4*numStrs, 0) && KSTR().insert<u32>(12+8*numStrs, 0)) )
                        return false;

                    for ( u32 i=1; i<=numStrs; i++ ) // Increment every string offset by 8 to accommodate the new header info
                        KSTR().replace<u32>(4+4*i, KSTR().get<u32>(4+4*i)+8);

                    numStrs ++;
                    KSTR().replace<u32>(4, numStrs);
                }
                else if ( !extended && (u32)stringNum > STR().get<u16>(0) )
                {
                    if ( STR().size() < 65534 )
                    {
                        u16 numStrs = STR().get<u16>(0);
                        for ( u16 i = 1; i <= numStrs; i++ ) // Increment every string offset by 2 to accommodate the new header info
                            STR().replace<u16>(2*i, STR().get<u16>(2*i)+2);

                        // Add a new string offset after all the others
                        if ( !STR().insert<u16>(2+2*numStrs, 0) )
                            return false;

                        numStrs ++;
                        STR().replace<u16>(0, numStrs);
                    }
                    else
                    {
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                        return false;
                    }
                }

                if ( extended )
                {
                    u32 strOffset = KSTR().size();
                    
                    if ( KSTR().addStr(str.c_str(), str.size()) )
                    {
                        if ( str[str.size()-1] != '\0' )
                        {
                            if ( !KSTR().add('\0') )
                                return false;
                        }
                        KSTR().replace<u32>(4+4*(u32(stringNum)), strOffset);
                        stringNum = (65536 - (u32)stringNum);
                        return true;
                    }
                }
                else
                {
                    if ( STR().size() < 65536 )
                    {
                        u16 strOffset = u16(STR().size());
                        if ( STR().addStr(str.c_str(), str.size()) )
                        {
                            if ( str[str.size()-1] != '\0' )
                            {
                                if ( !STR().add('\0') )
                                    return false;
                            }
                            STR().replace<u16>(2*(u32(stringNum)), strOffset);
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
}
template bool Scenario::addString<u16>(const RawString &str, u16 &stringNum, bool extended);
template bool Scenario::addString<u32>(const RawString &str, u32 &stringNum, bool extended);

template <typename numType> // Strings can, and can only be u16 or u32
bool Scenario::addString(const ChkdString &str, numType &stringNum, bool extended)
{
    RawString rawString;
    return ParseChkdStr(str, rawString) &&
        addString<numType>(rawString, stringNum, extended);
}
template bool Scenario::addString<u16>(const ChkdString &str, u16 &stringNum, bool extended);
template bool Scenario::addString<u32>(const ChkdString &str, u32 &stringNum, bool extended);

bool Scenario::addNonExistentString(RawString &str, u32& stringNum, bool extended, std::vector<StringTableNode> &strList)
{
    if ( cleanStringTable(extended, strList) )
    {
        StringUsageTable stringTable;
        if ( stringTable.populateTable(this, extended) )
        {
            if ( stringTable.useNext(stringNum) ) // Get an unused entry if possible
            {
                // Check for a need to make a new entry
                if ( extended && stringNum > KSTR().get<u32>(4) )
                {
                    u32 numStrs = KSTR().get<u32>(4);

                    // Add a new string offset after all the others and a prop struct after all the others
                    if ( !(KSTR().insert<u32>(8+4*numStrs, 0) && KSTR().insert<u32>(12+8*numStrs, 0)) )
                        return false;

                    for ( u32 i=1; i<=numStrs; i++ ) // Increment every string offset by 8 to accommodate the new header info
                        KSTR().replace<u32>(4+4*i, KSTR().get<u32>(4+4*i)+8);

                    numStrs ++;
                    KSTR().replace<u32>(4, numStrs);
                }
                else if ( !extended && stringNum > STR().get<u16>(0) )
                {
                    if ( STR().size() < 65534 )
                    {
                        u16 numStrs = STR().get<u16>(0);
                        for ( u16 i=1; i<= numStrs; i++ ) // Increment every string offset by 2 to accommodate the new header info
                            STR().replace<u16>(2*i, STR().get<u16>(2*i)+2);

                        // Add a new string offset after all the others
                        if ( !STR().insert<u16>(2+2*numStrs, 0) )
                            return false;

                        numStrs ++;
                        STR().replace<u16>(0, numStrs);
                    }
                    else
                    {
                        CHKD_ERR("Cannot add string, STR section has reached its structural limit");
                        return false;
                    }
                }

                if ( extended )
                {
                    u32 strOffset = KSTR().size();
                    
                    if ( KSTR().addStr(str.c_str(), str.size()) )
                    {
                        if ( str[str.size()-1] != '\0' )
                        {
                            if ( !KSTR().add('\0') )
                                return false;
                        }
                        KSTR().replace<u32>(4+4*stringNum, strOffset);
                        stringNum = 65536 - stringNum;
                        return true;
                    }
                }
                else
                {
                    if ( STR().size() < 65536 )
                    {
                        u16 strOffset = u16(STR().size());
                        if ( STR().addStr(str.c_str(), str.size()) )
                        {
                            if ( str[str.size()-1] != '\0' )
                            {
                                if ( !STR().add('\0') )
                                    return false;
                            }
                            STR().replace<u16>(2*stringNum, strOffset);
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
}

template <typename numType>
bool Scenario::replaceString(RawString &newString, numType& stringNum, bool extended, bool safeReplace)
{
    u32 newStringNum, oldStringNum = (u32)stringNum;

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
            if ( extended && !KSTR().exists() ) // Addition of the first KSTR() failed above, nothing more can be done
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
                        stringSectionBackup.setTitle((u32)SectionId::KSTR);
                    else
                        stringSectionBackup.setTitle((u32)SectionId::STR);

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
    }
    return false;
}
template bool Scenario::replaceString<u16>(RawString &newString, u16& stringNum, bool extended, bool safeReplace);
template bool Scenario::replaceString<u32>(RawString &newString, u32& stringNum, bool extended, bool safeReplace);

template <typename numType>
bool Scenario::replaceString(ChkdString &newString, numType &stringNum, bool extended, bool safeReplace)
{
    RawString rawString;
    return ParseChkdStr(newString, rawString) &&
        replaceString<numType>(rawString, stringNum, extended, safeReplace);
}
template bool Scenario::replaceString<u16>(ChkdString &newString, u16& stringNum, bool extended, bool safeReplace);
template bool Scenario::replaceString<u32>(ChkdString &newString, u32& stringNum, bool extended, bool safeReplace);

template <typename numType>
bool Scenario::editString(RawString &newString, numType stringNum, bool extended, bool safeEdit)
{
    RawString testEqual;
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
    }
    return false;
}
template bool Scenario::editString<u16>(RawString &newString, u16 stringNum, bool extended, bool safeEdit);
template bool Scenario::editString<u32>(RawString &newString, u32 stringNum, bool extended, bool safeEdit);

template <typename numType>
bool Scenario::editString(ChkdString &newString, numType stringNum, bool extended, bool safeEdit)
{
    RawString rawString;
    return ParseChkdStr(newString, rawString) &&
        editString(rawString, stringNum, extended, safeEdit);
}
template bool Scenario::editString<u16>(ChkdString &newString, u16 stringNum, bool extended, bool safeEdit);
template bool Scenario::editString<u32>(ChkdString &newString, u32 stringNum, bool extended, bool safeEdit);

void Scenario::replaceStringNum(u32 toReplace, u32 replacement)
{
    ChkLocation* loc;
    for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            if ( trig->actions[i].stringNum == toReplace )
                trig->actions[i].stringNum = replacement;
            if ( trig->actions[i].wavID == toReplace )
                trig->actions[i].wavID = replacement;
        }

        trigNum ++;
    }
    
    if ( SPRP().get<u16>(0) == toReplace ) // Scenario Name
        SPRP().replace<u16>(0, (u16)replacement);
    if ( SPRP().get<u16>(2) == toReplace ) // Scenario Description
        SPRP().replace<u16>(2, (u16)replacement);

    for ( int i=0; i<4; i++ )
    {
        if ( getForceStringNum(i) == toReplace )
            FORC().replace<u16>(8+2*i, (u16)replacement);
    }

    for ( u32 i=0; i<WAV().size()/4; i++ )
    {
        if ( WAV().get<u32>(i*4) == toReplace )
            WAV().replace<u32>(i*4, replacement);
    }

    for ( int i=0; i<228; i++ )
    {
        if ( UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == toReplace )
            UNIS().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)replacement);
        if ( UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == toReplace )
            UNIx().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)replacement);
    }

    for ( int i=0; i<256; i++ )
    {
        if ( SWNM().get<u32>(i*4) == toReplace )
            SWNM().replace<u32>(i*4, replacement);
    }
}

void Scenario::removeMetaStrings()
{

}

bool Scenario::cleanStringTable(bool extendedTable)
{
    std::vector<StringTableNode> strList;

    if ( extendedTable )
        return addAllUsedStrings(strList, false, true) && rebuildStringTable(strList, extendedTable);
    else
        return addAllUsedStrings(strList, true, false) && rebuildStringTable(strList, extendedTable);
}

bool Scenario::cleanStringTable(bool extendedTable, std::vector<StringTableNode> &strList)
{
    return rebuildStringTable(strList, extendedTable);
}

bool Scenario::removeDuplicateStrings()
{
    std::vector<StringTableNode> strList;

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
    else
        return false;
}

bool Scenario::compressStringTable(bool extendedTable, bool recycleSubStrings)
{
    if ( recycleSubStrings )
        return false; // Unimplemented

    removeDuplicateStrings();

    std::vector<StringTableNode> strList;

    StringUsageTable strUsage;
    if ( strUsage.populateTable(this, extendedTable) && addAllUsedStrings(strList, !extendedTable, extendedTable) )
    {
        u32 firstEmpty, lastFragmented;
        while ( strUsage.popFragmentedString(firstEmpty, lastFragmented) )
        {
            if ( ( extendedTable && KSTR().swap<u32>(4+4*lastFragmented, 4+4*firstEmpty) ) ||
                 ( !extendedTable && STR().swap<u16>(lastFragmented*2, firstEmpty*2) ) )
            {
                ChkLocation* loc;
                for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
                {
                    if ( getLocation(loc, u8(i)) && loc->stringNum == lastFragmented )
                        loc->stringNum = (u16)firstEmpty;
                }

                Trigger* trig;
                int trigNum = 0;
                while ( getTrigger(trig, trigNum) )
                {
                    for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
                    for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
                    {
                        if ( trig->actions[i].stringNum == lastFragmented )
                            trig->actions[i].stringNum = firstEmpty;
                        if ( trig->actions[i].wavID == lastFragmented )
                            trig->actions[i].wavID = firstEmpty;
                    }

                    trigNum ++;
                }
    
                if ( (u32)SPRP().get<u16>(0) == lastFragmented ) // Scenario Name
                    SPRP().replace<u16>(0, (u16)firstEmpty);

                if ( (u32)SPRP().get<u16>(2) == lastFragmented ) // Scenario Description
                    SPRP().replace<u16>(2, (u16)firstEmpty);

                for ( int i=0; i<4; i++ )
                {
                    if ( getForceStringNum(i) == lastFragmented )
                        FORC().replace<u16>(8+2*i, (u16)firstEmpty);
                }

                for ( u32 i=0; i<WAV().size()/4; i++ )
                {
                    if ( WAV().get<u32>(i*4) == lastFragmented )
                        WAV().replace<u32>(i*4, firstEmpty);
                }

                buffer &unitSettings = this->unitSettings();
                for ( int i=0; i<228; i++ )
                {
                    if ( unitSettings.get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) == lastFragmented )
                        unitSettings.replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)firstEmpty);
                }

                for ( int i=0; i<256; i++ )
                {
                    if ( SWNM().get<u32>(i*4) == lastFragmented )
                        SWNM().replace<u32>(i*4, firstEmpty);
                }
            }
        }

        StringUsageTable altStrUsage;
        if ( altStrUsage.populateTable(this, extendedTable) )
        {
            u32 lastUsed = altStrUsage.lastUsedString();
            if ( lastUsed <= 1024 && STR().get<u16>(0) > 1024 )
                STR().replace<u16>(0, 1024);
        }

        std::vector<StringTableNode> newStrList;
        return addAllUsedStrings(newStrList, !extendedTable, extendedTable) && rebuildStringTable(newStrList, extendedTable);
    }

    return false;
}

bool Scenario::repairStringTable(bool extendedTable)
{
    u32 stringNum = 0;

    ChkLocation* loc;
    for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            stringNum = trig->actions[i].stringNum;
            ActionId action = (ActionId)trig->actions[i].action;
            if ( action == ActionId::Comment || action == ActionId::DisplayTextMessage || action == ActionId::LeaderboardCtrlAtLoc ||
                 action == ActionId::LeaderboardCtrl || action == ActionId::LeaderboardKills || action == ActionId::LeaderboardPoints ||
                 action == ActionId::LeaderboardResources || action == ActionId::LeaderboardGoalCtrlAtLoc ||
                 action == ActionId::LeaderboardGoalCtrl || action == ActionId::LeaderboardGoalKills ||
                 action == ActionId::LeaderboardGoalPoints || action == ActionId::LeaderboardGoalResources ||
                 action == ActionId::SetMissionObjectives || action == ActionId::SetNextScenario || action == ActionId::Transmission )
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
            action = (ActionId)trig->actions[i].action;
            if ( action == ActionId::PlayWav || action == ActionId::Transmission )
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
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
    
    stringNum = SPRP().get<u16>(0); // Scenario Name
    if ( RepairString(stringNum, false) )
        SPRP().replace<u16>(0, (u16)stringNum);

    stringNum = SPRP().get<u16>(2); // Scenario Description
    if ( RepairString(stringNum, false) )
        SPRP().replace<u16>(0, (u16)stringNum);

    for ( int i=0; i<4; i++ )
    {
        stringNum = getForceStringNum(i);
        if ( RepairString(stringNum, false) )
            FORC().replace<u16>(8+2*i, (u16)stringNum);
    }

    for ( u32 i=0; i<WAV().size()/4; i++ )
    {
        RawString str;
        stringNum = WAV().get<u32>(i*4);
        // if stringNum is nonzero and does not have valid contents...
        if ( stringNum != 0 && !( stringExists(stringNum) && GetString(str, stringNum) && str.size() > 0 ) )
        {
            WAV().replace<u32>(i*4, 0);
            removeUnusedString(stringNum);
        }
    }

    buffer &unitSettings = this->unitSettings();
    bool expansionUnitSettings = ( (&unitSettings) == (&UNIx()) );
    if ( expansionUnitSettings )
    {
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            if ( stringNum != 0 && RepairString(stringNum, false) )
                UNIx().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)stringNum);
        }
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            UNIS().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds));
        }
    }
    else
    {
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            if ( stringNum != 0 && RepairString(stringNum, false) )
                UNIS().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, (u16)stringNum);
        }
        for ( int i=0; i<228; i++ )
        {
            stringNum = UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds);
            UNIx().replace<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds, UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds));
        }
    }

    for ( int i=0; i<256; i++ )
    {
        stringNum = SWNM().get<u32>(i*4);
        if ( stringNum != 0 && RepairString(stringNum, false) )
            SWNM().replace<u32>(i*4, stringNum);
    }

    return compressStringTable(extendedTable, false);
}

bool Scenario::addAllUsedStrings(std::vector<StringTableNode>& strList, bool includeStandard, bool includeExtended)
{
    std::hash<std::string> strHash;
    std::unordered_multimap<u32, StringTableNode> stringSearchTable;
    strList.reserve(strList.size()+stringCapacity());
    StringTableNode node;
    int numMatching = 0;
    u32 hash = 0;
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
                    stringSearchTable.insert( std::pair<u32, StringTableNode>(          \
                        strHash(node.string), node) );                                  \
                }                                                                       \
            }                                                                           \
        }

    try
    {
        ChkLocation* loc;
        for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
        {
            if ( getLocation(loc, u8(i)) )
                AddStrIfOverZero(loc->stringNum);
        }

        Trigger* trig;
        int trigNum = 0;
        while ( getTrigger(trig, trigNum) )
        {
            for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
            {
                AddStrIfOverZero( trig->actions[i].stringNum );
                AddStrIfOverZero( trig->actions[i].wavID );
            }
            
            trigNum ++;
        }

        trigNum = 0;
        while ( getBriefingTrigger(trig, trigNum) )
        {
            for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
            {
                AddStrIfOverZero( trig->actions[i].stringNum );
                AddStrIfOverZero( trig->actions[i].wavID );
            }

            trigNum ++;
        }
    
        AddStrIfOverZero( SPRP().get<u16>(0) ); // Scenario Name
        AddStrIfOverZero( SPRP().get<u16>(2) ); // Scenario Description

        for ( int i=0; i<4; i++ )
            AddStrIfOverZero( getForceStringNum(i) );

        for ( u32 i=0; i<WAV().size()/4; i++ )
            AddStrIfOverZero( WAV().get<u32>(i*4) );

        buffer &unitSettings = this->unitSettings();
        for ( int i=0; i<228; i++ )
        {
            AddStrIfOverZero( UNIS().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
            AddStrIfOverZero( UNIx().get<u16>(i*2+(u32)UnitSettingsDataLoc::StringIds) );
        }

        for ( int i=0; i<256; i++ )
            AddStrIfOverZero( SWNM().get<u32>(i*4) );

        strList.shrink_to_fit();
        return true;
    }
    catch ( std::bad_alloc )
    {
        return false;
    }
}

bool Scenario::rebuildStringTable(std::vector<StringTableNode> &strList, bool extendedTable)
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
        numStrs = strList.back().stringNum+1;

    if ( numStrs < prevNumStrings ) // Ensure there are at least as many entries as before
        numStrs = prevNumStrings;
    if ( !extendedTable && numStrs < 1024 ) // Ensure there are at least 1024 string entries, unless it's an extended table
        numStrs = 1024;
    else if ( numStrs == 0 )
        numStrs = 1;

    // Create a new string section
    Section newStrSection(new buffer("nStr"));
    u32 strPortionOffset = 0;
    if ( extendedTable )
    {
        newStrSection->setTitle((u32)SectionId::KSTR);
        if ( !( newStrSection->add<u32>(2) && newStrSection->add<u32>(numStrs) ) )
            return false; // Out of memory
        strPortionOffset = 8 + 8*numStrs;
    }
    else
    {
        newStrSection->setTitle((u32)SectionId::STR);
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
        u32 lengthToSave = extendedTable ? 8 : 2;
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

        if ( KSTR().exists() )
        {
            if ( newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) )
                return KSTR().takeAllData(*newStrSection);
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
        if ( STR().exists() ) // Map had a string section
        {
            if ( newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) )
                return STR().takeAllData(*newStrSection);
            else
                return false; // Out of memory
        }
        else // Map didn't have a string section
        {
            return newStrSection->addStr((const char*)strPortion.getPtr(0), strPortion.size()) &&
                   AddSection(newStrSection);
        }
    }
}

bool Scenario::buildStringTables(std::vector<StringTableNode> &strList, bool extendedTable,
    Section &offsets, buffer &strPortion, u32 strPortionOffset, u32 numStrs, bool recycleSubStrings)
{
    u32 strIndex = 1;
    if ( recycleSubStrings )
    {
        std::vector<SubStringPtr> subStrings = SubString::GetSubStrings(strList);

        for ( auto &subStr : subStrings )
        {
            const StringTableNode& str = *(subStr->GetString());
            while ( strIndex < str.stringNum ) // Add any unused's till the next string, point them to the 'NUL' string
            {
                if ( (extendedTable && offsets->add<u32>(strPortionOffset)) ||      // Add string offset (extended)
                     (!extendedTable && offsets->add<u16>(u16(strPortionOffset))) ) // Add string offset (regular)
                {
                    strIndex++;
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
                    strIndex++;
                }
                else
                    return false; // Out of memory
            }
            else // This string is not a sub-string of another
            {
                subStr->userData = (void*)(strPortion.size() + strPortionOffset);

                if ( ((extendedTable && offsets->add<u32>(strPortion.size() + strPortionOffset)) ||        // Add string offset (extended)
                      (!extendedTable && offsets->add<u16>(u16(strPortion.size() + strPortionOffset)))) && // Add string offset (regular)
                     strPortion.addStr(str.string.c_str(), str.string.size() + 1) ) // Add the string + its NUL char
                {
                    strIndex++;
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
                u32 parentStart = (u32)parent->userData;
                u32 childStart = parentStart + (parent->GetString()->string.size() - str.string.size());

                if ( (extendedTable && offsets->replace<u32>(4+4*str.stringNum, childStart)) ||   // Add string offset (extended)
                    (!extendedTable && offsets->replace<u16>(2*str.stringNum, (u16)childStart)) ) // Add string offset (regular)
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
            while ( strIndex < str.stringNum ) // Add any unused's till the next string, point them to the 'NUL' string
            {
                if ( (extendedTable && offsets->add<u32>(strPortionOffset)) ||     // Add string offset (extended)
                    (!extendedTable && offsets->add<u16>(u16(strPortionOffset))) ) // Add string offset (regular)
                {
                    strIndex++;
                }
                else
                    return false; // Out of memory
            }

            if ( !extendedTable && strPortionOffset + strPortion.size() > 65535 )
            {
                CHKD_ERR("Structural STR limits exceeded, header information for %u string took up %u bytes.", numStrs, 2 + numStrs * 2);
                return false; // No room for the next string
            }

            if ( ((extendedTable && offsets->add<u32>(strPortion.size() + strPortionOffset)) ||        // Add string offset (extended)
                  (!extendedTable && offsets->add<u16>(u16(strPortion.size() + strPortionOffset)))) && // Add string offset (regular)
                 strPortion.addStr(str.string.c_str(), str.string.size() + 1) ) // Add the string + its NUL char
            {
                strIndex++;
            }
            else
                return false; // Out of memory
        }
    }

    // Add any unused's that come after the last string, point them to the 'NUL' string
    while ( strIndex <= numStrs )
    {
        if ( (extendedTable && offsets->add<u32>(strPortionOffset)) ||      // Add string offset (extended)
             (!extendedTable && offsets->add<u16>(u16(strPortionOffset))) ) // Add string offset (regular)
        {
            strIndex++;
        }
        else
            return false; // Out of memory
    }

    return true;
}

void Scenario::correctMTXM()
{
    bool didCorrection;

    do
    {
        didCorrection = false;

        Section lastInstance(nullptr);

        for ( auto &section : sections ) // Cycle through all sections
        {
            SectionId sectionId = (SectionId)section->titleVal();
            if ( sectionId == SectionId::MTXM )
            {
                if ( lastInstance != nullptr && lastInstance->size() == XSize()*YSize()*2 && section->size() != XSize()*YSize()*2 )
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
    } while ( didCorrection );
}

bool Scenario::setPlayerOwner(u8 player, u8 newOwner)
{
    IOWN().replace<u8>((u32)player, newOwner); // This section is not required by starcraft and shouldn't affect function success
    return OWNR().replace<u8>((u32)player, newOwner);
}

bool Scenario::setPlayerRace(u8 player, u8 newRace)
{
    return SIDE().replace<u8>((u32)player, newRace);
}

bool Scenario::setPlayerColor(u8 player, u8 newColor)
{
    return COLR().replace<u8>((u32)player, newColor);
}

bool Scenario::setPlayerForce(u8 player, u8 newForce)
{
    if ( player < 8 )
        return FORC().replace<u8>(player, newForce);
    else
        return false;
}

bool Scenario::setForceAllied(u8 forceNum, bool allied)
{
    if ( forceNum < 4 )
        return FORC().setBit(16+forceNum, 1, allied);
    else
        return false;
}

bool Scenario::setForceVision(u8 forceNum, bool sharedVision)
{
    if ( forceNum < 4 )
        return FORC().setBit(16+forceNum, 3, sharedVision);
    else
        return false;
}

bool Scenario::setForceRandom(u8 forceNum, bool randomStartLocs)
{
    if ( forceNum < 4 )
        return FORC().setBit(16+forceNum, 0, randomStartLocs);
    else
        return false;
}

bool Scenario::setForceAv(u8 forceNum, bool alliedVictory)
{
    if ( forceNum < 4 )
        return FORC().setBit(16+forceNum, 2, alliedVictory);
    else
        return false;
}

bool Scenario::setForceName(u8 forceNum, ChkdString &newName)
{
    u16* mapForceString = nullptr;
    return FORC().getPtr<u16>(mapForceString, 8 + 2 * (u32)forceNum, 2) &&
        replaceString(newName, *mapForceString, false, true);
}

bool Scenario::setUnitUseDefaults(u16 unitID, bool useDefaults)
{
    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = UNIx().replace<u8>((u32)unitID, newValue);
    setNormal = UNIS().replace<u8>((u32)unitID, newValue);

    return (isExpansion() && setExp) || (!isExpansion() && setNormal);
}

bool Scenario::setUnitEnabled(u16 unitId, bool enabled)
{
    if ( enabled )
        return PUNI().replace<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability, 1);
    else
        return PUNI().replace<u8>(unitId+(u32)PlayerUnitSettingsDataLoc::GlobalAvailability, 0);
}

bool Scenario::setUnitEnabledState(u16 unitId, u8 player, UnitEnabledState unitEnabledState)
{
    if ( unitEnabledState == UnitEnabledState::Default )
    {
        if ( PUNI().replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 1) ) // Make player use default for this unit
        {
            PUNI().replace<u8>((u32)unitId+228*(u32)player, 0); // Clear player's unit enabled state (for compression, not necessary)
            return true;
        }
    }
    else if ( unitEnabledState == UnitEnabledState::Enabled )
    {
        return PUNI().replace<u8>((u32)unitId+228*(u32)player, 1) && // Set player's unit enabled state to enabled
               PUNI().replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 0); // Make player override this unit's default
    }
    else if ( unitEnabledState == UnitEnabledState::Disabled )
    {
        return PUNI().replace<u8>((u32)unitId+228*(u32)player, 0) && // Set player's unit enabled state to disabed
               PUNI().replace<u8>((u32)unitId+228*(u32)player+(u32)PlayerUnitSettingsDataLoc::PlayerUsesDefault, 0); // Make player override this unit's default
    }
    return false;
}

bool Scenario::setUnitSettingsCompleteHitpoints(u16 unitId, u32 completeHitpoints)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
         replacedUNIx = UNIx().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);
    
    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsHitpoints(u16 unitId, u32 hitpoints)
{
    u32 completeHitpoints;
    if ( unitSettings().get<u32>(completeHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        u32 hitpointsByte = completeHitpoints%256;
        completeHitpoints = hitpoints*256+(u32)hitpointsByte;

        bool expansion = isExpansion(),
             replacedUNIS = UNIS().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
             replacedUNIx = UNIx().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);
        return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
    }
    else
        return false;
}

bool Scenario::setUnitSettingsHitpointByte(u16 unitId, u8 hitpointByte)
{
    u32 completeHitpoints;
    if ( unitSettings().get<u32>(completeHitpoints, 4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints) )
    {
        u32 hitpoints = completeHitpoints/256;
        completeHitpoints = 256*hitpoints+(u32)hitpointByte;

        bool expansion = isExpansion(),
             replacedUNIS = UNIS().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints),
             replacedUNIx = UNIx().replace<u32>(4*(u32)unitId+(u32)UnitSettingsDataLoc::HitPoints, completeHitpoints);
        return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
    }
    else
        return false;
}

bool Scenario::setUnitSettingsShieldPoints(u16 unitId, u16 shieldPoints)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::ShieldPoints, shieldPoints),
         replacedUNIx = UNIx().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::ShieldPoints, shieldPoints);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsArmor(u16 unitId, u8 armor)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u8>((u32)unitId+(u32)UnitSettingsDataLoc::Armor, armor),
         replacedUNIx = UNIx().replace<u8>((u32)unitId+(u32)UnitSettingsDataLoc::Armor, armor);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBuildTime(u16 unitId, u16 buildTime)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime, buildTime),
         replacedUNIx = UNIx().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::BuildTime, buildTime);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsMineralCost(u16 unitId, u16 mineralCost)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost, mineralCost),
         replacedUNIx = UNIx().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::MineralCost, mineralCost);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsGasCost(u16 unitId, u16 gasCost)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost, gasCost),
         replacedUNIx = UNIx().replace<u16>(2*(u32)unitId+(u32)UnitSettingsDataLoc::GasCost, gasCost);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBaseWeapon(u32 weaponId, u16 baseDamage)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon, baseDamage),
         replacedUNIx = UNIx().replace<u16>(2*weaponId+(u32)UnitSettingsDataLoc::BaseWeapon, baseDamage);
    
    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBonusWeapon(u32 weaponId, u16 bonusDamage)
{
    bool expansion = isExpansion(),
         replacedUNIS = UNIS().replace<u16>(UnitSettingsDataLocBonusWeapon(false)+2*weaponId, bonusDamage),
         replacedUNIx = UNIx().replace<u16>(UnitSettingsDataLocBonusWeapon(true)+2*weaponId, bonusDamage);

    return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitName(u16 unitId, RawString &unitName)
{
    u16* originalNameString = nullptr;
    u16* expansionNameString = nullptr;
    bool gotOrig = UNIx().getPtr<u16>(originalNameString, 2 * unitId + (u32)UnitSettingsDataLoc::StringIds, 2);
    bool gotExp = UNIS().getPtr<u16>(expansionNameString, 2 * unitId + (u32)UnitSettingsDataLoc::StringIds, 2);
    bool replacedOrig = false, replacedExp = false;
    gotOrig ? replacedOrig = replaceString(unitName, *originalNameString, false, true) : replacedOrig = false;
    gotExp ? replacedExp = replaceString(unitName, *expansionNameString, false, true) : replacedExp = false;
    return (isExpansion() && replacedExp) || (!isExpansion() && replacedOrig);
}


bool Scenario::setUnitName(u16 unitId, ChkdString &unitName)
{
    RawString rawUnitName;
    if ( ParseChkdStr(unitName, rawUnitName) )
        return setUnitName(unitId, rawUnitName);
    else
        return false;
}

bool Scenario::setUpgradeUseDefaults(u8 upgradeNum, bool useDefaults)
{
    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = UPGx().replace<u8>((u32)upgradeNum, newValue);
    setNormal = UPGS().replace<u8>((u32)upgradeNum, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setUpgradeMineralCost(u8 upgradeId, u16 mineralCost)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocMineralCost(false)+2*(u32)upgradeId, mineralCost),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocMineralCost(true)+2*(u32)upgradeId, mineralCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeMineralFactor(u8 upgradeId, u16 mineralFactor)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocMineralFactor(false)+2*(u32)upgradeId, mineralFactor),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocMineralFactor(true)+2*(u32)upgradeId, mineralFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeGasCost(u8 upgradeId, u16 gasCost)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocGasCost(false)+2*(u32)upgradeId, gasCost),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocGasCost(true)+2*(u32)upgradeId, gasCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeGasFactor(u8 upgradeId, u16 gasFactor)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocGasFactor(false)+2*(u32)upgradeId, gasFactor),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocGasFactor(true)+2*(u32)upgradeId, gasFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeTimeCost(u8 upgradeId, u16 timeCost)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocTimeCost(false)+2*(u32)upgradeId, timeCost),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocTimeCost(true)+2*(u32)upgradeId, timeCost);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeTimeFactor(u8 upgradeId, u16 timeFactor)
{
    bool expansion = isExpansion(),
         replacedUPGS = UPGS().replace<u16>(UpgradeSettingsDataLocTimeFactor(false)+2*(u32)upgradeId, timeFactor),
         replacedUPGx = UPGx().replace<u16>(UpgradeSettingsDataLocTimeFactor(true)+2*(u32)upgradeId, timeFactor);

    return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradePlayerDefaults(u8 upgradeId, u8 player, bool usesDefaultLevels)
{
    u8 newValue = 0;
    if ( usesDefaultLevels )
        newValue = 1;

    bool expansion = isExpansion(),
         replacedUPGR = UPGR().replace<u8>(UpgradeSettingsDataLocPlayerUsesDefault(false, player)+(u32)upgradeId, newValue),
         replacedPUPx = PUPx().replace<u8>(UpgradeSettingsDataLocPlayerUsesDefault(true, player)+(u32)upgradeId, newValue);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 newStartLevel)
{
    bool expansion = isExpansion(),
         replacedUPGR = UPGR().replace<u8>(UpgradeSettingsDataLocPlayerStartLevel(false, player)+(u32)upgradeId, newStartLevel),
         replacedPUPx = PUPx().replace<u8>(UpgradeSettingsDataLocPlayerStartLevel(true, player)+(u32)upgradeId, newStartLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 newMaxLevel)
{
    bool expansion = isExpansion(),
         replacedUPGR = UPGR().replace<u8>(UpgradeSettingsDataLocPlayerMaxLevel(false, player)+(u32)upgradeId, newMaxLevel),
         replacedPUPx = PUPx().replace<u8>(UpgradeSettingsDataLocPlayerMaxLevel(true, player)+(u32)upgradeId, newMaxLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradeDefaultStartLevel(u8 upgradeId, u8 newStartLevel)
{
    bool expansion = isExpansion(),
         replacedUPGR = UPGR().replace<u8>(UpgradeSettingsDataLocDefaultStartLevel(false)+(u32)upgradeId, newStartLevel),
         replacedPUPx = PUPx().replace<u8>(UpgradeSettingsDataLocDefaultStartLevel(true)+(u32)upgradeId, newStartLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradeDefaultMaxLevel(u8 upgradeId, u8 newMaxLevel)
{
    bool expansion = isExpansion(),
         replacedUPGR = UPGR().replace<u8>(UpgradeSettingsDataLocDefaultMaxLevel(false)+(u32)upgradeId, newMaxLevel),
         replacedPUPx = PUPx().replace<u8>(UpgradeSettingsDataLocDefaultMaxLevel(true)+(u32)upgradeId, newMaxLevel);

    return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setTechUseDefaults(u8 techNum, bool useDefaults)
{
    bool setExp = false, setNormal = false;
    u8 newValue = 0;
    if ( useDefaults )
        newValue = 1;

    setExp = TECx().replace<u8>((u32)techNum, newValue);
    setNormal = TECS().replace<u8>((u32)techNum, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechMineralCost(u8 techId, u16 newMineralCost)
{
    bool expansion = isExpansion(),
         replacedTECS = TECS().replace<u16>(TechSettingsDataLocMineralCost(false)+2*(u32)techId, newMineralCost),
         replacedTECx = TECx().replace<u16>(TechSettingsDataLocMineralCost(true)+2*(u32)techId, newMineralCost);
    
    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechGasCost(u8 techId, u16 newGasCost)
{
    bool expansion = isExpansion(),
         replacedTECS = TECS().replace<u16>(TechSettingsDataLocGasCost(false)+2*(u32)techId, newGasCost),
         replacedTECx = TECx().replace<u16>(TechSettingsDataLocGasCost(true)+2*(u32)techId, newGasCost);
    
    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechTimeCost(u8 techId, u16 newTimeCost)
{
    bool expansion = isExpansion(),
         replacedTECS = TECS().replace<u16>(TechSettingsDataLocTimeCost(false)+2*(u32)techId, newTimeCost),
         replacedTECx = TECx().replace<u16>(TechSettingsDataLocTimeCost(true)+2*(u32)techId, newTimeCost);
    
    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechEnergyCost(u8 techId, u16 newEnergyCost)
{
    bool expansion = isExpansion(),
         replacedTECS = TECS().replace<u16>(TechSettingsDataLocEnergyCost(false)+2*(u32)techId, newEnergyCost),
         replacedTECx = TECx().replace<u16>(TechSettingsDataLocEnergyCost(true)+2*(u32)techId, newEnergyCost);
    
    return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechAvailableForPlayer(u8 techId, u8 player, bool availableForPlayer)
{
    u8 newValue = 0;
    if ( availableForPlayer )
        newValue = 1;

    bool setExp = PTEx().replace<u8>(PlayerTechSettingsDataLocAvailableForPlayer(true, player)+(u32)techId, newValue),
         setNormal = PTEC().replace<u8>(PlayerTechSettingsDataLocAvailableForPlayer(false, player)+(u32)techId, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechResearchedForPlayer(u8 techId, u8 player, bool researchedForPlayer)
{
    u8 newValue = 0;
    if ( researchedForPlayer )
        newValue = 1;

    bool setExp = PTEx().replace<u8>(PlayerTechSettingsDataLocResearchedForPlayer(true, player)+(u32)techId, newValue),
         setNormal = PTEC().replace<u8>(PlayerTechSettingsDataLocResearchedForPlayer(false, player)+(u32)techId, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechDefaultAvailability(u8 techId, bool availableByDefault)
{
    u8 newValue = 0;
    if ( availableByDefault )
        newValue = 1;

    bool setExp = PTEx().replace<u8>(PlayerTechSettingsDataLocDefaultAvailability(true)+(u32)techId, newValue),
         setNormal = PTEC().replace<u8>(PlayerTechSettingsDataLocDefaultAvailability(false)+(u32)techId, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechDefaultResearched(u8 techId, bool researchedByDefault)
{
    u8 newValue = 0;
    if ( researchedByDefault )
        newValue = 1;

    bool setExp = PTEx().replace<u8>(PlayerTechSettingsDataLocDefaultReserached(true)+(u32)techId, newValue),
         setNormal = PTEC().replace<u8>(PlayerTechSettingsDataLocDefaultReserached(false)+(u32)techId, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setPlayerUsesDefaultTechSettings(u8 techId, u8 player, bool playerUsesDefaultSettings)
{
    u8 newValue = 0;
    if ( playerUsesDefaultSettings )
        newValue = 1;

    bool setExp = PTEx().replace<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(true, player)+(u32)techId, newValue),
         setNormal = PTEC().replace<u8>(PlayerTechSettingsDataLocPlayerUsesDefault(false, player)+(u32)techId, newValue);
    
    return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::addTrigger(Trigger &trigger)
{
    return TRIG().add<Trigger&>(trigger);
}

bool Scenario::insertTrigger(u32 triggerId, Trigger &trigger)
{
    return TRIG().insert<Trigger&>(triggerId*TRIG_STRUCT_SIZE, trigger);
}

bool Scenario::deleteTrigger(u32 triggerId)
{
    return TRIG().del(triggerId*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE);
}

bool Scenario::copyTrigger(u32 triggerId)
{
    Trigger trig;
    return TRIG().get<Trigger>(trig, triggerId*TRIG_STRUCT_SIZE) &&
           TRIG().insert<Trigger&>(triggerId*TRIG_STRUCT_SIZE+TRIG_STRUCT_SIZE, trig);
}

bool Scenario::moveTriggerUp(u32 triggerId)
{
    return triggerId > 0 && // Not already first trigger
           TRIG().swap<Trigger>(triggerId*TRIG_STRUCT_SIZE, (triggerId-1)*TRIG_STRUCT_SIZE);
}

bool Scenario::moveTriggerDown(u32 triggerId)
{
    return triggerId < ((TRIG().size()/TRIG_STRUCT_SIZE)-1) && // Not already the last trigger
           TRIG().swap<Trigger>(triggerId*TRIG_STRUCT_SIZE, (triggerId+1)*TRIG_STRUCT_SIZE);
}

bool Scenario::moveTrigger(u32 triggerId, u32 destId)
{
    if ( destId == triggerId-1 )
        return moveTriggerUp(triggerId);
    else if ( destId == triggerId+1 )
        return moveTriggerDown(triggerId);
    else
    {
        Trigger trigger;
        if ( triggerId != destId && // Not equivilant ids
             destId < (TRIG().size()/TRIG_STRUCT_SIZE) && // Destination less than num trigs
             TRIG().get<Trigger>(trigger, triggerId*TRIG_STRUCT_SIZE) && // Get contents of trigger
             TRIG().del<Trigger>(triggerId*TRIG_STRUCT_SIZE) ) // Delete trigger from old position
        {
            if ( TRIG().insert<Trigger&>(destId*TRIG_STRUCT_SIZE, trigger) ) // Attempt to insert at new position
                return true;
            else
                TRIG().insert<Trigger&>(triggerId*TRIG_STRUCT_SIZE, trigger); // Attempt to recover old position
        }
    }
    return false;
}

TrigSegment Scenario::GetTrigSection()
{
    return trig;
}

bool Scenario::GetCuwp(u8 cuwpIndex, ChkCuwp &outPropStruct)
{
    return UPRP().get<ChkCuwp>(outPropStruct, (u32)cuwpIndex*sizeof(ChkCuwp));
}

bool Scenario::AddCuwp(ChkCuwp &propStruct, u8 &outCuwpIndex)
{
    for ( u8 i = 0; i < 64; i++ )
    {
        if ( !IsCuwpUsed(i) )
        {
            if ( UPRP().replace<ChkCuwp>((u32)i*sizeof(ChkCuwp), propStruct) && UPUS().replace<u8>(i, 1) )
            {
                outCuwpIndex = i;
                return true;
            }
            else
                return false;
        }
    }
    return false;
}

bool Scenario::ReplaceCuwp(ChkCuwp &propStruct, u8 cuwpIndex)
{
    return UPRP().replace<ChkCuwp>((u32)cuwpIndex*sizeof(ChkCuwp), propStruct);
}

bool Scenario::IsCuwpUsed(u8 cuwpIndex)
{
    return UPUS().get<u8>(cuwpIndex) != 0;
}

bool Scenario::SetCuwpUsed(u8 cuwpIndex, bool isUsed)
{
    if ( isUsed )
        return UPUS().replace<u8>(cuwpIndex, 1);
    else
        return UPUS().replace<u8>(cuwpIndex, 0);
}

bool Scenario::FindCuwp(const ChkCuwp &cuwpToFind, u8 &outCuwpIndex)
{
    u8 cuwpCapacity = (u8)CuwpCapacity();
    ChkCuwp cuwp = {};
    for ( u8 i = 0; i < cuwpCapacity; i++ )
    {
        if ( UPRP().get<ChkCuwp>(cuwp, sizeof(ChkCuwp)*(u32)i) &&
             memcmp(&cuwpToFind, &cuwp, sizeof(ChkCuwp)) == 0 )
        {
            outCuwpIndex = i;
            return true;
        }
    }
    return false;
}

int Scenario::CuwpCapacity()
{
    return 64;
}

int Scenario::NumUsedCuwps()
{
    int numUsedCuwps = 0;
    for ( u8 i = 0; i < 64; i++ )
    {
        if ( IsCuwpUsed(i) )
            numUsedCuwps++;
    }
    return numUsedCuwps;
}

u32 Scenario::WavSectionCapacity()
{
    return WAV().size() / 4;
}

bool Scenario::GetWav(u16 wavIndex, u32 &outStringIndex)
{
    return WAV().get<u32>(outStringIndex, (u32)wavIndex * 4) && outStringIndex > 0;
}

bool Scenario::GetWavString(u16 wavIndex, RawString &outString)
{
    u32 stringIndex = 0;
    return WAV().get<u32>(stringIndex, (u32)wavIndex * 4) && stringIndex > 0 && GetString(outString, stringIndex);
}

bool Scenario::AddWav(u32 stringIndex)
{
    if ( IsStringUsedWithWavs(stringIndex) )
        return true; // No need to add a duplicate entry

    for ( u32 i = 0; i < 512; i++ )
    {
        if ( !WavHasString(i) && WAV().replace<u32>(i * 4, stringIndex) )
            return true;
    }
    return false;
}

bool Scenario::AddWav(RawString &wavMpqPath)
{
    u32 stringIndex = 0;
    if ( addString(wavMpqPath, stringIndex, false) )
    {
        if ( AddWav(stringIndex) )
        {
            return true;
        }
    }
    return false;
}

bool Scenario::RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed)
{
    u32 wavStringIndex = 0;
    if ( WAV().get<u32>(wavStringIndex, (u32)wavIndex * 4) && wavStringIndex > 0 )
    {
        if ( !removeIfUsed && MapUsesWavString(wavStringIndex) )
            return false;

        bool success = WAV().replace<u32>((u32)wavIndex * 4, 0);
        ZeroWavUsers(wavStringIndex);
        removeUnusedString(wavStringIndex);
        return success;
    }
    return false;
}

bool Scenario::RemoveWavByStringIndex(u32 wavStringIndex, bool removeIfUsed)
{
    if ( !removeIfUsed && MapUsesWavString(wavStringIndex) )
        return false;

    if ( wavStringIndex > 0 )
    {
        bool success = true;
        for ( u32 i = 0; i < 512; i++ )
        {
            u32 stringIndex = 0;
            if ( WAV().get<u32>(stringIndex, i * 4) && stringIndex == wavStringIndex )
            {
                if ( !WAV().replace<u32>((u32)i * 4, 0) )
                    success = false;
            }
        }
        ZeroWavUsers(wavStringIndex);
        removeUnusedString(wavStringIndex);
        return success;
    }
    return false;
}

bool Scenario::WavHasString(u16 wavIndex)
{
    u32 stringIndex = 0;
    return WAV().get<u32>(stringIndex, (u32)wavIndex * 4) && stringIndex > 0;
}

bool Scenario::IsStringUsedWithWavs(u32 stringIndex)
{
    for ( u32 i = 0; i < 512; i++ )
    {
        if ( stringIndex == WAV().get<u32>(i * 4) )
            return true;
    }
    return false;
}

bool Scenario::GetWavs(std::map<u32/*stringIndex*/, u16/*wavIndex*/> &wavMap, bool includePureStringWavs)
{
    try
    {
        for ( u16 i = 0; i < 512; i++ )
        {
            u32 stringIndex = WAV().get<u32>((u32)i * 4);
            if ( stringIndex > 0 )
                wavMap.insert(std::pair<u32, u16>(stringIndex, i));
        }

        if ( includePureStringWavs )
        {
            Trigger* trig = nullptr;
            int trigNum = 0;

            while ( getTrigger(trig, trigNum) )
            {
                for ( u8 i=0; i<NUM_TRIG_ACTIONS; i++ )
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
                for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
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
    }
    return false;
}

bool Scenario::MapUsesWavString(u32 wavStringIndex)
{
    Trigger* trig = nullptr;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( u8 i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringIndex )
                return true;
        }
        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringIndex )
                return true;
        }
        trigNum ++;
    }

    return false;
}

void Scenario::ZeroWavUsers(u32 wavStringIndex)
{
    Trigger* trig = nullptr;
    int trigNum = 0;

    while ( getTrigger(trig, trigNum) )
    {
        for ( u8 i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringIndex )
                action.wavID = 0;
        }
        trigNum ++;
    }

    trigNum = 0;
    while ( getBriefingTrigger(trig, trigNum) )
    {
        for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
        {
            Action &action = trig->action(i);
            if ( action.wavID == wavStringIndex )
                action.wavID = 0;
        }
        trigNum ++;
    }
}

bool Scenario::ParseScenario(buffer &chk)
{
    caching = false;
    u32 chkSize = chk.size();

    u32 position = 0,
        nextPosition = 0;

    bool parsing = true;

    do
    {
        if ( position + 8 < chkSize ) // Valid section header
        {
            if ( ParseSection(chk, position, nextPosition) )
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
            parsing = false;
        }
        else if ( position > chkSize ) // Oversized finish
        {
            mapIsProtected = true;
            parsing = false;
        }
        else // Natural finish (position == chkSize)
            parsing = false;

    } while ( parsing );

    // Pre-cache mergings/corrections
    correctMTXM();

    // Cache sections
    CacheSections();
    caching = true;

    return true;
}

bool Scenario::ParseSection(buffer &chk, u32 position, u32 &nextPosition)
{
    u32 sectionId = 0;
    s32 sectionSize = 0;

    if ( chk.get<u32>(sectionId, position) &&      // Get sectionId/title
         chk.get<s32>(sectionSize, position + 4) ) // Get section size
    {
        nextPosition = position + 8 + sectionSize;
    }
    else
        return false; // Unexpected read faliure

    if ( sectionSize >= 0 ) // Normal section
    {
        auto newSection = AddSection(sectionId);
        return nextPosition >= position + 8 && // Check for addition overflow
               (nextPosition > chk.size() || // Check for oversized
                sectionSize == 0 || // Check for empty section
                newSection->extract(chk, position+8, sectionSize)); // Move data to this section
    }
    else if ( sectionSize < 0 ) // Jump section
    {
        return nextPosition < position + 8 && // Check for addition underflow
               nextPosition != 0;             // Check for loop (right now it just checks for position to zero)
    }
    return true; // sectionSize == 0
}

bool Scenario::ToSingleBuffer(buffer& chk)
{
    chk.flush();
    if ( chk.setTitle("CHK ") )
    {
        for ( auto &section : sections )
        {
            if ( !chk.add<u32>(section->titleVal()) &&
                chk.add<u32>(section->size()) &&
                (section->size() == 0 || chk.addStr((const char*)section->getPtr(0), section->size())) )
            {
                return false;
            }
        }

        if ( tailLength > 0 && tailLength < 8 )
            chk.addStr((const char*)tailData, tailLength);

        return true;
    }
    else
        return false;
}

bool Scenario::Serialize(void* &data)
{
    buffer chk("CHK ");
    return ToSingleBuffer(chk) && chk.serialize(data);
}

bool Scenario::Deserialize(const void* data)
{
    buffer chk;
    if ( chk.deserialize(data) )
    {
        Flush();
        if ( ParseScenario(chk) )
            return true;
    }
    return false;
}

bool Scenario::hasPassword()
{
    return tailLength == 7;
}

bool Scenario::isPassword(std::string &password)
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
    }
    else // No password
        return true;
    
    return false;
}

bool Scenario::SetPassword(std::string &oldPass, std::string &newPass)
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

bool Scenario::Login(std::string &password)
{
    if ( isPassword(password) )
    {
        mapIsProtected = false;
        return true;
    }
    return false;
}

bool Scenario::CreateNew(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers)
{
    buffer chk("nMAP");

    if (    Get_TYPE(chk) && Get_VER (chk) && Get_IVE2(chk) && Get_VCOD(chk)
         && Get_IOWN(chk) && Get_OWNR(chk) && Get_ERA (chk, tileset) && Get_DIM (chk, width, height)
         && Get_SIDE(chk) && Get_MTXM(chk, width, height) && Get_PUNI(chk) && Get_UPGR(chk)
         && Get_UNIT(chk) && Get_PTEC(chk) && Get_ISOM(chk, width, height) && Get_TILE(chk, width, height)
         && Get_DD2 (chk) && Get_THG2(chk) && Get_MASK(chk, width, height) && Get_STR (chk)
         && Get_MRGN(chk, width, height)   && Get_SPRP(chk) && Get_FORC(chk) && Get_WAV (chk)
         && Get_UNIS(chk) && Get_UPGS(chk) && Get_TECS(chk) && Get_COLR(chk)
         && Get_PUPx(chk) && Get_PTEx(chk) && Get_UNIx(chk) && Get_UPGx(chk)
         && Get_TECx(chk) && Get_TRIG(chk) && Get_MBRF(chk) && Get_UPRP(chk)
         && Get_UPUS(chk) && Get_SWNM(chk) )
    {
        if ( ParseScenario(chk) )
        {
            return true;
        }
        else // Chkdraft failed to read a map it creates itself, should never happen!
            CHKD_SHOUT("New map parsing failed. Report this!");
    }
    return false;
}

void Scenario::WriteFile(FILE* pFile)
{
    if ( !isProtected() )
    {
        for ( auto &section : sections )
            section->write(pFile, true);

        if ( tailLength > 0 && tailLength < 8 )
            fwrite(tailData, tailLength, 1, pFile);
    }
}

bool Scenario::RemoveSection(SectionId sectionId)
{
    for ( auto &section : sections )
    {
        if ( section->titleVal() == (u32)sectionId )
            return RemoveSection(section);
    }
    return false;
}

bool Scenario::RemoveSection(Section sectionToRemove)
{
    auto sectionFound = std::find(sections.begin(), sections.end(), sectionToRemove);
    if ( sectionFound != sections.end() )
    {
        sections.erase(sectionFound);
        return true;
    }
    return false;
}

bool Scenario::AddSection(Section sectionToAdd)
{
    try {
        sections.insert(sections.end(), sectionToAdd);
        CacheSections();
        return true;
    }
    catch ( std::exception ) { }
    return false;
}

Section Scenario::AddSection(u32 sectionId)
{
    try {
        Section newSection(new buffer(sectionId));
        sections.push_back(newSection);
        CacheSections();
        return newSection;
    }
    catch ( std::bad_alloc ) { } // Out of memory
    return nullptr;
}

void Scenario::Flush()
{
    sections.clear();
    CacheSections();
    mapIsProtected = false;
}

void Scenario::CacheSections()
{
    type = getSection((u32)SectionId::TYPE); ver = getSection((u32)SectionId::VER);
    iver = getSection((u32)SectionId::IVER); ive2 = getSection((u32)SectionId::IVE2);
    vcod = getSection((u32)SectionId::VCOD); iown = getSection((u32)SectionId::IOWN);
    ownr = getSection((u32)SectionId::OWNR); era = getSection((u32)SectionId::ERA);
    dim = getSection((u32)SectionId::DIM); side = getSection((u32)SectionId::SIDE);
    mtxm = getSection((u32)SectionId::MTXM); puni = getSection((u32)SectionId::PUNI);
    upgr = getSection((u32)SectionId::UPGR); ptec = getSection((u32)SectionId::PTEC);
    unit = getSection((u32)SectionId::UNIT); isom = getSection((u32)SectionId::ISOM);
    tile = getSection((u32)SectionId::TILE); dd2 = getSection((u32)SectionId::DD2);
    thg2 = getSection((u32)SectionId::THG2); mask = getSection((u32)SectionId::MASK);
    str = getSection((u32)SectionId::STR); uprp = getSection((u32)SectionId::UPRP);
    upus = getSection((u32)SectionId::UPUS); mrgn = getSection((u32)SectionId::MRGN);
    trig = getSection((u32)SectionId::TRIG); mbrf = getSection((u32)SectionId::MBRF);
    sprp = getSection((u32)SectionId::SPRP); forc = getSection((u32)SectionId::FORC);
    wav = getSection((u32)SectionId::WAV); unis = getSection((u32)SectionId::UNIS);
    upgs = getSection((u32)SectionId::UPGS); tecs = getSection((u32)SectionId::TECS);
    swnm = getSection((u32)SectionId::SWNM); colr = getSection((u32)SectionId::COLR);
    pupx = getSection((u32)SectionId::PUPx); ptex = getSection((u32)SectionId::PTEx);
    unix = getSection((u32)SectionId::UNIx); upgx = getSection((u32)SectionId::UPGx);
    tecx = getSection((u32)SectionId::TECx);

    kstr = getSection((u32)SectionId::KSTR);
}

Section Scenario::getSection(u32 id)
{
    Section sectionPtr(nullptr);
    for ( auto &section : sections )
    {
        if ( id == section->titleVal() )
            sectionPtr = section;
    }
    return sectionPtr;
    /** Referances to null buffers are
        safe to use though you may violate
        user expectations if you do not check
        that they exist at some point. */
}

// Standard Sections
buffer& Scenario::TYPE() { return *type; } buffer& Scenario::VER() { return *ver; }
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
buffer& Scenario::KSTR() { return *kstr; }

bool Scenario::GetStrInfo(char* &ptr, size_t &length, u32 stringNum)
{
    buffer* strings = &STR();
    u32 numStrings = u32(strings->get<u16>(0));
    bool extended = false;

    if ( stringNum >= numStrings ) // Might be an extended string
    {
        if ( !KSTR().exists() )
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
        u32 start = 0, end = 0;

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
    else
        return false;
}

bool Scenario::ZeroOutString(u32 stringNum)
{
    char* str;
    size_t length;
    if ( GetStrInfo(str, length, stringNum) )
    {
        for ( size_t i=0; i<length; i++ ) // Zero-out characters
            str[i] = '\0';

        u32 numRegularStrings = (u32)strSectionCapacity();
        if ( stringNum < strSectionCapacity() )
        {
            if ( STR().size() > numRegularStrings*2+2 ) // If you can get to start of string data
                return STR().replace<u16>(2*stringNum, numRegularStrings*2+2); // Set string offset to start of string data (NUL)
            else
                return STR().replace<u16>(2*stringNum, 0); // Otherwise just zero it
        }
        else if ( KSTR().exists() )
        {
            u32 numExtendedStrings = KSTR().get<u32>(4);
            if ( KSTR().size() > numExtendedStrings*8+8 ) // If you can get to start of string data
                return KSTR().replace<u32>(stringNum*4+4, numExtendedStrings*8+8); // Set string offset to start of extended string data (NUL)
            else
                return KSTR().replace<u32>(stringNum*4+4, 0); // Otherwise just zero it
        }
    }
    return false;
}

bool Scenario::RepairString(u32& stringNum, bool extended)
{
    RawString str;
    if ( stringExists(stringNum) && GetString(str, stringNum) && str.size() > 0 )
        return false; // No Repair
    else
    {
        if ( addString(RawString("strRepair"), stringNum, extended) )
            return true;
        else
            return false;
    }
}
