#include "Chk.h"

Chk::Condition::Argument noCndArg = { Chk::Condition::ArgType::NoType, Chk::Condition::ArgField::NoField };
Chk::Condition::Argument amountCndArg = { Chk::Condition::ArgType::Amount, Chk::Condition::ArgField::Amount };
Chk::Condition::Argument numericComparisonCndArg = { Chk::Condition::ArgType::NumericComparison, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument switchStateCndArg = { Chk::Condition::ArgType::SwitchState, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument resourceCndArg = { Chk::Condition::ArgType::ResourceType, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument scoreCndArg = { Chk::Condition::ArgType::ScoreType, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument switchCndArg = { Chk::Condition::ArgType::Switch, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument playerCndArg = { Chk::Condition::ArgType::Player, Chk::Condition::ArgField::Player };
Chk::Condition::Argument unitCndArg = { Chk::Condition::ArgType::Unit, Chk::Condition::ArgField::UnitType };
Chk::Condition::Argument locationCndArg = { Chk::Condition::ArgType::Location, Chk::Condition::ArgField::LocationId };
Chk::Condition::Argument comparisonCndArg = { Chk::Condition::ArgType::Comparison, Chk::Condition::ArgField::Comparison };
Chk::Condition::Argument conditionTypeCndArg = { Chk::Condition::ArgType::ConditionType, Chk::Condition::ArgField::ConditionType };
Chk::Condition::Argument typeIndexCndArg = { Chk::Condition::ArgType::TypeIndex, Chk::Condition::ArgField::TypeIndex };
Chk::Condition::Argument flagsCndArg = { Chk::Condition::ArgType::Flags, Chk::Condition::ArgField::Flags };
Chk::Condition::Argument maskFlagCndArg = { Chk::Condition::ArgType::MaskFlag, Chk::Condition::ArgField::MaskFlag };
Chk::Condition::Argument memoryOffsetCndArg = { Chk::Condition::ArgType::MemoryOffset, Chk::Condition::ArgField::Player };

Chk::Condition::Argument Chk::Condition::classicArguments[NumConditionTypes][MaxArguments] = {
    /**  0 = No Condition         */ {},
    /**  1 = Countdown Timer ---- */ { numericComparisonCndArg, amountCndArg },
    /**  2 = Command              */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /**  3 = Bring -------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg, locationCndArg},
    /**  4 = Accumulate           */ { playerCndArg, numericComparisonCndArg, amountCndArg, resourceCndArg },
    /**  5 = Kill --------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /**  6 = Command The Most     */ { unitCndArg },
    /**  7 = Command The Most At  */ { unitCndArg, locationCndArg },
    /**  8 = Most Kills           */ { unitCndArg },
    /**  9 = Highest Score ------ */ { scoreCndArg },
    /** 10 = Most Resources       */ { resourceCndArg },
    /** 11 = Switch ------------- */ { switchCndArg, switchStateCndArg },
    /** 12 = Elapsed Time         */ { numericComparisonCndArg, amountCndArg },
    /** 13 = Is Briefing -------- */ {},
    /** 14 = Opponents            */ { playerCndArg, numericComparisonCndArg, amountCndArg },
    /** 15 = Deaths ------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /** 16 = Command The Least    */ { unitCndArg },
    /** 17 = Command The Least At */ { unitCndArg, locationCndArg },
    /** 18 = Least Kills          */ { unitCndArg },
    /** 19 = Lowest Score ------- */ { scoreCndArg },
    /** 20 = Least Resources      */ { resourceCndArg },
    /** 21 = Score -------------- */ { playerCndArg, scoreCndArg, numericComparisonCndArg, amountCndArg },
    /** 22 = Always               */ {},
    /** 23 = Never -------------- */ {}
};

Chk::Condition::Argument Chk::Condition::textArguments[NumConditionTypes][MaxArguments] = {
    /**  0 = No Condition         */ {},
    /**  1 = Countdown Timer ---- */ { numericComparisonCndArg, amountCndArg },
    /**  2 = Command              */ { playerCndArg, unitCndArg, numericComparisonCndArg, amountCndArg },
    /**  3 = Bring -------------- */ { playerCndArg, unitCndArg, locationCndArg, numericComparisonCndArg, amountCndArg },
    /**  4 = Accumulate           */ { playerCndArg, numericComparisonCndArg, amountCndArg, resourceCndArg },
    /**  5 = Kill --------------- */ { playerCndArg, unitCndArg, numericComparisonCndArg, amountCndArg },
    /**  6 = Command The Most     */ { unitCndArg },
    /**  7 = Command The Most At  */ { unitCndArg, locationCndArg },
    /**  8 = Most Kills           */ { unitCndArg },
    /**  9 = Highest Score ------ */ { scoreCndArg },
    /** 10 = Most Resources       */ { resourceCndArg },
    /** 11 = Switch ------------- */ { switchCndArg, switchStateCndArg },
    /** 12 = Elapsed Time         */ { numericComparisonCndArg, amountCndArg },
    /** 13 = Is Briefing -------- */ {},
    /** 14 = Opponents            */ { playerCndArg, numericComparisonCndArg, amountCndArg },
    /** 15 = Deaths ------------- */ { playerCndArg, numericComparisonCndArg, amountCndArg, unitCndArg },
    /** 16 = Command The Least    */ { unitCndArg },
    /** 17 = Command The Least At */ { unitCndArg, locationCndArg },
    /** 18 = Least Kills          */ { unitCndArg },
    /** 19 = Lowest Score ------- */ { scoreCndArg },
    /** 20 = Least Resources      */ { resourceCndArg },
    /** 21 = Score -------------- */ { playerCndArg, scoreCndArg, numericComparisonCndArg, amountCndArg },
    /** 22 = Always               */ {},
    /** 23 = Never -------------- */ {}
};

u8 Chk::Condition::defaultFlags[NumConditionTypes] = {
    /**  0 = No Condition         */ 0,
    /**  1 = Countdown Timer ---- */ 0,
    /**  2 = Command              */ (u8)Flags::UnitTypeUsed,
    /**  3 = Bring -------------- */ (u8)Flags::UnitTypeUsed,
    /**  4 = Accumulate           */ 0,
    /**  5 = Kill --------------- */ (u8)Flags::UnitTypeUsed,
    /**  6 = Command The Most     */ (u8)Flags::UnitTypeUsed,
    /**  7 = Command The Most At  */ (u8)Flags::UnitTypeUsed,
    /**  8 = Most Kills           */ (u8)Flags::UnitTypeUsed,
    /**  9 = Highest Score ------ */ 0,
    /** 10 = Most Resources       */ 0,
    /** 11 = Switch ------------- */ 0,
    /** 12 = Elapsed Time         */ 0,
    /** 13 = Is Briefing -------- */ 0,
    /** 14 = Opponents            */ 0,
    /** 15 = Deaths ------------- */ (u8)Flags::UnitTypeUsed,
    /** 16 = Command The Least    */ (u8)Flags::UnitTypeUsed,
    /** 17 = Command The Least At */ (u8)Flags::UnitTypeUsed,
    /** 18 = Least Kills          */ (u8)Flags::UnitTypeUsed,
    /** 19 = Lowest Score ------- */ 0,
    /** 20 = Least Resources      */ 0,
    /** 21 = Score -------------- */ 0,
    /** 22 = Always               */ 0,
    /** 23 = Never -------------- */ 0
};

std::unordered_map<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition> Chk::Condition::virtualConditions = {
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Custom, { Chk::Condition::VirtualType::Indeterminate, {
            locationCndArg, playerCndArg, amountCndArg, unitCndArg, comparisonCndArg, conditionTypeCndArg, typeIndexCndArg, flagsCndArg, maskFlagCndArg } } ),
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Memory, { Chk::Condition::VirtualType::Deaths, { memoryOffsetCndArg, numericComparisonCndArg, amountCndArg } } )
};

Chk::Action::Argument noArg = { Chk::Action::ArgType::NoType, Chk::Action::ArgField::NoField };
Chk::Action::Argument amountArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Number };
Chk::Action::Argument percentArg = { Chk::Action::ArgType::Percent, Chk::Action::ArgField::Number };
Chk::Action::Argument durationArg = { Chk::Action::ArgType::Duration, Chk::Action::ArgField::Time };
Chk::Action::Argument numUnitsArg = { Chk::Action::ArgType::NumUnits, Chk::Action::ArgField::Type2 };
Chk::Action::Argument numericModArg = { Chk::Action::ArgType::NumericMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument switchModArg = { Chk::Action::ArgType::SwitchMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument stateModArg = { Chk::Action::ArgType::StateMod, Chk::Action::ArgField::Type2 };
Chk::Action::Argument allyStateArg = { Chk::Action::ArgType::AllyState, Chk::Action::ArgField::Type };
Chk::Action::Argument scoreArg = { Chk::Action::ArgType::ScoreType, Chk::Action::ArgField::Type };
Chk::Action::Argument resourceArg = { Chk::Action::ArgType::ResourceType, Chk::Action::ArgField::Type };
Chk::Action::Argument scriptArg = { Chk::Action::ArgType::Script, Chk::Action::ArgField::Number };
Chk::Action::Argument textFlagsArg = { Chk::Action::ArgType::TextFlags, Chk::Action::ArgField::Flags };
Chk::Action::Argument orderArg = { Chk::Action::ArgType::Order, Chk::Action::ArgField::Type2 };
Chk::Action::Argument unitArg = { Chk::Action::ArgType::Unit, Chk::Action::ArgField::Type };
Chk::Action::Argument locationArg = { Chk::Action::ArgType::Location, Chk::Action::ArgField::LocationId }; // Only location/dest in Move Location/source in Order and Move Unit
Chk::Action::Argument playerArg = { Chk::Action::ArgType::Player, Chk::Action::ArgField::Group };
Chk::Action::Argument destPlayerArg = { Chk::Action::ArgType::Player, Chk::Action::ArgField::Number };
Chk::Action::Argument secondaryLocationArg = { Chk::Action::ArgType::Location, Chk::Action::ArgField::Number }; // Source in Move Location/dest in Order and Move Unit
Chk::Action::Argument cuwpArg = { Chk::Action::ArgType::CUWP, Chk::Action::ArgField::Number };
Chk::Action::Argument switchArg = { Chk::Action::ArgType::Switch, Chk::Action::ArgField::Number };
Chk::Action::Argument stringArg = { Chk::Action::ArgType::String, Chk::Action::ArgField::StringId };
Chk::Action::Argument soundArg = { Chk::Action::ArgType::Sound, Chk::Action::ArgField::SoundStringId };
Chk::Action::Argument memoryOffsetArg = { Chk::Action::ArgType::MemoryOffset, Chk::Action::ArgField::Group };
Chk::Action::Argument numberArg = { Chk::Action::ArgType::Number, Chk::Action::ArgField::Number };
Chk::Action::Argument typeIndexArg = { Chk::Action::ArgType::TypeIndex, Chk::Action::ArgField::Type };
Chk::Action::Argument actionTypeArg = { Chk::Action::ArgType::ActionType, Chk::Action::ArgField::ActionType };
Chk::Action::Argument secondaryTypeIndexArg = { Chk::Action::ArgType::SecondaryTypeIndex, Chk::Action::ArgField::Type2 };
Chk::Action::Argument flagsArg = { Chk::Action::ArgType::Flags, Chk::Action::ArgField::Flags };
Chk::Action::Argument paddingArg = { Chk::Action::ArgType::Padding, Chk::Action::ArgField::Padding };
Chk::Action::Argument maskFlagArg = { Chk::Action::ArgType::MaskFlag, Chk::Action::ArgField::MaskFlag };

Chk::Action::Argument briefingSlotArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Group };

Chk::Action::Argument Chk::Action::classicArguments[NumActionTypes][MaxArguments] = {
    /**  0 = No Action                              */ {},
    /**  1 = Victory ------------------------------ */ {},
    /**  2 = Defeat                                 */ {},
    /**  3 = Preserve Trigger --------------------- */ {},
    /**  4 = Wait                                   */ { durationArg },
    /**  5 = Pause Game --------------------------- */ {},
    /**  6 = Unpause Game                           */ {},
    /**  7 = Transmission ------------------------- */ { unitArg, locationArg, soundArg, numericModArg, amountArg, stringArg },
    /**  8 = Play Sound                             */ { soundArg },
    /**  9 = Display Text Message ----------------- */ { stringArg },
    /** 10 = Center View                            */ { locationArg },
    /** 11 = Create Unit with Properties ---------- */ { numUnitsArg, unitArg, locationArg, playerArg, cuwpArg },
    /** 12 = Set Mission Objectives                 */ { stringArg },
    /** 13 = Set Switch --------------------------- */ { switchModArg, switchArg },
    /** 14 = Set Countdown Timer                    */ { numericModArg, durationArg },
    /** 15 = Run AI Script ------------------------ */ { scriptArg },
    /** 16 = Run AI Script At Location              */ { scriptArg, locationArg },
    /** 17 = Leader Board (Control) --------------- */ { unitArg, stringArg },
    /** 18 = Leader Board (Control At Location) --- */ { unitArg, locationArg, stringArg },
    /** 19 = Leader Board (Resources)               */ { resourceArg, stringArg },
    /** 20 = Leader Board (Kills) ----------------- */ { unitArg, stringArg },
    /** 21 = Leader Board (Points)                  */ { scoreArg, stringArg },
    /** 22 = Kill Unit ---------------------------- */ { unitArg, playerArg },
    /** 23 = Kill Unit At Location                  */ { numUnitsArg, unitArg, playerArg, locationArg },
    /** 24 = Remove Unit -------------------------- */ { unitArg, playerArg },
    /** 25 = Remove Unit At Location                */ { numUnitsArg, unitArg, playerArg, locationArg },
    /** 26 = Set Resources ------------------------ */ { playerArg, numericModArg, amountArg, resourceArg },
    /** 27 = Set Score                              */ { playerArg, numericModArg, amountArg, scoreArg },
    /** 28 = Minimap Ping ------------------------- */ { locationArg },
    /** 29 = Talking Portrait                       */ { unitArg, durationArg },
    /** 30 = Mute Unit Speech --------------------- */ {},
    /** 31 = Unmute Unit Speech                     */ {},
    /** 32 = Leaderboard Computer Players --------- */ { stateModArg },
    /** 33 = Leaderboard Goal (Control)             */ { amountArg, unitArg, stringArg },
    /** 34 = Leaderboard Goal (Control At Location) */ { amountArg, unitArg, locationArg, stringArg },
    /** 35 = Leaderboard Goal (Resources)           */ { amountArg, resourceArg, stringArg },
    /** 36 = Leaderboard Goal (Kills) ------------- */ { amountArg, unitArg, stringArg },
    /** 37 = Leaderboard Goal (Points)              */ { amountArg, scoreArg, stringArg },
    /** 38 = Move Location ------------------------ */ { secondaryLocationArg, unitArg, playerArg, locationArg }, // srcLocation, unit, player, destLocation
    /** 39 = Move Unit                              */ { numUnitsArg, unitArg, playerArg, locationArg, secondaryLocationArg }, // numUnits, unit, player, srcLocation, destLocation
    /** 40 = Leaderboard (Greed) ------------------ */ { amountArg },
    /** 41 = Set Next Scenario                      */ { stringArg },
    /** 42 = Set Doodad State --------------------- */ { stateModArg, unitArg, playerArg, locationArg },
    /** 43 = Set Invincibility                      */ { stateModArg, unitArg, playerArg, locationArg },
    /** 44 = Create Unit -------------------------- */ { numUnitsArg, unitArg, locationArg, playerArg },
    /** 45 = Set Deaths                             */ { playerArg, numericModArg, amountArg, unitArg },
    /** 46 = Order -------------------------------- */ { unitArg, playerArg, locationArg, orderArg, secondaryLocationArg }, // unit, player, srcLocation, order, destLocation
    /** 47 = Comment                                */ { stringArg },
    /** 48 = Give Units to Player ----------------- */ { numUnitsArg, unitArg, playerArg, locationArg, destPlayerArg },
    /** 49 = Modify Unit Hit Points                 */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 50 = Modify Unit Energy ------------------- */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 51 = Modify Unit Shield points              */ { numUnitsArg, unitArg, playerArg, locationArg, percentArg },
    /** 52 = Modify Unit Resource Amount ---------- */ { numUnitsArg, playerArg, locationArg, amountArg },
    /** 53 = Modify Unit Hanger Count               */ { amountArg, numUnitsArg, unitArg, locationArg, playerArg },
    /** 54 = Pause Timer -------------------------- */ {},
    /** 55 = Unpause Timer                          */ {},
    /** 56 = Draw --------------------------------- */ {},
    /** 57 = Set Alliance Status                    */ { playerArg, allyStateArg },
    /** 58 = Disable Debug Mode ------------------- */ {},
    /** 59 = Enable Debug Mode                      */ {}
};

Chk::Action::Argument Chk::Action::textArguments[NumActionTypes][MaxArguments] = {
    /**  0 = No Action                              */ {},
    /**  1 = Victory ------------------------------ */ {},
    /**  2 = Defeat                                 */ {},
    /**  3 = Preserve Trigger --------------------- */ {},
    /**  4 = Wait                                   */ { durationArg },
    /**  5 = Pause Game --------------------------- */ {},
    /**  6 = Unpause Game                           */ {},
    /**  7 = Transmission ------------------------- */ { textFlagsArg, stringArg, unitArg, locationArg, numericModArg, amountArg, soundArg, durationArg },
    /**  8 = Play Sound                             */ { soundArg, durationArg },
    /**  9 = Display Text Message ----------------- */ { textFlagsArg, stringArg },
    /** 10 = Center View                            */ { locationArg },
    /** 11 = Create Unit with Properties ---------- */ { playerArg, unitArg, numUnitsArg, locationArg, cuwpArg },
    /** 12 = Set Mission Objectives                 */ { stringArg },
    /** 13 = Set Switch --------------------------- */ { switchArg, switchModArg },
    /** 14 = Set Countdown Timer                    */ { numericModArg, durationArg },
    /** 15 = Run AI Script ------------------------ */ { scriptArg },
    /** 16 = Run AI Script At Location              */ { scriptArg, locationArg },
    /** 17 = Leader Board (Control) --------------- */ { stringArg, unitArg },
    /** 18 = Leader Board (Control At Location) --- */ { stringArg, unitArg, locationArg },
    /** 19 = Leader Board (Resources)               */ { stringArg, resourceArg },
    /** 20 = Leader Board (Kills) ----------------- */ { stringArg, unitArg },
    /** 21 = Leader Board (Points)                  */ { stringArg, scoreArg },
    /** 22 = Kill Unit ---------------------------- */ { playerArg, unitArg },
    /** 23 = Kill Unit At Location                  */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 24 = Remove Unit -------------------------- */ { playerArg, unitArg },
    /** 25 = Remove Unit At Location                */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 26 = Set Resources ------------------------ */ { playerArg, numericModArg, amountArg, resourceArg },
    /** 27 = Set Score                              */ { playerArg, numericModArg, amountArg, scoreArg },
    /** 28 = Minimap Ping ------------------------- */ { locationArg },
    /** 29 = Talking Portrait                       */ { unitArg, durationArg },
    /** 30 = Mute Unit Speech --------------------- */ {},
    /** 31 = Unmute Unit Speech                     */ {},
    /** 32 = Leaderboard Computer Players --------- */ { stateModArg },
    /** 33 = Leaderboard Goal (Control)             */ { stringArg, unitArg, amountArg },
    /** 34 = Leaderboard Goal (Control At Location) */ { stringArg, unitArg, amountArg, locationArg },
    /** 35 = Leaderboard Goal (Resources)           */ { stringArg, amountArg, resourceArg },
    /** 36 = Leaderboard Goal (Kills) ------------- */ { stringArg, unitArg, amountArg },
    /** 37 = Leaderboard Goal (Points)              */ { stringArg, scoreArg, amountArg },
    /** 38 = Move Location ------------------------ */ { playerArg, unitArg, locationArg, secondaryLocationArg }, // srcLocation, unit, player, destLocation
    /** 39 = Move Unit                              */ { playerArg, unitArg, numUnitsArg, locationArg, secondaryLocationArg }, // numUnits, unit, player, srcLocation, destLocation
    /** 40 = Leaderboard (Greed) ------------------ */ { amountArg },
    /** 41 = Set Next Scenario                      */ { stringArg },
    /** 42 = Set Doodad State --------------------- */ { playerArg, unitArg, locationArg, stateModArg },
    /** 43 = Set Invincibility                      */ { playerArg, unitArg, locationArg, stateModArg },
    /** 44 = Create Unit -------------------------- */ { playerArg, unitArg, numUnitsArg, locationArg },
    /** 45 = Set Deaths                             */ { playerArg, unitArg, numericModArg, amountArg },
    /** 46 = Order -------------------------------- */ { playerArg, unitArg, locationArg, secondaryLocationArg, orderArg }, // unit, player, srcLocation, order, destLocation
    /** 47 = Comment                                */ { stringArg },
    /** 48 = Give Units to Player ----------------- */ { playerArg, destPlayerArg, unitArg, numUnitsArg, locationArg },
    /** 49 = Modify Unit Hit Points                 */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 50 = Modify Unit Energy ------------------- */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 51 = Modify Unit Shield points              */ { playerArg, unitArg, percentArg, numUnitsArg, locationArg },
    /** 52 = Modify Unit Resource Amount ---------- */ { playerArg, amountArg, numUnitsArg, locationArg },
    /** 53 = Modify Unit Hanger Count               */ { playerArg, unitArg, amountArg, numUnitsArg, locationArg },
    /** 54 = Pause Timer -------------------------- */ {},
    /** 55 = Unpause Timer                          */ {},
    /** 56 = Draw --------------------------------- */ {},
    /** 57 = Set Alliance Status                    */ { playerArg, allyStateArg },
    /** 58 = Disable Debug Mode ------------------- */ {},
    /** 59 = Enable Debug Mode                      */ {}
};

u8 Chk::Action::defaultFlags[NumActionTypes] = {
    /**  0 = No Action                              */ 0,
    /**  1 = Victory ------------------------------ */ (u8)Flags::AlwaysDisplay,
    /**  2 = Defeat                                 */ (u8)Flags::AlwaysDisplay,
    /**  3 = Preserve Trigger --------------------- */ (u8)Flags::AlwaysDisplay,
    /**  4 = Wait                                   */ (u8)Flags::AlwaysDisplay,
    /**  5 = Pause Game --------------------------- */ (u8)Flags::AlwaysDisplay,
    /**  6 = Unpause Game                           */ (u8)Flags::AlwaysDisplay,
    /**  7 = Transmission ------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /**  8 = Play Sound                             */ (u8)Flags::AlwaysDisplay,
    /**  9 = Display Text Message ----------------- */ (u8)Flags::AlwaysDisplay,
    /** 10 = Center View                            */ (u8)Flags::AlwaysDisplay,
    /** 11 = Create Unit with Properties ---------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::UnitPropertiesUsed | (u8)Flags::AlwaysDisplay,
    /** 12 = Set Mission Objectives                 */ 0,
    /** 13 = Set Switch --------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 14 = Set Countdown Timer                    */ (u8)Flags::AlwaysDisplay,
    /** 15 = Run AI Script ------------------------ */ (u8)Flags::AlwaysDisplay,
    /** 16 = Run AI Script At Location              */ (u8)Flags::AlwaysDisplay,
    /** 17 = Leader Board (Control) --------------- */ (u8)Flags::UnitTypeUsed,
    /** 18 = Leader Board (Control At Location) --- */ (u8)Flags::UnitTypeUsed,
    /** 19 = Leader Board (Resources)               */ 0,
    /** 20 = Leader Board (Kills) ----------------- */ (u8)Flags::UnitTypeUsed,
    /** 21 = Leader Board (Points)                  */ 0,
    /** 22 = Kill Unit ---------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 23 = Kill Unit At Location                  */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 24 = Remove Unit -------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 25 = Remove Unit At Location                */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 26 = Set Resources ------------------------ */ (u8)Flags::AlwaysDisplay,
    /** 27 = Set Score                              */ (u8)Flags::AlwaysDisplay,
    /** 28 = Minimap Ping ------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 29 = Talking Portrait                       */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 30 = Mute Unit Speech --------------------- */ (u8)Flags::AlwaysDisplay,
    /** 31 = Unmute Unit Speech                     */ (u8)Flags::AlwaysDisplay,
    /** 32 = Leaderboard Computer Players --------- */ (u8)Flags::AlwaysDisplay,
    /** 33 = Leaderboard Goal (Control)             */ (u8)Flags::UnitTypeUsed,
    /** 34 = Leaderboard Goal (Control At Location) */ (u8)Flags::UnitTypeUsed,
    /** 35 = Leaderboard Goal (Resources)           */ 0,
    /** 36 = Leaderboard Goal (Kills) ------------- */ (u8)Flags::UnitTypeUsed,
    /** 37 = Leaderboard Goal (Points)              */ 0,
    /** 38 = Move Location ------------------------ */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 39 = Move Unit                              */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 40 = Leaderboard (Greed) ------------------ */ (u8)Flags::AlwaysDisplay,
    /** 41 = Set Next Scenario                      */ (u8)Flags::AlwaysDisplay,
    /** 42 = Set Doodad State --------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 43 = Set Invincibility                      */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 44 = Create Unit -------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 45 = Set Deaths                             */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 46 = Order -------------------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 47 = Comment                                */ 0,
    /** 48 = Give Units to Player ----------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 49 = Modify Unit Hit Points                 */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 50 = Modify Unit Energy ------------------- */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 51 = Modify Unit Shield points              */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 52 = Modify Unit Resource Amount ---------- */ (u8)Flags::AlwaysDisplay,
    /** 53 = Modify Unit Hanger Count               */ (u8)Flags::UnitTypeUsed | (u8)Flags::AlwaysDisplay,
    /** 54 = Pause Timer -------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 55 = Unpause Timer                          */ (u8)Flags::AlwaysDisplay,
    /** 56 = Draw --------------------------------- */ (u8)Flags::AlwaysDisplay,
    /** 57 = Set Alliance Status                    */ (u8)Flags::AlwaysDisplay,
    /** 58 = Disable Debug Mode ------------------- */ 0,
    /** 59 = Enable Debug Mode                      */ 0
};

Chk::Condition::ArgType Chk::Condition::getClassicArgType(Type conditionType, size_t argNum)
{
    if ( (size_t)conditionType < NumConditionTypes && argNum < MaxArguments )
        return classicArguments[(size_t)conditionType][argNum].type;
    else
        return Chk::Condition::ArgType::NoType;
}

Chk::Condition::ArgType Chk::Condition::getClassicArgType(VirtualType conditionType, size_t argNum)
{
    if ( argNum < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return classicArguments[(size_t)conditionType][argNum].type;

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argNum].type;
    }
    return Chk::Condition::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(Type actionType, size_t argNum)
{
    if ( (size_t)actionType < NumActionTypes && argNum < MaxArguments )
        return classicArguments[(size_t)actionType][argNum].type;
    else
        return Chk::Action::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(VirtualType actionType, size_t argNum)
{
    if ( argNum < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return classicArguments[(size_t)actionType][argNum].type;

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argNum].type;
    }
    return Chk::Action::ArgType::NoType;
}

inline bool Chk::Action::stringUsed(size_t stringId)
{
    return (size_t)actionType < NumActionTypes &&
        ((actionUsesStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

inline bool Chk::Action::gameStringUsed(size_t stringId)
{
    return (size_t)actionType < NumActionTypes &&
        ((actionUsesGameStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

inline bool Chk::Action::commentStringUsed(size_t stringId)
{
    return actionType == Type::Comment && this->stringId == stringId;
}

inline bool Chk::Action::briefingStringUsed(size_t stringId)
{
    return (size_t)actionType < NumBriefingActionTypes &&
        ((briefingActionUsesStringArg[(size_t)actionType] && this->stringId == stringId) ||
         (briefingActionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId));
}

inline void Chk::Action::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( actionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

inline void Chk::Action::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesGameStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( actionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

inline void Chk::Action::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( actionType == Type::Comment && stringId > 0 && stringId < Chk::MaxStrings )
        stringIdUsed[stringId] = true;
}

inline void Chk::Action::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    if ( (size_t)actionType < NumBriefingActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( briefingActionUsesSoundArg[(size_t)actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

void Chk::Action::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( actionUsesSoundArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::remapBriefingStringIds(std::map<u32, u32> stringIdRemappings)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( briefingActionUsesSoundArg[(size_t)actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::deleteString(size_t stringId)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[(size_t)actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( actionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId )
            this->soundStringId = 0;
    }
}

void Chk::Action::deleteBriefingString(size_t stringId)
{
    if ( (size_t)actionType < NumActionTypes )
    {
        if ( briefingActionUsesStringArg[(size_t)actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( briefingActionUsesSoundArg[(size_t)actionType] && this->soundStringId == stringId )
            this->soundStringId = 0;
    }
}

std::unordered_map<Chk::Action::VirtualType, Chk::Action::VirtualAction> Chk::Action::virtualActions = {
    std::pair<Chk::Action::VirtualType, Chk::Action::VirtualAction>(
        Chk::Action::VirtualType::Custom, { Chk::Action::VirtualType::Indeterminate, {
             locationArg, textFlagsArg, soundArg, durationArg, playerArg, numberArg, actionTypeArg, secondaryTypeIndexArg, flagsArg, paddingArg, maskFlagArg } } ),
    std::pair<Chk::Action::VirtualType, Chk::Action::VirtualAction>(
        Chk::Action::VirtualType::SetMemory, { Chk::Action::VirtualType::SetDeaths, { memoryOffsetArg, numericModArg, amountArg } } )
};

bool Chk::Action::actionUsesStringArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ true , /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ true , /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ true , /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ true ,
    /** 18 = Leader Board (Control At Location)     */ true , /** 19 = Leader Board (Resources)               */ true , /** 20 = Leader Board (Kills)                   */ true ,
    /** 21 = Leader Board (Points) ---------------- */ true , /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ true , /** 34 = Leaderboard Goal (Control At Location) */ true , /** 35 = Leaderboard Goal (Resources) --------- */ true ,
    /** 36 = Leaderboard Goal (Kills)               */ true , /** 37 = Leaderboard Goal (Points)              */ true , /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ true ,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ true ,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

bool Chk::Action::actionUsesGameStringArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ true , /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ true , /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ true , /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ true ,
    /** 18 = Leader Board (Control At Location)     */ true , /** 19 = Leader Board (Resources)               */ true , /** 20 = Leader Board (Kills)                   */ true ,
    /** 21 = Leader Board (Points) ---------------- */ true , /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ true , /** 34 = Leaderboard Goal (Control At Location) */ true , /** 35 = Leaderboard Goal (Resources) --------- */ true ,
    /** 36 = Leaderboard Goal (Kills)               */ true , /** 37 = Leaderboard Goal (Points)              */ true , /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ true ,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

bool Chk::Action::actionUsesSoundArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */  true, /**  8 = Play Sound                             */ true ,
    /**  9 = Display Text Message ----------------- */ false, /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ false, /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ false,
    /** 18 = Leader Board (Control At Location)     */ false, /** 19 = Leader Board (Resources)               */ false, /** 20 = Leader Board (Kills)                   */ false,
    /** 21 = Leader Board (Points) ---------------- */ false, /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ false, /** 34 = Leaderboard Goal (Control At Location) */ false, /** 35 = Leaderboard Goal (Resources) --------- */ false,
    /** 36 = Leaderboard Goal (Kills)               */ false, /** 37 = Leaderboard Goal (Points)              */ false, /** 38 = Move Location                          */ false,
    /** 39 = Move Unit ---------------------------- */ false, /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ false,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ false, /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

Chk::Action::Argument Chk::Action::classicBriefingArguments[NumBriefingActionTypes][MaxArguments] = {
    /** 0 = No Action                 */ {},
    /** 1 = Wait -------------------- */ { durationArg },
    /** 2 = Play Sound                */ { soundArg }, // Hidden: durationArg (auto-calculate)
    /** 3 = Text Message ------------ */ { durationArg, stringArg },
    /** 4 = Mission Objectives        */ { stringArg },
    /** 5 = Show Portrait ----------- */ { unitArg, briefingSlotArg }, // Flags::UnitTypeUsed is set
    /** 6 = Hide Portrait             */ { briefingSlotArg },
    /** 7 = Display Speaking Portrait */ { briefingSlotArg, durationArg },
    /** 8 = Transmission              */ { briefingSlotArg, soundArg, numericModArg, amountArg, stringArg }, // Hidden: durationArg (auto-calculate)
    /** 9 = Skip Tutorial Enabled --- */ {}
};

Chk::Action::Argument Chk::Action::briefingTextArguments[NumBriefingActionTypes][MaxArguments] = {
    /** 0 = No Action                 */ {},
    /** 1 = Wait -------------------- */ { durationArg },
    /** 2 = Play Sound                */ { soundArg, durationArg },
    /** 3 = Text Message ------------ */ { stringArg, durationArg },
    /** 4 = Mission Objectives        */ { stringArg },
    /** 5 = Show Portrait ----------- */ { unitArg, briefingSlotArg },
    /** 6 = Hide Portrait             */ { briefingSlotArg },
    /** 7 = Display Speaking Portrait */ { briefingSlotArg, durationArg },
    /** 8 = Transmission              */ { stringArg, briefingSlotArg, numericModArg, amountArg, soundArg, durationArg },
    /** 9 = Skip Tutorial Enabled --- */ {}
};

u8 Chk::Action::briefingDefaultFlags[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ 0,
    /** 1 = Wait -------------------- */ 0,
    /** 2 = Play Sound                */ 0,
    /** 3 = Text Message ------------ */ 0,
    /** 4 = Mission Objectives        */ 0,
    /** 5 = Show Portrait ----------- */ (u8)Chk::Action::Flags::UnitTypeUsed,
    /** 6 = Hide Portrait             */ 0,
    /** 7 = Display Speaking Portrait */ 0,
    /** 8 = Transmission              */ 0,
    /** 9 = Skip Tutorial Enabled --- */ 0
};

bool Chk::Action::briefingActionUsesStringArg[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ false, /** 1 = Wait -------------------- */ false, /** 2 = Play Sound                */ false, /** 3 = Text Message ------------ */ true ,
    /** 4 = Mission Objectives        */  true, /** 5 = Show Portrait ----------- */ false, /** 6 = Hide Portrait             */ false, /** 7 = Display Speaking Portrait */ false,
    /** 8 = Transmission              */  true, /** 9 = Skip Tutorial Enabled --- */ false
};

bool Chk::Action::briefingActionUsesSoundArg[NumBriefingActionTypes] = {
    /** 0 = No Action                 */ false, /** 1 = Wait -------------------- */ false, /** 2 = Play Sound                */  true, /** 3 = Text Message ------------ */ false,
    /** 4 = Mission Objectives        */ false, /** 5 = Show Portrait ----------- */ false, /** 6 = Hide Portrait             */ false, /** 7 = Display Speaking Portrait */ false,
    /** 8 = Transmission              */  true, /** 9 = Skip Tutorial Enabled --- */ false
};

Chk::Condition & Chk::Trigger::condition(size_t conditionIndex)
{
    if ( conditionIndex < MaxConditions )
        return conditions[conditionIndex];
    else
        throw std::out_of_range("conditionIndex " + std::to_string(conditionIndex) + " exceeds max " + std::to_string(MaxConditions-1));
}

Chk::Action & Chk::Trigger::action(size_t actionIndex)
{
    if ( actionIndex < MaxActions )
        return actions[actionIndex];
    else
        throw std::out_of_range("actionIndex " + std::to_string(actionIndex) + " exceeds max " + std::to_string(MaxActions-1));
}

void Chk::Condition::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

bool Chk::Condition::isDisabled()
{
    return (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled;
}

void Chk::Action::ToggleDisabled()
{
    if ( (flags & (u8)Flags::Disabled) == (u8)Flags::Disabled )
        flags &= (u8)Flags::xDisabled;
    else
        flags |= (u8)Flags::Disabled;
}

Chk::Trigger::Owned & Chk::Trigger::owned(size_t ownerIndex)
{
    if ( ownerIndex < MaxOwners )
        return owners[ownerIndex];
    else
        throw std::out_of_range("ownerIndex " + std::to_string(ownerIndex) + " exceeds max " + std::to_string(MaxOwners-1));
}

Chk::Trigger& Chk::Trigger::operator=(const Trigger &trigger)
{
    if ( this != &trigger )
        memcpy(this, &trigger, sizeof(Trigger));

    return *this;
}

void Chk::Trigger::deleteAction(size_t actionIndex, bool alignTop)
{
    if ( actionIndex < MaxActions )
    {
        actions[actionIndex].locationId = 0;
        actions[actionIndex].stringId = 0;
        actions[actionIndex].soundStringId = 0;
        actions[actionIndex].time = 0;
        actions[actionIndex].group = 0;
        actions[actionIndex].number = 0;
        actions[actionIndex].type = 0;
        actions[actionIndex].actionType = Chk::Action::Type::NoAction;
        actions[actionIndex].type2 = 0;
        actions[actionIndex].flags = 0;
        actions[actionIndex].padding = 0;
        actions[actionIndex].maskFlag = Chk::Action::MaskFlag::Disabled;

        if ( alignTop )
            alignActionsTop();
    }
}

void Chk::Trigger::deleteCondition(size_t conditionIndex, bool alignTop)
{
    if ( conditionIndex < MaxConditions )
    {
        conditions[conditionIndex].locationId = 0;
        conditions[conditionIndex].player = 0;
        conditions[conditionIndex].amount = 0;
        conditions[conditionIndex].unitType = Sc::Unit::Type::TerranMarine;
        conditions[conditionIndex].comparison = Chk::Condition::Comparison::AtLeast;
        conditions[conditionIndex].conditionType = Chk::Condition::Type::NoCondition;
        conditions[conditionIndex].typeIndex = 0;
        conditions[conditionIndex].flags = 0;
        conditions[conditionIndex].maskFlag = Chk::Condition::MaskFlag::Disabled;
    
        if ( alignTop )
            alignConditionsTop();
    }
}

bool Chk::Trigger::preserveTriggerFlagged()
{   
    return (flags & (u32)Flags::PreserveTrigger) == (u32)Flags::PreserveTrigger;
}

bool Chk::Trigger::disabled()
{
    return (flags & (u32)Flags::Disabled) == (u32)Flags::Disabled;
}

bool Chk::Trigger::ignoreConditionsOnce()
{
    return (flags & (u32)Flags::IgnoreConditionsOnce) == (u32)Flags::IgnoreConditionsOnce;
}

bool Chk::Trigger::ignoreWaitSkipOnce()
{
    return (flags & (u32)Flags::IgnoreWaitSkipOnce) == (u32)Flags::IgnoreWaitSkipOnce;
}

bool Chk::Trigger::ignoreMiscActionsOnce()
{
    return (flags & (u32)Flags::IgnoreMiscActions) == (u32)Flags::IgnoreMiscActions;
}

bool Chk::Trigger::ignoreDefeatDraw()
{
    return (flags & (u32)Flags::IgnoreDefeatDraw) == (u32)Flags::IgnoreDefeatDraw;
}

bool Chk::Trigger::pauseFlagged()
{
    return (flags & (u32)Flags::Paused) == (u32)Flags::Paused;
}

void Chk::Trigger::setPreserveTriggerFlagged(bool preserved)
{
    if ( preserved )
        flags |= (u32)Flags::PreserveTrigger;
    else
        flags &= ~(u32)Flags::PreserveTrigger;
}

void Chk::Trigger::setDisabled(bool disabled)
{
    if ( disabled )
        flags |= (u32)Flags::Disabled;
    else
        flags &= ~(u32)Flags::Disabled;
}

void Chk::Trigger::setIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
    if ( ignoreConditionsOnce )
        flags |= (u32)Flags::IgnoreConditionsOnce;
    else
        flags &= ~(u32)Flags::IgnoreConditionsOnce;
}

void Chk::Trigger::setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
    if ( ignoreWaitSkipOnce )
        flags |= (u32)Flags::IgnoreWaitSkipOnce;
    else
        flags &= ~(u32)Flags::IgnoreWaitSkipOnce;
}

void Chk::Trigger::setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
    if ( ignoreMiscActionsOnce )
        flags |= (u32)Flags::IgnoreMiscActions;
    else
        flags &= ~(u32)Flags::IgnoreMiscActions;
}

void Chk::Trigger::setIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
    if ( ignoreDefeatDraw )
        flags |= (u32)Flags::IgnoreDefeatDraw;
    else
        flags &= ~(u32)Flags::IgnoreDefeatDraw;
}

void Chk::Trigger::setPauseFlagged(bool pauseFlagged)
{
    if ( pauseFlagged )
        flags |= (u32)Flags::Paused;
    else
        flags &= ~(u32)Flags::Paused;
}

size_t Chk::Trigger::numUsedConditions()
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
    {
        if ( conditions[i].conditionType != Chk::Condition::Type::NoCondition )
            total ++;
    }
    return total;
}

size_t Chk::Trigger::getComment()
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( actions[i].actionType == Chk::Action::Type::Comment && !actions[i].isDisabled() )
            actions[i].stringId;
    }
    return Chk::StringId::NoString;
}

size_t Chk::Trigger::numUsedActions()
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( actions[i].actionType != Chk::Action::Type::NoAction )
            total ++;
    }
    return total;
}

bool Chk::Trigger::stringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].stringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::gameStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].gameStringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::commentStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].commentStringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::briefingStringUsed(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].briefingStringUsed(stringId) )
            return true;
    }
    return false;
}

void Chk::Trigger::markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedStrings(stringIdUsed);
}

void Chk::Trigger::markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedGameStrings(stringIdUsed);
}

void Chk::Trigger::markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedCommentStrings(stringIdUsed);
}

