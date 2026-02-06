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
    
    if ( !spriteGroup.memberSprites.empty() )
    {
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
}

void SpriteTree::addToTree(const TreeGroup & spriteGroup, HTREEITEM hParent)
{
    auto groupRoot = InsertTreeItem(hParent, spriteGroup.label, TreeTypeCategory | (LPARAM)Layer::Sprites);
    for ( const auto & subGroup : spriteGroup.subGroups )
        addToTree(subGroup, groupRoot);

    for ( const auto & memberSprite : spriteGroup.items )
    {
        if ( memberSprite < 0 && memberSprite > -10000 )
            continue;

        bool isPureSpriteUnit = memberSprite <= -10000 && memberSprite > -20000;
        bool isSpriteUnit = memberSprite <= -20000;
        s32 unitId = -1;
        s32 spriteId = memberSprite;
        if ( isPureSpriteUnit )
        {
            unitId = (-1*memberSprite)-10000;
            spriteId = 0;
            if ( std::size_t(unitId) < chkd.scData->units.numUnitTypes() )
            {
                u16 flingyId = chkd.scData->units.getUnit(Sc::Unit::Type(unitId)).graphics;
                if ( std::size_t(flingyId) < chkd.scData->units.numFlingies() )
                    spriteId = s32(chkd.scData->units.getFlingy(flingyId).sprite);
            }
        }
        else if ( isSpriteUnit )
        {
            unitId = (-1*memberSprite)-20000;
            spriteId = unitId;
        }

        LPARAM lParam = ((LPARAM)spriteId) | (isSpriteUnit ? TreeTypeSpriteUnit : TreeTypeSprite);
        std::string name = (isSpriteUnit | isPureSpriteUnit) ? chkd.scData->units.displayNames[unitId] : chkd.scData->sprites.spriteNames[spriteId];
        if ( isSpriteUnit )
            InsertTreeItem(groupRoot, "[u" + std::to_string(unitId) + "] " + name, lParam);
        else
            InsertTreeItem(groupRoot, "[" + std::to_string(spriteId) + "] " + name, lParam);
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
