#include "fog_change.h"
#include "ui/main_windows/gui_map.h"

FogChange::~FogChange()
{

}

std::shared_ptr<FogChange> FogChange::Make(u16 tileXc, u16 tileYc, u8 fogValue)
{
    return std::shared_ptr<FogChange>(new FogChange(tileXc, tileYc, fogValue));
}

void FogChange::Reverse(void *guiMap)
{
    //u8 replacedValue = ((GuiMap*)guiMap)->getFog(tileXc, tileYc);
    //((GuiMap*)guiMap)->setFog(tileXc, tileYc, fogValue);
    //fogValue = replacedValue;
}

int32_t FogChange::GetType()
{
    return UndoTypes::FogChange;
}

FogChange::FogChange(u16 tileXc, u16 tileYc, u8 fogValue) : tileXc(tileXc), tileYc(tileYc), fogValue(fogValue)
{

}
