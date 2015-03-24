#include "Scenario.h"
#include "DefaultCHK.h"
#include "StringUsage.h"
#include <iostream>
using namespace std;

#define FORCE_RANDOMIZE_START_LOCATION	BIT_0
#define FORCE_ALLIED					BIT_1
#define FORCE_ALLIED_VICTORY			BIT_2
#define FORCE_SHARED_VISION				BIT_3

section::section(char *title) : buf(title)
{

}

section::section(buffer& buf) : buf(buf)
{
	
}

Scenario::Scenario() : head(nullptr), mapIsProtected(false)
{

}

Scenario::~Scenario()
{
	section* next;
	while ( head != nullptr )
	{
		next = head->next;
		delete head;
		head = next;
	}
}

Scenario* Scenario::scenario()
{
	return this;
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
	return ERA().get<u16>(0);
}

u16 Scenario::numUnits()
{
	return u16(UNIT().size()/UNIT_STRUCT_SIZE);
}

bool Scenario::getUnit(ChkUnit* &unitRef, u16 index)
{
	return UNIT().getPtr(unitRef, index*UNIT_STRUCT_SIZE, UNIT_STRUCT_SIZE);
}

bool Scenario::getLocation(ChkLocation* &locRef, u16 index)
{
	return MRGN().getPtr(locRef, (u32(index))*CHK_LOCATION_SIZE, CHK_LOCATION_SIZE);
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
		else
			break;
	}
	return numLocs;
}

u32 Scenario::numTriggers()
{
	return u32(TRIG().size()/TRIG_STRUCT_SIZE);
}

bool Scenario::getTrigger(Trigger* &trigRef, u32 index)
{
	return TRIG().getPtr(trigRef, index*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE);
}

bool Scenario::getActiveComment(Trigger* trigger, string &comment)
{
	for ( u32 i=0; i<NUM_TRIG_ACTIONS; i++ )
	{
		Action action = trigger->actions[i];
		if ( action.action == AID_COMMENT )
		{
			if ( (action.flags&ACTION_FLAG_DISABLED) != ACTION_FLAG_DISABLED )
			{
				if ( action.stringNum != 0 && getRawString(comment, action.stringNum) )
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
		else if ( action.action == AID_NO_ACTION )
			break;
	}
	return false;
}

bool Scenario::getBriefingTrigger(Trigger* &trigRef, u32 index)
{
	return MBRF().getPtr(trigRef, index*TRIG_STRUCT_SIZE, TRIG_STRUCT_SIZE);
}

u32 Scenario::getForceStringNum(u8 index)
{
	if ( index < 4 )
		return FORC().get<u16>(8+2*index);
	else
		return 0;
}

std::string Scenario::getForceString(u8 forceNum)
{
	string forceString;
	u32 stringNum = getForceStringNum(forceNum);
	if ( stringNum != 0 )
		getString(forceString, stringNum);
	else
	{
		char num[12];
		_itoa_s(forceNum+1, num, 10);
		forceString += "Force ";
		forceString += num;
	}
	return forceString;
}

bool Scenario::getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av)
{
	if ( forceNum < 4 )
	{
		u8 forceProp;
		if ( FORC().get<u8>(forceProp, 16+forceNum) )
		{
			allied = ((forceProp&FORCE_ALLIED) == FORCE_ALLIED);
			vision = ((forceProp&FORCE_SHARED_VISION) == FORCE_SHARED_VISION);
			random = ((forceProp&FORCE_RANDOMIZE_START_LOCATION) == FORCE_RANDOMIZE_START_LOCATION);
			av = ((forceProp&FORCE_ALLIED_VICTORY) == FORCE_ALLIED_VICTORY);
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

bool Scenario::getUnitStringNum(u16 unitId, u16 &stringNum)
{
	if ( isExpansion() )
		return UNIx().get<u16>(stringNum, unitId*2+UNIT_SETTINGS_STRING_IDS);
	else
		return UNIS().get<u16>(stringNum, unitId*2+UNIT_SETTINGS_STRING_IDS);
}

u16 Scenario::numStrings()
{
	return STR().get<u16>(0);
}

u32 Scenario::totalStrings()
{
	return ((u32)STR().get<u16>(0)) + KSTR().get<u32>(4);
}

bool Scenario::getString(string &dest, u32 stringNum)
{
	dest.clear();

	char* srcStr;
	u32 length;

	if ( GetStrInfo(srcStr, length, stringNum) )
	{
		return MakeStr(dest, srcStr, length);
	}
	else if ( stringNum == 0 )
	{
		dest = "";
		return true;
	}

	return false;
}

bool Scenario::getRawString(string &dest, u32 stringNum)
{
	dest.clear();

	char* srcStr;
	u32 length;

	if ( GetStrInfo(srcStr, length, stringNum) )
	{
		try {
			dest += srcStr;
			return true;
		} catch ( std::exception ) { // Catch bad_alloc and length_error
			return false;
		}
	}
	else if ( stringNum == 0 )
	{
		dest = "";
		return true;
	}

	return false;
}

bool Scenario::getEscapedString(string &dest, u32 stringNum)
{
	dest.clear();

	char* srcStr;
	u32 length;

	if ( GetStrInfo(srcStr, length, stringNum) )
	{
		return MakeEscapedStr(dest, srcStr, length);
	}
	else if ( stringNum == 0 )
	{
		try {
			dest = "";
			return true;
		} catch ( std::exception ) {
			return false;
		}
	}

	return false;
}

bool Scenario::getLocationName(string &dest, u8 locationID)
{
	buffer& MRGN = this->MRGN();
	ChkLocation* loc;
	if ( MRGN.getPtr<ChkLocation>(loc, locationID*CHK_LOCATION_SIZE, CHK_LOCATION_SIZE) )
	{
		if ( loc->stringNum == 0 || !this->getEscapedString(dest, loc->stringNum) )
		{
			char locNum[8];
			_itoa_s(int(locationID), locNum, 10);

			dest.clear();
			try {
				dest += "Location ";
				dest += locNum;
				return true;
			} catch ( std::exception ) {
				return false;
			}
		}
		else
			return getEscapedString(dest, loc->stringNum);
	}
	return false;
}

void Scenario::getUnitName(string &dest, u16 unitID)
{
	buffer& settings = unitSettings();

	if ( unitID < 228 ) // Regular unit
	{
		if ( settings.get<u8>(unitID) == 0 && // Not default unit settings
			 settings.get<u16>(UNIT_SETTINGS_STRING_IDS+unitID*2) > 0 ) // Not default string
		{
			u16 stringID = settings.get<u16>(UNIT_SETTINGS_STRING_IDS+2*unitID);
			if ( !getString(dest, stringID) ) // Try to retrieve string name
				dest = DefaultUnitDisplayName[unitID]; // Couldn't retrieve name, return default
		}
		else // Default unit name
		{
			dest = DefaultUnitDisplayName[unitID];
		}
	}
	else // Extended unit
	{
		char unitName[16];
		sprintf_s(unitName, 16, "Unit #%u", unitID);
		dest = unitName;
	}
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
		if ( UNIS.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == stringNum )
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
		if ( UNIx.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == stringNum )
			return true;
	}

	return false;
}

bool Scenario::isExtendedString(u32 stringNum)
{
	return stringNum >= numStrings() &&
		   stringNum < 65536 &&
		   KSTR().exists() &&
		   (u32(65536-stringNum)) <= KSTR().get<u32>(4);
}

bool Scenario::stringExists(u32 stringNum)
{
	return isExtendedString(stringNum) ||
		   ( STR().exists() &&
		     stringNum < numStrings() );
}

bool Scenario::stringExists(string str, u32& stringNum)
{
	std::list<StringTableNode> strList;
	addAllUsedStrings(strList, STRADD_INCLUDE_ALL);

	for ( auto it = strList.begin(); it != strList.end(); it ++ )
	{
		if ( it->string.compare(str) == 0 )
		{
			stringNum = it->stringNum;
			return true;
		}
	}

	return false;
}

bool Scenario::stringExists(string str, u32& stringNum, bool extended)
{
	std::list<StringTableNode> strList;
	if ( extended )
		addAllUsedStrings(strList, STRADD_INCLUDE_EXTENDED);
	else
		addAllUsedStrings(strList, STRADD_INCLUDE_STANDARD);

	for ( auto it = strList.begin(); it != strList.end(); it ++ )
	{
		if ( it->string.compare(str) == 0 )
		{
			stringNum = it->stringNum;
			return true;
		}
	}

	return false;
}

bool Scenario::escStringDifference(string str, u32& stringNum)
{
	string existing;
	if ( getEscapedString(existing, stringNum) )
	{
		if ( str.compare(existing) == 0 )
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

#define IncrementIfEqual(index, counter) \
	if ( ((u32)index) == stringNum )	 \
		counter ++;

u32 Scenario::amountStringUsed(u32 stringNum)
{
	u32 amountStringUsed = 0;
	ChkLocation* loc; // MRGN - location strings
	for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
	{
		if ( getLocation(loc, u8(i)) )
			IncrementIfEqual(loc->stringNum, amountStringUsed);
	}
	Trigger* trig; // TRIG - trigger strings
	int trigNum = 0;
	while ( getTrigger(trig, trigNum) )
	{
		trigNum ++;
		for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
		{
			IncrementIfEqual(trig->actions[i].stringNum, amountStringUsed);
			IncrementIfEqual(trig->actions[i].wavID, amountStringUsed);
		}
	}
	trigNum = 0; // MBRF - briefing strings
	while ( getBriefingTrigger(trig, trigNum) )
	{
		trigNum ++;
		for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
		{
			IncrementIfEqual(trig->actions[i].stringNum, amountStringUsed);
			IncrementIfEqual(trig->actions[i].wavID, amountStringUsed);
		}
	}
	// SPRP - scenario property strings
	u16 strIndex = SPRP().get<u16>(0); // Scenario Name
	IncrementIfEqual(strIndex, amountStringUsed);
	strIndex = SPRP().get<u16>(2); // Scenario Description
	IncrementIfEqual(strIndex, amountStringUsed);
	for ( int i=0; i<4; i++ ) // FORC - force strings
		IncrementIfEqual(getForceStringNum(i), amountStringUsed);
	for ( u32 i=0; i<WAV().size()/4; i++ ) // WAV  - sound strings
		IncrementIfEqual(WAV().get<u32>(i*4), amountStringUsed);
	for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
		IncrementIfEqual(UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2), amountStringUsed);
	for ( int i=0; i<256; i++ ) // SWNM - switch strings
		IncrementIfEqual(SWNM().get<u32>(i*4), amountStringUsed);
	for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
		IncrementIfEqual(UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2), amountStringUsed);

	return amountStringUsed;
}

void Scenario::getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches)
{
	locs = trigs = briefs = props = forces = wavs = units = switches = 0;
	ChkLocation* loc; // MRGN - location strings
	for ( u32 i=0; i<MRGN().size()/CHK_LOCATION_SIZE; i++ )
	{
		if ( getLocation(loc, u8(i)) )
			IncrementIfEqual(loc->stringNum, locs);
	}
	Trigger* trig; // TRIG - trigger strings
	int trigNum = 0;
	while ( getTrigger(trig, trigNum) )
	{
		trigNum ++;
		for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
		{
			IncrementIfEqual(trig->actions[i].stringNum, trigs);
			IncrementIfEqual(trig->actions[i].wavID, trigs);
		}
	}
	trigNum = 0; // MBRF - briefing strings
	while ( getBriefingTrigger(trig, trigNum) )
	{
		trigNum ++;
		for ( int i=0; i<NUM_TRIG_ACTIONS; i++ )
		{
			IncrementIfEqual(trig->actions[i].stringNum, briefs);
			IncrementIfEqual(trig->actions[i].wavID, briefs);
		}
	}
	// SPRP - scenario property strings
	u16 strIndex = SPRP().get<u16>(0); // Scenario Name
	IncrementIfEqual(strIndex, props);
	strIndex = SPRP().get<u16>(2); // Scenario Description
	IncrementIfEqual(strIndex, props);
	for ( int i=0; i<4; i++ ) // FORC - force strings
		IncrementIfEqual(getForceStringNum(i), forces);
	for ( u32 i=0; i<WAV().size()/4; i++ ) // WAV  - sound strings
		IncrementIfEqual(WAV().get<u32>(i*4), wavs);
	for ( int i=0; i<228; i++ ) // UNIS - unit settings strings (vanilla)
		IncrementIfEqual(UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2), units);
	for ( int i=0; i<256; i++ ) // SWNM - switch strings
		IncrementIfEqual(SWNM().get<u32>(i*4), switches);
	for ( int i=0; i<228; i++ ) // UNIx - unit settings strings (brood war)
		IncrementIfEqual(UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2), units);
}

