#ifndef ACTIONDESCRIPTOR_H
#define ACTIONDESCRIPTOR_H

enum class ActionDescriptor
{
    None = 0,
    // Misc
    BeginPaste,
    DeleteSelection,
    UpdateMiscSelection,
    // Terrain
    BrushIsom,
    UpdateTileSel,
    ClearTileSel,
    DeleteTiles,
    PasteTiles,
    FillPasteTiles,
    OpenTileProperties,
    // Doodads
    UpdateDoodadSel,
    ClearDoodadSel,
    PasteDoodad,
    DeleteDoodad,
    ConvertDoodad,
    // Fog of War
    BrushFog,
    PasteFog,
    DeleteFogTiles,
    // Locations
    UpdateLocationSel,
    ClearLocationSel,
    CreateLocation,
    MoveLocation,
    DeleteLocations,
    // Units
    UpdateUnitSel,
    ClearUnitSel,
    CreateUnit,
    PasteUnits,
    DeleteUnits,
    StackUnits,
    CreateLocationForUnit,
    CreateInvertedLocationForUnit,
    CreateMobileInvertedLocationForUnit,
    // Sprites
    UpdateSpriteSel,
    ClearSpriteSel,
    CreateSprite,
    PasteSprites,
    DeleteSprites,
    StackSprites,
    // Cut Copy Paste
};

struct DescriptorIndex {
    ActionDescriptor descriptorIndex = ActionDescriptor::None;
    constexpr DescriptorIndex() noexcept = default;
    constexpr DescriptorIndex(ActionDescriptor actionDescriptor) noexcept : descriptorIndex(actionDescriptor) {}
    friend constexpr bool operator==(const DescriptorIndex & lhs, const DescriptorIndex & rhs) noexcept { return lhs.descriptorIndex == rhs.descriptorIndex; }
};

#endif