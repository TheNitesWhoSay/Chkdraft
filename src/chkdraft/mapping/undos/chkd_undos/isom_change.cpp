#include "isom_change.h"
#include "ui/main_windows/gui_map.h"

IsomChange::~IsomChange()
{

}

std::shared_ptr<IsomChange> IsomChange::Make(const Chk::IsomRectUndo & isomRectUndo)
{
    return std::shared_ptr<IsomChange>(new IsomChange(isomRectUndo));
}

void IsomChange::Reverse(void *guiMap)
{
    auto replacedValue = ((GuiMap*)guiMap)->getIsomRect({isomRectUndo.diamond.x, isomRectUndo.diamond.y});
    ((GuiMap*)guiMap)->isomRectAt({isomRectUndo.diamond.x, isomRectUndo.diamond.y}) = isomRectUndo.oldValue;
    isomRectUndo.newValue = isomRectUndo.oldValue;
    isomRectUndo.oldValue = replacedValue;
}

int32_t IsomChange::GetType()
{
    return UndoTypes::TerrainChange;
}

IsomChange::IsomChange(const Chk::IsomRectUndo & isomRectUndo) : isomRectUndo(isomRectUndo)
{

}
