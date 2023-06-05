#include "Chk.h"

Chk::Condition::Argument Chk::Condition::noArg = { Chk::Condition::ArgType::NoType, Chk::Condition::ArgField::NoField };
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
    /** 15 = Deaths ------------- */ { playerCndArg, unitCndArg, numericComparisonCndArg, amountCndArg },
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

bool Chk::Condition::conditionUsesLocationArg[NumConditionTypes] = {
    /**  0 = No Condition         */ false,
    /**  1 = Countdown Timer ---- */ false,
    /**  2 = Command              */ false,
    /**  3 = Bring -------------- */ true ,
    /**  4 = Accumulate           */ false,
    /**  5 = Kill --------------- */ false,
    /**  6 = Command The Most     */ false,
    /**  7 = Command The Most At  */ true ,
    /**  8 = Most Kills           */ false,
    /**  9 = Highest Score ------ */ false,
    /** 10 = Most Resources       */ false,
    /** 11 = Switch ------------- */ false,
    /** 12 = Elapsed Time         */ false,
    /** 13 = Is Briefing -------- */ false,
    /** 14 = Opponents            */ false,
    /** 15 = Deaths ------------- */ false,
    /** 16 = Command The Least    */ false,
    /** 17 = Command The Least At */ true ,
    /** 18 = Least Kills          */ false,
    /** 19 = Lowest Score ------- */ false,
    /** 20 = Least Resources      */ false,
    /** 21 = Score -------------- */ false,
    /** 22 = Always               */ false,
    /** 23 = Never -------------- */ false
};

std::unordered_map<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition> Chk::Condition::virtualConditions = {
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Custom, { Chk::Condition::VirtualType::Indeterminate, {
            locationCndArg, playerCndArg, amountCndArg, unitCndArg, comparisonCndArg, conditionTypeCndArg, typeIndexCndArg, flagsCndArg, maskFlagCndArg } } ),
    std::pair<Chk::Condition::VirtualType, Chk::Condition::VirtualCondition>(
        Chk::Condition::VirtualType::Memory, { Chk::Condition::VirtualType::Deaths, { memoryOffsetCndArg, numericComparisonCndArg, amountCndArg } } )
};

Chk::Action::Argument Chk::Action::noArg = { Chk::Action::ArgType::NoType, Chk::Action::ArgField::NoField };
Chk::Action::Argument amountArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Number };
Chk::Action::Argument percentArg = { Chk::Action::ArgType::Percent, Chk::Action::ArgField::Number };
Chk::Action::Argument durationArg = { Chk::Action::ArgType::Duration, Chk::Action::ArgField::Time };
Chk::Action::Argument numUnitsArg = { Chk::Action::ArgType::NumUnits, Chk::Action::ArgField::Type2 };
Chk::Action::Argument unitQuantityArg = { Chk::Action::ArgType::Amount, Chk::Action::ArgField::Type2 };
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
    /** 11 = Create Unit with Properties ---------- */ { unitQuantityArg, unitArg, locationArg, playerArg, cuwpArg },
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
    /** 44 = Create Unit -------------------------- */ { unitQuantityArg, unitArg, locationArg, playerArg },
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
    /** 11 = Create Unit with Properties ---------- */ { playerArg, unitArg, unitQuantityArg, locationArg, cuwpArg },
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
    /** 39 = Move Unit                              */ { playerArg, unitArg, numUnitsArg, locationArg, secondaryLocationArg }, // player, unit, numUnits, srcLocation, destLocation
    /** 40 = Leaderboard (Greed) ------------------ */ { amountArg },
    /** 41 = Set Next Scenario                      */ { stringArg },
    /** 42 = Set Doodad State --------------------- */ { playerArg, unitArg, locationArg, stateModArg },
    /** 43 = Set Invincibility                      */ { playerArg, unitArg, locationArg, stateModArg },
    /** 44 = Create Unit -------------------------- */ { playerArg, unitArg, unitQuantityArg, locationArg },
    /** 45 = Set Deaths                             */ { playerArg, unitArg, numericModArg, amountArg },
    /** 46 = Order -------------------------------- */ { playerArg, unitArg, locationArg, secondaryLocationArg, orderArg }, // unit, player, srcLocation, order, destLocation
    /** 47 = Comment                                */ { stringArg },
    /** 48 = Give Units to Player ----------------- */ { playerArg, destPlayerArg, unitArg, numUnitsArg, locationArg },
    /** 49 = Modify Unit Hit Points                 */ { playerArg, unitArg, percentArg, unitQuantityArg, locationArg },
    /** 50 = Modify Unit Energy ------------------- */ { playerArg, unitArg, percentArg, unitQuantityArg, locationArg },
    /** 51 = Modify Unit Shield points              */ { playerArg, unitArg, percentArg, unitQuantityArg, locationArg },
    /** 52 = Modify Unit Resource Amount ---------- */ { playerArg, amountArg, unitQuantityArg, locationArg },
    /** 53 = Modify Unit Hanger Count               */ { playerArg, unitArg, amountArg, unitQuantityArg, locationArg },
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

const Chk::Condition::Argument & Chk::Condition::getClassicArg(Type conditionType, size_t argIndex)
{
    if ( conditionType < NumConditionTypes && argIndex < MaxArguments )
        return classicArguments[conditionType][argIndex];
    else
        return Chk::Condition::noArg;
}

const Chk::Condition::Argument & Chk::Condition::getClassicArg(VirtualType conditionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return classicArguments[conditionType][argIndex];

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argIndex];
    }
    return Chk::Condition::noArg;
}