bool Scenario::isExpansion()
{
	return VER().get<u16>(0) >= 63;
}

buffer& Scenario::unitSettings()
{
	if ( isExpansion() )
		return this->UNIx();
	else
		return this->UNIS();
}

buffer& Scenario::upgradeSettings()
{
	if ( isExpansion() )
		return this->UPGx();
	else
		return this->UPGS();
}

buffer& Scenario::upgradeRestrictions()
{
	if ( isExpansion() )
		return this->PUPx();
	else
		return this->UPGR();
}

buffer& Scenario::techSettings()
{
	if ( isExpansion() )
		return this->TECx();
	else
		return this->TECS();
}

buffer& Scenario::techRestrictions()
{
	if ( isExpansion() )
		return this->PTEx();
	else
		return this->PTEC();
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

bool Scenario::getMapTitle(std::string& dest)
{
	dest = "";
	if ( SPRP().exists() )
	{
		u16 stringNum;
		if ( SPRP().get<u16>(stringNum, 0) )
			return getString(dest, (u32)stringNum);
	}
	dest = "Untitled Scenario";
	return false;
}

bool Scenario::getMapDescription(std::string& dest)
{
	dest = "";
	if ( SPRP().exists() )
	{
		u16 stringNum;
		if ( SPRP().get<u16>(stringNum, 2) )
			return getString(dest, (u32)stringNum);
	}
	dest = "Destroy all enemy buildings.";
	return false;
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
	return PUNI().get<u8>(PTEC_GLOBAL_AVAILABILITY+unitId) != 0;
}

u8 Scenario::getUnitEnabledState(u16 unitId, u8 player)
{
	if ( PUNI().get<u8>(PTEC_PLAYERUSESDEFAULT+unitId+player*228) == 0 ) // Override
	{
		if ( PUNI().get<u8>((u32)unitId+228*(u32)player) == 0 )
			return UNIT_STATE_DISABLEDFORPLAYER;
		else
			return UNIT_STATE_ENABLEDFORPLAYER;
	}
	else
		return UNIT_STATE_DEFAULTFORPLAYER;
}

bool Scenario::getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints)
{
	u32 untrimmedHitpoints;
	if ( unitSettings().get<u32>(untrimmedHitpoints, UNIT_SETTINGS_HITPOINTS+4*(u32)unitId) )
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
	if ( unitSettings().get<u32>(untrimmedHitpoints, UNIT_SETTINGS_HITPOINTS+4*(u32)unitId) )
	{
		hitpointByte = untrimmedHitpoints%256;
		return true;
	}
	else
		return false;
}

