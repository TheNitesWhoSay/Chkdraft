#include "sc_mini_map.h"

ClassicMiniMap::ClassicMiniMap(const Sc::Data & scData, const Scenario & map) : scData(scData), map(map), pixels(width*height)
{
    
}

void ClassicMiniMap::render()
{
    renderTerrain();
    renderUnits();
}

void ClassicMiniMap::renderTerrain()
{
    Sc::Terrain::Tileset tilesetIndex = map.getTileset();
    const Sc::Terrain::Tiles & tileset = scData.terrain.get(tilesetIndex);

    const auto & tiles = map->tiles;
    std::size_t tileWidth = map->dimensions.tileWidth;
    std::size_t tileHeight = map->dimensions.tileHeight;
    float scale = miniMapScale();

    u16 xOffset = (u16)((128-tileWidth*scale)/2),
        yOffset = (u16)((128-tileHeight*scale)/2);
    
    const auto & palette = scData.terrain.getStaticColorPalette(tilesetIndex);

    for ( s64 yc=0; yc<128-2*yOffset; yc++ ) // Cycle through all minimap pixel rows
    {
        s64 yMiniTile = 0;
        s64 yTile = (s64)(yc/scale); // Get the yc of tile used for the pixel
        if ( scale > 1 )
        {
            yMiniTile = yc%(int)scale; // Get the y-minitile used for the pixel
            if ( yMiniTile > 3 )
                yMiniTile %= 4; // Correct for invalid y-minitiles
        }

        for ( s64 xc=0; xc<128-2*xOffset; xc++ ) // Cycle through all minimap pixel columns
        {
            s64 xMiniTile = 0;
            s64 xTile = (s64)(xc/scale); // Get the xc of the tile used for the pixel
            if ( scale > 1 )
            {
                xMiniTile = xc%(int)scale; // Get the x-minitile used for the pixel
                if ( xMiniTile > 3 )
                    xMiniTile %= 4; // Correct for invalid x-minitiles
            }

            if ( size_t(xTile) < map.getTileWidth() && size_t(yTile) < map.getTileHeight() )
            {
                u16 tileIndex = map.getTile(size_t(xTile), size_t(yTile));
                size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                if ( groupIndex < tileset.tileGroups.size() )
                {
                    const Sc::Terrain::TileGroup & tileGroup = tileset.tileGroups[groupIndex];
                    const u16 & megaTileIndex = tileGroup.megaTileIndex[tileset.getGroupMemberIndex(tileIndex)];
                    const Sc::Terrain::TileGraphicsEx & tileGraphics = tileset.tileGraphics[megaTileIndex];

                    const size_t vr4Index = size_t(tileGraphics.miniTileGraphics[yMiniTile][xMiniTile].vr4Index());
                    const Sc::Terrain::MiniTilePixels & miniTilePixels = tileset.miniTilePixels[vr4Index];
                    const u8 & wpeIndex = miniTilePixels.wpeIndex[6][7];

                    pixels[size_t((yc + yOffset) * 128 + xc + xOffset)] = (std::uint32_t &)(palette[wpeIndex]);
                }
            }
        }
    }
}

