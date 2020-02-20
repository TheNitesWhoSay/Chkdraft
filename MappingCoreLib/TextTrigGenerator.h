#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Basics.h"
#include "Scenario.h"
#include <vector>
#include <string>
#include <map>

class TextTrigGenerator
{
    public:
        
        TextTrigGenerator(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigGenerator();

        // Places text trigs representative of the given TRIG section in trigString if successful
        bool GenerateTextTrigs(ScenarioPtr map, std::string & trigString);

        // Places text trigs representative of the given trigger in trigString if successful
        bool GenerateTextTrigs(ScenarioPtr map, u32 trigId, std::string & trigString);

        bool LoadScenario(ScenarioPtr map); // Loads data about the given scenario for use outside text trigs
        void ClearScenario(); // Clears loaded scenario data

        std::string GetConditionName(u8 CID);
        std::string GetConditionArgument(Chk::Condition & condition, u8 stdTextTrigArgNum);
        std::string GetConditionArgument(Chk::Condition & condition, Chk::Condition::Argument argument);
        std::string GetActionName(u8 AID);
        std::string GetActionArgument(Chk::Action & action, u8 stdTextTrigArgNum);
        std::string GetActionArgument(Chk::Action & action, Chk::Action::Argument argument);

        ChkdString GetTrigLocation(u32 locationNum);
        ChkdString GetTrigString(u32 stringNum);
        ChkdString GetTrigWav(u32 stringNum);
        ChkdString GetTrigPlayer(u32 groupNum);
        ChkdString GetTrigUnit(u16 unitId);
        ChkdString GetTrigSwitch(u32 switchNum);
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

        inline void AddConditionArgument(StringBuffer & output, Chk::Condition & condition, Chk::Condition::VirtualType conditionType, Chk::Condition::Argument argument);
        inline void AddActionArgument(StringBuffer & output, Chk::Action & action, Chk::Action::VirtualType actionType, Chk::Action::Argument argument);
        bool BuildTextTrigs(ScenarioPtr map, TrigSectionPtr triggerData, std::string & trigString);
        bool LoadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames);
        bool CorrectLineEndings(StringBuffer & buf); // Corrects any improperly formatted line endings


    private:

        bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        u32 deathTableOffset;
        std::vector<ChkdString> stringTable; // Array list of map strings
        std::vector<ChkdString> extendedStringTable; // Array list of extended map strings
        std::vector<ChkdString> locationTable; // Array of map locations
        std::vector<ChkdString> unitTable; // Array of map units
        std::vector<ChkdString> switchTable; // Array of map switches
        std::vector<ChkdString> groupTable; // Array of map groups
        std::map<u32, std::string> scriptTable; // Array of map scripts
        std::map<u32, std::string> scriptDescTable; // Array of script descriptions
        std::vector<std::string> conditionTable; // Array of condition names
        std::vector<std::string> actionTable; // Array of action names
        bool goodConditionTable;
        bool goodActionTable;

        bool PrepConditionTable(); // Fills conditionTable
        bool PrepActionTable(); // Fills actionTable
        bool PrepLocationTable(ScenarioPtr map, bool quoteArgs); // Fills locationTable
        bool PrepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames); // Fills unitTable
        bool PrepSwitchTable(ScenarioPtr map, bool quoteArgs); // Fills switchTable
        bool PrepGroupTable(ScenarioPtr map, bool quoteArgs); // Fills groupTable
        bool PrepScriptTable(ScenarioPtr map, bool quoteArgs); // Fills scriptTable
        bool PrepStringTable(ScenarioPtr map, bool quoteArgs); // Fills stringTable
};

#endif