#include "sprite_tree.h"
#include "chkdraft/chkdraft.h"

SpriteTree::~SpriteTree()
{

}

bool SpriteTree::AddThis(HWND hTree, HTREEITEM hParent)
{
    hSpriteRoot = hParent;
    if ( TreeViewControl::SetHandle(hTree) )
        return true;
    else
        return false;
}

void SpriteTree::addToTree(HTREEITEM parent, const Sc::Sprite::SpriteGroup & spriteGroup)
{
    auto groupRoot = InsertTreeItem(parent, spriteGroup.groupName, TreeTypeCategory | (LPARAM)Layer::Sprites);
    for ( const auto & subGroup : spriteGroup.subGroups )
        addToTree(groupRoot, subGroup);
    
    for ( const auto & memberSprite : spriteGroup.memberSprites )
    {
        const auto & spriteDat = chkd.scData->sprites.getSprite(memberSprite.spriteIndex);
        const auto & imageDat = chkd.scData->sprites.getImage(spriteDat.imageFile);
        const auto & imageFileStr = chkd.scData->sprites.imagesTbl->getString(imageDat.grpFile);
        std::string imageFileName = std::string("[") + (memberSprite.isUnit ? "u" : "") +
            std::to_string(memberSprite.spriteIndex) + "] " + memberSprite.spriteName;

        if ( memberSprite.isUnit )
            InsertTreeItem(groupRoot, imageFileName, memberSprite.spriteIndex|TreeTypeSpriteUnit);
        else
            InsertTreeItem(groupRoot, imageFileName, memberSprite.spriteIndex|TreeTypeSprite);
    }
}

void SpriteTree::addToTree(const TreeGroup & spriteGroup, HTREEITEM hParent)
{
    auto groupRoot = InsertTreeItem(hParent, spriteGroup.label, TreeTypeCategory | (LPARAM)Layer::Sprites);
    for ( const auto & subGroup : spriteGroup.subGroups )
        addToTree(subGroup, groupRoot);

    for ( const auto & memberSprite : spriteGroup.items )
    {
        if ( memberSprite < 0 ) // For negative numbers, unitId = (-1*memberSprite)-1
            InsertTreeItem(groupRoot, chkd.scData->units.displayNames[-1*memberSprite-1], ((LPARAM)(-1*memberSprite)-1)|TreeTypeSpriteUnit);
        else
            InsertTreeItem(groupRoot, chkd.scData->sprites.spriteNames[memberSprite], ((LPARAM)memberSprite)|TreeTypeSprite);
    }
}

void SpriteTree::UpdateSpriteTree()
{
    EmptySubTree(hSpriteRoot);

    ChkdProfile* currProfile = chkd.profiles.getCurrProfile();
    if ( currProfile != nullptr && !currProfile->sprites.parsedCustomTree.subGroups.empty() )
    {
        for ( const auto & spriteGroup : currProfile->sprites.parsedCustomTree.subGroups )
            addToTree(spriteGroup, hSpriteRoot);
    }
    else
    {
        for ( auto & spriteGroup : chkd.scData->sprites.spriteGroups )
            addToTree(hSpriteRoot, spriteGroup);
    }
}