Chk::Condition::ArgType Chk::Condition::getClassicArgType(Type conditionType, size_t argIndex)
{
    if ( conditionType < NumConditionTypes && argIndex < MaxArguments )
        return classicArguments[conditionType][argIndex].type;
    else
        return Chk::Condition::ArgType::NoType;
}

Chk::Condition::ArgType Chk::Condition::getClassicArgType(VirtualType conditionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return classicArguments[conditionType][argIndex].type;

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argIndex].type;
    }
    return Chk::Condition::ArgType::NoType;
}

const Chk::Condition::Argument & Chk::Condition::getTextArg(Type conditionType, size_t argIndex)
{
    if ( conditionType < NumConditionTypes && argIndex < MaxArguments )
        return textArguments[conditionType][argIndex];
    else
        return Chk::Condition::noArg;
}

const Chk::Condition::Argument & Chk::Condition::getTextArg(VirtualType conditionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return textArguments[conditionType][argIndex];

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argIndex];
    }
    return Chk::Condition::noArg;
}

Chk::Condition::ArgType Chk::Condition::getTextArgType(Type conditionType, size_t argIndex)
{
    if ( conditionType < NumConditionTypes && argIndex < MaxArguments )
        return textArguments[conditionType][argIndex].type;
    else
        return Chk::Condition::ArgType::NoType;
}

Chk::Condition::ArgType Chk::Condition::getTextArgType(VirtualType conditionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)conditionType < NumConditionTypes )
            return textArguments[conditionType][argIndex].type;

        auto virtualCondition = virtualConditions.find(conditionType);
        if ( virtualCondition != virtualConditions.end() )
            return virtualCondition->second.arguments[argIndex].type;
    }
    return Chk::Condition::ArgType::NoType;
}

u8 Chk::Condition::getDefaultFlags(Type conditionType)
{
    if ( conditionType < NumConditionTypes )
        return defaultFlags[conditionType];
    else
        return u8(0);
}

u8 Chk::Condition::getDefaultFlags(VirtualType conditionType)
{
    if ( conditionType < 0 )
    {
        switch ( conditionType )
        {
            // Don't include VirtualType::Custom, that is determined by parsing args
            case Condition::VirtualType::Memory: return defaultFlags[Condition::ExtendedBaseType::Memory];
            default: return u8(0);
        }
    }
    else
        return getDefaultFlags((Condition::Type)conditionType);
}

const Chk::Action::Argument & Chk::Action::getClassicArg(Type actionType, size_t argIndex)
{
    if ( actionType < NumActionTypes && argIndex < MaxArguments )
        return classicArguments[actionType][argIndex];
    else
        return Chk::Action::noArg;
}

const Chk::Action::Argument & Chk::Action::getClassicArg(VirtualType actionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return classicArguments[actionType][argIndex];

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argIndex];
    }
    return Chk::Action::noArg;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(Type actionType, size_t argIndex)
{
    if ( actionType < NumActionTypes && argIndex < MaxArguments )
        return classicArguments[actionType][argIndex].type;
    else
        return Chk::Action::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getClassicArgType(VirtualType actionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return classicArguments[actionType][argIndex].type;

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argIndex].type;
    }
    return Chk::Action::ArgType::NoType;
}

const Chk::Action::Argument & Chk::Action::getTextArg(Type actionType, size_t argIndex)
{
    if ( actionType < NumActionTypes && argIndex < MaxArguments )
        return textArguments[actionType][argIndex];
    else
        return Chk::Action::noArg;
}

const Chk::Action::Argument & Chk::Action::getTextArg(VirtualType actionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return textArguments[actionType][argIndex];

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argIndex];
    }
    return Chk::Action::noArg;
}

Chk::Action::ArgType Chk::Action::getTextArgType(Type actionType, size_t argIndex)
{
    if ( actionType < NumActionTypes && argIndex < MaxArguments )
        return textArguments[actionType][argIndex].type;
    else
        return Chk::Action::ArgType::NoType;
}

Chk::Action::ArgType Chk::Action::getTextArgType(VirtualType actionType, size_t argIndex)
{
    if ( argIndex < MaxArguments )
    {
        if ( (size_t)actionType < NumActionTypes )
            return textArguments[actionType][argIndex].type;

        auto virtualAction = virtualActions.find(actionType);
        if ( virtualAction != virtualActions.end() )
            return virtualAction->second.arguments[argIndex].type;
    }
    return Chk::Action::ArgType::NoType;
}

u8 Chk::Action::getDefaultFlags(Type actionType)
{
    if ( actionType < NumActionTypes )
        return defaultFlags[actionType];
    else
        return u8(0);
}

u8 Chk::Action::getDefaultFlags(VirtualType actionType)
{
    if ( actionType < 0 )
    {
        switch ( actionType )
        {
            // Don't include VirtualType::Custom, that is determined by parsing args
            case Action::VirtualType::SetMemory: return defaultFlags[Action::ExtendedBaseType::SetMemory];
            default: return u8(0);
        }
    }
    else
        return getDefaultFlags((Action::Type)actionType);
}

bool Chk::Action::hasStringArgument() const
{
    for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
    {
        if ( Chk::Action::getClassicArgType(actionType, i) == Chk::Action::ArgType::String )
            return true;
    }
    return false;
}

bool Chk::Action::hasSoundArgument() const
{
    for ( u8 i = 0; i < Chk::Action::MaxArguments; i++ )
    {
        if ( Chk::Action::getClassicArgType(actionType, i) == Chk::Action::ArgType::Sound )
            return true;
    }
    return false;
}

