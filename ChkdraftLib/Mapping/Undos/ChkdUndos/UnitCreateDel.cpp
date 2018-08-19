#include "UnitCreateDel.h"
#include "../../../Windows/MainWindows/GuiMap.h"

UnitCreateDel::~UnitCreateDel()
{

}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index));
}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index, ChkUnit &unit)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index, unit));
}

void UnitCreateDel::Reverse(void *guiMap)
{
    if ( unit == nullptr ) // Do delete
    {
        unit = std::unique_ptr<ChkUnit>(new ChkUnit);
        *unit = ((GuiMap*)guiMap)->getUnit(index);
        ((GuiMap*)guiMap)->deleteUnit(index);
    }
    else // Do create
    {
        ((GuiMap*)guiMap)->insertUnit(index, *unit);
        unit = nullptr;
    }
}

int32_t UnitCreateDel::GetType()
{
    return (int32_t)UndoTypes::UnitChange;
}

UnitCreateDel::UnitCreateDel(u16 index, ChkUnit &unit) // Undo deletion
    : index(index), unit(nullptr)
{
    this->unit = std::unique_ptr<ChkUnit>(new ChkUnit);
    (*(this->unit)) = unit;
}

UnitCreateDel::UnitCreateDel(u16 index) // Undo creation
    : index(index), unit(nullptr)
{

}
