#include "UnitCreateDel.h"
#include "../../../Windows/MainWindows/GuiMap.h"

UnitCreateDel::~UnitCreateDel()
{

}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index));
}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index, Chk::Unit &unit)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index, unit));
}

void UnitCreateDel::Reverse(void *guiMap)
{
    if ( unit == nullptr ) // Do delete
    {
        unit = std::unique_ptr<Chk::Unit>(new Chk::Unit);
        *unit = *((GuiMap*)guiMap)->layers.getUnit(index);
        ((GuiMap*)guiMap)->layers.deleteUnit(index);
    }
    else // Do create
    {
        Chk::UnitPtr newUnit = Chk::UnitPtr(new Chk::Unit(*unit));
        ((GuiMap*)guiMap)->layers.insertUnit(index, newUnit);
        unit = nullptr;
    }
}

int32_t UnitCreateDel::GetType()
{
    return (int32_t)UndoTypes::UnitChange;
}

UnitCreateDel::UnitCreateDel(u16 index, Chk::Unit &unit) // Undo deletion
    : index(index), unit(nullptr)
{
    this->unit = std::unique_ptr<Chk::Unit>(new Chk::Unit);
    (*(this->unit)) = unit;
}

UnitCreateDel::UnitCreateDel(u16 index) // Undo creation
    : index(index), unit(nullptr)
{

}
