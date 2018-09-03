#include "ChkCuwp.h"

bool ChkCuwp::IsCloakValid()
{
    return (validSpecProps & (u16)ValidCUWPSpecPropFlags::CloakIsValid) == (u16)ValidCUWPSpecPropFlags::CloakIsValid;
}

bool ChkCuwp::IsBurrowValid()
{
    return (validSpecProps & (u16)ValidCUWPSpecPropFlags::BurrowIsValid) == (u16)ValidCUWPSpecPropFlags::BurrowIsValid;
}

bool ChkCuwp::IsTransitValid()
{
    return (validSpecProps & (u16)ValidCUWPSpecPropFlags::InTransitIsValid) == (u16)ValidCUWPSpecPropFlags::InTransitIsValid;
}

bool ChkCuwp::IsHallucinateValid()
{
    return (validSpecProps & (u16)ValidCUWPSpecPropFlags::HallucinateIsValid) == (u16)ValidCUWPSpecPropFlags::HallucinateIsValid;
}

bool ChkCuwp::IsInvincibleValid()
{
    return (validSpecProps & (u16)ValidCUWPSpecPropFlags::InvincibleIsValid) == (u16)ValidCUWPSpecPropFlags::InvincibleIsValid;
}

void ChkCuwp::SetCloakIsValid(bool cloakIsValid)
{
    if ( cloakIsValid )
        validSpecProps |= (u16)ValidCUWPSpecPropFlags::CloakIsValid;
    else
        validSpecProps &= (u16)ValidCUWPSpecPropFlags::xCloakIsValid;
}

void ChkCuwp::SetBurrowIsValid(bool burrowIsValid)
{
    if ( burrowIsValid )
        validSpecProps |= (u16)ValidCUWPSpecPropFlags::BurrowIsValid;
    else
        validSpecProps &= (u16)ValidCUWPSpecPropFlags::xBurrowIsValid;
}

void ChkCuwp::SetInTransitIsValid(bool inTransitIsValid)
{
    if ( inTransitIsValid )
        validSpecProps |= (u16)ValidCUWPSpecPropFlags::InTransitIsValid;
    else
        validSpecProps &= (u16)ValidCUWPSpecPropFlags::xInTransitIsValid;
}

void ChkCuwp::SetHallucinatedIsValid(bool hallucinatedIsValid)
{
    if ( hallucinatedIsValid )
        validSpecProps |= (u16)ValidCUWPSpecPropFlags::HallucinateIsValid;
    else
        validSpecProps &= (u16)ValidCUWPSpecPropFlags::xHallucinateIsValid;
}

void ChkCuwp::SetInvincibleIsValid(bool invincibleIsValid)
{
    if ( invincibleIsValid )
        validSpecProps |= (u16)ValidCUWPSpecPropFlags::InvincibleIsValid;
    else
        validSpecProps &= (u16)ValidCUWPSpecPropFlags::xInvincibleIsValid;
}

bool ChkCuwp::IsOwnerValid()
{
    return (validElements & (u16)ValidCUWPElementFlags::OwnerIsValid) == (u16)ValidCUWPElementFlags::OwnerIsValid;
}

bool ChkCuwp::IsHitpointsValid()
{
    return (validElements & (u16)ValidCUWPElementFlags::HitpointsIsValid) == (u16)ValidCUWPElementFlags::HitpointsIsValid;
}

bool ChkCuwp::IsShieldsValid()
{
    return (validElements & (u16)ValidCUWPElementFlags::ShieldsIsValid) == (u16)ValidCUWPElementFlags::ShieldsIsValid;
}

bool ChkCuwp::IsEnergyValid()
{
    return (validElements & (u16)ValidCUWPElementFlags::EnergyIsValid) == (u16)ValidCUWPElementFlags::EnergyIsValid;
}

bool ChkCuwp::IsResourceAmountValid()
{
    return (validElements & (u16)ValidCUWPElementFlags::ResourceAmountIsValid) == (u16)ValidCUWPElementFlags::ResourceAmountIsValid;
}

bool ChkCuwp::IsValidElementsBit6Set()
{
    return (validElements & (u16)ValidCUWPElementFlags::UnknownBit6) == (u16)ValidCUWPElementFlags::UnknownBit6;
}

void ChkCuwp::SetOwnerIsValid(bool ownerIsValid)
{
    if ( ownerIsValid )
        validElements |= (u16)ValidCUWPElementFlags::OwnerIsValid;
    else
        validElements &= (u16)ValidCUWPElementFlags::xOwnerIsValid;
}

