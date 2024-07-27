#include "context_menu.h"

namespace WinLib
{
    ContextMenu::ContextMenu() : actions(std::make_shared<std::vector<std::function<void()>>>()) {}
    
    ContextMenu::ContextMenu(std::shared_ptr<std::vector<std::function<void()>>> actions) : actions(actions) {}
    
    ContextMenu::~ContextMenu() {
        if ( hMenu != NULL )
            DestroyMenu(hMenu);
    }

    ContextMenu::operator bool() {
        return hMenu != NULL;
    }
    
    ContextMenu & ContextMenu::append(const std::string & label, std::function<void()> action, bool checked, bool disabled) {
        UINT flags = checked ? MF_STRING|MF_CHECKED : MF_STRING;
        flags = disabled ? flags|MF_GRAYED : flags;
        actions->push_back(action);
        AppendMenu(hMenu, flags, UINT_PTR(actions->size()), icux::toUistring(label).c_str());
        return *this;
    }

    ContextMenu & ContextMenu::appendSubmenu(const std::string & label) {
        ContextMenu & inserted = ownedSubMenus.emplace_back(actions);
        AppendMenu(hMenu, MF_STRING|MF_POPUP, UINT_PTR(inserted.hMenu), icux::toUistring(label).c_str());
        return inserted;
    }

    ContextMenu & ContextMenu::appendSeparator() {
        AppendMenu(hMenu, MF_SEPARATOR, NULL, NULL);
        return *this;
    }

    void ContextMenu::select(HWND hParent, int x, int y) {
        BOOL result = TrackPopupMenu(hMenu, TPM_RETURNCMD, x, y, 0, hParent, NULL);
        if ( result > 0 )
            (*actions)[size_t(result)-1]();
    }
}

