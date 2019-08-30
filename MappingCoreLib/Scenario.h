#ifndef SCENARIO_H
#define SCENARIO_H
#include "Buffer.h"
#include "Basics.h"
#include "EscapeStrings.h"
#include "CoreStructs.h"
#include "Sections.h"
#include <cstdio>
#include <list>
#include <array>
#include <memory>
#include <string>
#include <map>

class Scenario;
class ScenarioSaver;
typedef std::shared_ptr<ScenarioSaver> ScenarioSaverPtr;
enum class ChkVersion : u32;

class Scenario
{
    public:
/*   Sections   */  Versions versions; // All version and validation related information
                    Strings strings; // All string related information: map title and description, strings used by other sections
                    Players players; // All player related information: slot owners, races, player colors, forces
                    Layers layers; // All layers (stuff with coordinates) - terrain, units, fog of war, sprites, doodads, locations
                    Properties properties; // All property sheets: unit settings, unit availability, upgrade costs, upgrade leveling, technology costs, technology leveling
                    Triggers triggers; // All scenario triggers and mission briefing triggers and their metadata - CUWPs, switches, and sounds

/* Constructors */  Scenario();

/* Destructors  */  virtual ~Scenario();

/*  Quick Pass  */  Scenario* scenario(); // Returns pointer to this scenario, useful for classes that inherit 'Scenario'

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*  Expansion   */  bool isExpansion(); // Check if the map uses expansion settings

/*   Version    */  bool HasVerSection();
                    bool IsScOrig();
                    bool IsHybrid();
                    bool IsScExp();
                    void ChangeToScOrig();
                    void ChangeToHybrid();
                    void ChangeToScExp();
                    bool isVersion(ChkVersion chkVersion) { return false; } // Checks whether all sections are valid for the given ChkVersion

/*  Description */  u16 GetMapTitleStrId();
                    u16 GetMapDescriptionStrId();
                    bool getMapTitle(ChkdString &dest);
                    bool getMapDescription(ChkdString &dest);
                    bool SetMapTitle(ChkdString &newMapTitle);
                    bool SetMapDescription(ChkdString &newMapDescription);


/*   Players    */  bool getPlayerOwner(u8 player, u8& owner); // Gets the current owner of a player
                    bool getPlayerRace(u8 player, u8& race); // Gets the players current race
                    bool getPlayerColor(u8 player, u8& color); // Gets the players current color
                    bool setPlayerOwner(u8 player, u8 newOwner); // Sets a new owner for a player
                    bool setPlayerRace(u8 player, u8 newRace); // Sets a new race for a player
                    bool setPlayerColor(u8 player, u8 newColor); // Sets a new color for a player


/*    Forces    */  bool hasForceSection();
                    u32 getForceStringNum(u8 index); // Gets the string number of the force at the given index
                    bool getForceString(ChkdString &str, u8 forceNum); // Gets the name of the given force num
                    bool getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av); // Attempts to get info for given force num
                    bool getPlayerForce(u8 playerNum, u8 &force); // Attempts to get the force a player belongs to
                    bool setPlayerForce(u8 player, u8 newForce); // Sets a new force for a player
                    bool setForceAllied(u8 forceNum, bool allied); // Specifies whether a force's players are allied
                    bool setForceVision(u8 forceNum, bool sharedVision); // Specifies whether a force's players have shared vision
                    bool setForceRandom(u8 forceNum, bool randomStartLocs); // Specifies whether a force has randomized start locations
                    bool setForceAv(u8 forceNum, bool alliedVictory); // Specifies whether a force's players have allied victory
                    bool setForceName(u8 forceNum, ChkdString &newName);


/*  Dimensions  */  u16 getWidth(); // Get width of map
                    u16 getHeight(); // Get height of map
                    bool setDimensions(u16 newWidth, u16 newHeight); // Sets new dimensions for the map


/*   Tileset    */  u16 getTileset(); // Gets the maps tileset
                    bool setTileset(u16 newTileset); // Sets a new value for the maps tileset


