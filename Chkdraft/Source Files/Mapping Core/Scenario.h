#ifndef SCENARIO_H
#define SCENARIO_H
#include "Buffer.h"
#include "Basics.h"
#include "ChkDoodad.h"
#include "ChkLocation.h"
#include "ChkUnit.h"
#include "ChkSprite.h"
#include "Trigger.h"
#include "Condition.h"
#include "Action.h"
#include "StringTableNode.h"
#include <list>
using namespace std;

class section
{
	public:
		buffer buf;
		section* next;
	
		section(char *title);
		section(buffer& buf);
};

class Scenario
{
	public:

/* Constructors */	Scenario();

/* Destructors  */	~Scenario();

/*  Quick Pass  */	Scenario* scenario(); // Returns pointer to this scenario, useful for classes that inherit 'Scenario'

/* Quick Access */	u16 XSize(); // Get width of map
					u16 YSize(); // Get height of map

					u16 getTileset(); // Gets the maps tileset

					u16 numUnits(); // Returns number of units in UNIT section
					bool getUnit(ChkUnit* &unitRef, u16 index); // Gets unit at index

					bool getLocation(ChkLocation* &locRef, u16 index); // Gets location at index
					u8 numLocations(); // Gets the current amount of locations in the map

					u32 numTriggers(); // Returns the number of triggers in this scenario
					bool getTrigger(Trigger* &trigRef, u32 index); // Gets the trigger at index
					bool getActiveComment(Trigger* trigger, string &comment);

					bool getBriefingTrigger(Trigger* &trigRef, u32 index); // Gets the briefing trigger at index

					u32 getForceStringNum(u8 index); // Gets the string number of the force at the given index
					string getForceString(u8 forceNum); // Gets the name of the string at the given force num
					bool getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av); // Attempts to get info for given force num
					bool getPlayerForce(u8 playerNum, u8 &force); // Attempts to get the force a player belongs to

					bool getUnitStringNum(u16 unitId, u16 &stringNum);

					u16 numStrings(); // Returns number of strings in the STR section
					u32 totalStrings(); // Returns number of strings in the STR and KSTR sections
					bool getString(string &dest, u32 stringNum);
					bool getRawString(string &dest, u32 stringNum);
					bool getEscapedString(string &dest, u32 stringNum);
					bool getLocationName(string &dest, u8 locationID);
					void getUnitName(string &dest, u16 unitID);

					bool stringIsUsed(u32 stringNum); // Returns whether the string is used in the map
					bool isExtendedString(u32 stringNum); // Returns whether the string is an extended string
					bool stringExists(u32 stringNum); // Checks if a valid string is associated with this stringNum
					bool stringExists(string str, u32& stringNum); // Checks if string exists, returns true and stringNum if so
					bool stringExists(string str, u32& stringNum, bool extended); // Checks if string exists in the same table
					bool escStringDifference(string str, u32& stringNum); // Checks if there's a difference between str and string at stringNum
					u32 extendedToRegularStr(u32 stringNum); // Returns string number for extended section

					bool stringUsedWithLocs(u32 stringNum); // Returns whether the string is used for a location
					u32 amountStringUsed(u32 stringNum); // Returns the number of times a string is used
					void getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches);

					bool isExpansion(); // Check if the map uses expansion settings

					buffer& unitSettings(); // Gets settings from UNIS/UNIx based on whether the map is expansion
					buffer& upgradeSettings(); // Gets settings from UPGS/UPGx based on whether the map is expansion
					buffer& upgradeRestrictions(); // Gets upgradeAvailability from UPGR/PUPx based on whether the map is expansion
					buffer& techSettings(); // Gets settings from TECS/TECx based on whether the map is expansion
					buffer& techRestrictions(); // Gets PTEC/PTEx based on whether the map is expansion

					bool GoodVCOD(); // Check if VCOD is valid
					bool isProtected(); // Checks if map is protected

					bool getMapTitle(std::string &dest);
					bool getMapDescription(std::string &dest);

					bool getPlayerOwner(u8 player, u8& owner); // Gets the current owner of a player
					bool getPlayerRace(u8 player, u8& race); // Gets the players current race
					bool getPlayerColor(u8 player, u8& color); // Gets the players current color

					bool unitUsesDefaultSettings(u16 unitId);
					bool unitIsEnabled(u16 unitId);
					u8 getUnitEnabledState(u16 unitId, u8 player);
						#define UNIT_STATE_DEFAULTFORPLAYER 0
						#define UNIT_STATE_ENABLEDFORPLAYER 1
						#define UNIT_STATE_DISABLEDFORPLAYER 2

					bool getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints);
					bool getUnitSettingsHitpointByte(u16 unitId, u8 &hitpointByte);
					bool getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints);
					bool getUnitSettingsArmor(u16 unitId, u8 &armor);
					bool getUnitSettingsBuildTime(u16 unitId, u16 &buildTime);
					bool getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost);
					bool getUnitSettingsGasCost(u16 unitId, u16 &gasCost);
					bool getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage);
					bool getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage);

					bool upgradeUsesDefaultCosts(u8 upgradeId);
					bool getUpgradeMineralCost(u8 upgradeId, u16 &mineralCost);
					bool getUpgradeMineralFactor(u8 upgradeId, u16 &mineralFactor);
					bool getUpgradeGasCost(u8 upgradeId, u16 &gasCost);
					bool getUpgradeGasFactor(u8 upgradeId, u16 &gasFactor);
					bool getUpgradeTimeCost(u8 upgradeId, u16 &timeCost);
					bool getUpgradeTimeFactor(u8 upgradeId, u16 &timeFactor);

					bool getUpgradeDefaultStartLevel(u8 upgradeId, u8 &startLevel);
					bool getUpgradeDefaultMaxLevel(u8 upgradeId, u8 &maxLevel);

					bool playerUsesDefaultUpgradeLevels(u8 upgradeId, u8 player);
					bool getUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 &startLevel);
					bool getUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 &maxLevel);

					bool techUsesDefaultCosts(u8 techId);
					bool getTechMineralCost(u8 techId, u16 &mineralCost);
					bool getTechGasCost(u8 techId, u16 &gasCost);
					bool getTechTimeCost(u8 techId, u16 &timeCost);
					bool getTechEnergyCost(u8 techId, u16 &energyCost);

					bool techAvailableForPlayer(u8 techId, u8 player);
					bool techResearchedForPlayer(u8 techId, u8 player);
					bool techAvailableByDefault(u8 techId);
					bool techResearchedByDefault(u8 techId);
					bool playerUsesDefaultTechSettings(u8 techId, u8 player);

