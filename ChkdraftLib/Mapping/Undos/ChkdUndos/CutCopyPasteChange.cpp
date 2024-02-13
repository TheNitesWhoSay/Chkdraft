#include "CutCopyPasteChange.h"
#include "../../../Chkdraft.h"
#include "../../../Windows/MainWindows/GuiMap.h"

CutCopyPasteChange::~CutCopyPasteChange()
{

}

std::shared_ptr<CutCopyPasteChange> CutCopyPasteChange::Make()
{
    return std::shared_ptr<CutCopyPasteChange>(new CutCopyPasteChange());
}

void CutCopyPasteChange::Reverse(void *guiMap)
{
}

int32_t CutCopyPasteChange::GetType()
{
    return UndoTypes::CutCopyPaste;
}

CutCopyPasteChange::CutCopyPasteChange()
{

}
