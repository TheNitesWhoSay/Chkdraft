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
        bool GenerateTextTrigs(ScenarioPtr map, size_t trigIndex, std::string & trigString);

        bool LoadScenario(ScenarioPtr map); // Loads data about the given scenario for use outside text trigs
        void ClearScenario(); // Clears loaded scenario data

        std::string GetConditionName(Chk::Condition::Type conditionType);
        std::string GetConditionArgument(Chk::Condition & condition, size_t textArgumentIndex);
        std::string GetConditionArgument(Chk::Condition & condition, Chk::Condition::Argument argument);
        std::string GetActionName(Chk::Action::Type actionType);
        std::string GetActionArgument(Chk::Action & action, size_t textArgumentIndex);
        std::string GetActionArgument(Chk::Action & action, Chk::Action::Argument argument);

        ChkdString GetTrigLocation(size_t locationId);
        ChkdString GetTrigString(size_t stringId);
        ChkdString GetTrigWav(size_t stringId);
        ChkdString GetTrigPlayer(size_t playerIndex);
        ChkdString GetTrigUnit(Sc::Unit::Type unitType);
        ChkdString GetTrigSwitch(size_t switchIndex);
        std::string GetTrigScoreType(Chk::Trigger::ScoreType scoreType);
        std::string GetTrigResourceType(Chk::Trigger::ResourceType resourceType);
        std::string GetTrigOrder(Chk::Action::Order order);
        std::string GetTrigStateModifier(Chk::Trigger::ValueModifier stateModifier);
        std::string GetTrigSwitchState(Chk::Trigger::ValueModifier switchState);
        std::string GetTrigSwitchModifier(Chk::Trigger::ValueModifier switchModifier);
        std::string GetTrigAllyState(Chk::Action::AllianceStatus allyState);
        std::string GetTrigNumericComparison(Chk::Condition::Comparison numericComparison);
        std::string GetTrigNumericModifier(Chk::Trigger::ValueModifier numericModifier);
        std::string GetTrigScript(Sc::Ai::ScriptId scriptId);
        std::string GetTrigNumUnits(Chk::Action::NumUnits numUnits);
        std::string GetTrigNumber(u32 number);
        std::string GetTrigTextFlags(Chk::Action::Flags textFlags);
    
    protected:

        inline void AddConditionArgument(StringBuffer & output, Chk::Condition & condition, Chk::Condition::Argument argument);
        inline void AddActionArgument(StringBuffer & output, Chk::Action & action, Chk::Action::Argument argument);
        bool BuildTextTrigs(ScenarioPtr map, TrigSectionPtr trigData, std::string & trigString);
        bool LoadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames);
        bool CorrectLineEndings(StringBuffer & buf); // Corrects any improperly formatted line endings

        inline void appendLocation(StringBuffer & output, const size_t & locationId);
        inline void appendString(StringBuffer & output, const size_t & stringId);
        inline void appendSound(StringBuffer & output, const size_t & stringId);
        inline void appendPlayer(StringBuffer & output, const size_t & playerIndex);
        inline void appendAmount(StringBuffer & output, const u32 & amount);
        inline void appendUnit(StringBuffer & output, const Sc::Unit::Type & unitType);
        inline void appendSwitch(StringBuffer & output, const size_t & switchIndex);
        inline void appendScoreType(StringBuffer & output, const Chk::Trigger::ScoreType & scoreType);
        inline void appendResourceType(StringBuffer & output, const Chk::Trigger::ResourceType & resourceType);
        inline void appendOrder(StringBuffer & output, const Chk::Action::Order & order);
        inline void appendStateModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & stateModifier);
        inline void appendSwitchState(StringBuffer & output, const Chk::Trigger::ValueModifier & switchState);
        inline void appendSwitchModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & switchModifier);
        inline void appendAllyState(StringBuffer & output, const Chk::Action::AllianceStatus & allyState);
        inline void appendNumericComparison(StringBuffer & output, const Chk::Condition::Comparison & numericComparison);
        inline void appendNumericModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & numericModifier);
        inline void appendScript(StringBuffer & output, const Sc::Ai::ScriptId & scriptId);
        inline void appendNumUnits(StringBuffer & output, const Chk::Action::NumUnits & numUnits);
        template<typename T> inline void appendNumber(StringBuffer & output, const T & number) { output += number; }
        inline void appendConditionMaskFlag(StringBuffer & output, const Chk::Condition::MaskFlag & maskFlag);
        inline void appendActionMaskFlag(StringBuffer & output, const Chk::Action::MaskFlag & maskFlag);
        inline void appendMemory(StringBuffer & output, const u32 & memory);
        inline void appendTextFlags(StringBuffer & output, const Chk::Action::Flags & textFlags);

    private:

        bool useAddressesForMemory; // If true, uses 1.16.1 addresses for memory conditions and actions
        u32 deathTableOffset;
        std::vector<ChkdString> stringTable; // Array list of map strings
        std::vector<ChkdString> extendedStringTable; // Array list of extended map strings
        std::vector<ChkdString> locationTable; // Array of map locations
        std::vector<ChkdString> unitTable; // Array of map units
        std::vector<ChkdString> switchTable; // Array of map switches
        std::vector<ChkdString> groupTable; // Array of map groups
        std::map<Sc::Ai::ScriptId, std::string> scriptTable; // Array of map scripts
        std::map<Sc::Ai::ScriptId, std::string> scriptDescTable; // Array of script descriptions
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