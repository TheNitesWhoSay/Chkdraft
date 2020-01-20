#ifndef CHK_H
#define CHK_H
#include "Basics.h"
#include "Sc.h"
#include <bitset>

#undef PlaySound

/** 
    This file defines all static structures, constants, and enumerations specific to scenario files (.chk)

    Note that many constants and enumerations may be more general to StarCraft (see Sc.h), than specific to scenario files.

    This same sort of information in wiki form can be found here: http://www.staredit.net/wiki/index.php/Scenario.chk
*/

namespace Chk {
#pragma pack(push, 1)

    struct Unit; struct IsomEntry; struct Doodad; struct Sprite; struct Cuwp; struct Location; struct Condition; struct Action; struct Trigger; struct StringProperties;
    using UnitPtr = std::shared_ptr<Unit>; using IsomEntryPtr = std::shared_ptr<IsomEntry>; using DoodadPtr = std::shared_ptr<Doodad>; using SpritePtr = std::shared_ptr<Sprite>;
    using CuwpPtr = std::shared_ptr<Cuwp>; using LocationPtr = std::shared_ptr<Location>; using ConditionPtr = std::shared_ptr<Condition>; using ActionPtr = std::shared_ptr<Action>;
    using TriggerPtr = std::shared_ptr<Trigger>; using StringPropertiesPtr = std::shared_ptr<StringProperties>;

    enum_t(Type, u32, { // u32
        RAWS = 0x53574152, // 1.04 StarCraft and above ("hybrid")
        RAWB = 0x42574152 // Brood War
    });
    
    enum_t(Version, u16, { // u16
        StarCraft_Original = 59,
        StarCraft_Hybrid = 63, // 1.04 StarCraft and above ("hybrid")
        StarCraft_BroodWar = 205 // Brood War
    });
    
    enum_t(IVersion, u16, { // u16
        Beta = 9,
        Current = 10
    });
    
    enum_t(I2Version, u16, { // u16
        StarCraft_1_04 = 11 // 1.04 StarCraft and above ("hybrid") or Brood War
    });
    
    enum_t(ValidationOpCodes, u8, { // u8
        XOR_Shifted_Sections = 0, // XOR the current hash with total values of OWNR, SIDE*256, FORC*65536
        Add_Shifted_Sections = 1, // Adds to the current hash with the total values of OWNR, SIDE*256, FORC*65536
        Sub_Shifted_Sections = 2, // Subtracts from the current hash with the total values of OWNR, SIDE*256, FORC*65536
        XOR_Sections = 3, // XOR the current hash with the VCOD table at index OWNR, SIDE, FORC, and 0
        XOR_Sections_04 = 4, // Same as XOR_Sections
        XOR_Sections_05 = 5, // Same as XOR_Sections
        ORs_And_Shifts = 6, // Complex function consisting of ORs and shifts
        ORs_And_Reverse_Shifts = 7, // Same as ORS_AND_SHIFTS with shift direction inverted
        NOOP = 8 // No operation
    });
    
    constexpr u32 TotalValidationSeeds = 256;
    constexpr u32 TotalValidationOpCodes = 16;
    
    enum_t(Race, u8, { // u8
        Zerg = 0,
        Terran = 1,
        Protoss = 2,
        Independent = 3,
        Neutral = 4,
        UserSelectable = 5,
        Random = 6,
        Inactive = 7
    });
    
    enum_t(Available, u8, { // u8
        No = 0, // Unit/technology is not availble for production/research
        Yes = 1 // Unit/technology is available for production/research
    });
    
    enum_t(UseDefault, u8, { // u8
        No = 0, // Player does not use defaults for this unit/upgrade/technology or unit/upgrade/technlology does not use default settings
        Yes = 1 // Player uses defaults for this unit/upgrade/technology or unit/upgrade/technology does not use default settings
    });
    
    enum_t(Researched, u8, { // u8
        No = 0, // Technology is not researched
        Yes = 1 // Technology is researched
    });
    
    __declspec(align(1)) struct Unit {
        enum_t(RelationFlag, u16, { // u16
            NydusLink = BIT_9,
            AddonLink = BIT_10
        });
        enum_t(State, u16, { // u16
            Cloak = BIT_0,
            Burrow = BIT_1,
            InTransit = BIT_2,
            Hallucinated = BIT_3,
            Invincible = BIT_4,

            xCloak = x16BIT_0,
            xBurrow = x16BIT_1,
            xInTransit = x16BIT_2,
            xHallucinate = x16BIT_3,
            xInvincible = x16BIT_4
        });
        enum_t(ValidField, u16, { // u16
            Owner = BIT_0, 
            Hitpoints = BIT_1,
            Shields = BIT_2,
            Energy = BIT_3,
            Resources = BIT_4,
            Hanger = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHanger = x16BIT_5
        });
        enum class Field { // Typeless
            ClassId, Xc, Yc, Type, RelationFlags, ValidStateFlags, ValidFieldFlags, Owner, HitpointPercent, ShieldPercent, EnergyPercent, ResourceAmount, HangerAmount, StateFlags, Unused, RelationClassId
        };

        u32 classId; // Unique number used to identify this unit for related units (may be an addon or the building that has an addon)
        u16 xc;
        u16 yc;
        Sc::Unit::Type type; // u16
        u16 relationFlags;
        u16 validStateFlags;
        u16 validFieldFlags;
        u8 owner;
        u8 hitpointPercent;
        u8 shieldPercent;
        u8 energyPercent;
        u32 resourceAmount;
        u16 hangerAmount;
        u16 stateFlags;
        u32 unused;
        u32 relationClassId; // classId of related unit (may be an addon or the building that has an addon)
    }; // 36 (0x24) bytes