void ChkCuwp::SetHitpointsIsValid(bool hitpointsIsValid)
{
    if ( hitpointsIsValid )
        validElements |= (u16)ValidCUWPElementFlags::HitpointsIsValid;
    else
        validElements &= (u16)ValidCUWPElementFlags::xHitpointsIsValid;
}

void ChkCuwp::SetShieldsIsValid(bool shieldsIsValid)
{
    if ( shieldsIsValid )
        validElements |= (u16)ValidCUWPElementFlags::ShieldsIsValid;
    else
        validElements &= (u16)ValidCUWPElementFlags::xShieldsIsValid;
}

void ChkCuwp::SetEnergyIsValid(bool energyIsValid)
{
    if ( energyIsValid )
        validElements |= (u16)ValidCUWPElementFlags::EnergyIsValid;
    else
        validElements &= (u16)ValidCUWPElementFlags::xEnergyIsValid;
}

void ChkCuwp::SetResourceAmountIsValid(bool resourceAmountIsValid)
{
    if ( resourceAmountIsValid )
        validElements |= (u16)ValidCUWPElementFlags::ResourceAmountIsValid;
    else
        validElements &= (u16)ValidCUWPElementFlags::xResourceAmountIsValid;
}

void ChkCuwp::SetValidElementsBit6(bool validElementsBit6)
{
    if ( validElementsBit6 )
        validElements |= (u16)ValidCUWPElementFlags::UnknownBit6;
    else
        validElements |= (u16)ValidCUWPElementFlags::xUnknownBit6;
}

bool ChkCuwp::IsUnitCloaked()
{
    return (flags & (u16)CUWPUnitPropertyFlags::UnitIsCloaked) == (u16)CUWPUnitPropertyFlags::UnitIsCloaked;
}

bool ChkCuwp::IsUnitBurrowed()
{
    return (flags & (u16)CUWPUnitPropertyFlags::UnitIsBurrowed) == (u16)CUWPUnitPropertyFlags::UnitIsBurrowed;
}

bool ChkCuwp::IsBuildingInTransit()
{
    return (flags & (u16)CUWPUnitPropertyFlags::BuildingInTransit) == (u16)CUWPUnitPropertyFlags::BuildingInTransit;
}

bool ChkCuwp::IsUnitHallucinated()
{
    return (flags & (u16)CUWPUnitPropertyFlags::UnitIsHallucinated) == (u16)CUWPUnitPropertyFlags::UnitIsHallucinated;
}

bool ChkCuwp::IsUnitInvincible()
{
    return (flags & (u16)CUWPUnitPropertyFlags::UnitIsInvincible) == (u16)CUWPUnitPropertyFlags::UnitIsInvincible;
}

void ChkCuwp::SetUnitIsCloaked(bool unitIsCloaked)
{
    if ( unitIsCloaked )
        flags |= (u16)CUWPUnitPropertyFlags::UnitIsCloaked;
    else
        flags &= (u16)CUWPUnitPropertyFlags::xUnitIsCloaked;
}

void ChkCuwp::SetUnitIsBurrowed(bool unitIsBurrowed)
{
    if ( unitIsBurrowed )
        flags |= (u16)CUWPUnitPropertyFlags::UnitIsBurrowed;
    else
        flags &= (u16)CUWPUnitPropertyFlags::xUnitIsBurrowed;
}

void ChkCuwp::SetBuildingInTransit(bool buildingInTransit)
{
    if ( buildingInTransit )
        flags |= (u16)CUWPUnitPropertyFlags::BuildingInTransit;
    else
        flags &= (u16)CUWPUnitPropertyFlags::xBuildingInTransit;
}

void ChkCuwp::SetUnitIsHallucinated(bool unitIsHallucinated)
{
    if ( unitIsHallucinated )
        flags |= (u16)CUWPUnitPropertyFlags::UnitIsHallucinated;
    else
        flags &= (u16)CUWPUnitPropertyFlags::xUnitIsHallucinated;
}

void ChkCuwp::SetUnitIsInvincible(bool unitIsInvincible)
{
    if ( unitIsInvincible )
        flags |= (u16)CUWPUnitPropertyFlags::UnitIsInvincible;
    else
        flags &= (u16)CUWPUnitPropertyFlags::xUnitIsInvincible;
}
