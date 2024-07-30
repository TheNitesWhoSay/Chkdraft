#include "cut_copy_paste_change.h"
#include "chkdraft/chkdraft.h"
#include "ui/main_windows/gui_map.h"

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