/*    Tiles     */  u16 getTile(u16 xc, u16 yc);
                    u16 getTileSectionTile(u16 xc, u16 yc);
                    bool getTile(u16 xc, u16 yc, u16 &tileValue);
                    bool getTileSectionTile(u16 xc, u16 yc, u16 &tileValue);
                    bool setTile(u16 xc, u16 yc, u16 value);


/*    Units     */  u16 numUnits(); // Returns number of units in UNIT section
                    //bool getUnit(Chk::Unit* &unitRef, u16 index); // Gets unit at index
                    bool addUnit(u16 unitID, u8 owner, u16 xc, u16 yc, u16 stateFlags); // Attempts to create a unit
                    bool getUnitName(RawString &dest, u16 unitID);
                    bool getUnitName(ChkdString &dest, u16 unitID);
                    bool deleteUnit(u16 index);
                    u32 GetUnitFieldData(u16 unitIndex, Chk::Unit::Field field);
                    bool SetUnitFieldData(u16 unitIndex, Chk::Unit::Field field, u32 data);
                    bool SetUnitHitpoints(u16 unitIndex, u8 newHitpoints);
                    bool SetUnitEnergy(u16 unitIndex, u8 newEnergy);
                    bool SetUnitShields(u16 unitIndex, u8 newShields);
                    bool SetUnitResources(u16 unitIndex, u32 newResources);
                    bool SetUnitHanger(u16 unitIndex, u16 newHanger);
                    bool SetUnitTypeId(u16 unitIndex, u16 newTypeId);
                    bool SetUnitXc(u16 unitIndex, u16 newXc);
                    bool SetUnitYc(u16 unitIndex, u16 newYc);


/*   Sprites    */  u16 SpriteSectionCapacity();
                    bool GetSpriteRef(Chk::Sprite* &spriteRef, u16 index);
                    

/*  Locations   */  bool HasLocationSection();
                    u32 GetLocationFieldData(u16 locationIndex, Chk::Location::Field field);
                    bool SetLocationFieldData(u16 locationIndex, Chk::Location::Field field, u32 data);
                    bool insertLocation(u16 index, Chk::Location &location, RawString &name);
                    bool insertLocation(u16 index, Chk::Location &location, ChkdString &name);
                    u8 numLocations(); // Gets the current amount of used locations in the map
                    u16 locationCapacity(); // Gets the map's current location capacity
                    bool locationIsUsed(u16 locationIndex);
                    bool createLocation(s32 xc1, s32 yc1, s32 xc2, s32 yc2, u16& locationIndex); // Create a location
                    void deleteLocation(u16 locationIndex); // Unconditionally deletes a location
                    bool stringUsedWithLocs(u32 stringNum); // Returns whether the string is used for a location


/*   Triggers   */  bool HasTrigSection();
                    bool AddOrReplaceTrigSection(buffer & newTrigSection);
                    u32 numTriggers(); // Returns the number of triggers in this scenario
                    bool getActiveComment(Chk::Trigger* trigger, RawString &comment) { return false; }
                    bool addTrigger(Chk::Trigger &trigger) { return false; }
                    bool insertTrigger(u32 triggerId, Chk::Trigger &trigger) { return false; }
                    bool deleteTrigger(u32 triggerId);
                    bool copyTrigger(u32 triggerId);
                    bool moveTriggerUp(u32 triggerId); // Moves the given trigger up one absolute index
                    bool moveTriggerDown(u32 triggerId); // Moves the given trigger down one absolute index
                    bool moveTrigger(u32 triggerId, u32 destId); // Moves the given trigger to the destination index
                    TrigSectionPtr GetTrigSection();
                    

/*     CUWPs    */  bool GetCuwp(u8 cuwpIndex, Chk::Cuwp &outPropStruct);
                    bool AddCuwp(Chk::Cuwp &propStruct, u8 &outCuwpIndex);
                    bool ReplaceCuwp(Chk::Cuwp &propStruct, u8 cuwpIndex);
                    bool IsCuwpUsed(u8 cuwpIndex);
                    bool SetCuwpUsed(u8 cuwpIndex, bool isUsed);
                    bool FindCuwp(const Chk::Cuwp &cuwpToFind, u8 &outCuwpIndex);
                    int CuwpCapacity();
                    int NumUsedCuwps();