    __declspec(align(1)) struct IsomEntry { // 8 bytes
        u16 left;
        u16 top;
        u16 right;
        u16 bottom;
    };
    
    __declspec(align(1)) struct Doodad
    {
        enum_t(Enabled, u8, {
            Enabled = 0, // Doodad is enabled
            Disabled = 1 // Doodad is disabled
        });

        Sc::Terrain::Doodad::Type type;
        u16 xc;
        u16 yc;
        u8 owner;
        Enabled enabled; 
    };

    __declspec(align(1)) struct Sprite
    {
        enum_t(SpriteFlags, u16, {
            DrawAsSprite = BIT_12, // If deselected this is a SpriteUnit
            Disabled = BIT_15 // Only valid if draw as sprite is unchecked, disables the unit
        });

        bool isDrawnAsSprite();

        Type type;
        u16 xc;
        u16 yc;
        u8 owner;
        u8 unused;
        u16 flags;
    };

    enum_t(FogOfWarPlayers, u8, { // u8
        Visible = 0,
        Player1 = BIT_0,
        Player2 = BIT_1,
        Player3 = BIT_2,
        Player4 = BIT_3,
        Player5 = BIT_4,
        Player6 = BIT_5,
        Player7 = BIT_6,
        Player8 = BIT_7,
        Opaque = u8_max
    });

    constexpr size_t MaxStrings = u16_max + 1; // Including stringId:0
    constexpr size_t MaxKStrings = MaxStrings;
    
    enum_t(StringId, size_t, { // size_t
        NoString = 0,
        DefaultFileName = 0,
        DefaultDescription = 0,
        UnusedSound = 0
    });

    enum_t(CuwpUsed, u8, { // u8
        No = 0, // CUWP slot is unused
        Yes = 1 // CUWP slot is used
    });

    __declspec(align(1)) struct Cuwp
    {
        enum_t(State, u16, { // u16
            Cloak = BIT_0,
            Burrow = BIT_1,
            InTransit = BIT_2,
            Hallucinated = BIT_3,
            Invincible = BIT_4,

            xCloak = x16BIT_0,
            xBurrow = x16BIT_1,
            xInTransit = x16BIT_2,
            xHallucinated = x16BIT_3,
            xInvincible = x16BIT_4
        });
        enum_t(ValidField, u16, { // u16
            Owner = BIT_0, 
            Hitpoints = BIT_1,
            Shields = BIT_2,
            Energy = BIT_3,
            Resources = BIT_4,
            Hanger = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHanger = x16BIT_5
        });
        
        bool isCloaked();
        bool isBurrowed();
        bool isInTransit();
        bool isHallucinated();
        bool isInvincible();
        
        void setCloaked(bool cloaked);
        void setBurrowed(bool burrowed);
        void setInTransit(bool inTransit);
        void setHallucinated(bool hallucinated);
        void setInvincible(bool invincible);

        u16 validUnitStateFlags;
        u16 validUnitFieldFlags;
        u8 owner;
        u8 hitpointPercent;
        u8 shieldPercent;
        u8 energyPercent;
        u32 resourceAmount;
        u16 hangerAmount;
        u16 unitStateFlags;
        u32 unknown;
    };

    constexpr size_t TotalOriginalLocations = 64;
    constexpr size_t TotalLocations = 255;

    enum_t(LocationId, size_t, { // size_t
        NoLocation = 0,
        Anywhere = 64
    });
    
    __declspec(align(1)) struct Location
    {
        enum_t(Elevation, u16, {
            LowElevation = BIT_0,
            MediumElevation = BIT_1,
            HighElevation = BIT_2,
            LowAir = BIT_3,
            MediumAir = BIT_4,
            HighAir = BIT_5,
            All = LowElevation | MediumElevation | HighElevation | LowAir | MediumAir | HighAir
        });
        enum class Field { // Typeless
            Left, Top, Right, Bottom, StringId, ElevationFlags
        };

        Location();

        bool isBlank();
        
        u32 left;
        u32 top;
        u32 right;
        u32 bottom;
        u16 stringId;
        u16 elevationFlags;
    };

    constexpr u32 TotalForces = 4;
    
    enum_t(Force, u8, { // u8
        Force1 = 0,
        Force2 = 1,
        Force3 = 2,
        Force4 = 3
    });

    enum_t(ForceFlags, u8, { // u8
        RandomizeStartLocation = BIT_0,
        RandomAllies = BIT_1,
        AlliedVictory = BIT_2,
        SharedVision = BIT_3,

        xRandomizeStartLocation = x8BIT_0,
        xRandomAllies = x8BIT_1,
        xAlliedVictory = x8BIT_2,
        xSharedVision = x8BIT_3,

        All = (RandomizeStartLocation | RandomAllies | AlliedVictory | SharedVision)
    });
    
    constexpr u32 TotalSounds = 512;
    
    constexpr u32 TotalSwitches = 256;

    constexpr u32 MaximumTriggers = 894785; // 894784 real triggers at 2400 bytes each could fit in MaxChkSectionSize: s32_max, maybe last couple bytes would be padded adding a partial trigger

