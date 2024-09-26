#ifndef CHK_H
#define CHK_H
#include <rarecpp/reflect.h>
#include "basics.h"
#include "escape_strings.h"
#include "sc.h"
#include <bitset>
#include <map>

#undef PlaySound

/** 
    This file defines all static structures, constants, and enumerations specific to scenario files (.chk)

    Note that many constants and enumerations may be more general to StarCraft (see Sc.h), than specific to scenario files.

    This same sort of information in wiki form can be found here: http://www.staredit.net/wiki/index.php/Scenario.chk
*/

namespace Chk {
#pragma pack(push, 1)

    struct Unit; struct IsomRect; struct Doodad; struct Sprite; struct Cuwp; struct Location; struct Condition; struct Action; struct Trigger; struct StringProperties;
    struct ExtendedTrigData; struct TriggerGroup;

    enum_t(Type, u32, { // u32
        RAWS = 0x53574152, // 1.04 StarCraft and above ("hybrid")
        RAWB = 0x42574152 // Brood War
    });
    
    enum_t(Version, u16, { // u16
        StarCraft_Original = 59,
        StarCraft_Hybrid = 63, // 1.04 StarCraft and above ("hybrid")
        StarCraft_BroodWar = 205, // Brood War
        StarCraft_Remastered = 206 // Remastered
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
        NoOp = 8 // No operation
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

    enum_t(PlayerColorSetting, u8, {
        RandomPredefined = 0,
        PlayerChoice = 1,
        Custom = 2,
        UseId = 3
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
            Hangar = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHangar = x16BIT_5
        });
        enum class Field { // Typeless
            ClassId, Xc, Yc, Type, RelationFlags, ValidStateFlags, ValidFieldFlags, Owner, HitpointPercent, ShieldPercent, EnergyPercent, ResourceAmount, HangarAmount, StateFlags, Unused, RelationClassId
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
        u16 hangarAmount;
        u16 stateFlags;
        u32 unused;
        u32 relationClassId; // classId of related unit (may be an addon or the building that has an addon)
        
        constexpr bool isLifted() const { return (stateFlags & State::InTransit) == State::InTransit; }
        constexpr bool isAttached() const { return (relationFlags & RelationFlag::AddonLink) == RelationFlag::AddonLink; }
        constexpr bool isLinked() const { return relationFlags != 0 && classId != relationClassId; }

        REFLECT(Unit, classId, xc, yc, type, relationFlags, validStateFlags, validFieldFlags, owner,
            hitpointPercent, shieldPercent, energyPercent, resourceAmount, hangarAmount, stateFlags, unused, relationClassId)
    }; // 36 (0x24) bytes

    struct IsomRect
    {
        using Side = Sc::Isom::Side;

        struct EditorFlag_ {
            enum uint16_t_ : uint16_t {
                Modified = 0x0001,
                Visited = 0x8000,

                xModified = 0xFFFE,
                xVisited = 0x7FFF,

                ClearAll = 0x7FFE
            };
        };
        using EditorFlag = EditorFlag_::uint16_t_;

        struct Point
        {
            size_t x;
            size_t y;
        };

        struct IsomDiamond // A "diamond" exists along the isometric coordinate space and has a projection to an 8x4 rectangular shape with four quadrants
        {
            struct Neighbor_ {
                enum int_ : int {
                    UpperLeft,
                    UpperRight,
                    LowerRight,
                    LowerLeft
                };
            };
            using Neighbor = Neighbor_::int_;
            static constexpr Neighbor neighbors[] { Neighbor::UpperLeft, Neighbor::UpperRight, Neighbor::LowerRight, Neighbor::LowerLeft };

            size_t x;
            size_t y;

            constexpr IsomDiamond getNeighbor(Neighbor neighbor) const {
                switch ( neighbor ) {
                    case Neighbor::UpperLeft: return { x - 1, y - 1 };
                    case Neighbor::UpperRight: return { x + 1, y - 1 };
                    case Neighbor::LowerRight: return { x + 1, y + 1 };
                    default: /*Neighbor::LowerLeft*/ return { x - 1, y + 1 };
                }
            }
            constexpr Point getRectangleCoords(Sc::Isom::Quadrant quadrant) const {
                switch ( quadrant ) {
                    case Sc::Isom::Quadrant::TopLeft: return Point { x - 1, y - 1 };
                    case Sc::Isom::Quadrant::TopRight: return Point { x, y - 1 };
                    case Sc::Isom::Quadrant::BottomRight: return Point { x, y }; // Diamond (x, y) is the same as the diamonds bottom-right rectangle (x, y)
                    default: /*Sc::Isom::Quadrant::BottomLeft*/ return Point { x - 1, y };
                }
            }
            constexpr operator Point() const { return { x, y }; } // Conversion implies going to the bottom-right rectangle for the isom diamond
            constexpr bool isValid() const { return (x+y)%2 == 0; }

            static constexpr IsomDiamond fromMapCoordinates(size_t x, size_t y)
            {
                s32 calcX = s32(x) - s32(y)*2;
                s32 calcY = s32(x)/2 + s32(y);
                calcX -= ((calcX-64) & 127);
                calcY -= ((calcY-32) & 63);
                return { size_t(((calcY + 32 + (calcX / 2 + 64 / 2)) / 32) / 2), size_t(((calcY + 32 - (calcX / 2 + 64 / 2)) / 2) / 32) };
            }

            static constexpr IsomDiamond none() { return {std::numeric_limits<size_t>::max(), std::numeric_limits<size_t>::max()}; }

            constexpr bool operator!=(const IsomDiamond & other) { return this->x != other.x || this->y != other.y; }
        };

        uint16_t left = 0;
        uint16_t top = 0;
        uint16_t right = 0;
        uint16_t bottom = 0;

        constexpr IsomRect() = default;
        constexpr IsomRect(uint16_t left, uint16_t top, uint16_t right, uint16_t bottom) : left(left), top(top), right(right), bottom(bottom) {}

        constexpr uint16_t & side(Side side) {
            switch ( side ) {
                case Side::Left: return left;
                case Side::Top: return top;
                case Side::Right: return right;
                default: /*Side::Bottom*/ return bottom;
            }
        }

        constexpr uint16_t getIsomValue(Side side) const {
            switch ( side ) {
                case Side::Left: return left & EditorFlag::ClearAll;
                case Side::Top: return top & EditorFlag::ClearAll;
                case Side::Right: return right & EditorFlag::ClearAll;
                default: /*Side::Bottom*/ return bottom & EditorFlag::ClearAll;
            }
        }

        constexpr void setIsomValue(Side side, uint16_t value) {
            this->side(side) = value;
        }

        constexpr uint32_t getHash(Span<Sc::Isom::ShapeLinks> isomLinks) const
        {
            uint32_t hash = 0;
            uint16_t lastTerrainType = 0;
            for ( auto side : Sc::Isom::sides )
            {
                auto isomValue = this->getIsomValue(side);
                if ( (isomValue >> 4) < isomLinks.size() )
                {
                    const auto & shapeLinks = isomLinks[isomValue >> 4];
                    auto edgeLink = shapeLinks.getEdgeLink(isomValue);
                    hash = (hash | uint32_t(edgeLink)) << 6;

                    if ( shapeLinks.terrainType != 0 && edgeLink > Sc::Isom::Link::SoftLinks )
                        lastTerrainType = shapeLinks.terrainType;
                }
            }
            return hash | lastTerrainType; // 6 bits per component (left, top, right, bottom, terrainType)
        }

        constexpr void set(Sc::Isom::ProjectedQuadrant quadrant, uint16_t value) {
            setIsomValue(quadrant.firstSide, (value << 4) | quadrant.firstEdgeFlags);
            setIsomValue(quadrant.secondSide, (value << 4) | quadrant.secondEdgeFlags);
        }

        constexpr bool isLeftModified() const { return left & EditorFlag::Modified; }

        constexpr bool isLeftOrRightModified() const { return ((left | right) & EditorFlag::Modified) == EditorFlag::Modified; }

        constexpr void setModified(Sc::Isom::ProjectedQuadrant quadrant) {
            this->side(quadrant.firstSide) |= EditorFlag::Modified;
            this->side(quadrant.secondSide) |= EditorFlag::Modified;
        }

        constexpr bool isVisited() const { return (right & EditorFlag::Visited) == EditorFlag::Visited; }

        constexpr void setVisited() { right |= EditorFlag::Visited; }

        constexpr void clearEditorFlags() {
            left &= EditorFlag::ClearAll;
            top &= EditorFlag::ClearAll;
            right &= EditorFlag::ClearAll;
            bottom &= EditorFlag::ClearAll;
        }

        constexpr void clear() {
            left = 0;
            top = 0;
            right = 0;
            bottom = 0;
        }

        REFLECT(IsomRect, left, top, right, bottom)
    };
    static_assert(sizeof(IsomRect) == 8, "IsomRect must be exactly 8 bytes!");

    using IsomDiamond = IsomRect::IsomDiamond;

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

        REFLECT(Doodad, type, xc, yc, owner, enabled)
    };

    __declspec(align(1)) struct Sprite
    {
        enum_t(SpriteFlags, u16, {
            
            BitZero = BIT_0, // Set in staredit, not read by SC
            BitOne = BIT_1, // Bit 1 Unused in staredit/SC
            BitTwo = BIT_2, // Bit 2 Unused in staredit/SC
            BitThree = BIT_3, // Bit 3 Unused in staredit/SC
            BitFour = BIT_4, // Scmdraft notes "(Provides Cover?)", the flag is set in some doodads and is not read by SC
            BitFive = BIT_5, // Bit 5 Unused in staredit/SC
            BitSix = BIT_6, // Bit 6 Unused in staredit/SC
            BitSeven = BIT_7, // Scmdraft notes "(Always set!)"; set for doodads which include an overlay (& some which do not) and is not read by SC
            BitEight = BIT_8, // Set in staredit, not read by SC
            BitNine = BIT_9, // Scmdraft notes "(Medium Ground Lvl?), it's set for some doodads, and is not read by SC
            BitTen = BIT_10, // Scmdraft notes "(High Ground Lvl?), it's set for some doodads, and is not read by SC
            BitEleven = BIT_11, // Bit 11 Unused in staredit/SC
            DrawAsSprite = BIT_12, // Indicates whether this sprite should be treated as a unit; in SC: receeding creep
            IsUnit = BIT_13, // Set in staredit, but is not read by SC - rather SpriteOverlay or !SpriteOverlay is checked
            OverlayFlipped_Deprecated = BIT_14, // In SC: temporary creep
            SpriteUnitDiabled = BIT_15 // If the SpriteOverlay flag is NOT set (this is a sprite-unit), then the unit is disabled
        });
        enum class Field {
            Type, Xc, Yc, Owner, Unused, Flags
        };

        inline static u16 toPureSpriteFlags(u16 flags)
        {
            flags |= SpriteFlags::DrawAsSprite;
            flags &= (~SpriteFlags::IsUnit);
            flags &= (~SpriteFlags::SpriteUnitDiabled);
            return flags;
        }

        bool isDrawnAsSprite() const;
        bool isUnit() const;

        Sc::Sprite::Type type;
        u16 xc;
        u16 yc;
        u8 owner;
        u8 unused;
        u16 flags;

        REFLECT(Sprite, type, xc, yc, owner, unused, flags)
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

    enum_t(StringUserFlag, u32, {
        None = 0,
        ScenarioName = BIT_0,
        ScenarioDescription = BIT_1,
        Force = BIT_2,
        Location = BIT_3,
        OriginalUnitSettings = BIT_4,
        ExpansionUnitSettings = BIT_5,
        Sound = BIT_6,
        Switch = BIT_7,
        TriggerAction = BIT_8,
        TriggerActionSound = BIT_9,
        ExtendedTriggerComment = BIT_10,
        ExtendedTriggerNotes = BIT_11,
        BriefingTriggerAction = BIT_12,
        BriefingTriggerActionSound = BIT_13,

        ScenarioProperties = ScenarioName | ScenarioDescription,
        BothUnitSettings = OriginalUnitSettings | ExpansionUnitSettings,
        AnyTrigger = TriggerAction | TriggerActionSound,
        AnyTriggerExtension = ExtendedTriggerComment | ExtendedTriggerNotes,
        AnyBriefingTrigger = BriefingTriggerAction | BriefingTriggerActionSound,
        xTrigger = x32BIT_8,

        All = u32_max
    });

    struct StringUser
    {
        Chk::StringUserFlag userFlags;
        size_t index;
        size_t subIndex;

        StringUser(Chk::StringUserFlag userFlags = Chk::StringUserFlag::All, size_t index = 0, size_t subIndex = 0)
            : userFlags(userFlags), index(index), subIndex(subIndex) {}
    };

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
            Hangar = BIT_5,

            xOwner = x16BIT_0,
            xHitpoints = x16BIT_1,
            xShields = x16BIT_2,
            xEnergy = x16BIT_3,
            xResources = x16BIT_4,
            xHangar = x16BIT_5
        });
        
