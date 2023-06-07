#ifndef TEXTTRIGGENERATOR_H
#define TEXTTRIGGENERATOR_H
#include "../RareCpp/include/rarecpp/string_buffer.h"
#include "Basics.h"
#include "Scenario.h"
#include <vector>
#include <string>
#include <map>

using RareBufferedStream::StringBuffer;

class TextTrigGenerator
{
    public:
        
        TextTrigGenerator(bool useAddressesForMemory, u32 deathTableOffset);
        virtual ~TextTrigGenerator();

        // Places text trigs representative of the given TRIG section in trigString if successful
        bool generateTextTrigs(const Scenario & map, std::string & trigString);

        // Places text trigs representative of the given trigger in trigString if successful
        bool generateTextTrigs(const Scenario & map, size_t trigIndex, std::string & trigString);
        
        bool loadScenario(const Scenario & map); // Loads data about the given scenario for use outside text trigs

        void clearScenario(); // Clears loaded scenario data

        std::string getConditionName(Chk::Condition::Type conditionType) const;
        std::string getConditionArgument(const Chk::Condition & condition, size_t textArgumentIndex) const;
        std::string getConditionArgument(const Chk::Condition & condition, Chk::Condition::Argument argument) const;
        std::string getActionName(Chk::Action::Type actionType) const;
        std::string getActionArgument(const Chk::Action & action, size_t textArgumentIndex) const;
        std::string getActionArgument(const Chk::Action & action, Chk::Action::Argument argument) const;

        ChkdString getTrigLocation(size_t locationId) const;
        ChkdString getTrigString(size_t stringId) const;
        ChkdString getTrigSound(size_t stringId) const;
        ChkdString getTrigPlayer(size_t playerIndex) const;
        ChkdString getTrigUnit(Sc::Unit::Type unitType) const;
        ChkdString getTrigSwitch(size_t switchIndex) const;
        std::string getTrigScoreType(Chk::Trigger::ScoreType scoreType) const;
        std::string getTrigResourceType(Chk::Trigger::ResourceType resourceType) const;
        std::string getTrigOrder(Chk::Action::Order order) const;
        std::string getTrigStateModifier(Chk::Trigger::ValueModifier stateModifier) const;
        std::string getTrigSwitchState(Chk::Trigger::ValueModifier switchState) const;
        std::string getTrigSwitchModifier(Chk::Trigger::ValueModifier switchModifier) const;
        std::string getTrigAllyState(Chk::Action::AllianceStatus allyState) const;
        std::string getTrigNumericComparison(Chk::Condition::Comparison numericComparison) const;
        std::string getTrigNumericModifier(Chk::Trigger::ValueModifier numericModifier) const;
        std::string getTrigScript(Sc::Ai::ScriptId scriptId) const;
        std::string getTrigNumUnits(Chk::Action::NumUnits numUnits) const;
        std::string getTrigNumber(u32 number) const;
        std::string getTrigTextFlags(Chk::Action::Flags textFlags) const;
    
    protected:

        bool loadScenario(const Scenario & map, bool quoteArgs, bool useCustomNames);
        bool correctLineEndings(StringBuffer & buf) const; // Corrects any improperly formatted line endings
        
        bool buildTextTrigs(const Scenario & scenario, std::string & trigString);
        bool buildTextTrig(const Chk::Trigger & trigger, std::string & trigString);
        inline void appendTriggers(StringBuffer & output, const Scenario & scenario) const;
        inline void appendTrigger(StringBuffer & output, const Chk::Trigger & trigger) const;
        inline void appendConditionArgument(StringBuffer & output, const Chk::Condition & condition, Chk::Condition::Argument argument) const;
        inline void appendActionArgument(StringBuffer & output, const Chk::Action & action, Chk::Action::Argument argument) const;

        inline void appendLocation(StringBuffer & output, const size_t & locationId) const;
        inline void appendString(StringBuffer & output, const size_t & stringId) const;
        inline void appendSound(StringBuffer & output, const size_t & stringId) const;
        inline void appendPlayer(StringBuffer & output, const size_t & playerIndex) const;
        inline void appendAmount(StringBuffer & output, const u32 & amount) const;
        inline void appendUnit(StringBuffer & output, const Sc::Unit::Type & unitType) const;
        inline void appendSwitch(StringBuffer & output, const size_t & switchIndex) const;
        inline void appendScoreType(StringBuffer & output, const Chk::Trigger::ScoreType & scoreType) const;
        inline void appendResourceType(StringBuffer & output, const Chk::Trigger::ResourceType & resourceType) const;
        inline void appendOrder(StringBuffer & output, const Chk::Action::Order & order) const;
        inline void appendStateModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & stateModifier) const;
        inline void appendSwitchState(StringBuffer & output, const Chk::Trigger::ValueModifier & switchState) const;
        inline void appendSwitchModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & switchModifier) const;
        inline void appendAllyState(StringBuffer & output, const Chk::Action::AllianceStatus & allyState) const;
        inline void appendNumericComparison(StringBuffer & output, const Chk::Condition::Comparison & numericComparison) const;
        inline void appendNumericModifier(StringBuffer & output, const Chk::Trigger::ValueModifier & numericModifier) const;
        inline void appendScript(StringBuffer & output, const Sc::Ai::ScriptId & scriptId) const;
        inline void appendNumUnits(StringBuffer & output, const Chk::Action::NumUnits & numUnits) const;
        template<typename T> inline void appendNumber(StringBuffer & output, const T & number) const
        {
            if constexpr ( std::is_enum<T>::value )
                output.appendNumber((typename RareTs::promote_char_t<typename std::underlying_type_t<T>>)number);
            else
                output.appendNumber((typename RareTs::promote_char_t<T>)number);
        }
        inline void appendConditionMaskFlag(StringBuffer & output, const Chk::Condition::MaskFlag & maskFlag) const;
        inline void appendActionMaskFlag(StringBuffer & output, const Chk::Action::MaskFlag & maskFlag) const;
        inline void appendMemory(StringBuffer & output, const u32 & memory) const;
        inline void appendTextFlags(StringBuffer & output, const Chk::Action::Flags & textFlags) const;

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
        bool prepLocationTable(const Scenario & map, bool quoteArgs); // Fills locationTable
        bool prepUnitTable(const Scenario & map, bool quoteArgs, bool useCustomNames); // Fills unitTable
        bool prepSwitchTable(const Scenario & map, bool quoteArgs); // Fills switchTable
        bool prepGroupTable(const Scenario & map, bool quoteArgs); // Fills groupTable
        bool prepScriptTable(const Scenario & map, bool quoteArgs); // Fills scriptTable
        bool prepStringTable(const Scenario & map, bool quoteArgs); // Fills stringTable
};

#endif