bool Scenario::getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints)
{
	return unitSettings().get<u16>(shieldPoints, UNIT_SETTINGS_SHIELDPOINTS+2*(u32)unitId);
}

bool Scenario::getUnitSettingsArmor(u16 unitId, u8 &armor)
{
	return unitSettings().get<u8>(armor, UNIT_SETTINGS_ARMOR+(u32)unitId);
}

bool Scenario::getUnitSettingsBuildTime(u16 unitId, u16 &buildTime)
{
	return unitSettings().get<u16>(buildTime, UNIT_SETTINGS_BUILDTIME+2*(u32)unitId);
}

bool Scenario::getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost)
{
	return unitSettings().get<u16>(mineralCost, UNIT_SETTINGS_MINERALCOST+2*(u32)unitId);
}

bool Scenario::getUnitSettingsGasCost(u16 unitId, u16 &gasCost)
{
	return unitSettings().get<u16>(gasCost, UNIT_SETTINGS_GASCOST+2*(u32)unitId);
}

bool Scenario::getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage)
{
	return unitSettings().get<u16>(baseDamage, UNIT_SETTINGS_BASEWEAPON+2*weaponId);
}

bool Scenario::getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage)
{
	return unitSettings().get<u16>(bonusDamage, UNIT_SETTINGS_BONUSWEAPON(isExpansion())+2*weaponId);
}

bool Scenario::upgradeUsesDefaultCosts(u8 upgradeId)
{
	return upgradeSettings().get<u8>((u32)upgradeId) == 1;
}

