#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "Basics.h"
#include "Scenario.h"
#include "StringBuffer.h"
#include <vector>
#include <string>
#include <map>

using namespace BufferedStream;

class TextTrigGenerator
{
    public:
        
        TextTrigGenerator(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigGenerator();

        // Places text trigs representative of the given TRIG section in trigString if successful
        bool generateTextTrigs(ScenarioPtr map, std::string & trigString);

        // Places text trigs representative of the given trigger in trigString if successful
        bool generateTextTrigs(ScenarioPtr map, size_t trigIndex, std::string & trigString);
        
        bool loadScenario(ScenarioPtr map); // Loads data about the given scenario for use outside text trigs

        void clearScenario(); // Clears loaded scenario data

        std::string getConditionName(Chk::Condition::Type conditionType);
        std::string getConditionArgument(Chk::Condition & condition, size_t textArgumentIndex);
        std::string getConditionArgument(Chk::Condition & condition, Chk::Condition::Argument argument);
        std::string getActionName(Chk::Action::Type actionType);
        std::string getActionArgument(Chk::Action & action, size_t textArgumentIndex);
        std::string getActionArgument(Chk::Action & action, Chk::Action::Argument argument);

        ChkdString getTrigLocation(size_t locationId);
        ChkdString getTrigString(size_t stringId);
        ChkdString getTrigWav(size_t stringId);
        ChkdString getTrigPlayer(size_t playerIndex);
        ChkdString getTrigUnit(Sc::Unit::Type unitType);
        ChkdString getTrigSwitch(size_t switchIndex);
        std::string getTrigScoreType(Chk::Trigger::ScoreType scoreType);
        std::string getTrigResourceType(Chk::Trigger::ResourceType resourceType);
        std::string getTrigOrder(Chk::Action::Order order);
        std::string getTrigStateModifier(Chk::Trigger::ValueModifier stateModifier);
        std::string getTrigSwitchState(Chk::Trigger::ValueModifier switchState);
        std::string getTrigSwitchModifier(Chk::Trigger::ValueModifier switchModifier);
        std::string getTrigAllyState(Chk::Action::AllianceStatus allyState);
        std::string getTrigNumericComparison(Chk::Condition::Comparison numericComparison);
        std::string getTrigNumericModifier(Chk::Trigger::ValueModifier numericModifier);
        std::string getTrigScript(Sc::Ai::ScriptId scriptId);
        std::string getTrigNumUnits(Chk::Action::NumUnits numUnits);
        std::string getTrigNumber(u32 number);
        std::string getTrigTextFlags(Chk::Action::Flags textFlags);
    
    protected:

        bool loadScenario(ScenarioPtr map, bool quoteArgs, bool useCustomNames);
        bool correctLineEndings(StringBuffer & buf); // Corrects any improperly formatted line endings
        
        bool buildTextTrigs(ScenarioPtr scenario, std::string & trigString);
        bool buildTextTrig(Chk::Trigger & trigger, std::string & trigString);
        inline void appendTriggers(StringBuffer & output, ScenarioPtr scenario);
        inline void appendTrigger(StringBuffer & output, Chk::Trigger & trigger);
        inline void appendConditionArgument(StringBuffer & output, Chk::Condition & condition, Chk::Condition::Argument argument);
        inline void appendActionArgument(StringBuffer & output, Chk::Action & action, Chk::Action::Argument argument);

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
        template<typename T> inline void appendNumber(StringBuffer & output, const T & number)
        {
            if constexpr ( std::is_enum<T>::value )
                output.appendNumber((typename promote_char<typename std::underlying_type<T>::type>::type)number);
            else
                output.appendNumber((typename promote_char<T>::type)number);
        }
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

        bool prepConditionTable(); // Fills conditionTable
        bool prepActionTable(); // Fills actionTable
        bool prepLocationTable(ScenarioPtr map, bool quoteArgs); // Fills locationTable
        bool prepUnitTable(ScenarioPtr map, bool quoteArgs, bool useCustomNames); // Fills unitTable
        bool prepSwitchTable(ScenarioPtr map, bool quoteArgs); // Fills switchTable
        bool prepGroupTable(ScenarioPtr map, bool quoteArgs); // Fills groupTable
        bool prepScriptTable(ScenarioPtr map, bool quoteArgs); // Fills scriptTable
        bool prepStringTable(ScenarioPtr map, bool quoteArgs); // Fills stringTable
};

#endif