#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H
#include <Windows.h>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "cross_cut/simple_icu.h"

namespace WinLib
{
    class ContextMenu
    {
        HMENU hMenu = ::CreatePopupMenu();
        std::vector<ContextMenu> ownedSubMenus {};
        std::shared_ptr<std::vector<std::function<void()>>> actions = nullptr;

    public:
        ContextMenu();

        ContextMenu(std::shared_ptr<std::vector<std::function<void()>>> actions);

        ~ContextMenu();

        operator bool();

        ContextMenu & append(const std::string & label, std::function<void()> action, bool checked = false, bool disabled = false);

        ContextMenu & appendSubmenu(const std::string & label);

        ContextMenu & appendSeparator();

        void select(HWND hParent, int x, int y);
    };
}

#endif