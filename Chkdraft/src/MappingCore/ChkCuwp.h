#ifndef CHKCUWP_H
#define CHKCUWP_H
#include "Basics.h"

class ChkCuwp
{
    public:
        u16 validSpecProps;
        u16 validElements;
        u8 owner; // Always 0?
        u8 percentHitpoints;
        u8 percentShieldPoints;
        u8 percentEnergyPoints;
        u32 resourceAmount;
        u16 numInHanger;
        u16 flags;
        u32 unknown;

        bool IsCloakValid();
        bool IsBurrowValid();
        bool IsTransitValid();
        bool IsHallucinateValid();
        bool IsInvincibleValid();

        void SetCloakIsValid(bool cloakIsValid);
        void SetBurrowIsValid(bool burrowIsValid);
        void SetInTransitIsValid(bool inTransitIsValid);
        void SetHallucinatedIsValid(bool hallucinatedIsValid);
        void SetInvincibleIsValid(bool invincibleIsValid);

        bool IsOwnerValid();
        bool IsHitpointsValid();
        bool IsShieldsValid();
        bool IsEnergyValid();
        bool IsResourceAmountValid();
        bool IsValidElementsBit6Set();

        void SetOwnerIsValid(bool ownerIsValid);
        void SetHitpointsIsValid(bool hitpointsIsValid);
        void SetShieldsIsValid(bool shieldsIsValid);
        void SetEnergyIsValid(bool energyIsValid);
        void SetResourceAmountIsValid(bool resourceAmountIsValid);
        void SetValidElementsBit6(bool validElementsBit6);

        bool IsUnitCloaked();
        bool IsUnitBurrowed();
        bool IsBuildingInTransit();
        bool IsUnitHallucinated();
        bool IsUnitInvincible();

        void SetUnitIsCloaked(bool unitIsCloaked);
        void SetUnitIsBurrowed(bool unitIsBurrowed);
        void SetBuildingInTransit(bool buildingInTransit);
        void SetUnitIsHallucinated(bool unitIsHallucinated);
        void SetUnitIsInvincible(bool unitIsInvincible);
};

enum class ValidCUWPSpecPropFlags : uint16_t
{
    CloakIsValid = BIT_0,
    BurrowIsValid = BIT_1,
    InTransitIsValid = BIT_2,
    HallucinateIsValid = BIT_3,
    InvincibleIsValid = BIT_4,

    xCloakIsValid = x16BIT_0,
    xBurrowIsValid = x16BIT_1,
    xInTransitIsValid = x16BIT_2,
    xHallucinateIsValid = x16BIT_3,
    xInvincibleIsValid = x16BIT_4
};

enum class ValidCUWPElementFlags : uint16_t
{
    OwnerIsValid = BIT_0,
    HitpointsIsValid = BIT_1,
    ShieldsIsValid = BIT_2,
    EnergyIsValid = BIT_3,
    ResourceAmountIsValid = BIT_4,
    HangerAmountIsValid = BIT_5,
    UnknownBit6 = BIT_6,

    xOwnerIsValid = x16BIT_0,
    xHitpointsIsValid = x16BIT_1,
    xShieldsIsValid = x16BIT_2,
    xEnergyIsValid = x16BIT_3,
    xResourceAmountIsValid = x16BIT_4,
    xHangerAmountIsValid = x16BIT_5,
    xUnknownBit6 = x16BIT_6
};

enum class CUWPUnitPropertyFlags : uint16_t
{
    UnitIsCloaked = BIT_0,
    UnitIsBurrowed = BIT_1,
    BuildingInTransit = BIT_2,
    UnitIsHallucinated = BIT_3,
    UnitIsInvincible = BIT_4,

    xUnitIsCloaked = x16BIT_0,
    xUnitIsBurrowed = x16BIT_1,
    xBuildingInTransit = x16BIT_2,
    xUnitIsHallucinated = x16BIT_3,
    xUnitIsInvincible = x16BIT_4
};

#endif