inline bool Chk::Action::locationUsed(size_t locationId) const
{
    return actionType < NumActionTypes &&
        ((actionUsesLocationArg[actionType] && this->locationId == locationId) ||
         (actionUsesSecondaryLocationArg[actionType] && this->number == locationId));
}

inline bool Chk::Action::stringUsed(size_t stringId, u32 userMask) const
{
    return actionType < NumActionTypes &&
        (((userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction && actionUsesStringArg[actionType] && this->stringId == stringId) ||
         ((userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound && actionUsesSoundArg[actionType] && this->soundStringId == stringId));
}

inline bool Chk::Action::gameStringUsed(size_t stringId, u32 userMask) const
{
    return actionType < NumActionTypes &&
        (((userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction && actionUsesGameStringArg[actionType] && this->stringId == stringId) ||
         ((userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound && actionUsesSoundArg[actionType] && this->soundStringId == stringId));
}

inline bool Chk::Action::commentStringUsed(size_t stringId) const
{
    return actionType == Type::Comment && this->stringId == stringId;
}

inline bool Chk::Action::briefingStringUsed(size_t stringId, u32 userMask) const
{
    return actionType < NumBriefingActionTypes &&
        (((userMask & Chk::StringUserFlag::BriefingTriggerAction) == Chk::StringUserFlag::BriefingTriggerAction && briefingActionUsesStringArg[actionType] && this->stringId == stringId) ||
         ((userMask & Chk::StringUserFlag::BriefingTriggerActionSound) == Chk::StringUserFlag::BriefingTriggerActionSound && briefingActionUsesSoundArg[actionType] && this->soundStringId == stringId));
}

inline void Chk::Action::markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    if ( actionType < NumActionTypes )
    {
        if ( actionUsesLocationArg[actionType] && locationId != Chk::LocationId::NoLocation && locationId <= Chk::TotalLocations )
            locationIdUsed[locationId] = true;

        if ( actionUsesSecondaryLocationArg[actionType] && number != Chk::LocationId::NoLocation && number <= Chk::TotalLocations )
            locationIdUsed[number] = true;
    }
}

inline void Chk::Action::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( actionType < NumActionTypes )
    {
        if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction && actionUsesStringArg[actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( (userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound && actionUsesSoundArg[actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

inline void Chk::Action::markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( actionType < NumActionTypes )
    {
        if ( (userMask & Chk::StringUserFlag::TriggerAction) == Chk::StringUserFlag::TriggerAction && actionUsesGameStringArg[actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( (userMask & Chk::StringUserFlag::TriggerActionSound) == Chk::StringUserFlag::TriggerActionSound && actionUsesSoundArg[actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

inline void Chk::Action::markUsedCommentStrings(std::bitset<Chk::MaxStrings> & stringIdUsed) const
{
    if ( actionType == Type::Comment && stringId > 0 && stringId < Chk::MaxStrings )
        stringIdUsed[stringId] = true;
}

inline void Chk::Action::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    if ( actionType < NumBriefingActionTypes )
    {
        if ( (userMask & Chk::StringUserFlag::BriefingTriggerAction) == Chk::StringUserFlag::BriefingTriggerAction && briefingActionUsesStringArg[actionType] && stringId > 0 && stringId < Chk::MaxStrings )
            stringIdUsed[stringId] = true;

        if ( (userMask & Chk::StringUserFlag::BriefingTriggerActionSound) == Chk::StringUserFlag::BriefingTriggerActionSound && briefingActionUsesSoundArg[actionType] && soundStringId > 0 && soundStringId < Chk::MaxStrings )
            stringIdUsed[soundStringId] = true;
    }
}

void Chk::Action::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    if ( actionType < NumActionTypes )
    {
        if ( actionUsesLocationArg[actionType] )
        {
            auto found = locationIdRemappings.find(locationId);
            if ( found != locationIdRemappings.end() )
                locationId = found->second;
        }

        if ( actionUsesSecondaryLocationArg[actionType] )
        {
            auto found = locationIdRemappings.find(locationId);
            if ( found != locationIdRemappings.end() )
                locationId = found->second;
        }
    }
}

void Chk::Action::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    if ( actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( actionUsesSoundArg[actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    if ( actionType < NumBriefingActionTypes )
    {
        if ( briefingActionUsesStringArg[actionType] )
        {
            auto found = stringIdRemappings.find(stringId);
            if ( found != stringIdRemappings.end() )
                stringId = found->second;
        }

        if ( briefingActionUsesSoundArg[actionType] )
        {
            auto found = stringIdRemappings.find(soundStringId);
            if ( found != stringIdRemappings.end() )
                soundStringId = found->second;
        }
    }
}

void Chk::Action::deleteLocation(size_t locationId)
{
    if ( actionType < NumActionTypes )
    {
        if ( actionUsesLocationArg[actionType] && this->locationId == locationId )
            this->locationId = Chk::LocationId::NoLocation;

        if ( actionUsesSecondaryLocationArg[actionType] && this->number == locationId )
            this->number = Chk::LocationId::NoLocation;
    }
}

void Chk::Action::deleteString(size_t stringId)
{
    if ( actionType < NumActionTypes )
    {
        if ( actionUsesStringArg[actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( actionUsesSoundArg[actionType] && this->soundStringId == stringId )
            this->soundStringId = 0;
    }
}

void Chk::Action::deleteBriefingString(size_t stringId)
{
    if ( actionType < NumBriefingActionTypes )
    {
        if ( briefingActionUsesStringArg[actionType] && this->stringId == stringId )
            this->stringId = 0;

        if ( briefingActionUsesSoundArg[actionType] && this->soundStringId == stringId )
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

bool Chk::Action::actionUsesLocationArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ true , /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ false, /** 10 = Center View -------------------------- */ true , /** 11 = Create Unit with Properties ---------- */ true ,
    /** 12 = Set Mission Objectives                 */ false, /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ true , /** 17 = Leader Board (Control) --------------- */ false,
    /** 18 = Leader Board (Control At Location)     */ true , /** 19 = Leader Board (Resources)               */ false, /** 20 = Leader Board (Kills)                   */ false,
    /** 21 = Leader Board (Points) ---------------- */ false, /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ true ,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ true , /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ true , /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ false, /** 34 = Leaderboard Goal (Control At Location) */ true , /** 35 = Leaderboard Goal (Resources) --------- */ false,
    /** 36 = Leaderboard Goal (Kills)               */ false, /** 37 = Leaderboard Goal (Points)              */ false, /** 38 = Move Location                          */ true ,
    /** 39 = Move Unit ---------------------------- */ true , /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ false,
    /** 42 = Set Doodad State                       */ true , /** 43 = Set Invincibility                      */ true , /** 44 = Create Unit                            */ true ,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ true , /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ true , /** 49 = Modify Unit Hit Points                 */ true , /** 50 = Modify Unit Energy                     */ true ,
    /** 51 = Modify Unit Shield points ------------ */ true , /** 52 = Modify Unit Resource Amount ---------- */ true , /** 53 = Modify Unit Hanger Count ------------- */ true ,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
};

bool Chk::Action::actionUsesSecondaryLocationArg[NumActionTypes] = {
    /**  0 = No Action                              */ false, /**  1 = Victory                                */ false, /**  2 = Defeat                                 */ false,
    /**  3 = Preserve Trigger --------------------- */ false, /**  4 = Wait --------------------------------- */ false, /**  5 = Pause Game --------------------------- */ false,
    /**  6 = Unpause Game                           */ false, /**  7 = Transmission                           */ false, /**  8 = Play Sound                             */ false,
    /**  9 = Display Text Message ----------------- */ false, /** 10 = Center View -------------------------- */ false, /** 11 = Create Unit with Properties ---------- */ false,
    /** 12 = Set Mission Objectives                 */ false, /** 13 = Set Switch                             */ false, /** 14 = Set Countdown Timer                    */ false,
    /** 15 = Run AI Script ------------------------ */ false, /** 16 = Run AI Script At Location ------------ */ false, /** 17 = Leader Board (Control) --------------- */ false,
    /** 18 = Leader Board (Control At Location)     */ false, /** 19 = Leader Board (Resources)               */ false, /** 20 = Leader Board (Kills)                   */ false,
    /** 21 = Leader Board (Points) ---------------- */ false, /** 22 = Kill Unit ---------------------------- */ false, /** 23 = Kill Unit At Location ---------------- */ false,
    /** 24 = Remove Unit                            */ false, /** 25 = Remove Unit At Location                */ false, /** 26 = Set Resources                          */ false,
    /** 27 = Set Score ---------------------------- */ false, /** 28 = Minimap Ping ------------------------- */ false, /** 29 = Talking Portrait --------------------- */ false,
    /** 30 = Mute Unit Speech                       */ false, /** 31 = Unmute Unit Speech                     */ false, /** 32 = Leaderboard Computer Players           */ false,
    /** 33 = Leaderboard Goal (Control) ----------- */ false, /** 34 = Leaderboard Goal (Control At Location) */ false, /** 35 = Leaderboard Goal (Resources) --------- */ false,
    /** 36 = Leaderboard Goal (Kills)               */ false, /** 37 = Leaderboard Goal (Points)              */ false, /** 38 = Move Location                          */ true ,
    /** 39 = Move Unit ---------------------------- */ true , /** 40 = Leaderboard (Greed) ------------------ */ false, /** 41 = Set Next Scenario -------------------- */ false,
    /** 42 = Set Doodad State                       */ false, /** 43 = Set Invincibility                      */ false, /** 44 = Create Unit                            */ false,
    /** 45 = Set Deaths --------------------------- */ false, /** 46 = Order -------------------------------- */ true , /** 47 = Comment ------------------------------ */ false,
    /** 48 = Give Units to Player                   */ false, /** 49 = Modify Unit Hit Points                 */ false, /** 50 = Modify Unit Energy                     */ false,
    /** 51 = Modify Unit Shield points ------------ */ false, /** 52 = Modify Unit Resource Amount ---------- */ false, /** 53 = Modify Unit Hanger Count ------------- */ false,
    /** 54 = Pause Timer                            */ false, /** 55 = Unpause Timer                          */ false, /** 56 = Draw                                   */ false,
    /** 57 = Set Alliance Status ------------------ */ false, /** 58 = Disable Debug Mode ------------------- */ false, /** 59 = Enable Debug Mode -------------------- */ false
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
    /** 5 = Show Portrait ----------- */ Chk::Action::Flags::UnitTypeUsed,
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

const Chk::Condition & Chk::Trigger::condition(size_t conditionIndex) const
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

const Chk::Action & Chk::Trigger::action(size_t actionIndex) const
{
    if ( actionIndex < MaxActions )
        return actions[actionIndex];
    else
        throw std::out_of_range("actionIndex " + std::to_string(actionIndex) + " exceeds max " + std::to_string(MaxActions-1));
}

void Chk::Condition::toggleDisabled()
{
    if ( (flags & Flags::Disabled) == Flags::Disabled )
        flags &= Flags::xDisabled;
    else
        flags |= Flags::Disabled;
}

bool Chk::Condition::isDisabled() const
{
    return (flags & Flags::Disabled) == Flags::Disabled;
}

inline bool Chk::Condition::locationUsed(size_t locationId) const
{
    return conditionType < NumConditionTypes && conditionUsesLocationArg[conditionType] && this->locationId == locationId;
}

inline void Chk::Condition::markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    if ( conditionType < NumConditionTypes && conditionUsesLocationArg[conditionType] && locationId != Chk::LocationId::NoLocation && locationId <= Chk::TotalLocations )
        locationIdUsed[locationId] = true;
}

void Chk::Condition::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    if ( conditionType < NumConditionTypes )
    {
        if ( conditionUsesLocationArg[conditionType] )
        {
            auto found = locationIdRemappings.find(locationId);
            if ( found != locationIdRemappings.end() )
                locationId = found->second;
        }
    }
}

void Chk::Condition::deleteLocation(size_t locationId)
{
    if ( conditionType < NumConditionTypes && conditionUsesLocationArg[conditionType] && this->locationId == locationId )
        this->locationId = Chk::LocationId::NoLocation;
}

void Chk::Action::toggleDisabled()
{
    if ( (flags & Flags::Disabled) == Flags::Disabled )
        flags &= Flags::xDisabled;
    else
        flags |= Flags::Disabled;
}

Chk::Trigger::Owned & Chk::Trigger::owned(size_t ownerIndex)
{
    if ( ownerIndex < MaxOwners )
        return owners[ownerIndex];
    else
        throw std::out_of_range("ownerIndex " + std::to_string(ownerIndex) + " exceeds max " + std::to_string(MaxOwners-1));
}

const Chk::Trigger::Owned & Chk::Trigger::owned(size_t ownerIndex) const
{
    if ( ownerIndex < MaxOwners )
        return owners[ownerIndex];
    else
        throw std::out_of_range("ownerIndex " + std::to_string(ownerIndex) + " exceeds max " + std::to_string(MaxOwners-1));
}

Chk::Trigger & Chk::Trigger::operator=(const Trigger & trigger)
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

bool Chk::Trigger::preserveTriggerFlagged() const
{   
    return (flags & Flags::PreserveTrigger) == Flags::PreserveTrigger;
}

bool Chk::Trigger::disabled() const
{
    return (flags & Flags::Disabled) == Flags::Disabled;
}

bool Chk::Trigger::ignoreConditionsOnce() const
{
    return (flags & Flags::IgnoreConditionsOnce) == Flags::IgnoreConditionsOnce;
}

bool Chk::Trigger::ignoreWaitSkipOnce() const
{
    return (flags & Flags::IgnoreWaitSkipOnce) == Flags::IgnoreWaitSkipOnce;
}

bool Chk::Trigger::ignoreMiscActionsOnce() const
{
    return (flags & Flags::IgnoreMiscActions) == Flags::IgnoreMiscActions;
}

bool Chk::Trigger::ignoreDefeatDraw() const
{
    return (flags & Flags::IgnoreDefeatDraw) == Flags::IgnoreDefeatDraw;
}

bool Chk::Trigger::pauseFlagged() const
{
    return (flags & Flags::Paused) == Flags::Paused;
}

void Chk::Trigger::setPreserveTriggerFlagged(bool preserved)
{
    if ( preserved )
        flags |= Flags::PreserveTrigger;
    else
        flags &= ~Flags::PreserveTrigger;
}

void Chk::Trigger::setDisabled(bool disabled)
{
    if ( disabled )
        flags |= Flags::Disabled;
    else
        flags &= ~Flags::Disabled;
}

void Chk::Trigger::setIgnoreConditionsOnce(bool ignoreConditionsOnce)
{
    if ( ignoreConditionsOnce )
        flags |= Flags::IgnoreConditionsOnce;
    else
        flags &= ~Flags::IgnoreConditionsOnce;
}

void Chk::Trigger::setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce)
{
    if ( ignoreWaitSkipOnce )
        flags |= Flags::IgnoreWaitSkipOnce;
    else
        flags &= ~Flags::IgnoreWaitSkipOnce;
}

void Chk::Trigger::setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce)
{
    if ( ignoreMiscActionsOnce )
        flags |= Flags::IgnoreMiscActions;
    else
        flags &= ~Flags::IgnoreMiscActions;
}

void Chk::Trigger::setIgnoreDefeatDraw(bool ignoreDefeatDraw)
{
    if ( ignoreDefeatDraw )
        flags |= Flags::IgnoreDefeatDraw;
    else
        flags &= ~Flags::IgnoreDefeatDraw;
}

void Chk::Trigger::setPauseFlagged(bool pauseFlagged)
{
    if ( pauseFlagged )
        flags |= Flags::Paused;
    else
        flags &= ~Flags::Paused;
}

size_t Chk::Trigger::getExtendedDataIndex() const
{
    u32 possibleExtendedData = (u32 &)owners[22];
    if ( (possibleExtendedData & ExtendedTrigDataIndex::CheckExtended) > 0 )
    {
        u32 extendedDataIndex = possibleExtendedData & ExtendedTrigDataIndex::GetIndex;
        if ( extendedDataIndex > 0 )
            return (size_t)extendedDataIndex;
    }
    return ExtendedTrigDataIndex::None;
}

void Chk::Trigger::setExtendedDataIndex(size_t extendedDataIndex)
{
    u32 sizedExtendedDataIndex = (u32)extendedDataIndex;
    if ( sizedExtendedDataIndex == 0 || sizedExtendedDataIndex == 1 || sizedExtendedDataIndex == 256 ||
        sizedExtendedDataIndex == 257 || sizedExtendedDataIndex == 65536 || sizedExtendedDataIndex == 65537 ||
        sizedExtendedDataIndex == 65792 || sizedExtendedDataIndex == 65793 || sizedExtendedDataIndex > 0x00FFFFFF ||
        sizedExtendedDataIndex > ExtendedTrigDataIndex::MaxIndex )
    {
        throw std::invalid_argument("Extended trigger data index " + std::to_string(sizedExtendedDataIndex) + " is invalid!");
    }
    else
        (u32 &)owners[22] = (((u32 &)owners[22]) & 0x1010101) | (u32)extendedDataIndex;
}

void Chk::Trigger::clearExtendedDataIndex()
{
    (u32 &)owners[22] = 0;
}

size_t Chk::Trigger::numUsedConditions() const
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxConditions; i++ )
    {
        if ( conditions[i].conditionType != Chk::Condition::Type::NoCondition )
            total ++;
    }
    return total;
}

size_t Chk::Trigger::getComment() const
{
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( actions[i].actionType == Chk::Action::Type::Comment && !actions[i].isDisabled() )
            return actions[i].stringId;
    }
    return Chk::StringId::NoString;
}

size_t Chk::Trigger::numUsedActions() const
{
    size_t total = 0;
    for ( size_t i=0; i<Chk::Trigger::MaxActions; i++ )
    {
        if ( actions[i].actionType != Chk::Action::Type::NoAction )
            total ++;
    }
    return total;
}

bool Chk::Trigger::locationUsed(size_t locationId) const
{
    for ( size_t i=0; i<MaxConditions; i++ )
    {
        if ( conditions[i].locationUsed(locationId) )
            return true;
    }
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].locationUsed(locationId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::stringUsed(size_t stringId, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].stringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

bool Chk::Trigger::gameStringUsed(size_t stringId, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].gameStringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

bool Chk::Trigger::commentStringUsed(size_t stringId) const
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].commentStringUsed(stringId) )
            return true;
    }
    return false;
}

bool Chk::Trigger::briefingStringUsed(size_t stringId, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
    {
        if ( actions[i].briefingStringUsed(stringId, userMask) )
            return true;
    }
    return false;
}

void Chk::Trigger::markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const
{
    for ( size_t i=0; i<MaxConditions; i++ )
        conditions[i].markUsedLocations(locationIdUsed);

    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedLocations(locationIdUsed);
}

void Chk::Trigger::markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedStrings(stringIdUsed, userMask);
}

void Chk::Trigger::markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedGameStrings(stringIdUsed, userMask);
}

void Chk::Trigger::markUsedCommentStrings(std::bitset<Chk::MaxStrings> & stringIdUsed) const
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedCommentStrings(stringIdUsed);
}

void Chk::Trigger::markUsedBriefingStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask) const
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].markUsedBriefingStrings(stringIdUsed, userMask);
}

void Chk::Trigger::remapLocationIds(const std::map<u32, u32> & locationIdRemappings)
{
    for ( size_t i=0; i<MaxConditions; i++ )
        conditions[i].remapLocationIds(locationIdRemappings);

    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapLocationIds(locationIdRemappings);
}

void Chk::Trigger::remapStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapStringIds(stringIdRemappings);
}

