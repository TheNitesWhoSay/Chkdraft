#include "unit_create_del.h"
#include "ui/main_windows/gui_map.h"

UnitCreateDel::~UnitCreateDel()
{

}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index));
}

std::shared_ptr<UnitCreateDel> UnitCreateDel::Make(u16 index, const Chk::Unit & unit)
{
    return std::shared_ptr<UnitCreateDel>(new UnitCreateDel(index, unit));
}

void UnitCreateDel::Reverse(void *guiMap)
{
    if ( unit == nullptr ) // Do delete
    {
        unit = std::unique_ptr<Chk::Unit>(new Chk::Unit);
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
    return UndoTypes::UnitChange;
}

UnitCreateDel::UnitCreateDel(u16 index, const Chk::Unit & unit) // Undo deletion
    : index(index), unit(nullptr)
{
    this->unit = std::unique_ptr<Chk::Unit>(new Chk::Unit);
    (*(this->unit)) = unit;
}

UnitCreateDel::UnitCreateDel(u16 index) // Undo creation
    : index(index), unit(nullptr)
{

}