/*     WAVs     */  u32 WavSectionCapacity();
                    virtual bool GetWav(u16 wavIndex, u32 &outStringId);
                    bool GetWavString(u16 wavIndex, RawString &outString);
                    bool AddWav(u32 stringId);
                    bool AddWav(const RawString &wavMpqPath);
                    virtual bool RemoveWavByWavIndex(u16 wavIndex, bool removeIfUsed);
                    virtual bool RemoveWavByStringId(u32 stringId, bool removeIfUsed);
                    bool WavHasString(u16 wavIndex);
                    bool IsStringUsedWithWavs(u32 stringId);
                    bool GetWavs(std::map<u32/*stringId*/, u16/*wavIndex*/> &wavMap, bool includePureStringWavs);
                    bool MapUsesWavString(u32 wavStringId);
                    void ZeroWavUsers(u32 wavStringId);

/*   Switches   */  u32 NumNameableSwitches();
                    bool getSwitchStrId(u8 switchId, u32 &stringNum);
                    bool getSwitchName(ChkdString &dest, u8 switchID);
                    bool setSwitchName(ChkdString &newName, u8 switchId, bool extended);
                    bool hasSwitchSection();
                    bool switchHasName(u8 switchId);
                    void removeSwitchName(u8 switchId);
                    bool SwitchUsesExtendedName(u8 switchId);
                    bool ToggleUseExtendedSwitchName(u8 switchId);

/*   Briefing   */  bool getBriefingTrigger(Chk::Trigger* &trigRef, u32 index); // Gets the briefing trigger at index


/* UnitSettings */  bool ReplaceUNISSection(buffer &newUNISSection);
                    bool ReplaceUNIxSection(buffer &newUNIxSection);
                    bool ReplacePUNISection(buffer &newPUNISection);
                    bool getUnitStringNum(u16 unitId, u16 &stringNum);
                    bool unitUsesDefaultSettings(u16 unitId);
                    bool unitIsEnabled(u16 unitId);
                    UnitEnabledState getUnitEnabledState(u16 unitId, u8 player);
                    bool hasUnitSettingsSection();


                    bool getUnitSettingsHitpoints(u16 unitId, u32 &hitpoints);
                    bool getUnitSettingsHitpointByte(u16 unitId, u8 &hitpointByte);
                    bool getUnitSettingsShieldPoints(u16 unitId, u16 &shieldPoints);
                    bool getUnitSettingsArmor(u16 unitId, u8 &armor);
                    bool getUnitSettingsBuildTime(u16 unitId, u16 &buildTime);
                    bool getUnitSettingsMineralCost(u16 unitId, u16 &mineralCost);
                    bool getUnitSettingsGasCost(u16 unitId, u16 &gasCost);
                    bool getUnitSettingsBaseWeapon(u32 weaponId, u16 &baseDamage);
                    bool getUnitSettingsBonusWeapon(u32 weaponId, u16 &bonusDamage);

                    bool getUnisStringId(u16 unitId, u16 &stringId);
                    bool getUnixStringId(u16 unitId, u16 &stringId);
                    bool setUnisStringId(u16 unitId, u16 newStringId);
                    bool setUnixStringId(u16 unitId, u16 newStringId);

                    bool setUnitUseDefaults(u16 unitID, bool useDefaults); // Specifies whether a unit uses default settings
                    bool setUnitEnabled(u16 unitId, bool enabled);
                    bool setUnitEnabledState(u16 unitId, u8 player, UnitEnabledState enabledState);
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
                    bool setUnitName(u16 unitId, RawString &newName);
                    bool setUnitName(u16 unitId, ChkdString &newName);