bool Scenario::getUpgradeMineralCost(u8 upgradeId, u16 &mineralCost)
{
	return upgradeSettings().get<u16>(mineralCost,
		UPGRADE_SETTINGS_MINERALCOST(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeMineralFactor(u8 upgradeId, u16 &mineralFactor)
{
	return upgradeSettings().get<u16>(mineralFactor,
		UPGRADE_SETTINGS_MINERALFACTOR(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeGasCost(u8 upgradeId, u16 &gasCost)
{
	return upgradeSettings().get<u16>(gasCost,
		UPGRADE_SETTINGS_GASCOST(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeGasFactor(u8 upgradeId, u16 &gasFactor)
{
	return upgradeSettings().get<u16>(gasFactor,
		UPGRADE_SETTINGS_GASFACTOR(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeTimeCost(u8 upgradeId, u16 &timeCost)
{
	return upgradeSettings().get<u16>(timeCost,
		UPGRADE_SETTINGS_TIMECOST(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeTimeFactor(u8 upgradeId, u16 &timeFactor)
{
	return upgradeSettings().get<u16>(timeFactor,
		UPGRADE_SETTINGS_TIMEFACTOR(isExpansion())+2*(u32)upgradeId);
}

bool Scenario::getUpgradeDefaultStartLevel(u8 upgradeId, u8 &startLevel)
{
	return upgradeRestrictions().get<u8>(startLevel, UPGRADE_DEFAULTSTARTLEVEL(isExpansion())+(u32)upgradeId);
}

bool Scenario::getUpgradeDefaultMaxLevel(u8 upgradeId, u8 &maxLevel)
{
	return upgradeRestrictions().get<u8>(maxLevel, UPGRADE_DEFAULTMAXLEVEL(isExpansion())+(u32)upgradeId);
}

bool Scenario::playerUsesDefaultUpgradeLevels(u8 upgradeId, u8 player)
{
	return upgradeRestrictions().get<u8>(UPGRADE_PLAYERUSESDEFAULT(isExpansion(), player)+(u32)upgradeId) == 1;
}

bool Scenario::getUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 &startLevel)
{
	return upgradeRestrictions().get<u8>(startLevel, UPGRADE_PLAYERSTARTLEVEL(isExpansion(), player)+(u32)upgradeId);
}

bool Scenario::getUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 &maxLevel)
{
	return upgradeRestrictions().get<u8>(maxLevel, UPGRADE_PLAYERMAXLEVEL(isExpansion(), player)+(u32)upgradeId);
}

bool Scenario::techUsesDefaultCosts(u8 techId)
{
	return techSettings().get<u8>((u32)techId) == 1;
}

bool Scenario::getTechMineralCost(u8 techId, u16 &mineralCost)
{
	return techSettings().get<u16>(mineralCost, TECH_COST_MINERALS(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechGasCost(u8 techId, u16 &gasCost)
{
	return techSettings().get<u16>(gasCost, TECH_COST_GAS(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechTimeCost(u8 techId, u16 &timeCost)
{
	return techSettings().get<u16>(timeCost, TECH_COST_TIME(isExpansion())+2*(u32)techId);
}

bool Scenario::getTechEnergyCost(u8 techId, u16 &energyCost)
{
	return techSettings().get<u16>(energyCost, TECH_COST_ENERGY(isExpansion())+2*(u32)techId);
}

bool Scenario::techAvailableForPlayer(u8 techId, u8 player)
{
	return techRestrictions().get<u8>(TECH_AVAILABLEFORPLAYER(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::techResearchedForPlayer(u8 techId, u8 player)
{
	return techRestrictions().get<u8>(TECH_RESERACHEDFORPLAYER(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::techAvailableByDefault(u8 techId)
{
	return techRestrictions().get<u8>(TECH_DEFAULTAVAILABILITY(isExpansion())+(u32)techId) == 1;
}

bool Scenario::techResearchedByDefault(u8 techId)
{
	return techRestrictions().get<u8>(TECH_DEFAULTRESERACHED(isExpansion())+(u32)techId) == 1;
}

bool Scenario::playerUsesDefaultTechSettings(u8 techId, u8 player)
{
	return techRestrictions().get<u8>(TECH_PLAYERUSESDEFAULT(isExpansion(), player)+(u32)techId) == 1;
}

bool Scenario::setTileset(u16 newTileset)
{
	return ERA().replace<u16>(0, newTileset);
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
			char locNum[8];
			_itoa_s(int(unusedIndex), locNum, 10);

			std::string str = "Location ";
			str += locNum;
			u32 newStrNum;
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
		if ( UNIS.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == stringNum )
			UNIS.replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, 0);
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
		if ( UNIx.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == stringNum )
			UNIx.replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, 0);
	}

	if ( !cleanStringTable(isExtendedString(stringNum)) ) // Scrub string clean from table
		ZeroOutString(stringNum); // Not enough memory for a full clean at this time, just zero-out the string
}

bool Scenario::addString(string str, u32& stringNum, bool extended)
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
bool Scenario::replaceString(string newString, numType& stringNum, bool extended, bool safeReplace)
{
	u32 newStringNum, oldStringNum = (u32)stringNum;

	if ( stringNum != 0 && amountStringUsed(stringNum) == 1 )
	{
		string testEqual;
		if ( getRawString(testEqual, oldStringNum) && testEqual.compare(newString) == 0 )
			return false;
		else
			return editString<numType>(newString, stringNum, extended, safeReplace);
	}
	else if ( stringExists(newString, newStringNum, extended) )
	{
		stringNum = (numType)newStringNum;
		removeUnusedString(oldStringNum);
		return true;
	}
	else if ( addString(newString, newStringNum, extended) )
	{
		stringNum = (numType)newStringNum;
		removeUnusedString(oldStringNum);
		return true;
	}
	else // String addition failed, try preserving strings, removing the old string, then adding
	{
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
		std::string stringBackup;

		bool didBackupSection = strings->exists() && stringSectionBackup.addStr((const char*)strings->getPtr(0), strings->size()),
			 didBackupString = false,
			 wasExtended = false;

		if ( !didBackupSection && !safeReplace )
		{
			didBackupString = getString(stringBackup, stringNum);
			wasExtended = isExtendedString(stringNum);
		}
		
		// Check if deleting the original can be safely done or safeReplace is not set
		if ( didBackupSection || !safeReplace )
		{
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
					stringSectionBackup.setTitle(HEADER_KSTR);
				else
					stringSectionBackup.setTitle(HEADER_STR);

				strings->takeAllData(stringSectionBackup);
			}
			else if ( didBackupString ) // String addition failed and full section could not be backed up
			{
				if ( addString(stringBackup, newStringNum, wasExtended) ) // Attempt to re-insert the original string
					stringNum = (numType)newStringNum;
			}
		}
	}
	return false;
}
template bool Scenario::replaceString<u16>(string newString, u16& stringNum, bool extended, bool safeReplace);
template bool Scenario::replaceString<u32>(string newString, u32& stringNum, bool extended, bool safeReplace);

template <typename numType>
bool Scenario::editString(string newString, numType stringNum, bool extended, bool safeEdit)
{
	string testEqual;
	if ( stringNum == 0 || (getRawString(testEqual, stringNum) && testEqual.compare(newString) == 0) )
		return false;
	else if ( stringNum != 0 )
		testEqual.clear();

	u32 newStringNum = 0;
	if ( stringExists(newString, newStringNum, extended) )
	{
		replaceStringNum(stringNum, newStringNum);
		return true;
	}
	else
	{
		std::list<StringTableNode> strList;
		if ( extended == false && addAllUsedStrings(strList, STRADD_INCLUDE_STANDARD) ||
			 extended == true && addAllUsedStrings(strList, STRADD_INCLUDE_EXTENDED) )
		{
			for ( auto it = strList.begin(); it != strList.end(); it ++ )
			{
				if ( it->stringNum == stringNum )
				{
					it->string = newString; // Replace the old string with the new string
					return rebuildStringTable(strList, extended); // Attempt rebuilding string table
				}
			}
		}
	}
	return false;
}
template bool Scenario::editString<u16>(string newString, u16 stringNum, bool extended, bool safeEdit);
template bool Scenario::editString<u32>(string newString, u32 stringNum, bool extended, bool safeEdit);

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
		if ( UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == toReplace )
			UNIS().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, (u16)replacement);
		if ( UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == toReplace )
			UNIx().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, (u16)replacement);
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
	std::list<StringTableNode> strList;

	u32 flags = 0;
	if ( extendedTable )
		flags = STRADD_INCLUDE_EXTENDED;
	else
		flags = STRADD_INCLUDE_STANDARD;

	return addAllUsedStrings(strList, flags) && rebuildStringTable(strList, extendedTable);
}

bool Scenario::removeDuplicateStrings()
{
	std::list<StringTableNode> strList;

	if ( addAllUsedStrings(strList, STRADD_INCLUDE_ALL) )
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
		return false;

	removeDuplicateStrings();

	std::list<StringTableNode> strList;
	u32 flags = 0;
	if ( extendedTable )
		flags = STRADD_INCLUDE_EXTENDED;
	else
		flags = STRADD_INCLUDE_STANDARD;

	StringUsageTable strUsage;
	if ( strUsage.populateTable(this, extendedTable) && addAllUsedStrings(strList, flags) )
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
					if ( unitSettings.get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) == lastFragmented )
						unitSettings.replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, (u16)firstEmpty);
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

		std::list<StringTableNode> newStrList;
		return addAllUsedStrings(newStrList, flags) && rebuildStringTable(newStrList, extendedTable);
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
			u8 action = trig->actions[i].action;
			if ( action == AID_COMMENT || action == AID_DISPLAY_TEXT_MESSAGE || action == AID_LEADERBOARD_CONTROL_AT_LOCATION ||
				 action == AID_LEADERBOARD_CONTROL || action == AID_LEADERBOARD_KILLS || action == AID_LEADERBOARD_POINTS ||
				 action == AID_LEADERBOARD_RESOURCES || action == AID_LEADERBOARD_GOAL_CONTROL_AT_LOCATION ||
				 action == AID_LEADERBOARD_GOAL_CONTROL || action == AID_LEADERBOARD_GOAL_KILLS || action == AID_LEADERBOARD_GOAL_POINTS ||
				 action == AID_LEADERBOARD_GOAL_RESOURCES || action == AID_SET_MISSION_OBJECTIVES || action == AID_SET_NEXT_SCENARIO ||
				 action == AID_TRANSMISSION )
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
			if ( action == AID_PLAY_WAV || action == AID_TRANSMISSION )
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
		string str;
		stringNum = WAV().get<u32>(i*4);
		if ( stringNum != 0 && !( stringExists(stringNum) && getString(str, stringNum) && str.c_str()[0] != '\0' ) )
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
			stringNum = UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2);
			if ( stringNum != 0 && RepairString(stringNum, false) )
				UNIx().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, (u16)stringNum);
		}
		for ( int i=0; i<228; i++ )
		{
			stringNum = UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2);
			UNIS().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2));
		}
	}
	else
	{
		for ( int i=0; i<228; i++ )
		{
			stringNum = UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2);
			if ( stringNum != 0 && RepairString(stringNum, false) )
				UNIS().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, (u16)stringNum);
		}
		for ( int i=0; i<228; i++ )
		{
			stringNum = UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2);
			UNIx().replace<u16>(UNIT_SETTINGS_STRING_IDS+i*2, UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2));
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

bool Scenario::addAllUsedStrings(std::list<StringTableNode>& strList, u32 flags)
{
	StringTableNode node;
	u32 standardNumStrings = numStrings();
	u32 extendedNumStrings = KSTR().get<u32>(4);

	#define AddStrIfOverZero(index)																		\
		if ( index > 0 &&																				\
			 (																							\
			   ( (flags&STRADD_INCLUDE_STANDARD) > 0 && (u32(index)) <= standardNumStrings ) ||			\
			   ( (flags&STRADD_INCLUDE_EXTENDED) > 0 && (u32(65536-index)) <= extendedNumStrings )		\
			 )																							\
		   )																							\
		{																								\
			if ( getRawString(node.string, index) ) {													\
				node.stringNum = index;																	\
				if ( std::find(strList.begin(), strList.end(), node) == strList.end() )					\
					strList.push_back(node); /* add if the string isn't in the list */					\
			}																							\
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
			AddStrIfOverZero( UNIS().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) );
			AddStrIfOverZero( UNIx().get<u16>(UNIT_SETTINGS_STRING_IDS+i*2) );
		}

		for ( int i=0; i<256; i++ )
			AddStrIfOverZero( SWNM().get<u32>(i*4) );

		return true;
	}
	catch ( std::bad_alloc )
	{
		return false;
	}
}

bool Scenario::rebuildStringTable(std::list<StringTableNode> strList, bool extendedTable)
{
	if ( extendedTable )
	{
		for ( auto it = strList.begin(); it != strList.end(); it ++ )
		{
			if ( it->stringNum >= numStrings() )
				it->stringNum = 65536 - it->stringNum;
		}
	}

	// Sort string list from lowest to highest index
	strList.sort(CompareStrTblNode);

	// Decide what the max strings should be
	u32 prevNumStrings;
	if ( extendedTable )
		prevNumStrings = KSTR().get<u32>(4);
	else
		prevNumStrings = numStrings();

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
	buffer* newStrSection = new buffer("nStr");
	u32 strPortionOffset = 0;
	if ( extendedTable )
	{
		newStrSection->setTitle(HEADER_KSTR);
		if ( !( newStrSection->add<u32>(2) && newStrSection->add<u32>(numStrs) ) )
			return false; // Out of memory
		strPortionOffset = 8 + 8*numStrs;
	}
	else
	{
		newStrSection->setTitle(HEADER_STR);
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
	u32 strIndex = 1;

	for ( auto it = strList.begin(); it != strList.end(); it ++ )
	{
		while ( strIndex < it->stringNum ) // Add any unused's till the next string, point them to the 'NUL' string
		{
			if ( ( extendedTable && newStrSection->add<u32>(strPortionOffset) ) ||		// Add string offset (extended)
				 ( !extendedTable && newStrSection->add<u16>(u16(strPortionOffset)) ) // Add string offset (regular)
			   )
			{
				strIndex ++;
			}
			else
				return false; // Out of memory
		}
		
		if ( !extendedTable && strPortionOffset+strPortion.size() > 65535 )
		{
			CHKD_ERR("Structural STR limits exceeded, header information for %u string took up %u bytes.", numStrs, 2+numStrs*2);
			return false; // No room for the next string
		}

		if ( (
			   ( extendedTable && newStrSection->add<u32>(strPortion.size()+strPortionOffset) ) || // Add string offset (extedned)
			   ( !extendedTable && newStrSection->add<u16>(u16(strPortion.size()+strPortionOffset)) ) // Add string offset (regular)
			 ) && strPortion.addStr(it->string.c_str(), it->string.size()+1) // Add the string + its NUL char
		   )
		{
			strIndex ++;
		}
		else
			return false; // Out of memory
	}

	// Add any unused's that come after the last string, point them to the 'NUL' string
	while ( strIndex <= numStrs )
	{
		if ( ( extendedTable && newStrSection->add<u32>(strPortionOffset) ) ||		// Add string offset (extended)
			 ( !extendedTable && newStrSection->add<u16>(u16(strPortionOffset)) ) // Add string offset (regular)
		   )
		{
			strIndex ++;
		}
		else
			return false; // Out of memory
	}

	if ( extendedTable )
	{
		if ( !newStrSection->add<u32>(0, numStrs) ) // Add space for prop stuct
			return false;

		for ( auto it = strList.begin(); it != strList.end(); it ++ ) // Fill in prop struct if it's used
		{
			if ( it->propStruct != 0 )
				newStrSection->replace<u32>(4+4*numStrs+4*it->stringNum, it->propStruct);
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
				   AddSection(*newStrSection);   
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
				   AddSection(*newStrSection);
		}
	}
}

void Scenario::correctMTXM()
{
	bool didCorrection;

	do
	{
		didCorrection = false;

		section* lastInstance = nullptr,
			   * curr		  = head   ;

		while ( curr != nullptr ) // Cycle through all sections
		{
			u32 currID = (u32 &)curr->buf.title()[0]; // First four characters can also be considered a representative integer

			if ( currID == HEADER_MTXM )
			{
				if ( lastInstance != nullptr && lastInstance->buf.size() == XSize()*YSize()*2 && curr->buf.size() != XSize()*YSize()*2 )
					// A second MTXM section was found
					// The second MTXM section is not long enough to hold all the tiles, but the first is
					// However, starcraft will still read in all the tiles it can from this section first
					// Correct by merging the second section into the first section, then deleting the second section
				{
					buffer& srcBuf = curr->buf;
					char* data = (char*)srcBuf.getPtr(0);
					lastInstance->buf.replaceStr(0, data, srcBuf.size());
					RemoveSection(&srcBuf);
					didCorrection = true;
					break; // curr section was removed, must break out of this loop
				}
				lastInstance = curr;
			}
			curr = curr->next;
		}
	} while ( didCorrection );
}

bool Scenario::setPlayerOwner(u8 player, u8 newOwner)
{
	IOWN().replace<u8>((u32)player, newOwner); // This section is not required by starcraft and should not affect function success/faliure
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
		return PUNI().replace<u8>(PTEC_GLOBAL_AVAILABILITY+unitId, 1);
	else
		return PUNI().replace<u8>(PTEC_GLOBAL_AVAILABILITY+unitId, 0);
}

bool Scenario::setUnitEnabledState(u16 unitId, u8 player, u8 unitEnabledState)
{
	if ( unitEnabledState == UNIT_STATE_DEFAULTFORPLAYER )
	{
		if ( PUNI().replace<u8>(PTEC_PLAYERUSESDEFAULT+(u32)unitId+228*(u32)player, 1) ) // Make player use default for this unit
		{
			PUNI().replace<u8>((u32)unitId+228*(u32)player, 0); // Clear player's unit enabled state (for compression, not necessary)
			return true;
		}
	}
	else if ( unitEnabledState == UNIT_STATE_ENABLEDFORPLAYER )
	{
		return PUNI().replace<u8>((u32)unitId+228*(u32)player, 1) && // Set player's unit enabled state to enabled
			   PUNI().replace<u8>(PTEC_PLAYERUSESDEFAULT+(u32)unitId+228*(u32)player, 0); // Make player override this unit's default
	}
	else if ( unitEnabledState == UNIT_STATE_DISABLEDFORPLAYER )
	{
		return PUNI().replace<u8>((u32)unitId+228*(u32)player, 0) && // Set player's unit enabled state to disabed
			   PUNI().replace<u8>(PTEC_PLAYERUSESDEFAULT+(u32)unitId+228*(u32)player, 0); // Make player override this unit's default
	}
	return false;
}

bool Scenario::setUnitSettingsCompleteHitpoints(u16 unitId, u32 completeHitpoints)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints),
		 replacedUNIx = UNIx().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints);
	
	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsHitpoints(u16 unitId, u32 hitpoints)
{
	u32 completeHitpoints;
	if ( unitSettings().get<u32>(completeHitpoints, UNIT_SETTINGS_HITPOINTS+4*(u32)unitId) )
	{
		u32 hitpointsByte = completeHitpoints%256;
		completeHitpoints = hitpoints*256+(u32)hitpointsByte;

		bool expansion = isExpansion(),
			 replacedUNIS = UNIS().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints),
			 replacedUNIx = UNIx().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints);
		return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
	}
	else
		return false;
}

bool Scenario::setUnitSettingsHitpointByte(u16 unitId, u8 hitpointByte)
{
	u32 completeHitpoints;
	if ( unitSettings().get<u32>(completeHitpoints, UNIT_SETTINGS_HITPOINTS+4*(u32)unitId) )
	{
		u32 hitpoints = completeHitpoints/256;
		completeHitpoints = 256*hitpoints+(u32)hitpointByte;

		bool expansion = isExpansion(),
			 replacedUNIS = UNIS().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints),
			 replacedUNIx = UNIx().replace<u32>(UNIT_SETTINGS_HITPOINTS+4*(u32)unitId, completeHitpoints);
		return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
	}
	else
		return false;
}

bool Scenario::setUnitSettingsShieldPoints(u16 unitId, u16 shieldPoints)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_SHIELDPOINTS+2*(u32)unitId, shieldPoints),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_SHIELDPOINTS+2*(u32)unitId, shieldPoints);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsArmor(u16 unitId, u8 armor)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u8>(UNIT_SETTINGS_ARMOR+(u32)unitId, armor),
		 replacedUNIx = UNIx().replace<u8>(UNIT_SETTINGS_ARMOR+(u32)unitId, armor);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBuildTime(u16 unitId, u16 buildTime)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_BUILDTIME+2*(u32)unitId, buildTime),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_BUILDTIME+2*(u32)unitId, buildTime);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsMineralCost(u16 unitId, u16 mineralCost)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_MINERALCOST+2*(u32)unitId, mineralCost),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_MINERALCOST+2*(u32)unitId, mineralCost);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsGasCost(u16 unitId, u16 gasCost)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_GASCOST+2*(u32)unitId, gasCost),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_GASCOST+2*(u32)unitId, gasCost);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBaseWeapon(u32 weaponId, u16 baseDamage)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_BASEWEAPON+2*weaponId, baseDamage),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_BASEWEAPON+2*weaponId, baseDamage);
	
	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
}