    __declspec(align(1)) struct Condition
    {
        static constexpr size_t NumConditionTypes = 24;
        static constexpr size_t MaxArguments = 9;
        enum_t(Type, u8, { // u8
            Accumulate = 4,
            Always = 22,
            Bring = 3,
            Command = 2,
            CommandTheLeast = 16,
            CommandTheLeastAt = 17,
            CommandTheMost = 6,
            CommandTheMostAt = 7,
            CountdownTimer = 1,
            Deaths = 15,
            ElapsedTime = 12,
            HighestScore = 9,
            Kill = 5,
            LeastKills = 18,
            LeastResources = 20,
            LowestScore = 19,
            MostKills = 8,
            MostResources = 10,
            Never = 23,
            IsBriefing = 13,
            NoCondition = 0,
            Opponents = 14,
            Score = 21,
            Switch = 11
        });
        enum_t(VirtualType, s32, { // s32
            Accumulate = 4,
            Always = 22,
            Bring = 3,
            Command = 2,
            CommandTheLeast = 16,
            CommandTheLeastAt = 17,
            CommandTheMost = 6,
            CommandTheMostAt = 7,
            CountdownTimer = 1,
            Deaths = 15,
            ElapsedTime = 12,
            HighestScore = 9,
            Kill = 5,
            LeastKills = 18,
            LeastResources = 20,
            LowestScore = 19,
            MostKills = 8,
            MostResources = 10,
            Never = 23,
            NeverAlt = 13,
            NoCondition = 0,
            Opponents = 14,
            Score = 21,
            Switch = 11,

            Custom = -1,
            Memory = -2,

            Indeterminate = s32_min
        });
        enum_t(ExtendedBaseType, s32, { // s32
            Memory = (u8)Type::Deaths, // Deaths
        });
        enum_t(Flags, u8, { // u8
            Disabled = BIT_1, // If set, the trigger condition is disabled/ignored
            UnitTypeUsed = BIT_4, // If set the unitType field is used

            xDisabled = x8BIT_1
        });
        enum class ArgType { // Typeless
            NoType = 0,
            Unit = 1,
            Location = 2,
            Player = 3,
            Amount = 4,
            NumericComparison = 5,
            ResourceType = 6,
            ScoreType = 7,
            Switch = 8,
            SwitchState = 9,
            Comparison = 10, // NumericComparison, SwitchState
            ConditionType = 11,
            TypeIndex = 12, // ResourceType, ScoreType, Switch
            Flags = 13,
            MaskFlag = 14,
            MemoryOffset = 15
        };
        enum class ArgField : u32 { // u32
            LocationId = 0,
            Player = 4,
            Amount = 8,
            UnitType = 12,
            Comparison = 14,
            ConditionType = 15,
            TypeIndex = 16,
            Flags = 17,
            MaskFlag = 18,
            NoField = u32_max
        };
        enum_t(Amount, u32, { // u32
            All = 0
        });
        enum_t(Comparison, u8, { // u8
            AtLeast = 0,
            AtMost = 1,
            Set = 2,
            NotSet = 3,
            Exactly = 10
        });
        enum_t(ScoreType, u8, { // u8
            Total = 0,
            Units = 1,
            Buildings = 2,
            UnitsAndBuildings = 3,
            Kills = 4,
            Razings = 5,
            KillsAndRazings = 6,
            Custom = 7
        });
        enum_t(MaskFlag, u16, { // u16
            Enabled = 0x4353, // "SC" in little-endian; 'S' = 0x53, 'C' = 0x43
            Disabled = 0
        });
        struct Argument {
            ArgType type;
            ArgField field;
        };
        struct VirtualCondition {
            VirtualType baseConditionType;
            Argument arguments[MaxArguments];
        };

        u32 locationId; // 1 based, is also the bitmask for deaths
        u32 player;
        u32 amount;
        Sc::Unit::Type unitType;
        Comparison comparison;
        Type conditionType;
        u8 typeIndex; // Resource type/score type/switch num
        u8 flags;
        MaskFlag maskFlag; // Set to "SC" (0x53, 0x43) for masked deaths; leave as zero otherwise

        void ToggleDisabled();
        bool isDisabled();
        inline bool locationUsed(size_t locationId);
        inline void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed);
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void deleteLocation(size_t locationId);
        static ArgType getClassicArgType(Type conditionType, size_t argNum);
        static ArgType getClassicArgType(VirtualType conditionType, size_t argNum);