/* UpgrSettings */  bool ReplaceUPGSSection(buffer &newUPGSSection);
                    bool ReplaceUPGxSection(buffer &newUPGxSection);
                    bool ReplaceUPGRSection(buffer &newUPGRSection);
                    bool ReplacePUPxSection(buffer &newPUPxSection);
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


/* TechSettings */  bool ReplaceTECSSection(buffer &newTECSSection);
                    bool ReplaceTECxSection(buffer &newTECxSection);
                    bool ReplacePTECSection(buffer &newPTECSection);
                    bool ReplacePTExSection(buffer &newPTExSection);
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


/*   Strings    */  u16 strSectionCapacity(); // The current capacity of the STR section
                    u32 kstrSectionCapacity(); // The current capacity of the KSTR section
                    u32 stringCapacity(); // The sum of the capcities of the STR and KSTR section
                    u32 stringCapacity(bool extended); // Returns the capcity of the STR or KSTR section
                    bool hasStrSection(bool extended); // Checks if the STR or KSTR section exists
                    u32 strBytesUsed(); // Returns the amount of bytes used in the STR section

                    bool stringIsUsed(u32 stringNum); // Returns whether the string is used in the map
                    bool isExtendedString(u32 stringNum); // Returns whether the string is an extended string
                    bool stringExists(u32 stringNum); // Checks if a string is associated with this stringNum
                    bool stringExists(const RawString &str, u32& stringNum); // Checks if string exists, returns true and stringNum if so
                    bool stringExists(const ChkdString &str, u32 &stringNum);
                    bool stringExists(const RawString &str, u32& stringNum, bool extended); // Checks if string exists in the same table
                    bool stringExists(const RawString &str, u32& stringNum, std::vector<StringTableNode> &strList);
                    bool FindDifference(ChkdString &str, u32& stringNum); // Checks if there's a difference between str and string at stringNum
                    u32 extendedToRegularStr(u32 stringNum); // Returns string number for extended section

                    bool GetString(RawString &dest, u32 stringNum); // No special formatting
                    bool GetString(EscString &dest, u32 stringNum); // C++ formatting, see MakeEscString
                    bool GetString(ChkdString &dest, u32 stringNum); // Chkd and C++ formatting, see MakeChkdString

                    bool GetExtendedString(RawString &dest, u32 extendedStringSectionIndex); // No special formatting
                    bool GetExtendedString(EscString &dest, u32 extendedStringSectionIndex); // C++ formatting, see MakeEscString
                    bool GetExtendedString(ChkdString &dest, u32 extendedStringSectionIndex); // Chkd and C++ formatting, see MakeChkdString

                    inline void IncrementIfEqual(u32 lhs, u32 rhs, u32 &counter);
                    u32 amountStringUsed(u32 stringNum); // Returns the number of times a string is used
                    void getStringUse(u32 stringNum, u32& locs, u32& trigs, u32& briefs, u32& props, u32& forces, u32& wavs, u32& units, u32& switches);

                    bool removeUnusedString(u32 stringNum); // Delete string if it is no longer used anywhere in the map
                    void forceDeleteString(u32 stringNum); // Unconditionally deletes a string

                    /** Attempt to associate a stringNum with a string
                        Returns true and stringNum if string exists or was created successfully */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool addString(const RawString &str, numType &stringNum, bool extended);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool addString(const ChkdString &str, numType &stringNum, bool extended);

                    /** Attempts to assocate a stringNum with a string that has
                        already been confirmed to not exist in the string section
                        Returns true and stringNum if the string was created successfully */
                    bool addNonExistentString(RawString &str, u32 &stringNum, bool extended, std::vector<StringTableNode> &strList);

                    /** Attempt to replace a string (specific instance of string), if unsuccessful the old string will remain
                            (if it is not used elsewhere) based on safeReplace and memory conditions
                        stringNum: if not 0 and only used once, the map calls editString, otherwise stringNum is set if successful
                        safeReplace: if true the function will only attempt to make room for the new string if the old one will not be
                            lost; if false the fuction will always attempt to make room, and may lose the old string in low-mem
                            conditions
                        Returns true if the string exists already or was successfully created */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool replaceString(RawString &newString, numType &stringNum, bool extended, bool safeReplace);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool replaceString(ChkdString &newString, numType &stringNum, bool extended, bool safeReplace);

                    /** Attempts to edit the contents of a string (universal change to string), if unsucessesful the old string will
                            remain (if it is not used elsewhere) based on safeEdit and memory conditions
                        safeEdit: if true the function will only attempt to make room for the new string if the old one will not be lost
                            if false the function will always attempt to make room, and may lose the old string in low-mem conditions
                        Returns true if the string contents were successfully edited, always false if stringNum is 0 */
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool editString(RawString &newString, numType stringNum, bool extended, bool safeEdit);
                    template <typename numType> // Strings can, and can only be u16 or u32
                    bool editString(ChkdString &newString, numType stringNum, bool extended, bool safeEdit);

                    void replaceStringNum(u32 toReplace, u32 replacement);

                    void removeMetaStrings(); // Attemps to remove all strings not used in-game
                    bool cleanStringTable(bool extendedTable); // Rebuilds string table with current indexs, removes all unused strings
                    bool cleanStringTable(bool extendedTable, std::vector<StringTableNode> &strList); // Cleans string table with given list
                    bool removeDuplicateStrings();
                    bool compressStringTable(bool extendedTable, bool recycleSubStrings); // Rebuilds string table with minimum indexs and smallest size possible
                    bool repairStringTable(bool extendedTable); // Removes what Scmdraft considers string corruption
                    bool addAllUsedStrings(std::vector<StringTableNode>& strList, bool includeStandard, bool includeExtended);
                    bool rebuildStringTable(std::vector<StringTableNode> &strList, bool extendedTable); // Rebuilds string table using the provided list
                    bool buildStringTables(std::vector<StringTableNode> &strList, bool extendedTable,
                        StrSection &offsets, buffer &strPortion, s64 strPortionOffset, u32 numStrs, bool recycleSubStrings) { return false; }


