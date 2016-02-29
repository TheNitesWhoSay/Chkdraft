#ifndef TECHTREE_H
#define TECHTREE_H
#include "WindowsUI/WindowsUI.h"

class TechTree : public TreeViewControl
{
    public:
        bool CreateThis(HWND hParent, int x, int y, int width, int height, bool hasButtons, u32 id);

    private:
        void InsertTechs();
};

#endif