/*   Editing    */	bool setTileset(u16 newTileset); // Sets a new value for the maps tileset
					bool setDimensions(u16 newWidth, u16 newHeight); // Sets new dimensions for the map
					
					bool addUnit(u16 unitID, u8 owner, u16 xc, u16 yc, u16 stateFlags); // Attempts to create a unit

					bool createLocation(s32 xc1, s32 yc1, s32 xc2, s32 yc2, u16& locationIndex); // Create a location
					void deleteLocation(u16 locationIndex); // Unconditionally deletes a location
					
					bool removeUnusedString(u32 stringNum); // Delete string if it is no longer used anywhere in the map
					void forceDeleteString(u32 stringNum); // Unconditionally deletes a string

					/** Attempt to associate a stringNum with a string
						Returns true and stringNum if string exists or was created successfully */
					bool addString(string str, u32& stringNum, bool extended);

					/** Attempt to replace a string (specific instance of string), if unsuccessful the old string will remain
							(if it is not used elsewhere) based on safeReplace and memory conditions
						stringNum: if not 0 and only used once, the map calls editString, otherwise stringNum is set if successful
						safeReplace: if true the function will only attempt to make room for the new string if the old one will not be lost
									 if false the fuction will always attempt to make room, and may lose the old string in low-mem conditions
						Returns true if the string exists already or was successfully created */
					template <typename numType> // Strings can, and can only be u16 or u32
						bool replaceString(string newString, numType& stringNum, bool extended, bool safeReplace);

					/** Attempts to edit the contents of a string (universal change to string), if unsucessesful the old string will remain
							(if it is not used elsewhere) based on safeEdit and memory conditions
						safeEdit: if true the function will only attempt to make room for the new string if the old one will not be lost
								  if false the function will always attempt to make room, and may lose the old string in low-mem conditions
						Returns true if the string contents were successfully edited, always false if stringNum is 0 */
					template <typename numType> // Strings can, and can only be u16 or u32
						bool editString(string newString, numType stringNum, bool extended, bool safeEdit);

					void replaceStringNum(u32 toReplace, u32 replacement);

					void removeMetaStrings(); // Attemps to remove all strings not used in-game
					bool cleanStringTable(bool extendedTable); // Rebuilds string table with current indexs, removes all unused strings
					bool removeDuplicateStrings();
					bool compressStringTable(bool extendedTable, bool recycleSubStrings); // Rebuilds string table with minimum indexs and smallest size possible
					bool repairStringTable(bool extendedTable); // Removes what Scmdraft considers string corruption
					bool addAllUsedStrings(std::list<StringTableNode>& strList, u32 flags); // Adds all used strings to the strList
						#define STRADD_INCLUDE_STANDARD BIT_0
						#define STRADD_INCLUDE_EXTENDED BIT_1
						#define STRADD_INCLUDE_ALL		(STRADD_INCLUDE_STANDARD|STRADD_INCLUDE_EXTENDED)
					bool rebuildStringTable(std::list<StringTableNode> strList, bool extendedTable); // Rebuilds string table using the provided list

					void correctMTXM(); // Corrects MTXM of protected maps for view-only purposes

					bool setPlayerOwner(u8 player, u8 newOwner); // Sets a new owner for a player
					bool setPlayerRace(u8 player, u8 newRace); // Sets a new race for a player
					bool setPlayerColor(u8 player, u8 newColor); // Sets a new color for a player
					bool setPlayerForce(u8 player, u8 newForce); // Sets a new force for a player

					bool setForceAllied(u8 forceNum, bool allied); // Specifies whether a force's players are allied
					bool setForceVision(u8 forceNum, bool sharedVision); // Specifies whether a force's players have shared vision
					bool setForceRandom(u8 forceNum, bool randomStartLocs); // Specifies whether a force has randomized start locations
					bool setForceAv(u8 forceNum, bool alliedVictory); // Specifies whether a force's players have allied victory

					bool setUnitUseDefaults(u16 unitID, bool useDefaults); // Specifies whether a unit uses default settings
					bool setUnitEnabled(u16 unitId, bool enabled);
					bool setUnitEnabledState(u16 unitId, u8 player, u8 enabledState);

					bool setUnitSettingsCompleteHitpoints(u16 unitId, u32 completeHitpoints);
					bool setUnitSettingsHitpoints(u16 unitId, u32 hitpoints);
					bool setUnitSettingsHitpointByte(u16 unitId, u8 hitpointByte);
					bool setUnitSettingsShieldPoints(u16 unitId, u16 shieldPoints);
					bool setUnitSettingsArmor(u16 unitId, u8 armor);
					bool setUnitSettingsBuildTime(u16 unitId, u16 buildTime);
					bool setUnitSettingsMineralCost(u16 unitId, u16 mineralCost);
					bool setUnitSettingsGasCost(u16 unitId, u16 gasCost);
					bool setUnitSettingsBaseWeapon(u32 weaponId, u16 baseDamage);
					bool setUnitSettingsBonusWeapon(u32 weaponId, u16 bonusDamage);

					bool setUpgradeUseDefaults(u8 upgradeNum, bool useDefaults); // Specifies whether an upgrade uses default costs
					bool setUpgradeMineralCost(u8 upgradeId, u16 mineralCost);
					bool setUpgradeMineralFactor(u8 upgradeId, u16 mineralFactor);
					bool setUpgradeGasCost(u8 upgradeId, u16 gasCost);
					bool setUpgradeGasFactor(u8 upgradeId, u16 gasFactor);
					bool setUpgradeTimeCost(u8 upgradeId, u16 timeCost);
					bool setUpgradeTimeFactor(u8 upgradeId, u16 timeFactor);

					bool setUpgradePlayerDefaults(u8 upgradeId, u8 player, bool usesDefaultLevels);
					bool setUpgradePlayerStartLevel(u8 upgradeId, u8 player, u8 newStartLevel);
					bool setUpgradePlayerMaxLevel(u8 upgradeId, u8 player, u8 newMaxLevel);

					bool setUpgradeDefaultStartLevel(u8 upgradeId, u8 newStartLevel);
					bool setUpgradeDefaultMaxLevel(u8 upgradeId, u8 newMaxLevel);

					bool setTechUseDefaults(u8 techNum, bool useDefaultCosts); // Specifies whether a tech uses default costs
					bool setTechMineralCost(u8 techId, u16 newMineralCost);
					bool setTechGasCost(u8 techId, u16 newGasCost);
					bool setTechTimeCost(u8 techId, u16 newTimeCost);
					bool setTechEnergyCost(u8 techId, u16 newEnergyCost);

					bool setTechAvailableForPlayer(u8 techId, u8 player, bool availableForPlayer);
					bool setTechResearchedForPlayer(u8 techId, u8 player, bool researchedForPlayer);
					bool setTechDefaultAvailability(u8 techId, bool availableByDefault);
					bool setTechDefaultResearched(u8 techId, bool researchedByDefault);
					bool setPlayerUsesDefaultTechSettings(u8 techId, u8 player, bool playerUsesDefaultSettings);

					bool addTrigger(Trigger &trigger);
					bool insertTrigger(u32 triggerId, Trigger &trigger);
					bool deleteTrigger(u32 triggerId);
					bool copyTrigger(u32 triggerId);
					bool moveTriggerUp(u32 triggerId); // Moves the given trigger up one absolute index
					bool moveTriggerDown(u32 triggerId); // Moves the given trigger down one absolute index
					bool moveTrigger(u32 triggerId, u32 destId); // Moves the given trigger to the destination index