/*  Validation  */  bool GoodVCOD(); // Check if VCOD is valid


/*   Security   */  bool isProtected(); // Checks if map is protected
                    bool hasPassword(); // Checks if the map has a password
                    bool isPassword(std::string &password); // Checks if this is the password the map has
                    bool SetPassword(std::string &oldPass, std::string &newPass); // Attempts to password-protect the 
                    bool Login(std::string &password); // Attempts to login to the map


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*   File IO    */  bool ParseScenario(buffer &chk); // Parses supplied scenario file data
                    bool CreateNew(u16 width, u16 height, Sc::Terrain::Tileset tileset, u32 terrain, u32 triggers);
                    void WriteFile(FILE* pFile); // Writes all sections to the supplied file
                    std::shared_ptr<void> Serialize(); /** Writes all sections to a buffer in memory as it would to a .chk file
                                                     includes a 4 byte "CHK " tag followed by a 4-byte size, followed by data */
                    bool Deserialize(const void* data); // "Opens" a serialized Scenario.chk file, data must be 8+ bytes
                    
                    /** Makes whatever changes are required for saving in the given ChkVersion
                        and writes the scenario to outputStream
                        
                        Returns true upon success, returns false and rolls back any changes made during save on failure */
                    bool Save(ChkVersion chkVersion, std::ostream &outputStream, ScenarioSaverPtr scenarioSaver = nullptr);
                    bool PrepareSaveSectionChanges(ChkVersion chkVersion, std::unordered_map<Section, Section> &sectionsToReplace,
                        std::vector<Section> sectionsToAdd, std::vector<SectionName> sectionsToRemove, ScenarioSaverPtr scenarioSaver = nullptr) { return false; }

/*  Rebuilders  */  void correctMTXM(); // Corrects MTXM of protected maps for view-only purposes