        bool isCloaked() const;
        bool isBurrowed() const;
        bool isInTransit() const;
        bool isHallucinated() const;
        bool isInvincible() const;
        
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
        u16 hangarAmount;
        u16 unitStateFlags;
        u32 unknown;

        REFLECT(Cuwp, validUnitStateFlags, validUnitFieldFlags, owner,
            hitpointPercent, shieldPercent, energyPercent, resourceAmount, hangarAmount, unitStateFlags, unknown)
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

        bool isBlank() const;
        
        u32 left;
        u32 top;
        u32 right;
        u32 bottom;
        u16 stringId;
        u16 elevationFlags;

        REFLECT(Location, left, top, right, bottom, stringId, elevationFlags)
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

        REFLECT(Condition, locationId, player, amount, unitType, comparison, conditionType, typeIndex, flags, maskFlag)

        void toggleDisabled();
        bool isDisabled() const;
        inline bool switchUsed(size_t switchId) const;
        inline bool locationUsed(size_t locationId) const;
        inline void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const;
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void deleteLocation(size_t locationId);
        static const Argument & getClassicArg(Type conditionType, size_t argIndex);
        static const Argument & getClassicArg(VirtualType conditionType, size_t argIndex);
        static ArgType getClassicArgType(Type conditionType, size_t argIndex);
        static ArgType getClassicArgType(VirtualType conditionType, size_t argIndex);
        static const Argument & getTextArg(Type conditionType, size_t argIndex);
        static const Argument & getTextArg(VirtualType conditionType, size_t argIndex);
        static ArgType getTextArgType(Type conditionType, size_t argIndex);
        static ArgType getTextArgType(VirtualType conditionType, size_t argIndex);
        static u8 getDefaultFlags(Type conditionType);
        static u8 getDefaultFlags(VirtualType conditionType);
        
