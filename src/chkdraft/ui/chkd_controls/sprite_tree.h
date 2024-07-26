#ifndef SPRITETREE_H
#define SPRITETREE_H
#include "../../../WindowsLib/WindowsUi.h"
#include "../../CommonFiles/CommonFiles.h"

class SpriteTree : public WinLib::TreeViewControl
{
    public:
        virtual ~SpriteTree();
        bool AddThis(HWND hTree, HTREEITEM hParent);
        void UpdateSpriteTree();

    private:
        void addToTree(HTREEITEM parent, const Sc::Sprite::SpriteGroup & spriteGroup);
        
        HTREEITEM hSpriteRoot;
};

#endif