void Chk::Trigger::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedBriefingStrings(stringIdUsed);
}

void Chk::Trigger::remapStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapStringIds(stringIdRemappings);
}

void Chk::Trigger::remapBriefingStringIds(std::map<u32, u32> stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapBriefingStringIds(stringIdRemappings);
}

void Chk::Trigger::deleteString(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].deleteString(stringId);
}

void Chk::Trigger::deleteBriefingString(size_t stringId)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].deleteBriefingString(stringId);
}

void Chk::Trigger::alignConditionsTop()
{
    for ( u8 i=0; i<MaxConditions; i++ )
    {
        if ( conditions[i].conditionType == Chk::Condition::Type::NoCondition )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<MaxConditions; j++ )
            {
                if ( conditions[j].conditionType != Chk::Condition::Type::NoCondition )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<MaxConditions; j++ )
                    conditions[j] = conditions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

void Chk::Trigger::alignActionsTop()
{
    for ( u8 i=0; i<MaxActions; i++ )
    {
        if ( actions[i].actionType == Chk::Action::Type::NoAction )
        {
            u8 firstUsedAfter = u8(-1);
            for ( u8 j=i+1; j<MaxActions; j++ )
            {
                if ( actions[j].actionType != Chk::Action::Type::NoAction )
                {
                    firstUsedAfter = j;
                    break;
                }
            }

            if ( firstUsedAfter != u8(-1) )
            {
                for ( u8 j=i; j<MaxActions; j++ )
                    actions[j] = actions[firstUsedAfter+(j-i)];
            }
            else
                break;
        }
    }
}

Chk::StringProperties::StringProperties()
    : Chk::StringProperties(0, 0, 0, false, false, false, false, false, 8)
{

}

Chk::StringProperties::StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size)
    : red(red), green(green), blue(blue)
{
    u8 flags = (isUsed ? (u8)StrFlags::isUsed : 0) | (hasPriority ? (u8)StrFlags::hasPriority : 0) | (isBold ? (u8)StrFlags::bold : 0) |
        (isUnderlined ? (u8)StrFlags::underlined : 0) | (isItalics ? (u8)StrFlags::italics : 0);

    constexpr u32 maxFontSize = baseFontSize + 7*fontStepSize;
    if ( size < baseFontSize || size > maxFontSize )
        throw InvalidSize(size, baseFontSize, fontStepSize);

    size -= baseFontSize;
    if ( size > 4*fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusFourSteps;
        size -= 4*fontStepSize;
    }
    if ( size > 2*fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusTwoSteps;
        size -= 2*fontStepSize;
    }
    if ( size > fontStepSize )
    {
        flags |= (u8)StrFlags::sizePlusOneStep;
        size -= fontStepSize;
    }

    if ( size > 0 )
        throw InvalidSize(size, baseFontSize, fontStepSize);

    this->flags = flags;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Unit & unit)
{
    out.write(reinterpret_cast<const char*>(&unit), sizeof(Chk::Unit));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::IsomEntry & isomEntry)
{
    out.write(reinterpret_cast<const char*>(&isomEntry), sizeof(Chk::IsomEntry));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Doodad & doodad)
{
    out.write(reinterpret_cast<const char*>(&doodad), sizeof(Chk::Doodad));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Sprite & sprite)
{
    out.write(reinterpret_cast<const char*>(&sprite), sizeof(Chk::Sprite));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Location & location)
{
    out.write(reinterpret_cast<const char*>(&location), sizeof(Chk::Location));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Condition & condition)
{
    out.write(reinterpret_cast<const char*>(&condition), sizeof(Chk::Condition));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Action & action)
{
    out.write(reinterpret_cast<const char*>(&action), sizeof(Chk::Action));
    return out;
}