        static Argument noArg;
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
        static constexpr size_t MaxArguments = 12;
        enum_t(Type, u8, { // u8
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisableDebugMode = 58,
            DisplayTextMessage = 9,
            Draw = 56,
            EnableDebugMode = 59,
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
            ModifyUnitHangarCount = 53,
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
            BriefingDisplaySpeakingPortrait = 7,
            BriefingTransmission = 8,
            BriefingSkipTutorialEnabled = 9
        });
        enum_t(VirtualType, s32, { // s32
            CenterView = 10,
            Comment = 47,
            CreateUnit = 44,
            CreateUnitWithProperties = 11,
            Defeat = 2,
            DisableDebugMode = 58,
            DisplayTextMessage = 9,
            Draw = 56,
            EnableDebugMode = 59,
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
            ModifyUnitHangarCount = 53,
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
            BriefingDisplaySpeakingPortrait = 7,
            BriefingTransmission = 8,
            BriefingSkipTutorialEnabled = 9,

            BriefingCustom = -3
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
            ActionType = 21,
            Flags = 22,
            Number = 23, // Amount, Group2, LocDest, UnitPropNum, ScriptNum
            TypeIndex = 24, // Unit, ScoreType, ResourceType, AllianceStatus
            SecondaryTypeIndex = 25, // NumUnits (0=all), SwitchAction, UnitOrder, ModifyType
            Padding = 26,
            MaskFlag = 27,
            MemoryOffset = 28,

            BriefingSlot = 29
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
        enum_t(AllianceStatus, u16, { // u16
            Enemy = 0,
            Ally = 1,
            AlliedVictory = 2
        });
        enum_t(Order, u8, { // u8
            Move = 0,
            Patrol = 1,
            Attack = 2
        });
        enum_t(MaskFlag, u16, { // u16
            Enabled = 0x4353, // "SC" in little-endian; 'S' = 0x53, 'C' = 0x43
            Disabled = 0
        });
        enum_t(BriefingSlot, u32, {
            Slot1 = 0,
            Slot2 = 1,
            Slot3 = 2,
            Slot4 = 3
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
        u32 number; // Amount/Group2/LocDest/UnitPropNum/ScriptNum/SwitchIndex
        u16 type; // Unit/score/resource type/alliance status
        Type actionType; // u8
        u8 type2; // Num units/switch action/unit order/modify type
        u8 flags;
        u8 padding;
        MaskFlag maskFlag; // u16, set to "SC" (0x53, 0x43) for masked deaths

        REFLECT(Action, locationId, stringId, soundStringId, time, group, number, type, actionType, type2, flags, padding, maskFlag)

        bool isDisabled() const { return (flags & Flags::Disabled) == Flags::Disabled; }
        void toggleDisabled();
        static const Argument & getClassicArg(Type actionType, size_t argIndex);
        static const Argument & getClassicArg(VirtualType actionType, size_t argIndex);
        static const Argument & getBriefingClassicArg(Type actionType, size_t argIndex);
        static const Argument & getBriefingClassicArg(VirtualType actionType, size_t argIndex);
        static ArgType getClassicArgType(Type actionType, size_t argIndex);
        static ArgType getClassicArgType(VirtualType actionType, size_t argIndex);
        static ArgType getBriefingClassicArgType(Type actionType, size_t argIndex);
        static ArgType getBriefingClassicArgType(VirtualType actionType, size_t argIndex);
        static const Argument & getTextArg(Type actionType, size_t argIndex);
        static const Argument & getTextArg(VirtualType actionType, size_t argIndex);
        static const Argument & getBriefingTextArg(Type actionType, size_t argIndex);
        static const Argument & getBriefingTextArg(VirtualType actionType, size_t argIndex);
        static ArgType getTextArgType(Type actionType, size_t argIndex);
        static ArgType getTextArgType(VirtualType actionType, size_t argIndex);
        static ArgType getBriefingTextArgType(Type actionType, size_t argIndex);
        static ArgType getBriefingTextArgType(VirtualType actionType, size_t argIndex);
        static u8 getDefaultFlags(Type actionType);
        static u8 getDefaultFlags(VirtualType actionType);
        static u8 getBriefingDefaultFlags(Type actionType);
        static u8 getBriefingDefaultFlags(VirtualType actionType);
        bool hasStringArgument() const;
        bool hasSoundArgument() const;
        bool hasBriefingStringArgument() const;
        bool hasBriefingSoundArgument() const;
        inline bool switchUsed(size_t switchId) const;
        inline bool locationUsed(size_t locationId) const;
        inline bool stringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        inline bool gameStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        inline bool commentStringUsed(size_t stringId) const;
        inline bool briefingStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyBriefingTrigger) const;
        inline void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const;
        inline void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        inline void markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        inline void markUsedCommentStrings(std::bitset<Chk::MaxStrings> & stringIdUsed) const;
        inline void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyBriefingTrigger) const;
        void remapLocationIds(const std::map<u32, u32> & locationIdRemappings);
        void remapStringIds(const std::map<u32, u32> & stringIdRemappings);
        void remapBriefingStringIds(const std::map<u32, u32> & stringIdRemappings);
        void deleteLocation(size_t locationId);
        void deleteString(size_t stringId);
        void deleteBriefingString(size_t stringId);
        
        static Argument noArg;
        static Argument classicArguments[NumActionTypes][MaxArguments];
        static Argument textArguments[NumActionTypes][MaxArguments];
        static u8 defaultFlags[NumActionTypes];
        static std::unordered_map<VirtualType, VirtualAction> virtualActions;
        static std::unordered_map<VirtualType, VirtualAction> virtualBriefingActions;
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
        enum_t(ResourceType, u8, { // u8
            Ore = 0,
            Gas = 1,
            OreAndGas = 2
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
        enum_t(ValueModifier, u8, { // u8
            Enable = 4,
            Enabled = 4,
            Set = 4,
            Disable = 5,
            Disabled = 5,
            Clear = 5,
            Toggle = 6,
            SetTo = 7,
            Add = 8,
            Subtract = 9,
            Randomize = 11
        });
        enum_t(Owned, u8, { // u8
            Yes = 1,
            No = 0
        });
        Condition & condition(size_t conditionIndex);
        const Condition & condition(size_t conditionIndex) const;
        Action & action(size_t actionIndex);
        const Action & action(size_t actionIndex) const;
        Owned & owned(size_t ownerIndex);
        const Owned & owned(size_t ownerIndex) const;
        Trigger & operator= (const Trigger & trigger);
        void deleteAction(size_t actionIndex, bool alignTop = true);
        void deleteCondition(size_t conditionIndex, bool alignTop = true);
        
        bool preserveTriggerFlagged() const;
        bool disabled() const;
        bool ignoreConditionsOnce() const;
        bool ignoreWaitSkipOnce() const;
        bool ignoreMiscActionsOnce() const;
        bool ignoreDefeatDraw() const;
        bool pauseFlagged() const;

        void setPreserveTriggerFlagged(bool preserved);
        void setDisabled(bool disabled);
        void setIgnoreConditionsOnce(bool ignoreConditionsOnce);
        void setIgnoreWaitSkipOnce(bool ignoreWaitSkipOnce);
        void setIgnoreMiscActionsOnce(bool ignoreMiscActionsOnce);
        void setIgnoreDefeatDraw(bool ignoreDefeatDraw);
        void setPauseFlagged(bool pauseFlagged);

        size_t getExtendedDataIndex() const;
        void setExtendedDataIndex(size_t extendedDataIndex);
        void clearExtendedDataIndex();

        size_t numUsedConditions() const;
        size_t numUsedActions() const;
        size_t getComment() const;
        bool switchUsed(size_t switchId) const;
        bool locationUsed(size_t locationId) const;
        bool stringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        bool gameStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        bool commentStringUsed(size_t stringId) const;
        bool briefingStringUsed(size_t stringId, u32 userMask = Chk::StringUserFlag::AnyBriefingTrigger) const;
        void markUsedLocations(std::bitset<Chk::TotalLocations+1> & locationIdUsed) const;
        void markUsedStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        void markUsedGameStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyTrigger) const;
        void markUsedCommentStrings(std::bitset<Chk::MaxStrings> & stringIdUsed) const;
        void markUsedBriefingStrings(std::bitset<Chk::MaxStrings> & stringIdUsed, u32 userMask = Chk::StringUserFlag::AnyBriefingTrigger) const;
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

        REFLECT(Trigger, conditions, actions, flags, owners, currentAction)
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
        Azure_NeutralColor = 11,
        None = std::numeric_limits<u8>::max()
    });

    constexpr size_t TotalColors = 12;

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

    constexpr u32 UnusedExtendedTrigDataIndexCheck = 0xFEFEFEFE; // If potentialIndex & this == 0, potentialIndex is unusable

    __declspec(align(1)) struct ExtendedTrigData
    {
        enum_t(GroupId, u32, {
            None = u32_max
        });
        enum_t(TrigNum, u32, {
            None = u32_max
        });
        enum_t(MaskId, u32, {
            None = u32_max
        });

        u32 commentStringId; // 0 for unused
        u32 notesStringId; // 0 for unused
        u32 groupId; // 0xFFFFFFFF for none
        u32 trigNum; // 0xFFFFFFFF for none
        u32 maskId; // 0xFFFFFFFF for none

        REFLECT(ExtendedTrigData, commentStringId, notesStringId, groupId, trigNum, maskId)

        ExtendedTrigData();
        bool isBlank() const;
    };

    __declspec(align(1)) struct TriggerGroupHeader // TODO: Unused? or at least, it doesn't yet figure into serialization
    {
        enum_t(Flags, u32, {
            groupExpanded = BIT_27,
            groupHidden = BIT_28
        });
        u32 flags;
        u32 templateInstanceId; // 1-based index of template details used to generate this group
        u32 numTriggers; // Number of triggers in this group
        u32 numGroups; // Number of groups in this group
        u32 commentStringId; // 0 for unused
        u32 notesStringId; // 0 for unused
        u32 parentGroupId; // 0xFFFFFFFF if this is a top-level group
        u32 bodyOffset; // Distance from section start to trigger group body, 0 if numTriggers+numGroups = 0
    };

    __declspec(align(1)) struct TriggerGroupBody
    {
        //u32 extendedTrigDataIndex[header.numTriggers];
        //u32 groupIndex[header.numGroups];
    };

    __declspec(align(1)) struct TriggerGroup
    {
        enum_t(Id, u32, {
            TopLevelGroup = 0xFFFFFFFF
        });

        TriggerGroup();

        bool groupExpanded;
        bool groupHidden;
        u32 templateInstanceId; // 1-based index of template details used to generate this group
        u32 commentStringId; // 0 for unused
        u32 notesStringId; // 0 for unused
        u32 parentGroupId; // 0xFFFFFFFF if this is a top-level group
        std::vector<u32> extendedTrigDataIndexes; // Extended trigger data indexes of triggers in this group
        std::vector<u32> groupIndexes; // Indexes of sub-groups

        REFLECT(TriggerGroup, groupExpanded, groupHidden, templateInstanceId, commentStringId, notesStringId, parentGroupId, extendedTrigDataIndexes, groupIndexes)
    };

    __declspec(align(1)) struct StringProperties {
        u8 red;
        u8 green;
        u8 blue;
        u8 flags;
        
        StringProperties();
        StringProperties(u8 red, u8 green, u8 blue, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics, u32 size);
        
        class InvalidSize : public std::exception
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
        Type scenarioType {Chk::Type::RAWS};
    }; // Size: 4 (not validated)

    __declspec(align(1)) struct VER {
        Version version {Chk::Version::StarCraft_Hybrid};
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct IVER {
        IVersion version {Chk::IVersion::Current};
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct IVE2 {
        I2Version version {Chk::I2Version::StarCraft_1_04};
    }; // Size: 2 (not validated)
    
    __declspec(align(1)) struct VCOD {
        u32 seedValues[TotalValidationSeeds] {
            0x77CA1934, 0x7168DC99, 0xC3BF600A, 0xA775E7A7, 0xA67D291F, 0xBB3AB0D7, 0xED2431CC, 0x0B134C17, 0xB7A22065, 0x6B18BD91, 0xDD5DC38D, 0x37D57AE2,
            0xD46459F6, 0x0F129A63, 0x462E5C43, 0x2AF874E3, 0x06376A08, 0x3BD6F637, 0x1663940E, 0xEC5C6745, 0xB7F77BD7, 0x9ED4FC1A, 0x8C3FFA73, 0x0FE1C02E,
            0x070974D1, 0xD764E395, 0x74681675, 0xDA4FA799, 0x1F1820D5, 0xBEA0E6E7, 0x1FE3B6A6, 0x70EF0CCA, 0x311AD531, 0x3524B84D, 0x7DC7F8E3, 0xDE581AE1,
            0x432705F4, 0x07DBACBA, 0x0ABE69DC, 0x49EC8FA8, 0x3F1658D7, 0x8AC1DBE5, 0x05C0CF41, 0x721CCA9D, 0xA55FB1A2, 0x9B7023C4, 0x14E10484, 0xDA907B80,
            0x0669DBFA, 0x400FF3A3, 0xD4CEF3BE, 0xD7CBC9E3, 0x3401405A, 0xF81468F2, 0x1AC58E38, 0x4B3DD6FE, 0xFA050553, 0x8E451034, 0xFE6991DD, 0xF0EEE0AF,
            0xDD7E48F3, 0x75DCAD9F, 0xE5AC7A62, 0x67621B31, 0x4D36CD20, 0x742198E0, 0x717909FB, 0x7FCD6736, 0x3CD65F77, 0xC6A6A2A2, 0x6ACEE31A, 0x6CA9CD4E,
            0x3B9DBA86, 0xFD76F4B5, 0xBCF044F8, 0x296EE92E, 0x6B2F2523, 0x4427AB08, 0x99CC127A, 0x75F2DCED, 0x7E383CC5, 0xC51B1CF7, 0x65942DD1, 0xDD48C906,
            0xAC2D32BE, 0x8132C9B5, 0x34D84A66, 0xDF153F35, 0xB6EBEEB2, 0x964DF604, 0x9C944235, 0x61D38A62, 0x6F7BA852, 0xF4FC61DC, 0xFE2D146C, 0x0AA4EA99,
            0x13FED9E8, 0x594448D0, 0xE3F36680, 0x198DD934, 0xFE63D716, 0x3A7E1830, 0xB10F8D9B, 0x8CF5F012, 0xDB58780A, 0x8CB8633E, 0x8EF3AA3A, 0x2E1A8A37,
            0xEFF9315C, 0x7EE36DE3, 0x133EBD9B, 0xB9C044C6, 0x90DA3ABC, 0x74B0ADA4, 0x892757F8, 0x373FE647, 0x5A7942E4, 0xEE8D43DF, 0xE8490AB4, 0x1A88C33C,
            0x766B0188, 0xA3FDC38A, 0x564E7A16, 0xBACB7FA7, 0xEC1C5E02, 0x76C9B9B0, 0x39B1821E, 0xC557C93E, 0x4C382419, 0xB8542F5D, 0x8E575D6F, 0x520AA130,
            0x5E71186D, 0x59C30613, 0x623EDC1F, 0xEBB5DADC, 0xF995911B, 0xDAD591A7, 0x6BCE5333, 0x017000F5, 0xE8EED87F, 0xCEF10AC0, 0xD3B6EB63, 0xA5CCEF78,
            0xA4BC5DAA, 0xD2F2AB96, 0x9AEAFF61, 0xA2ED6AA8, 0x61ED3EBD, 0x9282C139, 0xB1233616, 0xE524A0B0, 0xAAA79B05, 0x339B120D, 0xDA209283, 0xFCECB025,
            0x2338D024, 0x74F295FC, 0x19E57380, 0x447D5097, 0xDB449345, 0x691DADA2, 0xE7EE1444, 0xFF877F2C, 0xF1329E38, 0xDA29BC4D, 0xFE262742, 0xA92BD2C1,
            0x0E7A42F6, 0xD17CE8CB, 0x56EC5B0F, 0x3161B769, 0x25F96DB4, 0x6D793440, 0x0BA753FA, 0xCE82A4FA, 0x614945C3, 0x8F2C450D, 0xF7604928, 0x1EC97DF3,
            0xC189D00F, 0xD3F85226, 0x14358F4D, 0x0B5F9DBA, 0x004AA907, 0x2F2622F7, 0x1FFB673E, 0xC6119CA1, 0x665D4F69, 0x90153458, 0x4654E56C, 0xD6635FAF,
            0xDF950C8A, 0xAFE40DBD, 0x4C4040BF, 0x7151F6A3, 0xF826ED29, 0xD5222885, 0xFACFBEBF, 0x517FC528, 0x076306B8, 0x298FBDEC, 0x717E55FA, 0x6632401A,
            0x9DDED4E8, 0x93FC5ED4, 0x3BD53D7A, 0x802E75CD, 0x87744F0A, 0xEA8FCC1B, 0x7CDBA99A, 0xEFE55316, 0x6EC178AB, 0x5A8972A4, 0x50702C98, 0x1FDFA1FB,
            0x44D9B76B, 0x56828007, 0x83C0BFFD, 0x5BD0490E, 0x0E6A681E, 0x2F0BC29A, 0xE1A0438E, 0xB2F60C99, 0x5E1C7AE0, 0x45A0C82C, 0x88E90B3C, 0xC696B9AC,
            0x2A83AE74, 0x65FA13BB, 0xA61F4FEB, 0xE18A8AB0, 0xB9B8E981, 0x4E1555D5, 0x9BADF245, 0x7E35C23E, 0x722E925F, 0x23685BB6, 0x0E45C66E, 0xD4873BE9,
            0xE3C041F4, 0xBE4405A8, 0x138A0FE4, 0xF437C41A, 0xEF55405A, 0x4B1D799D, 0x9C3A794A, 0xCC378576, 0xB60F3D82, 0x7E93A660, 0xC4C25CBD, 0x907FC772,
            0x10961B4D, 0x68680513, 0xFF7BC035, 0x2A438546
        };
        ValidationOpCodes opCodes[TotalValidationOpCodes] {
            Chk::ValidationOpCodes::Add_Shifted_Sections, Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::XOR_Sections_05,
            Chk::ValidationOpCodes::ORs_And_Shifts      , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::Add_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::Sub_Shifted_Sections  , Chk::ValidationOpCodes::XOR_Shifted_Sections,
            Chk::ValidationOpCodes::XOR_Sections        , Chk::ValidationOpCodes::ORs_And_Reverse_Shifts, Chk::ValidationOpCodes::ORs_And_Reverse_Shifts,
            Chk::ValidationOpCodes::XOR_Sections_05     , Chk::ValidationOpCodes::XOR_Sections_04       , Chk::ValidationOpCodes::ORs_And_Shifts,
            Chk::ValidationOpCodes::XOR_Sections
        };

        REFLECT(VCOD, seedValues, opCodes)
    }; // Size: 1040 (validated)
    
    __declspec(align(1)) struct IOWN {
        Sc::Player::SlotType slotType[Sc::Player::Total] {
            Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
            Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
            Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
        };
    }; // Size: 12 (not validated)
    
    __declspec(align(1)) struct OWNR {
        Sc::Player::SlotType slotType[Sc::Player::Total] {
            Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
            Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen, Sc::Player::SlotType::GameOpen,
            Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive, Sc::Player::SlotType::Inactive
        };
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct ERA {
        Sc::Terrain::Tileset tileset {Sc::Terrain::Tileset::Badlands};
    }; // Size: 2 (validated)
    
    __declspec(align(1)) struct DIM {
        u16 tileWidth {64};
        u16 tileHeight {64};

        REFLECT(DIM, tileWidth, tileHeight)
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct SIDE {
        Race playerRaces[Sc::Player::Total] {
            Chk::Race::Terran  , Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran,
            Chk::Race::Zerg    , Chk::Race::Protoss , Chk::Race::Terran  , Chk::Race::Zerg,
            Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Inactive, Chk::Race::Neutral
        };
    }; // Size: 12 (validated)
    
    __declspec(align(1)) struct MTXM {
        //u16 tiles[0]; // u16 tiles[tileWidth][tileHeight];
    }; // Size <= 256*256*2 (validated)
    
    __declspec(align(1)) struct PUNI {
        Available playerUnitBuildable[Sc::Player::Total][Sc::Unit::TotalTypes] {};
        Available defaultUnitBuildable[Sc::Unit::TotalTypes] {};
        UseDefault playerUnitUsesDefault[Sc::Player::Total][Sc::Unit::TotalTypes] {};

        PUNI() {
            memset(&playerUnitBuildable, (int)Chk::Available::Yes, Sc::Player::Total*Sc::Unit::TotalTypes);
            memset(&defaultUnitBuildable, (int)Chk::Available::Yes, Sc::Unit::TotalTypes);
            memset(&playerUnitUsesDefault, (int)Chk::Available::Yes, Sc::Player::Total*Sc::Unit::TotalTypes);
        }

        REFLECT(PUNI, playerUnitBuildable, defaultUnitBuildable, playerUnitUsesDefault)
    }; // Size: 5700 (validated)
    
    __declspec(align(1)) struct UPGR {
        u8 playerMaxUpgradeLevel[Sc::Player::Total][Sc::Upgrade::TotalOriginalTypes] {};
        u8 playerStartUpgradeLevel[Sc::Player::Total][Sc::Upgrade::TotalOriginalTypes] {};
        u8 defaultMaxLevel[Sc::Upgrade::TotalOriginalTypes] {
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0
        };
        u8 defaultStartLevel[Sc::Upgrade::TotalOriginalTypes] {};
        UseDefault playerUpgradeUsesDefault[Sc::Player::Total][Sc::Upgrade::TotalOriginalTypes] {};

        UPGR() {
            memset(&playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Player::Total*Sc::Upgrade::TotalOriginalTypes);
        }

        REFLECT(UPGR, playerMaxUpgradeLevel, playerStartUpgradeLevel, defaultMaxLevel, defaultStartLevel, playerUpgradeUsesDefault)
    }; // Size: 1748 (validated)
    
    __declspec(align(1)) struct PTEC {
        Available techAvailableForPlayer[Sc::Player::Total][Sc::Tech::TotalOriginalTypes] {};
        Researched techResearchedForPlayer[Sc::Player::Total][Sc::Tech::TotalOriginalTypes] {};
        Available techAvailableByDefault[Sc::Tech::TotalOriginalTypes] {};
        Researched techResearchedByDefault[Sc::Tech::TotalOriginalTypes] {
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes
        };
        UseDefault playerUsesDefaultsForTech[Sc::Player::Total][Sc::Tech::TotalOriginalTypes] {};

        PTEC() {
            memset(&techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalOriginalTypes);
            memset(&playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Player::Total*Sc::Tech::TotalOriginalTypes);
        }

        REFLECT(PTEC, techAvailableForPlayer, techResearchedForPlayer, techAvailableByDefault, techResearchedByDefault, playerUsesDefaultsForTech)
    }; // Size: 912 (validated)
    
    __declspec(align(1)) struct UNIT {
        //Unit units[0]; // Unit units[numUnits];
    }; // Size: Multiple of 36 (validated)
    
    __declspec(align(1)) struct ISOM {
        //IsomRect isomRect[0]; // IsomRect isomRects[tileWidth/2 + 1][tileHeight + 1]; // Access x*2, width*y
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
        u16 numStrings {1024}; // Number of strings in the section (default 1024)
        // u16 stringOffsets[0]; // u16 stringOffsets[numStrings]; // stringId: 1 = stringOffsets[0];
                                 // Offsets to each string within the string section (not within stringData, but within the whole section)
        // void[] stringData; // Character data, usually starting wiht a NUL character, unless recycling sub-strings each string is null terminated
        // Possible non-string data appended after stringData
    }; // Size >= 1 (validated)
    
    __declspec(align(1)) struct UPRP {
        Cuwp createUnitProperties[Sc::Unit::MaxCuwps] {};
    }; // Size: 1280 (validated)
    
    __declspec(align(1)) struct UPUS {
        CuwpUsed cuwpUsed[Sc::Unit::MaxCuwps] {};
    }; // Size: 64 (validated
    
    __declspec(align(1)) struct MRGN {
        Location locations[TotalLocations] {}; // If this is a vanilla map, only the first 64 locations are usable
    }; // Size: 1280 in Original, 5100 in Hybrid or Broodwar (validated)
    
    __declspec(align(1)) struct TRIG {
        // Trigger triggers[0]; // Trigger[numTriggers] triggers;
    }; // Size: Multiple of 2400 (validated)

    __declspec(align(1)) struct MBRF {
        // Trigger triggers[0]; // Chk::Trigger[numTriggers];
    }; // Size: Multiple of 2400 (validated)
    
    __declspec(align(1)) struct SPRP {
        u16 scenarioNameStringId {1};
        u16 scenarioDescriptionStringId {2};

        REFLECT(SPRP, scenarioNameStringId, scenarioDescriptionStringId)
    }; // Size: 4 (validated)
    
    __declspec(align(1)) struct FORC {
        Force playerForce[Sc::Player::TotalSlots] {
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1,
            Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1, Chk::Force::Force1
        };
        u16 forceString[TotalForces] {4, 5, 6, 7};
        u8 flags[TotalForces] {Chk::ForceFlags::All, Chk::ForceFlags::All, Chk::ForceFlags::All, Chk::ForceFlags::All};
    }; // Size <= 20 (validated)
    
    __declspec(align(1)) struct WAV {
        u32 soundPathStringId[TotalSounds] {};
    }; // Size: 2048 (not validated)
    
    __declspec(align(1)) struct UNIS {
        UseDefault useDefault[Sc::Unit::TotalTypes] {};
        u32 hitpoints[Sc::Unit::TotalTypes] {}; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes] {};
        u8 armorLevel[Sc::Unit::TotalTypes] {}; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes] {};
        u16 mineralCost[Sc::Unit::TotalTypes] {};
        u16 gasCost[Sc::Unit::TotalTypes] {};
        u16 nameStringId[Sc::Unit::TotalTypes] {};
        u16 baseDamage[Sc::Weapon::TotalOriginal] {};
        u16 upgradeDamage[Sc::Weapon::TotalOriginal] {};

        UNIS() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
        }

        REFLECT(UNIS, useDefault, hitpoints, shieldPoints, armorLevel, buildTime, mineralCost, gasCost, nameStringId, baseDamage, upgradeDamage)
    }; // Size: 4048 (validated)

    __declspec(align(1)) struct UPGS {
        UseDefault useDefault[Sc::Upgrade::TotalOriginalTypes] {};
        u16 baseMineralCost[Sc::Upgrade::TotalOriginalTypes] {};
        u16 mineralCostFactor[Sc::Upgrade::TotalOriginalTypes] {};
        u16 baseGasCost[Sc::Upgrade::TotalOriginalTypes] {};
        u16 gasCostFactor[Sc::Upgrade::TotalOriginalTypes] {};
        u16 baseResearchTime[Sc::Upgrade::TotalOriginalTypes] {};
        u16 researchTimeFactor[Sc::Upgrade::TotalOriginalTypes] {};

        UPGS() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalOriginalTypes);
        }

        REFLECT(UPGS, useDefault, baseMineralCost, mineralCostFactor, baseGasCost, gasCostFactor, baseResearchTime, researchTimeFactor)
    }; // Size: 598 (validated)
    
