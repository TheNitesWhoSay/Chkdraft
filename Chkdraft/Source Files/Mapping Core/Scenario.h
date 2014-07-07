#ifndef SCENARIO_H
#define SCENARIO_H
#include "Buffer.h"
#include "Basics.h"
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

					bool getTrigger(Trigger* &trigRef, u32 index); // Gets the trigger at index

					bool getBriefingTrigger(Trigger* &trigRef, u32 index); // Gets the briefing trigger at index

					u32 getForceStringNum(u8 index); // Gets the string number of the force at the given index
					string getForceString(u8 forceNum); // Gets the name of the string at the given force num
					bool getForceInfo(u8 forceNum, bool &allied, bool &vision, bool &random, bool &av); // Attempts to get info for given force num
					bool getPlayerForce(u8 playerNum, u8 &force); // Attempts to get the force a player belongs to

					u16 numStrings(); // Returns number of strings in the STR section
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
					u32 extendedToRegularStr(u32 stringNum); // Returns string number for extended section

					bool isExpansion(); // Check if the map uses expansion settings

					buffer& unitSettings(); // Gets settings from UNIS/UNIx based on whether the map is expansion

					bool GoodVCOD(); // Check if VCOD is valid
					bool isProtected(); // Checks if map is protected

					bool getMapTitle(std::string &dest);
					bool getMapDescription(std::string &dest);

					bool getPlayerOwner(u8 player, u8& owner); // Gets the current owner of a player
					bool getPlayerRace(u8 player, u8& race); // Gets the players current race
					bool getPlayerColor(u8 player, u8& color); // Gets the players current color

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

					/** Attempt to replace a string setting stringNum if successful, if unsuccessful the old string will remain
							(if it is not used elsewhere) based on safeReplace and memory conditions
						safeReplace: if true the function will only attempt to make room for the new string if the old one will not be lost
									 if false the fuction will always attempt to make room, and may lose the old string in low-mem conditions
						Returns true if the string exists already or was successfully created */
					template <typename numType>
						bool replaceString(string newString, numType& stringNum, bool extended, bool safeReplace);
					
					bool replaceStringNum(u32 toReplace, u32 replacement);

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

// Useful data locations/constants
#define UNIT_SETTINGS_STRING_IDS 3192 // Where strings begin in the UNIS and UNIX sections

#define FORCE_RANDOMIZE_START_LOCATION	BIT_0
#define FORCE_ALLIED					BIT_1
#define FORCE_ALLIED_VICTORY			BIT_2
#define FORCE_SHARED_VISION				BIT_3

#endif