void Chk::Trigger::remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings)
{
    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].remapBriefingStringIds(stringIdRemappings);
}

void Chk::Trigger::deleteLocation(size_t locationId)
{
    for ( size_t i=0; i<MaxConditions; i++ )
        conditions[i].deleteLocation(locationId);

    for ( size_t i=0; i<MaxActions; i++ )
        actions[i].deleteLocation(locationId);
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

Chk::ExtendedTrigData::ExtendedTrigData() : commentStringId(0), notesStringId(0), groupId(GroupId::None), trigNum(TrigNum::None), maskId(MaskId::None)
{

}

bool Chk::ExtendedTrigData::isBlank() const
{
    return commentStringId == 0 && notesStringId == 0 && groupId == GroupId::None && trigNum == TrigNum::None && maskId == MaskId::None;
}

Chk::TriggerGroup::TriggerGroup() : groupExpanded(false), groupHidden(false), templateInstanceId(0), commentStringId(0), notesStringId(0), parentGroupId(Id::TopLevelGroup)
{

}

Chk::StringProperties::StringProperties()
    : Chk::StringProperties(0, 0, 0, false, false, false, false, false, 8)
{

}

Chk::StringProperties::StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size)
    : red(red), green(green), blue(blue)
{
    u8 flags = (isUsed ? StrFlags::isUsed : 0) | (hasPriority ? StrFlags::hasPriority : 0) | (isBold ? StrFlags::bold : 0) |
        (isUnderlined ? StrFlags::underlined : 0) | (isItalics ? StrFlags::italics : 0);

    constexpr u32 maxFontSize = baseFontSize + 7*fontStepSize;
    if ( size < baseFontSize || size > maxFontSize )
        throw InvalidSize(size, baseFontSize, fontStepSize);

    size -= baseFontSize;
    if ( size > 4*fontStepSize )
    {
        flags |= StrFlags::sizePlusFourSteps;
        size -= 4*fontStepSize;
    }
    if ( size > 2*fontStepSize )
    {
        flags |= StrFlags::sizePlusTwoSteps;
        size -= 2*fontStepSize;
    }
    if ( size > fontStepSize )
    {
        flags |= StrFlags::sizePlusOneStep;
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



bool Chk::Sprite::isDrawnAsSprite() const
{
    return (flags & SpriteFlags::DrawAsSprite) == SpriteFlags::DrawAsSprite;
}

bool Chk::Cuwp::isCloaked() const
{
    return (unitStateFlags & State::Cloak) == State::Cloak;
}

bool Chk::Cuwp::isBurrowed() const
{
    return (unitStateFlags & State::Burrow) == State::Burrow;
}

bool Chk::Cuwp::isInTransit() const
{
    return (unitStateFlags & State::InTransit) == State::InTransit;
}

bool Chk::Cuwp::isHallucinated() const
{
    return (unitStateFlags & State::Hallucinated) == State::Hallucinated;
}

bool Chk::Cuwp::isInvincible() const
{
    return (unitStateFlags & State::Invincible) == State::Invincible;
}

void Chk::Cuwp::setCloaked(bool cloaked)
{
    unitStateFlags = cloaked ? unitStateFlags | State::Cloak : unitStateFlags & State::xCloak;
}

void Chk::Cuwp::setBurrowed(bool burrowed)
{
    unitStateFlags = burrowed ? unitStateFlags | State::Burrow : unitStateFlags & State::xBurrow;
}

void Chk::Cuwp::setInTransit(bool inTransit)
{
    unitStateFlags = inTransit ? unitStateFlags | State::InTransit : unitStateFlags & State::xInTransit;
}

void Chk::Cuwp::setHallucinated(bool hallucinated)
{
    unitStateFlags = hallucinated ? unitStateFlags | State::Hallucinated : unitStateFlags & State::xHallucinated;
}

void Chk::Cuwp::setInvincible(bool invincible)
{
    unitStateFlags = invincible ? unitStateFlags | State::Invincible : unitStateFlags & State::xInvincible;
}

Chk::Location::Location() : left(0), top(0), right(0), bottom(0), stringId(0), elevationFlags(0)
{

}

bool Chk::Location::isBlank() const
{
    return left == 0 && top == 0 && right == 0 && bottom == 0 && stringId == 0 && elevationFlags == 0;
}

std::string Chk::getNameString(Chk::SectionName sectionName)
{
    auto foundSectionNameString = sectionNameStrings.find(sectionName);
    if ( foundSectionNameString != sectionNameStrings.end() )
        return foundSectionNameString->second;
    else
    {
        char* data = reinterpret_cast<char*>(&sectionName);
        char result[5] { data[0], data[1], data[2], data[3], '\0' };
        return std::string(result);
    }
}

Chk::SectionValidationException::SectionValidationException(SectionName sectionName, std::string error) :
    error("Validation error in chk section " + Chk::getNameString(sectionName) + " : " + error)
{

}

Chk::MaximumStringsExceeded::MaximumStringsExceeded(std::string sectionName, size_t numStrings, size_t maxStrings)
    : StringException("Error with " + sectionName + " section: " + std::to_string(numStrings)
        + " strings given, but the " + sectionName + " section can have " + std::to_string(maxStrings) + " strings max.")
{
    
}

Chk::MaximumStringsExceeded::MaximumStringsExceeded()
    : StringException("Error placing string, limit of 65536 strings exceeded!")
{

}

Chk::InsufficientStringCapacity::InsufficientStringCapacity(std::string sectionName, size_t numStrings, size_t requestedCapacity, bool autoDefragment)
    : StringException("Error changing the " + sectionName + " section string capacity to " + std::to_string(requestedCapacity)
        + (autoDefragment || numStrings > requestedCapacity ? " there are " + std::to_string(numStrings) + " strings used in the map." : " the " + sectionName + " section would need to be defragmented."))
{

}

Chk::MaximumCharactersExceeded::MaximumCharactersExceeded(std::string sectionName, size_t numCharacters, size_t characterSpaceSize)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numCharacters) + " characters given, which cannot fit in the "
        + std::to_string(characterSpaceSize) + " bytes of pointable character space using selected compression.")
{

}