    __declspec(align(1)) struct TECS {
        UseDefault useDefault[Sc::Tech::TotalOriginalTypes] {};
        u16 mineralCost[Sc::Tech::TotalOriginalTypes] {};
        u16 gasCost[Sc::Tech::TotalOriginalTypes] {};
        u16 researchTime[Sc::Tech::TotalOriginalTypes] {};
        u16 energyCost[Sc::Tech::TotalOriginalTypes] {};

        TECS() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalOriginalTypes);
        }

        REFLECT(TECS, useDefault, mineralCost, gasCost, researchTime, energyCost)
    }; // Size: 216 (validated)
    
    __declspec(align(1)) struct SWNM {
        u32 switchName[TotalSwitches] {};
    }; // Size: 1024 (not validated)
    
    __declspec(align(1)) struct COLR {
        PlayerColor playerColor[Sc::Player::TotalSlots] {
            PlayerColor::Red, PlayerColor::Blue, PlayerColor::Teal, PlayerColor::Purple,
            PlayerColor::Orange, PlayerColor::Brown, PlayerColor::White, PlayerColor::Yellow
        };
    }; // Size: 8 (validated)
    
    __declspec(align(1)) struct PUPx {
        u8 playerMaxUpgradeLevel[Sc::Player::Total][Sc::Upgrade::TotalTypes] {};
        u8 playerStartUpgradeLevel[Sc::Player::Total][Sc::Upgrade::TotalTypes] {};
        u8 defaultMaxLevel[Sc::Upgrade::TotalTypes] {
            3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,    3, 3, 3, 3,
            1, 1, 0, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,
            1, 1, 1, 1,    1, 1, 1, 1,    1, 1, 1, 1,    1, 0, 0, 1,
            0, 1, 0, 1,    1, 1, 1, 0,    0, 0, 0, 0,    0
        };
        u8 defaultStartLevel[Sc::Upgrade::TotalTypes] {};
        UseDefault playerUpgradeUsesDefault[Sc::Player::Total][Sc::Upgrade::TotalTypes] {};

        PUPx() {
            memset(&playerUpgradeUsesDefault, (int)Chk::UseDefault::Yes, Sc::Player::Total*Sc::Upgrade::TotalTypes);
        }

        REFLECT(PUPx, playerMaxUpgradeLevel, playerStartUpgradeLevel, defaultMaxLevel, defaultStartLevel, playerUpgradeUsesDefault)
    }; // Size: 2318 (validated)
    
    __declspec(align(1)) struct PTEx {
        Available techAvailableForPlayer[Sc::Player::Total][Sc::Tech::TotalTypes] {};
        Researched techResearchedForPlayer[Sc::Player::Total][Sc::Tech::TotalTypes] {};
        Available techAvailableByDefault[Sc::Tech::TotalTypes] {};
        Researched techResearchedByDefault[Sc::Tech::TotalTypes] {
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No ,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No ,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::Yes,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::Yes, Chk::Researched::Yes, Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No,
            Chk::Researched::No , Chk::Researched::No , Chk::Researched::No , Chk::Researched::No
        };
        UseDefault playerUsesDefaultsForTech[Sc::Player::Total][Sc::Tech::TotalTypes] {};

        PTEx() {
            memset(&techAvailableByDefault, (int)Chk::Available::Yes, Sc::Tech::TotalTypes);
            memset(&playerUsesDefaultsForTech, (int)Chk::UseDefault::Yes, Sc::Player::Total*Sc::Tech::TotalTypes);
        }

        REFLECT(PTEx, techAvailableForPlayer, techResearchedForPlayer, techAvailableByDefault, techResearchedByDefault, playerUsesDefaultsForTech)
    }; // Size: 1672 (validated)
    
    __declspec(align(1)) struct UNIx {
        UseDefault useDefault[Sc::Unit::TotalTypes] {};
        u32 hitpoints[Sc::Unit::TotalTypes] {}; // Displayed value is hitpoints / 256
        u16 shieldPoints[Sc::Unit::TotalTypes] {};
        u8 armorLevel[Sc::Unit::TotalTypes] {}; // Note: 255 armor + ultra armor upgrade = 0 armor
        u16 buildTime[Sc::Unit::TotalTypes] {};
        u16 mineralCost[Sc::Unit::TotalTypes] {};
        u16 gasCost[Sc::Unit::TotalTypes] {};
        u16 nameStringId[Sc::Unit::TotalTypes] {};
        u16 baseDamage[Sc::Weapon::Total] {};
        u16 upgradeDamage[Sc::Weapon::Total] {};

        UNIx() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Unit::TotalTypes);
        }

        REFLECT(UNIx, useDefault, hitpoints, shieldPoints, armorLevel, buildTime, mineralCost, gasCost, nameStringId, baseDamage, upgradeDamage)
    }; // Size: 4168 (validated)
    
    __declspec(align(1)) struct UPGx {
        UseDefault useDefault[Sc::Upgrade::TotalTypes] {};
        u8 unused {};
        u16 baseMineralCost[Sc::Upgrade::TotalTypes] {};
        u16 mineralCostFactor[Sc::Upgrade::TotalTypes] {};
        u16 baseGasCost[Sc::Upgrade::TotalTypes] {};
        u16 gasCostFactor[Sc::Upgrade::TotalTypes] {};
        u16 baseResearchTime[Sc::Upgrade::TotalTypes] {};
        u16 researchTimeFactor[Sc::Upgrade::TotalTypes] {};

        UPGx() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Upgrade::TotalTypes);
        }

        REFLECT(UPGx, useDefault, unused, baseMineralCost, mineralCostFactor, baseGasCost, gasCostFactor, baseResearchTime, researchTimeFactor)
    }; // Size: 794 (validated)
    
    __declspec(align(1)) struct TECx {
        UseDefault useDefault[Sc::Tech::TotalTypes] {};
        u16 mineralCost[Sc::Tech::TotalTypes] {};
        u16 gasCost[Sc::Tech::TotalTypes] {};
        u16 researchTime[Sc::Tech::TotalTypes] {};
        u16 energyCost[Sc::Tech::TotalTypes] {};

        TECx() {
            memset(&useDefault, (int)Chk::UseDefault::Yes, Sc::Tech::TotalTypes);
        }

        REFLECT(TECx, useDefault, mineralCost, gasCost, researchTime, energyCost)
    }; // Size: 396 (validated)
    
    __declspec(align(1)) struct OSTR {
        u32 version {1}; // Current version: 1
        u32 scenarioName {};
        u32 scenarioDescription {};
        u32 forceName[Chk::TotalForces] {};
        u32 unitName[Sc::Unit::TotalTypes] {};
        u32 expUnitName[Sc::Unit::TotalTypes] {};
        u32 soundPath[Chk::TotalSounds] {};
        u32 switchName[Chk::TotalSwitches] {};
        u32 locationName[Chk::TotalLocations] {};

        REFLECT(OSTR, version, scenarioName, scenarioDescription, forceName, unitName, expUnitName, soundPath, switchName, locationName)
    };

    __declspec(align(1)) struct KSTR {
        static constexpr const u32 CurrentVersion = 3;

        u32 version {CurrentVersion}; // Current version: 3
        u32 numStrings {0}; // Number of strings in the section
        //u32 stringOffsets[0]; // u32 stringOffsets[numStrings]; // Offsets to each string within the string section (not within stringData, but within the whole section)
        // StringProperties[numStrings] stringProperties; // String properties
        // void[] stringData; // List of strings, each null terminated, starting with one NUL character
    }; // Size: 8+8*numStrings+stringDataSize (not validated)
    
    enum_t(ExtendedTrigDataIndex, u32, {
        None = 0xFFFFFFFF,
        CheckExtended = 0xFEFEFEFE,
        GetIndex = 0x00FFFFFF,
        MaxIndex = 0x00FEFEFE
    });

    __declspec(align(1)) struct KTRG {
        u32 version {2}; // Current version: 2
        //ExtendedTrigData extendedTrigData[numTrigs];
    }; // Size: 4+20*numTrigs

    __declspec(align(1)) struct KTGP {
        u32 version {1}; // Current version: 1
        u32 numGroups {0}; // The number of trigger groupings in this section
        //TriggerGroupHeader headers[numGroups];
        //TriggerGroupBody bodies[numGroups];
    };

    __declspec(align(1)) struct CRGB {
        u8 playerColor[8][3] {
            {0, 0, 0}, {0, 0, 1}, {0, 0, 2}, {0, 0, 3},
            {0, 0, 4}, {0, 0, 5}, {0, 0, 6}, {0, 0, 7}
        }; // RGB or unused-unused-colorIndex depending on playerSetting
        PlayerColorSetting playerSetting[8] {
            PlayerColorSetting::UseId, PlayerColorSetting::UseId, PlayerColorSetting::UseId, PlayerColorSetting::UseId,
            PlayerColorSetting::UseId, PlayerColorSetting::UseId, PlayerColorSetting::UseId, PlayerColorSetting::UseId
        };

        REFLECT(CRGB, playerColor, playerSetting)
    };

    struct Rgb {
        u8 red {};
        u8 green {};
        u8 blue {};
    };

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
        UNIx = 2018070101, UPGx = 2017939541, TECx = 2017674580, STRx = 2018661459,
        CRGB = 1111970371,

        OSTR = 1381258063, KSTR = 1381258059, KTRG = 1196577867, KTGP = 1346851915,

        UNKNOWN = u32_max
    });

    enum class LoadBehavior
    {
        Standard, // The last instance of a section is used
        Override, // The first instance of the section has part or all of its data overridden by subsequent instances
        Append // Subsequent instances of the section will be appended to the first instance
    };
    
    inline std::unordered_map<SectionName, LoadBehavior> nonStandardLoadBehaviors {
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::MTXM, Chk::LoadBehavior::Override),
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::UNIT, Chk::LoadBehavior::Append),
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::THG2, Chk::LoadBehavior::Append),
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::STR, Chk::LoadBehavior::Override),
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::TRIG, Chk::LoadBehavior::Append),
        std::pair<SectionName, Chk::LoadBehavior>(SectionName::MBRF, Chk::LoadBehavior::Append)
    };

    inline std::unordered_map<SectionName, std::string> sectionNameStrings {
        std::pair<SectionName, std::string>(SectionName::TYPE, "TYPE"), std::pair<SectionName, std::string>(SectionName::VER, "VER"),
        std::pair<SectionName, std::string>(SectionName::IVER, "IVER"), std::pair<SectionName, std::string>(SectionName::IVE2, "IVE2"),
        std::pair<SectionName, std::string>(SectionName::VCOD, "VCOD"), std::pair<SectionName, std::string>(SectionName::IOWN, "IOWN"),
        std::pair<SectionName, std::string>(SectionName::OWNR, "OWNR"), std::pair<SectionName, std::string>(SectionName::ERA, "ERA"),
        std::pair<SectionName, std::string>(SectionName::DIM, "DIM"), std::pair<SectionName, std::string>(SectionName::SIDE, "SIDE"),
        std::pair<SectionName, std::string>(SectionName::MTXM, "MTXM"), std::pair<SectionName, std::string>(SectionName::PUNI, "PUNI"),
        std::pair<SectionName, std::string>(SectionName::UPGR, "UPGR"), std::pair<SectionName, std::string>(SectionName::PTEC, "PTEC"),
        std::pair<SectionName, std::string>(SectionName::UNIT, "UNIT"), std::pair<SectionName, std::string>(SectionName::ISOM, "ISOM"),
        std::pair<SectionName, std::string>(SectionName::TILE, "TILE"), std::pair<SectionName, std::string>(SectionName::DD2, "DD2"),
        std::pair<SectionName, std::string>(SectionName::THG2, "THG2"), std::pair<SectionName, std::string>(SectionName::MASK, "MASK"),
        std::pair<SectionName, std::string>(SectionName::STR, "STR"), std::pair<SectionName, std::string>(SectionName::UPRP, "UPRP"),
        std::pair<SectionName, std::string>(SectionName::UPUS, "UPUS"), std::pair<SectionName, std::string>(SectionName::MRGN, "MRGN"),
        std::pair<SectionName, std::string>(SectionName::TRIG, "TRIG"), std::pair<SectionName, std::string>(SectionName::MBRF, "MBRF"),
        std::pair<SectionName, std::string>(SectionName::SPRP, "SPRP"), std::pair<SectionName, std::string>(SectionName::FORC, "FORC"),
        std::pair<SectionName, std::string>(SectionName::WAV, "WAV"), std::pair<SectionName, std::string>(SectionName::UNIS, "UNIS"),
        std::pair<SectionName, std::string>(SectionName::UPGS, "UPGS"), std::pair<SectionName, std::string>(SectionName::TECS, "TECS"),
        std::pair<SectionName, std::string>(SectionName::SWNM, "SWNM"), std::pair<SectionName, std::string>(SectionName::COLR, "COLR"),
        std::pair<SectionName, std::string>(SectionName::PUPx, "PUPx"), std::pair<SectionName, std::string>(SectionName::PTEx, "PTEx"),
        std::pair<SectionName, std::string>(SectionName::UNIx, "UNIx"), std::pair<SectionName, std::string>(SectionName::UPGx, "UPGx"),
        std::pair<SectionName, std::string>(SectionName::TECx, "TECx"), std::pair<SectionName, std::string>(SectionName::STRx, "STRx"),
        std::pair<SectionName, std::string>(SectionName::CRGB, "CRGB"),
    
        std::pair<SectionName, std::string>(SectionName::OSTR, "OSTR"), std::pair<SectionName, std::string>(SectionName::KSTR, "KSTR"),
        std::pair<SectionName, std::string>(SectionName::KTRG, "KTRG"), std::pair<SectionName, std::string>(SectionName::KTGP, "KTGP"),

        std::pair<SectionName, std::string>(SectionName::UNKNOWN, "UNKNOWN")
    };

    std::string getNameString(SectionName sectionName);

    enum_t(SectionIndex, u32, { // The index at which a section appears in the default scenario file (plus indexes for extended sections), this is not related to section names
        TYPE = 0, VER = 1, IVER = 2, IVE2 = 3,
        VCOD = 4, IOWN = 5, OWNR = 6, ERA = 7,
        DIM = 8, SIDE = 9, MTXM = 10, PUNI = 11,
        UPGR = 12, PTEC = 13, UNIT = 14, ISOM = 15,
        TILE = 16, DD2 = 17, THG2 = 18, MASK = 19,
        STR = 20, UPRP = 21, UPUS = 22, MRGN = 23,
        TRIG = 24, MBRF = 25, SPRP = 26, FORC = 27,
        WAV = 28, UNIS = 29, UPGS = 30, TECS = 31,
        SWNM = 32, COLR = 33, PUPx = 34, PTEx = 35,
        UNIx = 36, UPGx = 37, TECx = 38, STRx = 39,
        CRGB = 40,

        OSTR = 41, KSTR = 42, KTRG = 43, KTGP = 44,

        UNKNOWN = u32_max
    });
    
    static constexpr u32 TotalKnownChkSections = 40;
    
    using SectionSize = s32;
    static constexpr Chk::SectionSize MaxChkSectionSize = s32_max;

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
    std::ostream & operator<< (std::ostream & out, const IsomRect & isomRect);
    std::ostream & operator<< (std::ostream & out, const Doodad & doodad);
    std::ostream & operator<< (std::ostream & out, const Sprite & sprite);
    std::ostream & operator<< (std::ostream & out, const Location & location);
    std::ostream & operator<< (std::ostream & out, const Condition & condition);
    std::ostream & operator<< (std::ostream & out, const Action & action);
    std::ostream & operator<< (std::ostream & out, const Trigger & trigger);