void ClassicMiniMap::renderUnits()
{
    constexpr std::array<u32, 10> remasteredColors { // 0xAABBGGRR
        0x0074A47C, // 12
        0x007290B8, // 13
        0x0000E4FC, // 14
        0x00FFC4E4, // 15
        0x00808000, // 16
        0x00D2F53C, // 17
        0x00000080, // 18
        0x00F032E6, // 19
        0x00808080, // 20
        0x003C3C3C, // 21
    };
    const bool useCrgb = map->hasSection(Chk::SectionName::CRGB) && map->version >= Chk::Version::StarCraft_Remastered;
    const auto & units = map->units;
    const auto & sprites = map->sprites;

    auto tileWidth = map.getTileWidth();
    auto tileHeight = map.getTileHeight();
    int mapSize = tileWidth <= tileHeight ? tileHeight : tileWidth;
    int boxZoom = mapSize >= 192 ? 2 : 1;
    int tilePxSize = mapSize <= 64 ? 2 : 1;
    const auto numUnitTypes = scData.units.numUnitTypes();
    const auto pixelLimit = pixels.size();
    auto scaledTileWidth = tileWidth * tilePxSize / boxZoom;
    auto scaledTileHeight = tileHeight * tilePxSize / boxZoom;
    float scale = miniMapScale();

    u16 xOffset = (u16)((128-tileWidth*scale)/2),
        yOffset = (u16)((128-tileHeight*scale)/2);

    auto getPixelColor = [&](u8 owner, bool isResourceColor) -> std::uint32_t
    {
        if ( isResourceColor )
            return (std::uint32_t &)scData.tminimap.bgraPalette[Chk::PlayerColor::ResourceColor];
        else if ( owner >= Sc::Player::TotalSlots )
        {
            owner %= 22;
            if ( owner < 12 )
                return (std::uint32_t &)scData.tminimap.bgraPalette[owner];
            else
                return remasteredColors[owner-12];
        }
        else if ( useCrgb && map->version >= Chk::Version::StarCraft_Remastered )
        {
            switch ( map->customColors.playerSetting[owner] )
            {
                case 2: // Custom RGB
                {
                    auto & customColor = map->customColors.playerColor[owner]; // RGB -> 0xAABBGGRR
                    return u32(0xFF000000) | (u32(customColor[2]) << 16) | (u32(customColor[1]) << 8) | u32(customColor[0]);
                }
                case 3: // Blue color
                {
                    std::size_t color = map->customColors.playerColor[owner][2];
                    if ( color <= Chk::PlayerColor::Azure_NeutralColor || color > 21 )
                        return (u32 &)scData.tminimap.bgraPalette[color % 16];
                    else
                        return remasteredColors[color-12];
                }
                case 0: // Random
                case 1: // Player choice
                default:
                    return (u32 &)(scData.tminimap.bgraPalette[size_t(owner) < scData.tminimap.bgraPalette.size() ? size_t(owner) : size_t(owner)%scData.tminimap.bgraPalette.size()]);
            }
        }
        else
        {
            std::size_t color = static_cast<std::size_t>(map->playerColors[owner]);
            if ( color <= Chk::PlayerColor::Azure_NeutralColor || color > 21 )
                return (u32 &)(scData.tminimap.bgraPalette[color%16]);
            else
                return remasteredColors[color-12];
        }
    };

    auto drawUnit = [&](u8 owner, Sc::Unit::Type unitType, u16 unitXc, u16 unitYc)
    {
        if ( unitType < numUnitTypes )
        {
            const Sc::Unit::DatEntry & dat = scData.units.getUnit(unitType);
            bool isResourceColor = (dat.flags & Sc::Unit::Flags::ResourceContainer) && !(dat.starEditGroupFlags & BIT_4) &&
                owner == Sc::Player::Id::Player12_Neutral;

            u16 placementWidth = dat.starEditPlacementBoxWidth;
            u16 placementHeight = dat.starEditPlacementBoxHeight;
            bool isBuilding = ((dat.flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building);
            int left = (unitXc - placementWidth/2) * tilePxSize / boxZoom / 32;
            int top = (unitYc - placementHeight/2) * tilePxSize / boxZoom / 32;
            int width = placementWidth * tilePxSize / boxZoom / 32;
            int height = placementHeight * tilePxSize / boxZoom / 32;

            if ( width < 2 )
                width = 2;
            else if ( isBuilding ) 
            {
                if ( width > 4 )
                    width = 4;
            }
            else if ( width > 2 )
                width = 2;

            if ( height < 2 )
                height = 2;
            else if ( isBuilding )
            {
                if ( height > 4 )
                    height = 4;
            }
            else if ( height > 2 )
                height = 2;

            if ( left < 0 )
                left = 0;
            else if ( left + width - 1 >= scaledTileWidth )
                width = scaledTileWidth - left;

            if ( top < 0 )
                top = 0;
            else if ( top + height - 1 >= scaledTileHeight )
                height = scaledTileHeight - top;

            float nonStandardScale = scale / float(tilePxSize) * float(boxZoom);
            switch ( tileWidth ) {
                case 64: case 96: case 128: case 192: case 256: break;
                default: { // Non-standard width
                    left = std::round(left * nonStandardScale);
                    width = std::round(width * nonStandardScale);
                } break;
            }
            switch ( tileHeight ) {
                case 64: case 96: case 128: case 192: case 256: break;
                default: { // Non-standard height
                    top = std::round(top * nonStandardScale);
                    height = std::round(height * nonStandardScale);
                } break;
            }

            u32 pixelColor = getPixelColor(owner, isResourceColor);
            for ( int y=top; y<top+height; ++y )
            {
                for ( int x=left; x<left+width; ++x )
                {
                    u32 bitIndex = (u32(y)+u32(yOffset))*128 + u32(x)+u32(xOffset);

                    if ( bitIndex < pixelLimit )
                        pixels[bitIndex] = pixelColor;
                }
            }
        }
        else // Place a single-pixel dot for the unknown unit type
        {
            u32 bitIndex =
                ((u32)((unitYc / 32)*scale) + yOffset) * 128
                + (u32)((unitXc / 32)*scale) + xOffset;
            
            if ( bitIndex < pixelLimit )
                pixels[bitIndex] = getPixelColor(owner, false);
        }
    };

    for ( const auto & unit : map->units )
        drawUnit(unit.owner, unit.type, unit.xc, unit.yc);

    for ( const auto & sprite : map->sprites )
    {
        if ( sprite.isUnit() )
            drawUnit(sprite.owner, static_cast<Sc::Unit::Type>(sprite.type), sprite.xc, sprite.yc);
    }
}

void ClassicMiniMap::renderFog()
{
    const auto & fog = map->tileFog;
}

float ClassicMiniMap::miniMapScale()
{
    if ( map.read.dimensions.tileWidth >= map.read.dimensions.tileHeight && map.read.dimensions.tileWidth > 0 )
    {
        if ( map.read.dimensions.tileWidth == 192 || map.read.dimensions.tileWidth == 256 )
            return 0.5f;
        else if ( 128.0/map.read.dimensions.tileWidth > 1 )
            return (float)(128/map.read.dimensions.tileWidth);
        else
            return (float)(128.0/map.read.dimensions.tileWidth);
    }
    else
    {
        if ( map.read.dimensions.tileHeight == 192 || map.read.dimensions.tileHeight == 256 )
            return 0.5f;
        else if ( 128.0/map.read.dimensions.tileHeight > 1 && map.read.dimensions.tileHeight > 0 )
            return (float)(128/map.read.dimensions.tileHeight);
        else
            return (float)(128.0/map.read.dimensions.tileHeight);
    }
    return 1;
}