Chk::MaximumOffsetAndCharsExceeded::MaximumOffsetAndCharsExceeded(std::string sectionName, size_t numStrings, size_t numCharacters, size_t sectionSpace)
    : StringException("Error serializing " + sectionName + " section: " + std::to_string(numStrings) + " strings worth of offsets and " + std::to_string(numCharacters)
        + " characters given, which cannot together fit in the " + std::to_string(sectionSpace) + " bytes of section space using selected compression.")
{

}

StrProp::StrProp() : red(0), green(0), blue(0), isUsed(false), hasPriority(false), isBold(false), isUnderlined(false), isItalics(false), size(Chk::baseFontSize)
{

}

StrProp::StrProp(Chk::StringProperties stringProperties) :
    red(stringProperties.red), green(stringProperties.green), blue(stringProperties.blue),
    isUsed((stringProperties.flags & Chk::StrFlags::isUsed) == Chk::StrFlags::isUsed), hasPriority((stringProperties.flags & Chk::StrFlags::hasPriority) == Chk::StrFlags::hasPriority),
    isBold((stringProperties.flags & Chk::StrFlags::bold) == Chk::StrFlags::bold), isUnderlined((stringProperties.flags & Chk::StrFlags::underlined) == Chk::StrFlags::underlined),
    isItalics((stringProperties.flags & Chk::StrFlags::italics) == Chk::StrFlags::italics),
    size(Chk::baseFontSize +
        ((stringProperties.flags & Chk::StrFlags::sizePlusFourSteps) == Chk::StrFlags::sizePlusFourSteps) ? 4*Chk::fontStepSize : 0 +
        ((stringProperties.flags & Chk::StrFlags::sizePlusTwoSteps) == Chk::StrFlags::sizePlusTwoSteps) ? 2*Chk::fontStepSize : 0 +
        ((stringProperties.flags & Chk::StrFlags::sizePlusOneStep) == Chk::StrFlags::sizePlusOneStep) ? 1*Chk::fontStepSize : 0)
{
    
}

