#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"

#include <string>
#include <vector>

/**
	ArgumentMap
	argMap[i] = stdActionArgNum
*/

class TextTrigGenerator
{
	public:
		
		TextTrigGenerator();

		// Places text trigs representative of the given TRIG section in trigString if successful
		bool GenerateTextTrigs(ScenarioPtr map, std::string &trigString);

		// Places text trigs representative of the given trigger in trigString if successful
		bool GenerateTextTrigs(ScenarioPtr map, u32 trigId, std::string &trigString);

		bool LoadScenario(ScenarioPtr map); // Loads data about the given scenario for use outside text trigs
		void ClearScenario(); // Clears loaded scenario data

		std::string GetConditionName(u8 CID);
		std::string GetConditionArgument(Condition& condition, u8 stdTextTrigArgNum);
		std::string GetConditionArgument(Condition& condition, u8 argNum, std::vector<u8> &argMap);
		std::string GetActionArgument(Action& action, u8 stdTextTrigArgNum);
		std::string GetActionArgument(Action& action, u8 argNum, std::vector<u8> &argMap);

		std::string GetTrigLocation(u32 locationNum);
		std::string GetTrigString(u32 stringNum);
		std::string GetTrigPlayer(u32 groupNum);
		std::string GetTrigUnit(u16 unitId);
		std::string GetTrigSwitch(u32 switchNum);
		std::string GetTrigScoreType(u16 scoreType);
		std::string GetTrigResourceType(u16 resourceType);
		std::string GetTrigOrder(u8 order);
		std::string GetTrigStateModifier(u8 stateModifier);
		std::string GetTrigSwitchState(u8 switchState);
		std::string GetTrigSwitchModifier(u8 switchModifier);
		std::string GetTrigAllyState(u16 allyState);
		std::string GetTrigNumericComparison(u8 numericComparison);
		std::string GetTrigNumericModifier(u8 numericModifier);
		std::string GetTrigScript(u32 scriptNum);
		std::string GetTrigNumUnits(u8 numUnits);
		std::string GetTrigNumber(u32 number);
		std::string GetTrigTextFlags(u8 textFlags);
	
	protected:

		inline void AddConditionArgument(buffer &output, Condition &condition, u8 &CID, u8 &stdTextTrigArgNum);
		inline void AddActionArgument(buffer &output, Action &action, u8 &AID, u8 &stdTextTrigArgNum);
		bool GenerateTextTrigs(ScenarioPtr map, buffer &triggers, std::string &trigString);
		bool LoadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames);
		bool CorrectLineEndings(buffer& buf); // Corrects any improperly formatted line endings


	private:

		std::vector<std::string> stringTable; // Array list of map strings
		std::vector<std::string> extendedStringTable; // Array list of extended map strings
		std::vector<std::string> locationTable; // Array of map locations
		std::vector<std::string> unitTable; // Array of map units
		std::vector<std::string> switchTable; // Array of map switches
		std::vector<std::string> wavTable; // Array of map wavs
		std::vector<std::string> groupTable; // Array of map groups
		std::vector<std::string> conditionTable; // Array of condition names
		std::vector<std::string> actionTable; // Array of action names
		bool goodConditionTable;
		bool goodActionTable;
		char number[36];

		bool PrepConditionTable(); // Fills conditionTable
		bool PrepActionTable(); // Fills actionTable
		bool PrepLocationTable(ScenarioPtr map, bool quoteArgs); // Fills locationTable
		bool PrepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames); // Fills unitTable
		bool PrepSwitchTable(ScenarioPtr map, bool quoteArgs); // Fills switchTable
		bool PrepWavTable(ScenarioPtr map, bool quoteArgs); // Fills wavTable
		bool PrepGroupTable(ScenarioPtr map, bool quoteArgs); // Fills groupTable
		bool PrepStringTable(ScenarioPtr map, bool quoteArgs); // Fills stringTable
};

#endif