#pragma pack(pop)

    /** None - No compression methods applied
        DuplicateStringRecycling - All duplicate strings share one stringId
        LastStringTrick - The largest has the highest offset value
        ReverseStacking - All offsets use the highest necessary string ids such that character data preceeds all offsets (the NUL/zero-length string loses its sticky position)
        SizeBytesRecycling - If possible to do so, the bytes denoting the number of strings in the section are also used as characters
        SubStringRecycling - All eligible sub-strings (strings that fit in the end of some other string) are recycled
        OffsetInterlacing - Offsets are combined with character data where possible
        OrderShuffledInterlacing - OffsetInterlacing is attempted with strings arranged in every possible order
        SpareShuffledInterlacing - OffsetInterlacing is attempted with every possible distribution of one to total spare codepoints
        SubShuffledInterlacing - OffsetInterlacing is attempted with every possible permutation of potentially advantagous sub-strings unrecycled or recycled
            
        Fighting three limits...
        1.) Max Strings     :   numStrings <= 65535
        2.) Max Characters  :   SUM(LENGTH(rootStrings)) + numRootStrings - (LastStringTrick ? LENGTH(lastString) : 0) - (SizeBytesRecycling ? 0 : 2) < 65536 &&
                                SUM(LENGTH(rootStrings)) + numRootStrings <= 2147483647
        3.) Chars & Offsets :   IF ( LastStringTrick && MAX(LENGTH(string)) > 65538 && MAX(stringOffset) + LENGTH(lastString) >= 131074 ) {
                                    2147483647 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings +
                                        (SizeBytesRecycling ? 0 : 2) +
                                        (OffsetInterlacing ? offsetBytesBlocked - offsetBytesRecycled : 0) &&
                                    131074 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings - (LENGTH(lastString) + MAX(stringOffset) - 131074)
                                        (SizeBytesRecycling ? 0 : 2) +
                                        (OffsetInterlacing ? offsetBytesRecycled - offsetBytesBlocked : 0)
                                } else { // No excessively large last string
                                    131074 >= SUM(LENGTH(rootStrings)) + numRootStrings + 2*numStrings +  +
                                        (SizeByteRecycling ? 0 : 2) +
                                        (OffsetInterlacing ? offsetBytesBlocked - offsetBytesRecycled : 0)
                                }

        1.) This hard limit is given by numStrings being a u16, you only have usuable offsets at indexes 0-65535/stringIds 1-65536 .
        2.) The first part of this limit is given by string offsets being u16, such that no string points past byte 65535 .
            Each root string (potentially containing substrings) occupies its length plus one NUL character.
            The last string/substrings may have an ending that flows onto the 65536th byte and beyond, if it does, then that NUL character can be ignored.
            The first two bytes of the section which denote numStrings may also be used as characters.
            Even with the last string flowing over the regular limit, you'll still hit the section max (s32_max: 2147483647) eventually.
        3.) Offsets being an array of u16s with a max index of numStrings (65535) can only occupy space before the 131074th byte
            If the last string flows beyond that point then we're given two limits: first the limit of the section size
            and second all characters and offsets minus any overhang from the last string must fit in the first 131074 bytes.
            Else we're given one limit: all characters and offsets must fit in a space no larger than 131074 bytes.

        (1) is addressed only by "DuplicateStringRecycling", no other compression methods reduce the string count (sub-strings still have their own offset/are their own string)
        (2) is addressed by "DuplicateStringRecycling", "LastStringTrick", "ReverseStacking", "SizeBytesRecycling", and "SubStringRecycling"
        (3) is addressed by everything that addressed (2) (except "LastStringTrick"), plus all types of offset interlacing

        - Automatically fail an operation if none of the compression methods affecting that limit, combined in every possible way can make the above equations satisfy.
        - Calculate section configuration viability before physically laying them out in memory
    */
    enum_t(StrCompressFlag, u32, {
        DuplicateStringRecycling = BIT_0,
        LastStringTrick = BIT_1,
        ReverseStacking = BIT_2,
        SizeBytesRecycling = BIT_3,
        SubStringRecycling = BIT_4 | DuplicateStringRecycling,
        OffsetInterlacing = BIT_5 | SubStringRecycling, // When active, ignores ReverseStacking
        OrderShuffledInterlacing = BIT_6 | OffsetInterlacing, // When active, ignores LastStringTrick
        SpareShuffledInterlacing = BIT_7 | OffsetInterlacing,
        SubShuffledInterlacing = BIT_8 | OffsetInterlacing & (~SubStringRecycling) | DuplicateStringRecycling, // When active, ignores 

        SubLastStringTrick = SubStringRecycling | LastStringTrick, // Causes different LastStringTrick behavior
        SizedLastStringTrick = SizeBytesRecycling | LastStringTrick, // Causes different LastStringTrick behavior
        SubSizedLastStringTrick = SubStringRecycling | SizeBytesRecycling | LastStringTrick, // Causes different LastStringTrick behavior
        None = 0,
        AllNonInterlacing = DuplicateStringRecycling | LastStringTrick | ReverseStacking | SizeBytesRecycling | SubStringRecycling,
        AllInterlacing = OffsetInterlacing | OrderShuffledInterlacing | SpareShuffledInterlacing | SubShuffledInterlacing,
        All = AllNonInterlacing | AllInterlacing,
        Unchanged = u32_max,
        Default = DuplicateStringRecycling
    });
    
    class SectionValidationException : public std::exception
    {
        public:
            SectionValidationException(SectionName sectionName, std::string error);
            virtual ~SectionValidationException() { }
            virtual const char* what() const throw() { return error.c_str(); }

        private:
            std::string error;
            SectionValidationException(); // Disallow ctor
    };

    class StringException : public std::exception
    {
        public:
            StringException(const std::string & error) : error(error) { }
            virtual const char* what() const throw() { return error.c_str(); }
            const std::string error;
        private:
            StringException(); // Disallow ctor
    };

    class MaximumStringsExceeded : public StringException
    {
        public:
            MaximumStringsExceeded(std::string sectionName, size_t numStrings, size_t maxStrings);
            MaximumStringsExceeded();
            virtual const char* what() const throw() { return error.c_str(); }
    };

    class InsufficientStringCapacity : public StringException
    {
        public:
            InsufficientStringCapacity(std::string sectionName, size_t numStrings, size_t requestedCapacity, bool autoDefragment);
            virtual const char* what() const throw() { return error.c_str(); }
        private:
            InsufficientStringCapacity(); // Disallow ctor
    };

    class MaximumCharactersExceeded : public StringException
    {
        public:
            MaximumCharactersExceeded(std::string sectionName, size_t numCharacters, size_t characterSpaceSize);
            virtual const char* what() const throw() { return error.c_str(); }
        private:
            MaximumCharactersExceeded(); // Disallow ctor
    };

    class MaximumOffsetAndCharsExceeded : public StringException
    {
        public:
            MaximumOffsetAndCharsExceeded(std::string sectionName, size_t numStrings, size_t numCharacters, size_t sectionSpace);
            virtual const char* what() const throw() { return error.c_str(); }
        private:
            MaximumOffsetAndCharsExceeded(); // Disallow ctor
    };

    class StrProp {
        public:
            u8 red;
            u8 green;
            u8 blue;
            u32 size;
            bool isUsed;
            bool hasPriority;
            bool isBold;
            bool isUnderlined;
            bool isItalics;
    
            StrProp();
            StrProp(Chk::StringProperties stringProperties);
            StrProp(u8 red, u8 green, u8 blue, u32 size, bool isUsed, bool hasPriority, bool isBold, bool isUnderlined, bool isItalics);
    };

    class ScStr
    {
        public:
            const char* str; // The content of this string (RawString/no formatting)
        
            ScStr();
            ScStr(const std::string & str);
            ScStr(const std::string & str, const StrProp & strProp);
            ScStr(const char* str);
            ScStr(const u8* str);
            ScStr(const u8* str, size_t length);
            ScStr(const std::vector<u8> & strBytes);
            template <size_t N> ScStr(const char (&str)[N]) : ScStr(std::string(str)) {}

            bool empty() const;
            size_t length() const;
        
            StrProp & properties();
            const StrProp & properties() const;

            template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
            int compare(const StringType & str) const;

            template <typename StringType> // Strings may be RawString (no escaping), EscString (C++ style \r\r escape characters) or ChkdString (Editor <01>Style)
            StringType toString() const;

            ScStr* getParentStr() const;
            ScStr* getChildStr() const;
            static bool adopt(ScStr* lhs, ScStr* rhs); // Make lhs the parent of rhs if possible, or vice-versa
            void disown(); // Break any connections with parent and child strings

        private:
            ScStr* parentStr = nullptr; // The larger string inside which this string fits, if null then this is a root string
            ScStr* childStr = nullptr; // The next largest string that fits inside this string
            std::vector<char> allocation; // If parentStr is null, then this is the actual string data and str points to the first character
                                          // else str points to first character of this string within the allocation of the highest-order parent
            StrProp strProp; // Additional color and font details, if this string is extended and gets stored

            static void adopt(ScStr* parent, ScStr* child, size_t parentLength, size_t childLength, const char* parentSubString);
    };

    struct IsomRectUndo {
        Chk::IsomDiamond diamond {};
        Chk::IsomRect oldValue {};
        Chk::IsomRect newValue {};

        constexpr void setOldValue(const Chk::IsomRect & oldValue) {
            this->oldValue.left = oldValue.left & Chk::IsomRect::EditorFlag::ClearAll;
            this->oldValue.right = oldValue.right & Chk::IsomRect::EditorFlag::ClearAll;
            this->oldValue.top = oldValue.top & Chk::IsomRect::EditorFlag::ClearAll;
            this->oldValue.bottom = oldValue.bottom & Chk::IsomRect::EditorFlag::ClearAll;
        }

        constexpr void setNewValue(const Chk::IsomRect & newValue) {
            this->newValue.left = newValue.left & Chk::IsomRect::EditorFlag::ClearAll;
            this->newValue.right = newValue.right & Chk::IsomRect::EditorFlag::ClearAll;
            this->newValue.top = newValue.top & Chk::IsomRect::EditorFlag::ClearAll;
            this->newValue.bottom = newValue.bottom & Chk::IsomRect::EditorFlag::ClearAll;
        }

        IsomRectUndo(Chk::IsomDiamond diamond, const Chk::IsomRect & oldValue, const Chk::IsomRect & newValue)
            : diamond(diamond)
        {
            setOldValue(oldValue);
            setNewValue(newValue);
        }
    };

    // IsomCache holds all the data required to edit isometric terrain which is not a part of scenario; as well as methods that operate on said data exclusively
    // IsomCache is invalidated & must be re-created whenever tileset, map width, or map height changes
    struct IsomCache
    {
        size_t isomWidth; // This is a sort of isometric width, not tileWidth
        size_t isomHeight; // This is a sort of isometric height, not tileHeight
        Sc::BoundingBox changedArea {};

        std::vector<std::optional<IsomRectUndo>> undoMap {}; // Undo per x, y coordinate

        Span<Sc::Terrain::TileGroup> tileGroups {};
        Span<Sc::Isom::ShapeLinks> isomLinks {};
        Span<Sc::Isom::TerrainTypeInfo> terrainTypes {};
        Span<uint16_t> terrainTypeMap {};
        const std::unordered_map<uint32_t, std::vector<uint16_t>>* hashToTileGroup;

        inline IsomCache(Sc::Terrain::Tileset tileset, size_t tileWidth, size_t tileHeight, const Sc::Terrain::Tiles & tilesetData) :
            isomWidth(tileWidth/2 + 1),
            isomHeight(tileHeight + 1),
            tileGroups(&tilesetData.tileGroups[0], tilesetData.tileGroups.size()),
            isomLinks(&tilesetData.isomLinks[0], tilesetData.isomLinks.size()),
            terrainTypes(&tilesetData.terrainTypes[0], tilesetData.terrainTypes.size()),
            terrainTypeMap(&tilesetData.terrainTypeMap[0], tilesetData.terrainTypeMap.size()),
            hashToTileGroup(&tilesetData.hashToTileGroup),
            undoMap(isomWidth*isomHeight, std::nullopt)
        {
            resetChangedArea();
        }

        constexpr void resetChangedArea()
        {
            changedArea.left = isomWidth;
            changedArea.right = 0;
            changedArea.top = isomHeight;
            changedArea.bottom = 0;
        }

        constexpr void setAllChanged()
        {
            changedArea.left = 0;
            changedArea.right = isomWidth-1;
            changedArea.top = 0;
            changedArea.bottom = isomHeight-1;
        }

        constexpr uint16_t getTerrainTypeIsomValue(size_t terrainType) const
        {
            return terrainType < terrainTypes.size() ? terrainTypes[terrainType].isomValue : 0;
        }

        inline uint16_t getRandomSubtile(uint16_t tileGroup) const
        {
            if ( tileGroup < tileGroups.size() )
            {
                size_t totalCommon = 0;
                size_t totalRare = 0;
                for ( ; totalCommon < 16 && tileGroups[tileGroup].megaTileIndex[totalCommon] != 0; ++totalCommon );
                for ( ; totalCommon+totalRare+1 < 16 && tileGroups[tileGroup].megaTileIndex[totalCommon+totalRare+1] != 0; ++totalRare );

                if ( totalRare != 0 && std::rand() <= RAND_MAX / 20 ) // 1 in 20 chance of using a rare tile
                    return 16*tileGroup + uint16_t(totalCommon + 1 + (std::rand() % totalRare)); // Select particular rare tile
                else if ( totalCommon != 0 ) // Use a common tile
                    return 16*tileGroup + uint16_t(std::rand() % totalCommon); // Select particular common tile
            }
            return 16*tileGroup; // Default/fall-back to first tile in group
        }

        virtual inline void setTileValue(size_t tileX, size_t tileY, uint16_t tileValue) {} // Does nothing unless overridden

        virtual inline void addIsomUndo(const IsomRectUndo & /*isomUndo*/) {} // Does nothing unless overridden

        // Call when one undoable operation is complete, e.g. resize a map, or mouse up after pasting/brushing some terrain
        // When changing lots of terrain (e.g. by holding the mouse button and moving around), undos are blocked from being added to the same tiles multiple times
        // Calling this method clears out said blockers
        inline void finalizeUndoableOperation()
        {
            undoMap.assign(isomWidth*isomHeight, std::nullopt); // Clears out the undoMap so new entries can be set
        }
    };

    // When tiles change... doodads, units, and possibly other things may be invalidated and need to be removed (depending on editor configuration settings)
    // It can be very expensive to check for what occupies tiles when performing a lot of tile operations at once (e.g. dragging around an ISOM brush)
    // TileOccupationCache provides a constant-runtime means of checking whether a tile is occupied and potentially requires further operations
    class TileOccupationCache
    {
        std::vector<bool> tileIsOccupied {};

    public:
        TileOccupationCache() = default;

        TileOccupationCache(std::vector<bool> & tileOccupied) {
            this->tileIsOccupied.swap(tileOccupied);
        }

        bool tileOccupied(size_t x, size_t y, size_t tileWidth) const {
            return tileIsOccupied[tileWidth*y+x];
        }

        void swap(TileOccupationCache & other) {
            this->tileIsOccupied.swap(other.tileIsOccupied);
        }
    };
}

using Chk::SectionName;
using Chk::StrProp;
using Chk::ScStr;

#endif