/* Section mgmt */  Section FindLast(SectionName sectionName) { return nullptr; } // Finds the last instance of a section
                    bool RemoveSection(SectionName sectionName) { return false; } // Removes section containing this buffer
                    bool RemoveSection(Section sectionToRemove);
                    bool AddSection(Section sectionToAdd); // Adds a buffer as a new map section
                    Section AddSection(SectionName sectionName) { return nullptr; } // Adds a section with the given id to the map
                    bool AddOrReplaceSection(Section section);
                    void Flush(); // Deletes all sections and clears map protection

/*   Defaults   */  static ScenarioSaverPtr getDefaultScenarioSaver() { return nullptr; }


    private:
                    
                    /** Pointers to sections used by Chkdraft. These are never null, but unless they're in allSections they aren't saved with the map */
/*   Sections   */  TypeSectionPtr TYPE; VerSectionPtr VER; IverSectionPtr IVER; Ive2SectionPtr IVE2; VcodSectionPtr VCOD; IownSectionPtr IOWN;
                    OwnrSectionPtr OWNR; EraSectionPtr ERA; DimSectionPtr DIM; SideSectionPtr SIDE; MtxmSectionPtr MTXM; PuniSectionPtr PUNI;
                    UpgrSectionPtr UPGR; PtecSectionPtr PTEC; UnitSectionPtr UNIT; IsomSectionPtr ISOM; TileSectionPtr TILE; Dd2SectionPtr DD2;
                    Thg2SectionPtr THG2; MaskSectionPtr MASK; StrSectionPtr STR; UprpSectionPtr UPRP; UpusSectionPtr UPUS; MrgnSectionPtr MRGN;
                    TrigSectionPtr TRIG; MbrfSectionPtr MBRF; SprpSectionPtr SPRP; ForcSectionPtr FORC; WavSectionPtr WAV; UnisSectionPtr UNIS;
                    UpgsSectionPtr UPGS; TecsSectionPtr TECS; SwnmSectionPtr SWNM; ColrSectionPtr COLR; PupxSectionPtr PUPx; PtexSectionPtr PTEx;
                    UnixSectionPtr UNIx; UpgxSectionPtr UPGx; TecxSectionPtr TECx;

                    KstrSectionPtr KSTR;

    protected:

        bool ToSingleBuffer(buffer &chk); // Writes the chk to a buffer
        bool ParseSection(buffer &chk, s64 position, s64 &nextPosition, std::vector<Section> &sections);
        void CacheSections(); // Caches all section references for fast access

        bool GetStrInfo(char* &ptr, size_t &length, u32 stringNum); // Gets a pointer to the string and its length if successful
        bool ZeroOutString(u32 stringNum); // returns false if stringNum is not a string in any sense
        bool RepairString(u32& stringNum, bool extended); // Ensures that the string would not be considered corrupted by Scmdraft


    private:

        std::vector<Section> allSections; // Holds all the sections of a map
        std::array<u8, 7> tailData; // The 0-7 bytes just before the Scenario file ends, after the last valid section
        u8 tailLength; // 0 for no tail data, must be less than 8
        bool mapIsProtected; // Flagged if map is protected
        bool caching; // if true, section add/remove/moves currently result in re-caching
        bool jumpCompress; // If true, the map will attempt to compress using jump sections when saving

        /*// Cached regular section pointers
        Section type, ver, iver, ive2, vcod, iown, ownr, era,
            dim, side, mtxm, puni, upgr, ptec, unit, isom,
            tile, dd2, thg2, mask, str, uprp, upus, mrgn,
            trig, mbrf, sprp, forc, wav, unis, upgs, tecs,
            swnm, colr, pupx, ptex, unix, upgx, tecx;

        // Cached extended section pointers
        Section kstr;*/
};
typedef std::shared_ptr<Scenario> ScenarioPtr;

class ScenarioSaver
{
    public:
        virtual bool confirmRemoveLocations(Scenario &chk) = 0;
};

enum class ChkVersion : u32
{
    StarCraft = 0,
    Hybrid = 1,
    BroodWar = 2,
    Unchanged = 3
};

#endif