bool Scenario::setUnitSettingsBonusWeapon(u32 weaponId, u16 bonusDamage)
{
	bool expansion = isExpansion(),
		 replacedUNIS = UNIS().replace<u16>(UNIT_SETTINGS_BONUSWEAPON(false)+2*weaponId, bonusDamage),
		 replacedUNIx = UNIx().replace<u16>(UNIT_SETTINGS_BONUSWEAPON(true)+2*weaponId, bonusDamage);

	return ( expansion && replacedUNIx ) || ( !expansion && replacedUNIS );
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
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_MINERALCOST(false)+2*(u32)upgradeId, mineralCost),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_MINERALCOST(true)+2*(u32)upgradeId, mineralCost);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeMineralFactor(u8 upgradeId, u16 mineralFactor)
{
	bool expansion = isExpansion(),
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_MINERALFACTOR(false)+2*(u32)upgradeId, mineralFactor),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_MINERALFACTOR(true)+2*(u32)upgradeId, mineralFactor);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeGasCost(u8 upgradeId, u16 gasCost)
{
	bool expansion = isExpansion(),
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_GASCOST(false)+2*(u32)upgradeId, gasCost),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_GASCOST(true)+2*(u32)upgradeId, gasCost);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeGasFactor(u8 upgradeId, u16 gasFactor)
{
	bool expansion = isExpansion(),
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_GASFACTOR(false)+2*(u32)upgradeId, gasFactor),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_GASFACTOR(true)+2*(u32)upgradeId, gasFactor);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeTimeCost(u8 upgradeId, u16 timeCost)
{
	bool expansion = isExpansion(),
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_TIMECOST(false)+2*(u32)upgradeId, timeCost),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_TIMECOST(true)+2*(u32)upgradeId, timeCost);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradeTimeFactor(u8 upgradeId, u16 timeFactor)
{
	bool expansion = isExpansion(),
		 replacedUPGS = UPGS().replace<u16>(UPGRADE_SETTINGS_TIMEFACTOR(false)+2*(u32)upgradeId, timeFactor),
		 replacedUPGx = UPGx().replace<u16>(UPGRADE_SETTINGS_TIMEFACTOR(true)+2*(u32)upgradeId, timeFactor);

	return ( expansion && replacedUPGx ) || ( !expansion && replacedUPGS );
}