/*   File IO	*/	bool ParseBuffer(buffer &chk); // Parses supplied scenario file data
					bool ToSingleBuffer(buffer &chk); // Writes the chk to a buffer
					bool CreateNew(u16 width, u16 height, u16 tileset, u32 terrain, u32 triggers);
					void WriteFile(FILE* pFile); // Writes all sections to the supplied file
					bool Serialize(void* &data); /** Writes all sections to a buffer in memory as it would to a .chk file
													 includes a 4 byte "CHK " tag followed by a 4-byte size, followed by data */
					bool Deserialize(const void* data); // "Opens" a serialized Scenario.chk file, data must be 8+ bytes

/* Section mgmt */	bool RemoveSection(buffer* buf); // Removes section containing this buffer
					bool AddSection(buffer& buf); // Adds a buffer as a new map section
					void Flush(); // Deletes all sections and clears map protection

					/** Find section with the given header id
						ie: to get a referance to VCOD call getSection(HEADER_VCOD); */
					buffer& getSection(u32 id);

					/** Referances to section buffers used by Chkdraft. All buffer methods are
						safe to call whether or not the map/buffer exists, though you should
						check that they exist at some point to match the user expectations. */
					buffer& TYPE();	buffer& VER ();	buffer& IVER();	buffer& IVE2();
					buffer& VCOD();	buffer& IOWN();	buffer& OWNR();	buffer& ERA ();
					buffer& DIM ();	buffer& SIDE();	buffer& MTXM();	buffer& PUNI();
					buffer& UPGR();	buffer& PTEC();	buffer& UNIT();	buffer& ISOM();
					buffer& TILE();	buffer& DD2 ();	buffer& THG2();	buffer& MASK();
					buffer& STR ();	buffer& UPRP();	buffer& UPUS();	buffer& MRGN();
					buffer& TRIG();	buffer& MBRF();	buffer& SPRP();	buffer& FORC();
					buffer& WAV ();	buffer& UNIS();	buffer& UPGS();	buffer& TECS();
					buffer& SWNM();	buffer& COLR();	buffer& PUPx();	buffer& PTEx();
					buffer& UNIx();	buffer& UPGx();	buffer& TECx();

					// Extended sections
					buffer& KSTR();


	protected:

		bool GetStrInfo(char* &ptr, u32 &length, u32 stringNum); // Gets a pointer to the string and its length if successful
		bool MakeStr(string& dest, char* src, u32 srcLen); // Makes a C++ string from a C string
		bool MakeEscapedStr(string& dest, char* src, u32 srcLen); // Makes a C++ string with escaped characters from a C string
		bool ZeroOutString(u32 stringNum); // returns false if stringNum is not a string in any sense
		bool RepairString(u32& stringNum, bool extended); // Ensures that the string would not be considered corrupted by Scmdraft


	private:

		section* head; // Linked list of sections
		bool mapIsProtected; // Flagged if map is protected
};

