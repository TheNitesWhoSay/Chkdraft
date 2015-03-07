#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include <vector>

class TextTrigGenerator
{
	public:
		
		TextTrigGenerator();

		// Places text trigs representative of the given TRIG section in trigString if successful
		bool GenerateTextTrigs(Scenario* map, string &trigString);

		// Places text trigs representative of the given trigger in trigString if successful
		bool GenerateTextTrigs(Scenario* map, u32 trigId, string &trigString);

		bool LoadScenario(Scenario* map); // Loads data about the given scenario for use outside text trigs
		void ClearScenario(); // Clears loaded scenario data

		string GetTrigLocation(u32 locationNum);
		string GetTrigString(u32 stringNum);
		string GetTrigPlayer(u32 groupNum);
		string GetTrigUnit(u16 unitId);
		string GetTrigSwitch(u32 switchNum);
		string GetTrigScoreType(u16 scoreType);
		string GetTrigResourceType(u16 resourceType);
		string GetTrigOrder(u8 order);
		string GetTrigStateModifier(u8 stateModifier);
		string GetTrigSwitchState(u8 switchState);
		string GetTrigSwitchModifier(u8 switchModifier);
		string GetTrigAllyState(u16 allyState);
		string GetTrigNumericComparison(u8 numericComparison);
		string GetTrigNumericModifier(u8 numericModifier);
		string GetTrigScript(u32 scriptNum);
		string GetTrigNumUnits(u8 numUnits);
		string GetTrigNumber(u32 number);
		string GetTrigTextFlags(u8 textFlags);
	
	protected:

		bool GenerateTextTrigs(Scenario* map, buffer &triggers, string &trigString);
		bool LoadScenario(Scenario* map, bool quoteArgs, bool useCustomNames);
		bool CorrectLineEndings(buffer& buf); // Corrects any improperly formatted line endings


	private:

		std::vector<string> stringTable; // Array list of map strings
		std::vector<string> extendedStringTable; // Array list of extended map strings
		std::vector<string> locationTable; // Array of map locations
		std::vector<string> unitTable; // Array of map units
		std::vector<string> switchTable; // Array of map switches
		std::vector<string> wavTable; // Array of map wavs
		std::vector<string> groupTable; // Array of map groups
		std::vector<string> conditionTable; // Array of condition names
		std::vector<string> actionTable; // Array of action names
		bool goodConditionTable;
		bool goodActionTable;

		bool PrepConditionTable(); // Fills conditionTable
		bool PrepActionTable(); // Fills actionTable
		bool PrepLocationTable(Scenario* map, bool quoteArgs); // Fills locationTable
		bool PrepUnitTable(Scenario* map, bool quoteArgs, bool useCustomNames); // Fills unitTable
		bool PrepSwitchTable(Scenario* map, bool quoteArgs); // Fills switchTable
		bool PrepWavTable(Scenario* map, bool quoteArgs); // Fills wavTable
		bool PrepGroupTable(Scenario* map, bool quoteArgs); // Fills groupTable
		bool PrepStringTable(Scenario* map, bool quoteArgs); // Fills stringTable
};

#endif