StrProp::StrProp(u8 red, u8 green, u8 blue, u32 size, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics)
    : red(red), green(green), blue(blue), size(size), isUsed(isUsed), hasPriority(hasPriority), isBold(isBold), isUnderlined(isUnderlined), isItalics(isItalics)
{

}

ScStr::ScStr() : strProp()
{
    allocation.push_back('\0');
    this->str = &allocation[0];
}

ScStr::ScStr(const std::string & str) : strProp()
{
    for ( const char & c : str )
        allocation.push_back(c);

    if ( allocation.size() == 0 || allocation.back() != '\0' )
        allocation.push_back('\0');

    this->str = &allocation[0];
}

ScStr::ScStr(const std::string & str, const StrProp & strProp) : strProp(strProp)
{
    for ( const char & c : str )
        allocation.push_back(c);

    if ( allocation.size() == 0 || allocation.back() != '\0' )
        allocation.push_back('\0');

    this->str = &allocation[0];
}

ScStr::ScStr(const char* str) : ScStr(std::string(str))
{

}

ScStr::ScStr(const u8* str) : ScStr(std::string((const char*)str))
{

}

ScStr::ScStr(const u8* str, size_t length) : ScStr(std::string((const char*)str, length))
{

}

ScStr::ScStr(const std::vector<u8> & strBytes) : ScStr(std::string((const char*)&strBytes[0], strBytes.size()))
{

}