#define HEADER_TYPE 1162893652
#define HEADER_VER   542262614
#define HEADER_IVER 1380275785
#define HEADER_IVE2  843404873
#define HEADER_VCOD 1146045270
#define HEADER_IOWN 1314344777
#define HEADER_OWNR 1380865871
#define HEADER_ERA   541151813
#define HEADER_DIM   541935940
#define HEADER_SIDE 1162103123
#define HEADER_MTXM 1297634381
#define HEADER_PUNI 1229870416
#define HEADER_UPGR 1380405333
#define HEADER_PTEC 1128617040
#define HEADER_UNIT 1414090325
#define HEADER_ISOM 1297044297
#define HEADER_TILE 1162627412
#define HEADER_DD2   540165188
#define HEADER_THG2  843532372
#define HEADER_MASK 1263747405
#define HEADER_STR   542266451
#define HEADER_UPRP 1347571797
#define HEADER_UPUS 1398100053
#define HEADER_MRGN 1313296973
#define HEADER_TRIG 1195987540
#define HEADER_MBRF 1179796045
#define HEADER_SPRP 1347571795
#define HEADER_FORC 1129467718
#define HEADER_WAV   542523735
#define HEADER_UNIS 1397313109
#define HEADER_UPGS 1397182549
#define HEADER_TECS 1396917588
#define HEADER_SWNM 1296979795
#define HEADER_COLR 1380732739
#define HEADER_PUPx 2018530640
#define HEADER_PTEx 2017809488
#define HEADER_UNIx 2018070101
#define HEADER_UPGx 2017939541
#define HEADER_TECx 2017674580

