#include "MtxmChange.h"
#include "../../../Windows/MainWindows/GuiMap.h"

MtxmChange::~MtxmChange()
{

}

std::shared_ptr<MtxmChange> MtxmChange::Make(u16 xc, u16 yc, u16 tileValue)
{
    return std::shared_ptr<MtxmChange>(new MtxmChange(xc, yc, tileValue));
}

void MtxmChange::Reverse(void *guiMap)
{
    u16 replacedValue = ((GuiMap*)guiMap)->getTile(xc, yc, Chk::StrScope::Game);
    ((GuiMap*)guiMap)->setTile(xc, yc, tileValue, Chk::StrScope::Game);
    tileValue = replacedValue;
}

int32_t MtxmChange::GetType()
{
    return UndoTypes::MtxmChange;
}

MtxmChange::MtxmChange(u16 xc, u16 yc, u16 tileValue) : xc(xc), yc(yc), tileValue(tileValue)
{

}
