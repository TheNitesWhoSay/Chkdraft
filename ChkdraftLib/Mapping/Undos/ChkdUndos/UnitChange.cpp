#include "UnitChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

UnitChange::~UnitChange()
{

}

std::shared_ptr<UnitChange> UnitChange::Make(u16 unitIndex, Chk::Unit::Field field, u32 data)
{
    return std::shared_ptr<UnitChange>(new UnitChange(unitIndex, field, data));
}

void UnitChange::Reverse(void *guiMap)
{
    u32 replacedData = 0;
    switch ( field )
    {
        case Chk::Unit::Field::ClassId: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->classId);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->classId = data; break;
        case Chk::Unit::Field::Xc: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->xc);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->xc = (u16)data; break;
        case Chk::Unit::Field::Yc: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->yc);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->yc = (u16)data; break;
        case Chk::Unit::Field::Type: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->type);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->type = (Sc::Unit::Type)data; break;
        case Chk::Unit::Field::RelationFlags: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->relationFlags);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->relationFlags = (u16)data; break;
        case Chk::Unit::Field::ValidStateFlags: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->validStateFlags);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->validStateFlags = (u16)data; break;
        case Chk::Unit::Field::ValidFieldFlags: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->validFieldFlags);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->validFieldFlags = (u16)data; break;
        case Chk::Unit::Field::Owner: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->owner);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->owner = (u8)data; break;
        case Chk::Unit::Field::HitpointPercent: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->hitpointPercent);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->hitpointPercent = (u8)data; break;
        case Chk::Unit::Field::ShieldPercent: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->shieldPercent);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->shieldPercent = (u8)data; break;
        case Chk::Unit::Field::EnergyPercent: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->energyPercent);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->energyPercent = (u8)data; break;
        case Chk::Unit::Field::ResourceAmount: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->resourceAmount);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->resourceAmount = data; break;
        case Chk::Unit::Field::HangerAmount: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->hangerAmount);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->hangerAmount = (u16)data; break;
        case Chk::Unit::Field::StateFlags: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->stateFlags);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->stateFlags = (u16)data; break;
        case Chk::Unit::Field::Unused: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->unused);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->unused = data; break;
        case Chk::Unit::Field::RelationClassId: replacedData = u32(((GuiMap*)guiMap)->layers.getUnit(unitIndex)->relationClassId);
            ((GuiMap*)guiMap)->layers.getUnit(unitIndex)->relationClassId = data; break;
    }
    data = replacedData;
}

int32_t UnitChange::GetType()
{
    return UndoTypes::UnitChange;
}

UnitChange::UnitChange(u16 unitIndex, Chk::Unit::Field field, u32 data)
    : unitIndex(unitIndex), field(field), data(data)
{

}