bool ScStr::empty() const
{
    return parentStr == nullptr ? allocation.size() <= 1 : parentStr->empty();
}

size_t ScStr::length() const
{
    return parentStr == nullptr ? (allocation.size() > 0 ? allocation.size()-1 : 0) : strlen(str);
}

StrProp & ScStr::properties()
{
    return strProp;
}

const StrProp & ScStr::properties() const
{
    return strProp;
}

template <typename StringType>
int ScStr::compare(const StringType & str) const
{
    RawString rawStr;
    convertStr<StringType, RawString>(str, rawStr);
    return strcmp(this->str, rawStr.c_str());
}
template int ScStr::compare<RawString>(const RawString & str) const;
template int ScStr::compare<EscString>(const EscString & str) const;
template int ScStr::compare<ChkdString>(const ChkdString & str) const;
template int ScStr::compare<SingleLineChkdString>(const SingleLineChkdString & str) const;

template <typename StringType>
StringType ScStr::toString() const
{
    StringType destStr {};
    convertStr<RawString, StringType>(str, destStr);
    return destStr;
}
template RawString ScStr::toString<RawString>() const;
template EscString ScStr::toString<EscString>() const;
template ChkdString ScStr::toString<ChkdString>() const;
template SingleLineChkdString ScStr::toString<SingleLineChkdString>() const;

