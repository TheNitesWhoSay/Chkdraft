#include "IsomChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

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
    return UndoTypes::MtxmChange;
}

IsomChange::IsomChange(const Chk::IsomRectUndo & isomRectUndo) : isomRectUndo(isomRectUndo)
{

}