// Extended sections
#define HEADER_KSTR 1381258059

// Data locations/constants
#define UNIT_SETTINGS_HITPOINTS		228
#define UNIT_SETTINGS_SHIELDPOINTS	1140
#define UNIT_SETTINGS_ARMOR			1596
#define UNIT_SETTINGS_BUILDTIME		1824
#define UNIT_SETTINGS_MINERALCOST	2280
#define UNIT_SETTINGS_GASCOST		2736
#define UNIT_SETTINGS_STRING_IDS	3192 // Where strings begin in the UNIS and UNIX sections
#define UNIT_SETTINGS_BASEWEAPON	3648
#define UNIT_SETTINGS_BONUSWEAPON(isExpansion) (isExpansion?3908:3848)
#define PTEC_GLOBAL_AVAILABILITY	2736
#define PTEC_PLAYERUSESDEFAULT		2964

#define UPGRADE_SETTINGS_MINERALCOST(isExpansion) (isExpansion?62:46)
#define UPGRADE_SETTINGS_MINERALFACTOR(isExpansion) (isExpansion?184:138)
#define UPGRADE_SETTINGS_GASCOST(isExpansion) (isExpansion?306:230)
#define UPGRADE_SETTINGS_GASFACTOR(isExpansion) (isExpansion?428:322)
#define UPGRADE_SETTINGS_TIMECOST(isExpansion) (isExpansion?550:414)
#define UPGRADE_SETTINGS_TIMEFACTOR(isExpansion) (isExpansion?672:506)

#define UPGRADE_PLAYERMAXLEVEL(isExpansion, player) (isExpansion?(61*(u32)player):(46*(u32)player))
#define UPGRADE_PLAYERSTARTLEVEL(isExpansion, player) (isExpansion?(732+61*(u32)player):(552+46*(u32)player))
#define UPGRADE_DEFAULTMAXLEVEL(isExpansion) (isExpansion?1464:1104)
#define UPGRADE_DEFAULTSTARTLEVEL(isExpansion) (isExpansion?1525:1150)
#define UPGRADE_PLAYERUSESDEFAULT(isExpansion, player) (isExpansion?(1586+61*(u32)player):(1196+46*(u32)player))

#define TECH_COST_MINERALS(isExpansion) (isExpansion?44:24)
#define TECH_COST_GAS(isExpansion) (isExpansion?132:72)
#define TECH_COST_TIME(isExpansion) (isExpansion?220:120)
#define TECH_COST_ENERGY(isExpansion) (isExpansion?308:168)

#define TECH_AVAILABLEFORPLAYER(isExpansion, player) (isExpansion?(44*(u32)player):(24*(u32)player))
#define TECH_RESERACHEDFORPLAYER(isExpansion, player) (isExpansion?(528+44*(u32)player):(288+24*(u32)player))
#define TECH_DEFAULTAVAILABILITY(isExpansion) (isExpansion?1056:576)
#define TECH_DEFAULTRESERACHED(isExpansion) (isExpansion?1100:600)
#define TECH_PLAYERUSESDEFAULT(isExpansion, player) (isExpansion?(1144+44*(u32)player):(624+24*(u32)player))

#endif