ScStr* ScStr::getParentStr() const
{
    return parentStr;
}

ScStr* ScStr::getChildStr() const
{
    return childStr;
}

bool ScStr::adopt(ScStr* lhs, ScStr* rhs)
{
    if ( rhs != nullptr && lhs != nullptr && rhs->parentStr == nullptr || lhs->parentStr == nullptr )
    {
        size_t lhsLength = lhs->parentStr == nullptr ? lhs->allocation.size()-1 : strlen(lhs->str);
        size_t rhsLength = rhs->parentStr == nullptr ? rhs->allocation.size()-1 : strlen(rhs->str);
        if ( rhsLength > lhsLength ) // The length of rhs is greater
        {
            const char* rhsSubString = &rhs->str[rhsLength-lhsLength];
            if ( strcmp(lhs->str, rhsSubString) == 0 ) // rhs can adopt lhs
                ScStr::adopt(rhs, lhs, rhsLength, lhsLength, rhsSubString);
        }
        else // lhsLength >= rhsLength - the length of lhs is greater or equal to rhs
        {
            const char* lhsSubString = &lhs->str[lhsLength-rhsLength];
            if ( strcmp(rhs->str, lhsSubString) == 0 ) // lhs can adopt rhs
                ScStr::adopt(lhs, rhs, lhsLength, rhsLength, lhsSubString);
        }
    }
    return false;
}

void ScStr::disown()
{
    if ( childStr != nullptr )
    {
        childStr->parentStr = parentStr; // Make child's parent its grandparent (or nullptr if none)
        childStr = nullptr;
    }
    if ( parentStr != nullptr )
    {
        parentStr->childStr = childStr; // Make parent's child its grandchild (or nullptr if none)
        parentStr = nullptr;
    }
}

void ScStr::adopt(ScStr* parent, ScStr* child, size_t parentLength, size_t childLength, const char* parentSubString)
{
    ScStr* nextChild = parent->childStr; // Some descendent of parent may be a more suitable parent for this child
    while ( nextChild != nullptr && nextChild->length() > childLength )
    {
        parent = nextChild;
        nextChild = parent->childStr;
    }
    if ( nextChild != nullptr ) // There are more children smaller than this one that this child should adopt
    {
        child->childStr = parent->childStr; // Make child's child the smaller child
        child->childStr->parentStr = child; // Make parent for that same smaller child this child
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
    else // nextChild == nullptr, there are no more children smaller than this one
    {
        child->parentStr = parent; // Make child's parent the smallest suitable parent selected above
        parent->childStr = child; // Assign the child as child to the parent
    }
}
