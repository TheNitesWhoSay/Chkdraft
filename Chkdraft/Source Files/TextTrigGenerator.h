#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
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
		bool GenerateTextTrigs(ScenarioPtr map, string &trigString);

		// Places text trigs representative of the given trigger in trigString if successful
		bool GenerateTextTrigs(ScenarioPtr map, u32 trigId, string &trigString);

		bool LoadScenario(ScenarioPtr map); // Loads data about the given scenario for use outside text trigs
		void ClearScenario(); // Clears loaded scenario data

		string GetConditionName(u8 CID);
		string GetConditionArgument(Condition& condition, u8 stdTextTrigArgNum);
		string GetConditionArgument(Condition& condition, u8 argNum, std::vector<u8> &argMap);
		string GetActionArgument(Action& action, u8 stdTextTrigArgNum);
		string GetActionArgument(Action& action, u8 argNum, std::vector<u8> &argMap);

		ChkdString GetTrigLocation(u32 locationNum);
		ChkdString GetTrigString(u32 stringNum);
		ChkdString GetTrigPlayer(u32 groupNum);
		ChkdString GetTrigUnit(u16 unitId);
		ChkdString GetTrigSwitch(u32 switchNum);
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

		inline void AddConditionArgument(buffer &output, Condition &condition, u8 &CID, u8 &stdTextTrigArgNum);
		inline void AddActionArgument(buffer &output, Action &action, u8 &AID, u8 &stdTextTrigArgNum);
		bool GenerateTextTrigs(ScenarioPtr map, buffer &triggers, string &trigString);
		bool LoadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames);
		bool CorrectLineEndings(buffer& buf); // Corrects any improperly formatted line endings


	private:

		std::vector<ChkdString> stringTable; // Array list of map strings
		std::vector<ChkdString> extendedStringTable; // Array list of extended map strings
		std::vector<ChkdString> locationTable; // Array of map locations
		std::vector<ChkdString> unitTable; // Array of map units
		std::vector<ChkdString> switchTable; // Array of map switches
		std::vector<ChkdString> wavTable; // Array of map wavs
		std::vector<ChkdString> groupTable; // Array of map groups
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