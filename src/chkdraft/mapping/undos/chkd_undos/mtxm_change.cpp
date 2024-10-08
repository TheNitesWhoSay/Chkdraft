#include "mtxm_change.h"
#include "ui/main_windows/gui_map.h"

MtxmChange::~MtxmChange()
{

}

std::shared_ptr<MtxmChange> MtxmChange::Make(u16 xc, u16 yc, u16 tileValue)
{
    return std::shared_ptr<MtxmChange>(new MtxmChange(xc, yc, tileValue));
}

void MtxmChange::Reverse(void *guiMap)
{
    u16 replacedValue = ((GuiMap*)guiMap)->getTile(xc, yc, Chk::Scope::Game);
    ((GuiMap*)guiMap)->setTile(xc, yc, tileValue, Chk::Scope::Game);
    tileValue = replacedValue;
}

int32_t MtxmChange::GetType()
{
    return UndoTypes::TerrainChange;
}

MtxmChange::MtxmChange(u16 xc, u16 yc, u16 tileValue) : xc(xc), yc(yc), tileValue(tileValue)
{

}