        static Argument classicArguments[NumConditionTypes][MaxArguments];
        static Argument textArguments[NumConditionTypes][MaxArguments];
        static u8 defaultFlags[NumConditionTypes];
        static std::unordered_map<VirtualType, VirtualCondition> virtualConditions;
        static bool conditionUsesLocationArg[NumConditionTypes];
    }; // 20 (0x14) bytes

    __declspec(align(1)) struct Action
    {
        static constexpr size_t NumActionTypes = 60;
        static constexpr size_t NumBriefingActionTypes = 10;
        static constexpr size_t InternalDataBytes = 3;
        static constexpr size_t MaxArguments = 11;
        enum_t(Type, u8, { // u8
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisplayTextMessage = 9,
            Draw = 56,
            GiveUnitsToPlayer = 48,
            KillUnit = 22,
            KillUnitAtLocation = 23,
            LeaderboardCtrlAtLoc = 18,
            LeaderboardCtrl = 17,
            LeaderboardGreed = 40,
            LeaderboardKills = 20,
            LeaderboardPoints = 21,
            LeaderboardResources = 19,
            LeaderboardCompPlayers = 32,
            LeaderboardGoalCtrlAtLoc = 34,
            LeaderboardGoalCtrl = 33,
            LeaderboardGoalKills = 36,
            LeaderboardGoalPoints = 37,
            LeaderboardGoalResources = 35,
            MinimapPing = 28,
            ModifyUnitEnergy = 50,
            ModifyUnitHangerCount = 53,
            ModifyUnitHitpoints = 49,
            ModifyUnitResourceAmount = 52,
            ModifyUnitShieldPoints = 51,
            MoveLocation = 38,
            MoveUnit = 39,
            MuteUnitSpeech = 30,
            NoAction = 0,
            Order = 46,
            PauseGame = 5,
            PauseTimer = 54,
            PlaySound = 8,
            PreserveTrigger = 3,
            RemoveUnit = 24,
            RemoveUnitAtLocation = 25,
            RunAiScript = 15,
            RunAiScriptAtLocation = 16,
            SetAllianceStatus = 57,
            SetCountdownTimer = 14,
            SetDeaths = 45,
            SetDoodadState = 42,
            SetInvincibility = 43,
            SetMissionObjectives = 12,
            SetNextScenario = 41,
            SetResources = 26,
            SetScore = 27,
            SetSwitch = 13,
            TalkingPortrait = 29,
            Transmission = 7,
            UnmuteUnitSpeech = 31,
            UnpauseGame = 6,
            UnpauseTimer = 55,
            Victory = 1,
            Wait = 4,
            LastAction = 59,

            BriefingNoAction = 0,
            BriefingWait = 1,
            BriefingPlaySound = 2,
            BriefingTextMessage = 3,
            BriefingMissionObjectives = 4,
            BriefingShowPortrait = 5,
            BriefingHidePortrait = 6,
            BriefingDisplayTalkingPortrait = 7,
            BriefingTransmission = 8,
            BriefingEnableSkipTutorial = 9
        });
        enum_t(VirtualType, s32, { // s32
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisplayTextMessage = 9,
            Draw = 56,
            GiveUnitsToPlayer = 48,
            KillUnit = 22,
            KillUnitAtLocation = 23,
            LeaderboardCtrlAtLoc = 18,
            LeaderboardCtrl = 17,
            LeaderboardGreed = 40,
            LeaderboardKills = 20,
            LeaderboardPoints = 21,
            LeaderboardResources = 19,
            LeaderboardCompPlayers = 32,
            LeaderboardGoalCtrlAtLoc = 34,
            LeaderboardGoalCtrl = 33,
            LeaderboardGoalKills = 36,
            LeaderboardGoalPoints = 37,
            LeaderboardGoalResources = 35,
            MinimapPing = 28,
            ModifyUnitEnergy = 50,
            ModifyUnitHangerCount = 53,
            ModifyUnitHitpoints = 49,
            ModifyUnitResourceAmount = 52,
            ModifyUnitShieldPoints = 51,
            MoveLocation = 38,
            MoveUnit = 39,
            MuteUnitSpeech = 30,
            NoAction = 0,
            Order = 46,
            PauseGame = 5,
            PauseTimer = 54,
            PlaySound = 8,
            PreserveTrigger = 3,
            RemoveUnit = 24,
            RemoveUnitAtLocation = 25,
            RunAiScript = 15,
            RunAiScriptAtLocation = 16,
            SetAllianceStatus = 57,
            SetCountdownTimer = 14,
            SetDeaths = 45,
            SetDoodadState = 42,
            SetInvincibility = 43,
            SetMissionObjectives = 12,
            SetNextScenario = 41,
            SetResources = 26,
            SetScore = 27,
            SetSwitch = 13,
            TalkingPortrait = 29,
            Transmission = 7,
            UnmuteUnitSpeech = 31,
            UnpauseGame = 6,
            UnpauseTimer = 55,
            Victory = 1,
            Wait = 4,

            Custom = -1,
            SetMemory = -2,

            Indeterminate = s32_min,

            BriefingNoAction = 0,
            BriefingWait = 1,
            BriefingPlaySound = 2,
            BriefingTextMessage = 3,
            BriefingMissionObjectives = 4,
            BriefingShowPortrait = 5,
            BriefingHidePortrait = 6,
            BriefingDisplayTalkingPortrait = 7,
            BriefingTransmission = 8,
            BriefingEnableSkipTutorial = 9
        });
        enum_t(ExtendedBaseType, s32, { // s32
            SetMemory = (u8)Type::SetDeaths, // SetDeaths
        });
        enum_t(Flags, u8, { // u8
            Disabled = BIT_1, // If set, the trigger action is disabled/ignored
            AlwaysDisplay = BIT_2, // If set, the text message will display regardless of the users subtitles setting
            UnitPropertiesUsed = BIT_3, // If set the unitProperties field is used
            UnitTypeUsed = BIT_4, // If set the unitType field is used

            xDisabled = x8BIT_1,
            xAlwaysDisplay = x8BIT_2
        });
        enum class ArgType : u32 { // u32
            NoType = 0,
            Location = 1,
            String = 2,
            Player = 3,
            Unit = 4,
            NumUnits = 5,
            CUWP = 6,
            TextFlags = 7,
            Amount = 8,
            ScoreType = 9,
            ResourceType = 10,
            StateMod = 11,
            Percent = 12,
            Order = 13,
            Sound = 14,
            Duration = 15,
            Script = 16,
            AllyState = 17,
            NumericMod = 18,
            Switch = 19,
            SwitchMod = 20,
            Type = 21,
            ActionType = 22,
            SecondaryType = 23,
            Flags = 24,
            Number = 25, // Amount, Group2, LocDest, UnitPropNum, ScriptNum
            TypeIndex = 26, // Unit, ScoreType, ResourceType, AllianceStatus
            SecondaryTypeIndex = 27, // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
            Padding = 28,
            MaskFlag = 29,
            MemoryOffset = 30
        };
        enum class ArgField : u32 { // u32
            LocationId = 0,
            StringId = 4,
            SoundStringId = 8,
            Time = 12,
            Group = 16,
            Number = 20,
            Type = 24,
            ActionType = 26,
            Type2 = 27,
            Flags = 28,
            Padding = 29,
            MaskFlag = 30,
            NoField = u32_max
        };
        enum_t(Amount, u32, { // u32
            All = 0
        });
        enum_t(NumUnits, u8, { // u8
            All = 0
        });
        enum_t(ResourceType, u8, { // u8
            Ore = 0,
            Gas = 1,
            OreAndGas = 2
        });
        enum_t(AllianceStatus, u16, { // u16
            Enemy = 0,
            Ally = 1,
            AlliedVictory = 2
        });
        enum_t(ValueModifier, u8, { // u8
            Enable = 4,
            Enabled = 4,
            Set = 4,
            Disable = 5,
            Diabled = 5,
            Clear = 5,
            Toggle = 6,
            SetTo = 7,
            Add = 8,
            Subtract = 9,
            Randomize = 11
        });
        enum_t(UnitOrders, u8, { // u8
            Move = 0,
            Patrol = 1,
            Attack = 2
        });
        enum_t(MaskFlag, u16, { // u16
            Enabled = 0x4353, // "SC" in little-endian; 'S' = 0x53, 'C' = 0x43
            Disabled = 0
        });
        struct Argument {
            ArgType type;
            ArgField field;
        };
        struct VirtualAction {
            VirtualType baseActionType;
            Argument arguments[MaxArguments];
        };

        u32 locationId; // 1 based, is also the bitmask for deaths
        u32 stringId;
        u32 soundStringId;
        u32 time;
        u32 group; // Group/ZeroBasedBriefingSlot
        u32 number; // Amount/Group2/LocDest/UnitPropNum/ScriptNum
        u16 type; // Unit/score/resource type/alliance status
        Type actionType; // u8
        u8 type2; // Num units/switch action/unit order/modify type
        u8 flags;
        u8 padding;
        MaskFlag maskFlag; // u16, set to "SC" (0x53, 0x43) for masked deaths

        bool isDisabled() { return (flags & Flags::Disabled) == Flags::Disabled; }
        void ToggleDisabled();
        static ArgType getClassicArgType(Type actionType, size_t argNum);
        static ArgType getClassicArgType(VirtualType actionType, size_t argNum);
        inline bool locationUsed(size_t locationId);
        inline bool stringUsed(size_t stringId);
        inline bool gameStringUsed(size_t stringId);
        inline bool commentStringUsed(size_t stringId);
        inline bool briefingStringUsed(size_t stringId);
        inline void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed);
        inline void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        inline void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteLocation(size_t locationId);
        void deleteString(size_t stringId);
        void deleteBriefingString(size_t stringId);

        static Argument classicArguments[NumActionTypes][MaxArguments];
        static Argument textArguments[NumActionTypes][MaxArguments];
        static u8 defaultFlags[NumActionTypes];
        static std::unordered_map<VirtualType, VirtualAction> virtualActions;
        static bool actionUsesLocationArg[NumActionTypes];
        static bool actionUsesSecondaryLocationArg[NumActionTypes];
        static bool actionUsesStringArg[NumActionTypes];
        static bool actionUsesGameStringArg[NumActionTypes]; // The only editor-specific string in trigger actions is "comment"
        static bool actionUsesSoundArg[NumActionTypes];
        static Argument classicBriefingArguments[NumBriefingActionTypes][MaxArguments];
        static Argument briefingTextArguments[NumBriefingActionTypes][MaxArguments];
        static u8 briefingDefaultFlags[NumBriefingActionTypes];
        static bool briefingActionUsesStringArg[NumBriefingActionTypes];
        static bool briefingActionUsesSoundArg[NumBriefingActionTypes];
    }; // 32 (0x20) bytes

    __declspec(align(1)) struct Trigger
    {
        static constexpr size_t MaxConditions = 16;
        static constexpr size_t MaxActions = 64;
        static constexpr size_t MaxOwners = 27;
        enum_t(Flags, u32, {
            IgnoreConditionsOnce = BIT_0,
            IgnoreDefeatDraw = BIT_1,
            PreserveTrigger = BIT_2,
            Disabled = BIT_3,
            IgnoreMiscActions = BIT_4,
            Paused = BIT_5,
            IgnoreWaitSkipOnce = BIT_6
        });
        enum_t(Owned, u8, { // u8
            Yes = 1,
            No = 0
        });
        Condition & condition(size_t conditionIndex);
        Action & action(size_t actionIndex);
        Owned & owned(size_t ownerIndex);
        Trigger& operator= (const Trigger &trigger);
        void deleteAction(size_t actionIndex, bool alignTop = true);
        void deleteCondition(size_t conditionIndex, bool alignTop = true);
        
        bool preserveTriggerFlagged();
        bool disabled();
        bool ignoreConditionsOnce();
        bool ignoreWaitSkipOnce();
        bool ignoreMiscActionsOnce();
        bool ignoreDefeatDraw();
        bool pauseFlagged();

        void setPreserveTriggerFlagged(bool preserved);
        void setDisabled(bool disabled);
        void setIgnoreConditionsOnce(bool ignoreConditionsOnce);
        void setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce);
        void setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce);
        void setIgnoreDefeatDraw(bool ignoreDefeatDraw);
        void setPauseFlagged(bool pauseFlagged);

        size_t numUsedConditions();
        size_t numUsedActions();
        size_t getComment();
        bool locationUsed(size_t locationId);
        bool stringUsed(size_t stringId);
        bool gameStringUsed(size_t stringId);
        bool commentStringUsed(size_t stringId);
        bool briefingStringUsed(size_t stringId);
        void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed);
        void markUsedStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedCommentStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> &stringIdUsed);
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteLocation(size_t locationId);
        void deleteString(size_t stringId);
        void deleteBriefingString(size_t stringId);

        void alignConditionsTop();
        void alignActionsTop();

        Condition conditions[MaxConditions]; // 20 bytes * 16 conditions = 320 bytes, start at 0x0
        Action actions[MaxActions]; // 32 bytes (0x20) * 64 actions = 2048 bytes, start at 0x140
        u32 flags; // 4 bytes
        Owned owners[MaxOwners]; // 1 byte * 27 owners = 27 bytes
        u8 currentAction; // Used internally by StarCraft, incremented after each action is executed, trigger execution ends when currentAction equals MaxActions or Action::Type::NoAction is hit
    }; // 2400 (0x960) bytes total

    enum_t(PlayerColor, u8, { // u8
        Red = 0,
        Blue = 1,
        Teal = 2,
        Purple = 3,
        Orange = 4,
        Brown = 5,
        White = 6,
        Yellow = 7,
        Green = 8,
        PaleYellow = 9,
        Tan = 10,
        Azure_NeutralColor = 11
    });

    constexpr u32 baseFontSize = 8;
    constexpr u32 fontStepSize = 2;

    enum_t(StrFlags, u8, { // u8
        isUsed = BIT_0,
        hasPriority = BIT_1,
        bold = BIT_2,
        underlined = BIT_3,
        italics = BIT_4,
        sizePlusFourSteps = BIT_5,
        sizePlusTwoSteps = BIT_6,
        sizePlusOneStep = BIT_7
    });
    
    enum_t(Scope, u32, { // u32
        None = 0,
        Game = BIT_1,
        Editor = BIT_2,
        GameOverEditor = BIT_3,
        EditorOverGame = BIT_4,
        Both = (Game | Editor),
        Either = Both
    });

    enum class UseExpSection {
        Auto = 0, // Auto-read based on current version, if updating update both
        Yes = 1, // Always use expansion section
        No = 2, // Always use original section
        Both = 3, // Auto-read based on current version, if updating update both
        YesIfAvailable = 4, // Default to expansion, use original if expansion is unavailable
        NoIfOrigAvailable = 5 // Default to original, use expansion if original is unavailable
    };

    enum_t(KstrVersion, u32, { // u32
        Deprecated = 1,
        Current = 2
    });

    __declspec(align(1)) struct StringProperties {
        u8 red;
        u8 green;
        u8 blue;
        u8 flags;
        
        StringProperties();
        StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size);
        
        class InvalidSize : std::exception
        {
            public:
                InvalidSize(u32 size, u32 baseFontSize, u32 fontStepSize) : error(error) { }
                virtual const char* what() const throw() { return error.c_str(); }
                const std::string error;
            private:
                InvalidSize(); // Disallow ctor
        };
    };
    
    __declspec(align(1)) struct TYPE {
        Type scenarioType;
    }; // Size: 4 (not validated)

    __declspec(align(1)) struct VER {
        Version version;
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct IVER {
        IVersion version;
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct IVE2 {
        I2Version version;
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct VCOD {
        u32 seedValues[TotalValidationSeeds];
        ValidationOpCodes opCodes[TotalValidationOpCodes];
    }; // Size: 1040 (validated)
    
    __declspec(align(1)) struct IOWN {
        Sc::Player::SlotType slotType[Sc::Player::Total];
    }; // Size: 12 (not validated)
    
    __declspec(align(1)) struct OWNR {
        Sc::Player::SlotType slotType[Sc::Player::Total];
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct ERA {
        Sc::Terrain::Tileset tileset;
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct DIM {
        u16 tileWidth;
        u16 tileHeight;
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct SIDE {
        Race playerRaces[Sc::Player::Total];
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct MTXM {
        //u16 tiles[0]; // u16 tiles[tileWidth][tileHeight];
    }; // Size <= 256*256*2 (validated)
    
    __declspec(align(1)) struct PUNI {
        Available playerUnitBuildable[Sc::Unit::TotalTypes][Sc::Player::Total];
        Available defaultUnitBuildable[Sc::Unit::TotalTypes];
        UseDefault playerUnitUsesDefault[Sc::Unit::TotalTypes][Sc::Player::Total];
    }; // Size: 5700 (validated)
    
    __declspec(align(1)) struct UPGR {
        u8 playerMaxUpgradeLevel[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
        u8 playerStartUpgradeLevel[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
        u8 defaultMaxLevel[Sc::Upgrade::TotalOriginalTypes];
        u8 defaultStartLevel[Sc::Upgrade::TotalOriginalTypes];
        UseDefault playerUpgradeUsesDefault[Sc::Upgrade::TotalOriginalTypes][Sc::Player::Total];
    }; // Size: 1748 (validated)
    
    __declspec(align(1)) struct PTEC {
        Available techAvailableForPlayer[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
        Researched techResearchedForPlayer[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
        Available techAvailableByDefault[Sc::Tech::TotalOriginalTypes];
        Researched techResearchedByDefault[Sc::Tech::TotalOriginalTypes];
        UseDefault playerUsesDefaultsForTech[Sc::Tech::TotalOriginalTypes][Sc::Player::Total];
    }; // Size: 912 (validated)
    
    __declspec(align(1)) struct UNIT {
        //Unit units[0]; // Unit units[numUnits];
    }; // Size: Multiple of 36 (validated)
    
    __declspec(align(1)) struct ISOM {
        //IsomEntry isomData[0]; // IsomEntry isomData[tileWidth/2 + 1][tileHeight + 1]; // Access x*2, width*y
    }; // Size: (tileWidth / 2 + 1) * (tileHeight + 1) * 8 (not validated)
    
    __declspec(align(1)) struct TILE {
        //u16 tiles[0]; // u16 tiles[tileWidth][tileHeight];
    }; // Size: tileWidth*tileHeight*2 (not validated)
    
    __declspec(align(1)) struct DD2 {
        //Doodad doodads[0]; // Doodad doodads[numDoodads];
    }; // Size: Multiple of 8 (not validated)
    
    __declspec(align(1)) struct THG2 {
        //Sprite sprites[0]; // Sprite sprites[numSprites];
    }; // Size: Multiple of 10 (not validated)
    
    __declspec(align(1)) struct MASK {
        //u8 tileFog[0]; // u8 tileFog[tileWidth][tileHeight];
    }; // Size: tileWidth*tileHeight (not validated)
    
    __declspec(align(1)) struct STR {
        u16 numStrings; // Number of strings in the section (default 1024)
        // u16 stringOffsets[0]; // u16 stringOffsets[numStrings]; // stringId: 1 = stringOffsets[0];
                                 // Offsets to each string within the string section (not within stringData, but within the whole section)
        // void[] stringData; // Character data, usually starting wiht a NUL character, unless recycling sub-strings each string is null terminated
        // Possible non-string data appended after stringData
    }; // Size >= 1 (validated)
    
    __declspec(align(1)) struct UPRP {
        Cuwp createUnitProperties[Sc::Unit::MaxCuwps];
    }; // Size: 1280 (validated)
    
    __declspec(align(1)) struct UPUS {
        CuwpUsed cuwpUsed[Sc::Unit::MaxCuwps];
    }; // Size: 64 (validated
    
    __declspec(align(1)) struct MRGN {
        Location locations[TotalLocations]; // If this is a vanilla map, only the first 64 locations are usable
    }; // Size: 1280 in Original, 5100 in Hybrid or Broodwar (validated)
    
    __declspec(align(1)) struct TRIG {
        // Trigger triggers[0]; // Trigger[numTriggers] triggers;
    }; // Size: Multiple of 2400 (validated)

    __declspec(align(1)) struct MBRF {
        // Trigger triggers[0]; // Chk::Trigger[numTriggers];
    }; // Size: Multiple of 2400 (validated)
    
    __declspec(align(1)) struct SPRP {
        u16 scenarioNameStringId;
        u16 scenarioDescriptionStringId;
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct FORC {
        Force playerForce[Sc::Player::TotalSlots];
        u16 forceString[TotalForces];
        u8 flags[TotalForces];
    }; // Size <= 20 (validated)
    
    __declspec(align(1)) struct WAV {
        u32 soundPathStringId[TotalSounds];
    }; // Size: 2048 (not validated)
    
    __declspec(align(1)) struct UNIS {
        UseDefault useDefault[Sc::Unit::TotalTypes];
        u32 hitpoints[Sc::Unit::TotalTypes]; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes];
        u8 armorLevel[Sc::Unit::TotalTypes]; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes];
        u16 mineralCost[Sc::Unit::TotalTypes];
        u16 gasCost[Sc::Unit::TotalTypes];
        u16 nameStringId[Sc::Unit::TotalTypes];
        u16 baseDamage[Sc::Weapon::TotalOriginal];
        u16 upgradeDamage[Sc::Weapon::TotalOriginal];
    }; // Size: 4048 (validated)

    __declspec(align(1)) struct UPGS {
        UseDefault useDefault[Sc::Upgrade::TotalOriginalTypes];
        u16 baseMineralCost[Sc::Upgrade::TotalOriginalTypes];
        u16 mineralCostFactor[Sc::Upgrade::TotalOriginalTypes];
        u16 baseGasCost[Sc::Upgrade::TotalOriginalTypes];
        u16 gasCostFactor[Sc::Upgrade::TotalOriginalTypes];
        u16 baseResearchTime[Sc::Upgrade::TotalOriginalTypes];
        u16 researchTimeFactor[Sc::Upgrade::TotalOriginalTypes];
    }; // Size: 598 (validated)
    
    __declspec(align(1)) struct TECS {
        UseDefault useDefault[Sc::Tech::TotalOriginalTypes];
        u16 mineralCost[Sc::Tech::TotalOriginalTypes];
        u16 gasCost[Sc::Tech::TotalOriginalTypes];
        u16 researchTime[Sc::Tech::TotalOriginalTypes];
        u16 energyCost[Sc::Tech::TotalOriginalTypes];
    }; // Size: 216 (validated)
    
    __declspec(align(1)) struct SWNM {
        u32 switchName[TotalSwitches];
    }; // Size: 1024 (not validated)
    
    __declspec(align(1)) struct COLR {
        PlayerColor playerColor[Sc::Player::TotalSlots];
    }; // Size: 8 (validated)
    
    __declspec(align(1)) struct PUPx {
        u8 playerMaxUpgradeLevel[Sc::Upgrade::TotalTypes][Sc::Player::Total];
        u8 playerStartUpgradeLevel[Sc::Upgrade::TotalTypes][Sc::Player::Total];
        u8 defaultMaxLevel[Sc::Upgrade::TotalTypes];
        u8 defaultStartLevel[Sc::Upgrade::TotalTypes];
        UseDefault playerUpgradeUsesDefault[Sc::Upgrade::TotalTypes][Sc::Player::Total];
    }; // Size: 2318 (validated)
    
    __declspec(align(1)) struct PTEx {
        Available techAvailableForPlayer[Sc::Tech::TotalTypes][Sc::Player::Total];
        Researched techResearchedForPlayer[Sc::Tech::TotalTypes][Sc::Player::Total];
        Available techAvailableByDefault[Sc::Tech::TotalTypes];
        Researched techResearchedByDefault[Sc::Tech::TotalTypes];
        UseDefault playerUsesDefaultsForTech[Sc::Tech::TotalTypes][Sc::Player::Total];
    }; // Size: 1672 (validated)
    
    __declspec(align(1)) struct UNIx {
        UseDefault useDefault[Sc::Unit::TotalTypes];
        u32 hitpoints[Sc::Unit::TotalTypes]; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes];
        u8 armorLevel[Sc::Unit::TotalTypes]; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes];
        u16 mineralCost[Sc::Unit::TotalTypes];
        u16 gasCost[Sc::Unit::TotalTypes];
        u16 nameStringId[Sc::Unit::TotalTypes];
        u16 baseDamage[Sc::Weapon::Total];
        u16 upgradeDamage[Sc::Weapon::Total];
    }; // Size: 4168 (validated)
    
    __declspec(align(1)) struct UPGx {
        UseDefault useDefault[Sc::Upgrade::TotalTypes];
        u8 unused;
        u16 baseMineralCost[Sc::Upgrade::TotalTypes];
        u16 mineralCostFactor[Sc::Upgrade::TotalTypes];
        u16 baseGasCost[Sc::Upgrade::TotalTypes];
        u16 gasCostFactor[Sc::Upgrade::TotalTypes];
        u16 baseResearchTime[Sc::Upgrade::TotalTypes];
        u16 researchTimeFactor[Sc::Upgrade::TotalTypes];
    }; // Size: 794 (validated)
    
    __declspec(align(1)) struct TECx {
        UseDefault useDefault[Sc::Tech::TotalTypes];
        u16 mineralCost[Sc::Tech::TotalTypes];
        u16 gasCost[Sc::Tech::TotalTypes];
        u16 researchTime[Sc::Tech::TotalTypes];
        u16 energyCost[Sc::Tech::TotalTypes];
    }; // Size: 396 (validated)
    
    __declspec(align(1)) struct OSTR {
        u32 version; // Current version: 1
        u32 scenarioName;
        u32 scenarioDescription;
        u32 forceName[Chk::TotalForces];
        u32 unitName[Sc::Unit::TotalTypes];
        u32 expUnitName[Sc::Unit::TotalTypes];
        u32 soundPath[Chk::TotalSounds];
        u32 switchName[Chk::TotalSwitches];
        u32 locationName[Chk::TotalLocations+1];
    };

    __declspec(align(1)) struct KSTR {
        u32 version; // Current version: 2
        u32 numStrings; // Number of strings in the section
        //u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        // StringProperties[numStrings] stringProperties; // String properties
        // void[] stringData; // List of strings, each null terminated, starting with one NUL character
    }; // Size: 8+8*numStrings+stringDataSize (not validated)
    
    enum_t(SectionName, u32, { // The section name values, as they appear in the binary scenario file
        TYPE = 1162893652, VER = 542262614, IVER = 1380275785, IVE2 = 843404873,
        VCOD = 1146045270, IOWN = 1314344777, OWNR = 1380865871, ERA = 541151813,
        DIM = 541935940, SIDE = 1162103123, MTXM = 1297634381, PUNI = 1229870416,
        UPGR = 1380405333, PTEC = 1128617040, UNIT = 1414090325, ISOM = 1297044297,
        TILE = 1162627412, DD2 = 540165188, THG2 = 843532372, MASK = 1263747405,
        STR = 542266451, UPRP = 1347571797, UPUS = 1398100053, MRGN = 1313296973,
        TRIG = 1195987540, MBRF = 1179796045, SPRP = 1347571795, FORC = 1129467718,
        WAV = 542523735, UNIS = 1397313109, UPGS = 1397182549, TECS = 1396917588,
        SWNM = 1296979795, COLR = 1380732739, PUPx = 2018530640, PTEx = 2017809488,
        UNIx = 2018070101, UPGx = 2017939541, TECx = 2017674580,

        OSTR = 1381258063, KSTR = 1381258059, KTRG = 1196577867, KTGP = 1346851915,

        UNKNOWN = u32_max
    });
    
    using SectionSize = s32;

    __declspec(align(1)) struct SectionHeader {
        SectionName name;
        SectionSize sizeInBytes;
    };

    constexpr u32 CHK = 541804611; // "CHK " = 43|48|4B|20
    using Size = u64;

    __declspec(align(1)) struct ChkHeader { // Not used by a Scenario.chk file, used by serializations
        u32 name; // Set to CHK
        Size sizeInBytes;
    };

    __declspec(align(1)) struct SerializedChk {
        ChkHeader header;
        u8 data[1]; // u8 data[sizeInBytes]
    };
    
    std::ostream & operator<< (std::ostream & out, const Unit & unit);
    std::ostream & operator<< (std::ostream & out, const IsomEntry & isomEntry);
    std::ostream & operator<< (std::ostream & out, const Doodad & doodad);
    std::ostream & operator<< (std::ostream & out, const Sprite & sprite);
    std::ostream & operator<< (std::ostream & out, const Location & location);
    std::ostream & operator<< (std::ostream & out, const Condition & condition);
    std::ostream & operator<< (std::ostream & out, const Action & action);
    std::ostream & operator<< (std::ostream & out, const Trigger & trigger);

#pragma pack(pop)
}

#endif