std::ostream & Chk::operator<< (std::ostream & out, const Chk::Trigger & trigger)
{
    out.write(reinterpret_cast<const char*>(&trigger), sizeof(Chk::Trigger));
    return out;
}



bool Chk::Sprite::isDrawnAsSprite()
{
    return (flags & (u16)SpriteFlags::DrawAsSprite) == (u16)SpriteFlags::DrawAsSprite;
}

bool Chk::Cuwp::isCloaked()
{
    return (unitStateFlags & (u16)State::Cloak) == (u16)State::Cloak;
}

bool Chk::Cuwp::isBurrowed()
{
    return (unitStateFlags & (u16)State::Burrow) == (u16)State::Burrow;
}

bool Chk::Cuwp::isInTransit()
{
    return (unitStateFlags & (u16)State::InTransit) == (u16)State::InTransit;
}

bool Chk::Cuwp::isHallucinated()
{
    return (unitStateFlags & (u16)State::Hallucinated) == (u16)State::Hallucinated;
}

bool Chk::Cuwp::isInvincible()
{
    return (unitStateFlags & (u16)State::Invincible) == (u16)State::Invincible;
}

void Chk::Cuwp::setCloaked(bool cloaked)
{
    unitStateFlags = cloaked ? unitStateFlags | (u16)State::Cloak : unitStateFlags & (u16)State::xCloak;
}

void Chk::Cuwp::setBurrowed(bool burrowed)
{
    unitStateFlags = burrowed ? unitStateFlags | (u16)State::Burrow : unitStateFlags & (u16)State::xBurrow;
}

void Chk::Cuwp::setInTransit(bool inTransit)
{
    unitStateFlags = inTransit ? unitStateFlags | (u16)State::InTransit : unitStateFlags & (u16)State::xInTransit;
}

void Chk::Cuwp::setHallucinated(bool hallucinated)
{
    unitStateFlags = hallucinated ? unitStateFlags | (u16)State::Hallucinated : unitStateFlags & (u16)State::xHallucinated;
}

void Chk::Cuwp::setInvincible(bool invincible)
{
    unitStateFlags = invincible ? unitStateFlags | (u16)State::Invincible : unitStateFlags & (u16)State::xInvincible;
}

Chk::Location::Location() : left(0), top(0), right(0), bottom(0), stringId(0), elevationFlags(0)
{

}