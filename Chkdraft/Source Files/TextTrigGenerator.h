#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Common Files/CommonFiles.h"
#include "Mapping Core/MappingCore.h"
#include <vector>

class TextTrigGenerator
{
	public:
		
		TextTrigGenerator();

		// Places text trigs representative of the given TRIG section in output if successful
		bool GenerateTextTrigs(Scenario* map, string &trigString);

	
	protected:

		bool LoadScenario(Scenario* map);
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
		bool PrepLocationTable(Scenario* map); // Fills locationTable
		bool PrepUnitTable(Scenario* map); // Fills unitTable
		bool PrepSwitchTable(Scenario* map); // Fills switchTable
		bool PrepWavTable(Scenario* map); // Fills wavTable
		bool PrepGroupTable(Scenario* map); // Fills groupTable
		bool PrepStringTable(Scenario* map); // Fills stringTable
};

#endif