bool Scenario::setUpgradePlayerDefaults(u8 upgradeId, u8 player, bool usesDefaultLevels)
{
	u8 newValue = 0;
	if ( usesDefaultLevels )
		newValue = 1;

	bool expansion = isExpansion(),
		 replacedUPGR = UPGR().replace<u8>(UPGRADE_PLAYERUSESDEFAULT(false, player)+(u32)upgradeId, newValue),
		 replacedPUPx = PUPx().replace<u8>(UPGRADE_PLAYERUSESDEFAULT(true, player)+(u32)upgradeId, newValue);

	return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 newStartLevel)
{
	bool expansion = isExpansion(),
		 replacedUPGR = UPGR().replace<u8>(UPGRADE_PLAYERSTARTLEVEL(false, player)+(u32)upgradeId, newStartLevel),
		 replacedPUPx = PUPx().replace<u8>(UPGRADE_PLAYERSTARTLEVEL(true, player)+(u32)upgradeId, newStartLevel);

	return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 newMaxLevel)
{
	bool expansion = isExpansion(),
		 replacedUPGR = UPGR().replace<u8>(UPGRADE_PLAYERMAXLEVEL(false, player)+(u32)upgradeId, newMaxLevel),
		 replacedPUPx = PUPx().replace<u8>(UPGRADE_PLAYERMAXLEVEL(true, player)+(u32)upgradeId, newMaxLevel);

	return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradeDefaultStartLevel(u8 upgradeId, u8 newStartLevel)
{
	bool expansion = isExpansion(),
		 replacedUPGR = UPGR().replace<u8>(UPGRADE_DEFAULTSTARTLEVEL(false)+(u32)upgradeId, newStartLevel),
		 replacedPUPx = PUPx().replace<u8>(UPGRADE_DEFAULTSTARTLEVEL(true)+(u32)upgradeId, newStartLevel);

	return ( expansion && replacedPUPx ) || ( !expansion && replacedUPGR );
}

bool Scenario::setUpgradeDefaultMaxLevel(u8 upgradeId, u8 newMaxLevel)
{
	bool expansion = isExpansion(),
		 replacedUPGR = UPGR().replace<u8>(UPGRADE_DEFAULTMAXLEVEL(false)+(u32)upgradeId, newMaxLevel),
		 replacedPUPx = PUPx().replace<u8>(UPGRADE_DEFAULTMAXLEVEL(true)+(u32)upgradeId, newMaxLevel);

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
		 replacedTECS = TECS().replace<u16>(TECH_COST_MINERALS(false)+2*(u32)techId, newMineralCost),
		 replacedTECx = TECx().replace<u16>(TECH_COST_MINERALS(true)+2*(u32)techId, newMineralCost);
	
	return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechGasCost(u8 techId, u16 newGasCost)
{
	bool expansion = isExpansion(),
		 replacedTECS = TECS().replace<u16>(TECH_COST_GAS(false)+2*(u32)techId, newGasCost),
		 replacedTECx = TECx().replace<u16>(TECH_COST_GAS(true)+2*(u32)techId, newGasCost);
	
	return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechTimeCost(u8 techId, u16 newTimeCost)
{
	bool expansion = isExpansion(),
		 replacedTECS = TECS().replace<u16>(TECH_COST_TIME(false)+2*(u32)techId, newTimeCost),
		 replacedTECx = TECx().replace<u16>(TECH_COST_TIME(true)+2*(u32)techId, newTimeCost);
	
	return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechEnergyCost(u8 techId, u16 newEnergyCost)
{
	bool expansion = isExpansion(),
		 replacedTECS = TECS().replace<u16>(TECH_COST_ENERGY(false)+2*(u32)techId, newEnergyCost),
		 replacedTECx = TECx().replace<u16>(TECH_COST_ENERGY(true)+2*(u32)techId, newEnergyCost);
	
	return ( expansion && replacedTECx ) || ( !expansion && replacedTECS );
}

bool Scenario::setTechAvailableForPlayer(u8 techId, u8 player, bool availableForPlayer)
{
	u8 newValue = 0;
	if ( availableForPlayer )
		newValue = 1;

	bool setExp = PTEx().replace<u8>(TECH_AVAILABLEFORPLAYER(true, player)+(u32)techId, newValue),
		 setNormal = PTEC().replace<u8>(TECH_AVAILABLEFORPLAYER(false, player)+(u32)techId, newValue);
	
	return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechResearchedForPlayer(u8 techId, u8 player, bool researchedForPlayer)
{
	u8 newValue = 0;
	if ( researchedForPlayer )
		newValue = 1;

	bool setExp = PTEx().replace<u8>(TECH_RESERACHEDFORPLAYER(true, player)+(u32)techId, newValue),
		 setNormal = PTEC().replace<u8>(TECH_RESERACHEDFORPLAYER(false, player)+(u32)techId, newValue);
	
	return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechDefaultAvailability(u8 techId, bool availableByDefault)
{
	u8 newValue = 0;
	if ( availableByDefault )
		newValue = 1;

	bool setExp = PTEx().replace<u8>(TECH_DEFAULTAVAILABILITY(true)+(u32)techId, newValue),
		 setNormal = PTEC().replace<u8>(TECH_DEFAULTAVAILABILITY(false)+(u32)techId, newValue);
	
	return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setTechDefaultResearched(u8 techId, bool researchedByDefault)
{
	u8 newValue = 0;
	if ( researchedByDefault )
		newValue = 1;

	bool setExp = PTEx().replace<u8>(TECH_DEFAULTRESERACHED(true)+(u32)techId, newValue),
		 setNormal = PTEC().replace<u8>(TECH_DEFAULTRESERACHED(false)+(u32)techId, newValue);
	
	return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::setPlayerUsesDefaultTechSettings(u8 techId, u8 player, bool playerUsesDefaultSettings)
{
	u8 newValue = 0;
	if ( playerUsesDefaultSettings )
		newValue = 1;

	bool setExp = PTEx().replace<u8>(TECH_PLAYERUSESDEFAULT(true, player)+(u32)techId, newValue),
		 setNormal = PTEC().replace<u8>(TECH_PLAYERUSESDEFAULT(false, player)+(u32)techId, newValue);
	
	return ( isExpansion() && setExp ) || ( !isExpansion() && setNormal );
}

bool Scenario::addTrigger(Trigger &trigger)
{
	return TRIG().add<Trigger>(trigger);
}

bool Scenario::insertTrigger(u32 triggerId, Trigger &trigger)
{
	return TRIG().insert<Trigger>(triggerId*TRIG_STRUCT_SIZE, trigger);
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

bool Scenario::ParseBuffer(buffer &chk)
{
	int maxSections = 500;
	u32 position = 0;
	s32 size = 0;

	char header[5] = { };

	section* last = nullptr,
		   * curr;
	
	while ( position+8 < chk.size() )
	{
		if ( !chk.get<u32>((u32&)header, position) )
			return false;

		section* checkExisting = head;
		while ( checkExisting != nullptr && !isProtected() )
		{
			if ( !strcmp(checkExisting->buf.title(), header) ) // Duplicate section
				mapIsProtected = true;

			checkExisting = checkExisting->next;
		}

		try { curr = new section(header); }
		catch ( std::bad_alloc ) { return false; }

		curr->next = nullptr;

		if ( head == nullptr )
			head = curr;
		else if ( last != nullptr )
			last->next = curr;
		
		last = curr;

		if ( !curr->buf.extract(chk, position) && chk.get<s32>(size, position) )
		{
			if ( chk.get<s32>(size, position) )
			{
				position += 4;
				if ( position+size > chk.size() && !isProtected() ) // The size would take you past the end of the data
					mapIsProtected = true;

				position += (s32)size;
			}
			else // Your position was too great to get an int
				mapIsProtected = true;
		}

		maxSections--;
		if ( !maxSections )
			mapIsProtected = true;
	}

	if ( !GoodVCOD() )
		mapIsProtected = true;

	if ( isProtected() )
	{
		correctMTXM();
		#ifdef CHKDRAFT
			mb("Map is protected and will be opened as view-only.");
		#endif
	}

	return true;
}

bool Scenario::ToSingleBuffer(buffer& chk)
{
	if ( chk.setTitle("CHK ") )
	{
		section* curr = head;
		while ( curr != nullptr )
		{
			if ( !( chk.add<u32>((u32&)curr->buf.title()[0]) &&
					chk.add<u32>(curr->buf.size()) &&
					( curr->buf.size() == 0 || chk.addStr((const char*)curr->buf.getPtr(0), curr->buf.size()) ) ) )
			{
				return false;
			}
			curr = curr->next;
		}
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
		if ( ParseBuffer(chk) )
			return true;
	}
	return false;
}

bool Scenario::CreateNew(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers)
{
	buffer chk("nMAP");

	if (	Get_TYPE(chk) && Get_VER (chk) && Get_IVE2(chk)	&& Get_VCOD(chk)
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
		if ( ParseBuffer(chk) )
			return true;
		else // Chkdraft failed to read a map it creates itself, should never happen!
		{
			#ifdef CHKDRAFT
				Error("New map parsing failed! Report this!");
			#endif
		}
	}
	return false;
}

void Scenario::WriteFile(FILE* pFile)
{
	if ( !isProtected() )
	{
		section* curr = head;
		while ( curr != nullptr )
		{
			curr->buf.write(pFile);
			curr = curr->next;
		}
	}
}

bool Scenario::RemoveSection(buffer* buf)
{
	section* curr = head,
		   * prev = nullptr;

	while ( curr )
	{
		if ( &curr->buf == buf )
		{
			if ( prev )
				prev->next = curr->next;
			else
				head = curr->next;

			delete curr;
			curr = nullptr;
			return true;
		}
		prev = curr;
		curr = curr->next;
	}
	return false;
}

bool Scenario::AddSection(buffer& buf)
{
	section* add;
	try {
		add = new section(buf);
	} catch ( std::bad_alloc ) { return false; }

	add->next = nullptr;

	if ( head == nullptr )
		head = add;
	else
	{
		section* last = head; // Invariant: head != nullptr
		while ( last->next != nullptr )
			last = last->next;

		last->next = add;
	}

	return true;
}

void Scenario::Flush()
{
	section* next;
	while ( head != nullptr )
	{
		next = head->next;
		delete head;
		head = next;
	}
	mapIsProtected = false;
}

buffer& Scenario::getSection(u32 id)
{
	section* lastInstance = nullptr, // Only the last instance of a section is used
		   * curr		  = head   ;

	while ( curr != nullptr ) // Cycle through all sections
	{
		u32 currID = (u32 &)curr->buf.title()[0]; // First four characters can also be considered a representative integer

		if ( currID == id )
			lastInstance = curr;

		curr = curr->next;
	}

	if ( lastInstance == nullptr )
	{
		buffer* nullBuffer = nullptr;
		return *nullBuffer;
	}
	else
		return lastInstance->buf;
	/** Referances to null buffers are
		safe to use though you may violate
		user expectations if you do not check
		that they exist at some point. */
}

// Standard Sections
buffer& Scenario::TYPE() { return getSection(HEADER_TYPE); } buffer& Scenario::VER () { return getSection(HEADER_VER ); }
buffer& Scenario::IVER() { return getSection(HEADER_IVER); } buffer& Scenario::IVE2() { return getSection(HEADER_IVE2); }
buffer& Scenario::VCOD() { return getSection(HEADER_VCOD); } buffer& Scenario::IOWN() { return getSection(HEADER_IOWN); }
buffer& Scenario::OWNR() { return getSection(HEADER_OWNR); } buffer& Scenario::ERA () { return getSection(HEADER_ERA ); }
buffer& Scenario::DIM () { return getSection(HEADER_DIM ); } buffer& Scenario::SIDE() { return getSection(HEADER_SIDE); }
buffer& Scenario::MTXM() { return getSection(HEADER_MTXM); } buffer& Scenario::PUNI() { return getSection(HEADER_PUNI); }
buffer& Scenario::UPGR() { return getSection(HEADER_UPGR); } buffer& Scenario::PTEC() { return getSection(HEADER_PTEC); }
buffer& Scenario::UNIT() { return getSection(HEADER_UNIT); } buffer& Scenario::ISOM() { return getSection(HEADER_ISOM); }
buffer& Scenario::TILE() { return getSection(HEADER_TILE); } buffer& Scenario::DD2 () { return getSection(HEADER_DD2 ); }
buffer& Scenario::THG2() { return getSection(HEADER_THG2); } buffer& Scenario::MASK() { return getSection(HEADER_MASK); }
buffer& Scenario::STR () { return getSection(HEADER_STR ); } buffer& Scenario::UPRP() { return getSection(HEADER_UPRP); }
buffer& Scenario::UPUS() { return getSection(HEADER_UPUS); } buffer& Scenario::MRGN() { return getSection(HEADER_MRGN); }
buffer& Scenario::TRIG() { return getSection(HEADER_TRIG); } buffer& Scenario::MBRF() { return getSection(HEADER_MBRF); }
buffer& Scenario::SPRP() { return getSection(HEADER_SPRP); } buffer& Scenario::FORC() { return getSection(HEADER_FORC); }
buffer& Scenario::WAV () { return getSection(HEADER_WAV ); } buffer& Scenario::UNIS() { return getSection(HEADER_UNIS); }
buffer& Scenario::UPGS() { return getSection(HEADER_UPGS); } buffer& Scenario::TECS() { return getSection(HEADER_TECS); }
buffer& Scenario::SWNM() { return getSection(HEADER_SWNM); } buffer& Scenario::COLR() { return getSection(HEADER_COLR); }
buffer& Scenario::PUPx() { return getSection(HEADER_PUPx); } buffer& Scenario::PTEx() { return getSection(HEADER_PTEx); }
buffer& Scenario::UNIx() { return getSection(HEADER_UNIx); } buffer& Scenario::UPGx() { return getSection(HEADER_UPGx); }
buffer& Scenario::TECx() { return getSection(HEADER_TECx); }

// Extended Sections
buffer& Scenario::KSTR() { return getSection(HEADER_KSTR); }

bool Scenario::GetStrInfo(char* &ptr, u32 &length, u32 stringNum)
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
		u32 start,
			end;

		if ( extended )
			start = strings->get<u32>(4+4*stringNum);
		else
			start = strings->get<u16>(2*stringNum);
		
		if ( strings->getNext('\0', start, end) ) // Next NUL char marks the end of the string
			end ++; // Add space for NUL char
		else // Much corruption
			end = strings->size(); // Just end it where section ends

		length = end-start;
		ptr = (char*)strings->getPtr(start);
		return true;
	}
	else
		return false;
}

bool Scenario::MakeStr(string& dest, char* src, u32 srcLen)
{
	try
	{
		for ( u32 i=0; i<srcLen-1; i++ )
		{
			if ( (u8)src[i] < 32 && src[i] != '\n' && src[i] != '\r' && src[i] != '\11' )
			{
				dest.push_back('<');
				if ( src[i]/16 > 9 )
					dest.push_back(src[i]/16+'A'-10);
				else
					dest.push_back(src[i]/16+'0');
				if ( src[i]%16 > 9 )
					dest.push_back(src[i]%16+'A'-10);
				else
					dest.push_back(src[i]%16+'0');
				dest.push_back('>');
			}
			else
				dest.push_back(src[i]);
		}
		return true;
	}
	catch ( std::exception ) // Catch bad_alloc and length_error
	{
		dest.clear();
		return false;
	}
}

bool Scenario::MakeEscapedStr(string& dest, char* src, u32 srcLen)
{
	try
	{
		for ( u32 i=0; i<srcLen-1; i++ )
		{
			if ( src[i] == '\n' )
				dest.append("\\n");
			else if ( src[i] == '\r' )
				dest.append("\\r");
			else if ( src[i] == '\"' )
				dest.append("\\\"");
			else if ( src[i] < 32 && src[i] != '\11')
			{
				dest.push_back('\\');
				dest.push_back('x');
				dest.push_back('0');
				if ( unsigned char(src[i])/16 <= 9 )
					dest.push_back(unsigned char(src[i])/16+'0');
				else
					dest.push_back(unsigned char(src[i])/16-10+'A');
				if ( unsigned char(src[i])%16 <= 9 )
					dest.push_back(unsigned char(src[i])%16+'0');
				else
					dest.push_back(unsigned char(src[i])%16-10+'A');
			}
			else if ( src[i] == '\\' )
				dest.append("\\\\");
			else
				dest.push_back(src[i]);
		}
		return true;
	}
	catch ( std::exception ) // Catch bad_alloc and length_error
	{
		dest.clear();
		return false;
	}
}

bool Scenario::ZeroOutString(u32 stringNum)
{
	char* str;
	u32 length;
	if ( GetStrInfo(str, length, stringNum) )
	{
		for ( u32 i=0; i<length; i++ ) // Zero-out characters
			str[i] = '\0';

		if ( stringNum < numStrings() )
		{
			if ( STR().size() > u32(numStrings())*2+2 ) // If you can get to start of string data
				return STR().replace<u16>(2*stringNum, numStrings()*2+2); // Set string offset to start of string data (NUL)
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
	string str;
	if ( stringExists(stringNum) && getString(str, stringNum) && str.c_str()[0] != '\0' )
		return false; // No Repair
	else
	{
		if ( addString("strRepair", stringNum, extended) )
			return true;
		else
			return false;
	}
}
