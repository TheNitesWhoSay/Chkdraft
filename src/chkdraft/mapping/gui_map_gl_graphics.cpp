#include "gui_map_gl_graphics.h"
#include <chkdraft/ui/main_windows/gui_map.h>

GuiMapGraphics::GuiMapGraphics(Sc::Data & data, GuiMap & guiMap)
    : MapGraphics(data, static_cast<Scenario &>(guiMap), guiMap.animations.value()), guiMap(guiMap)
{

}

void GuiMapGraphics::drawTileSelection()
{
    const Sc::Terrain::Tiles & tiles = scData.terrain.get(guiMap->tileset);
    triangleVertices.clear();
    lineVertices.clear();

    auto selTiles = guiMap.selections.renderTiles.tiles;
    if ( !guiMap.view.tiles.sel().empty() )
    {
        auto tileWidth = guiMap.getTileWidth();
        auto xBegin = guiMap.selections.renderTiles.xBegin;
        auto xEnd = guiMap.selections.renderTiles.xEnd;
        auto yBegin = guiMap.selections.renderTiles.yBegin;
        auto yEnd = guiMap.selections.renderTiles.yEnd;
        for ( std::size_t y=yBegin; y<yEnd; ++y )
        {
            for ( std::size_t x=xBegin; x<xEnd; ++x )
            {
                auto selTile = selTiles[y*tileWidth + x];
                if ( selTile )
                {
                    auto neighbors = *selTile;
                    gl::Rect2D<GLfloat> rect {
                        GLfloat(32*x),
                        GLfloat(32*y),
                        GLfloat(32*x+32),
                        GLfloat(32*y+32),
                    };
                    triangleVertices.vertices.insert(triangleVertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                        rect.left, rect.bottom,
                        rect.left, rect.bottom,
                        rect.right, rect.bottom,
                        rect.right, rect.top
                        });

                    if ( neighbors != TileNeighbor::None ) // Need to draw tile edge line
                    {
                        if ( (neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left, rect.top,
                                rect.right, rect.top,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right, rect.top,
                                rect.right, rect.bottom,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right, rect.bottom,
                                rect.left, rect.bottom,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left, rect.bottom,
                                rect.left, rect.top,
                                });
                        }
                    }
                }
            }
        }
    }

    if ( !triangleVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0x30640000); // MiniTile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();
        if ( !lineVertices.vertices.empty() )
        {
            solidColorShader.solidColor.setColor(0xFFFFFF00); // Line color: 0xAABBGGRR
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }
}

void GuiMapGraphics::drawClipboardActors(bool hasCrgb)
{
    const auto & clipboardUnitActors = guiMap.clipboard.animations.getUnitActors();
    const auto & clipboardSpriteActors = guiMap.clipboard.animations.getSpriteActors();
    guiMap.clipboard.animations.cleanDrawList();
    std::size_t drawListSize = guiMap.clipboard.animations.drawList.size();
    for ( std::size_t i=1; i<drawListSize; ++i )
    {
        std::uint64_t drawEntry = guiMap.clipboard.animations.drawList[i];
        if ( drawEntry == MapAnimations::UnusedDrawEntry )
            break;
        else
        {
            std::size_t index = static_cast<std::size_t>(drawEntry & MapAnimations::MaskIndex);
            point paste = guiMap.selections.endDrag;
            if ( drawEntry & MapAnimations::FlagUnitActor )
                drawActor(guiMap.clipboard.animations, clipboardUnitActors[index], paste.x, paste.y, hasCrgb);
            else
                drawActor(guiMap.clipboard.animations, clipboardSpriteActors[index], paste.x, paste.y, hasCrgb);
        }
    }
}

void GuiMapGraphics::drawTemporaryLocations()
{
    auto start = guiMap.selections.startDrag;
    auto end = guiMap.selections.endDrag;
    auto selectedLocation = guiMap.selections.getSelectedLocation();
    if ( guiMap.selections.getLocationFlags() == LocSelFlags::None ) // Draw location creation preview
    {
        drawSelectionRectangle({ GLfloat(start.x), GLfloat(start.y), GLfloat(end.x), GLfloat(end.y) });
    }
    else if ( selectedLocation != NO_LOCATION && selectedLocation < guiMap.numLocations() ) // Draw location resize/movement graphics
    {
        auto locFlags = guiMap.selections.getLocationFlags();
        const Chk::Location & loc = guiMap.getLocation((size_t)selectedLocation);
        s32 locLeft = loc.left;
        s32 locRight = loc.right;
        s32 locTop = loc.top;
        s32 locBottom = loc.bottom;
        s32 dragX = end.x-start.x;
        s32 dragY = end.y-start.y;
        if ( locFlags != LocSelFlags::Middle )
        {
            if ( locTop > locBottom )
            {
                if ( (locFlags & LocSelFlags::North) == LocSelFlags::North )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::North)|LocSelFlags::South);
                else if ( (locFlags & LocSelFlags::South) == LocSelFlags::South )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::South)|LocSelFlags::North);
            }

            if ( locLeft > locRight )
            {
                if ( (locFlags & LocSelFlags::West) == LocSelFlags::West )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::West)|LocSelFlags::East);
                else if ( (locFlags & LocSelFlags::East) == LocSelFlags::East )
                    locFlags = LocSelFlags(locFlags&(~LocSelFlags::East)|LocSelFlags::West);
            }
        }

        switch ( locFlags )
        {
        case LocSelFlags::North: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top+dragY), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
        case LocSelFlags::South: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom+dragY) }); break;
        case LocSelFlags::East: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right+dragX), GLfloat(loc.bottom) }); break;
        case LocSelFlags::West: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
        case LocSelFlags::NorthWest: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top+dragY), GLfloat(loc.right), GLfloat(loc.bottom) }); break;
        case LocSelFlags::NorthEast: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top+dragY), GLfloat(loc.right+dragX), GLfloat(loc.bottom) }); break;
        case LocSelFlags::SouthWest: drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top), GLfloat(loc.right), GLfloat(loc.bottom+dragY) }); break;
        case LocSelFlags::SouthEast: drawSelectionRectangle({ GLfloat(loc.left), GLfloat(loc.top), GLfloat(loc.right+dragX), GLfloat(loc.bottom+dragY) }); break;

        case LocSelFlags::Middle:
            drawSelectionRectangle({ GLfloat(loc.left+dragX), GLfloat(loc.top+dragY), GLfloat(loc.right+dragX), GLfloat(loc.bottom+dragY) });
            break;
        }
    }
}

void GuiMapGraphics::drawFogTileSelection()
{
    triangleVertices.clear();
    lineVertices.vertices.clear();
    u8 currPlayer = guiMap.getCurrPlayer();
    u8 currPlayerMask = u8Bits[currPlayer];

    auto selFogTiles = guiMap.selections.renderFogTiles.tiles;
    if ( !guiMap.view.tileFog.sel().empty() )
    {
        auto tileWidth = guiMap.getTileWidth();
        auto xBegin = guiMap.selections.renderFogTiles.xBegin;
        auto xEnd = guiMap.selections.renderFogTiles.xEnd;
        auto yBegin = guiMap.selections.renderFogTiles.yBegin;
        auto yEnd = guiMap.selections.renderFogTiles.yEnd;
        for ( std::size_t y=yBegin; y<yEnd; ++y )
        {
            for ( std::size_t x=xBegin; x<xEnd; ++x )
            {
                auto selTile = selFogTiles[y*tileWidth + x];
                if ( selTile )
                {
                    auto neighbors = *selTile;
                    gl::Rect2D<GLfloat> rect {
                        GLfloat(x*32),
                        GLfloat(y*32),
                        GLfloat(x*32+32),
                        GLfloat(y*32+32),
                    };
                    triangleVertices.vertices.insert(triangleVertices.vertices.begin(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                        rect.left, rect.bottom,
                        rect.left, rect.bottom,
                        rect.right, rect.bottom,
                        rect.right, rect.top
                        });

                    if ( neighbors != TileNeighbor::None ) // Some edges need to be drawn
                    {
                        if ( (neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left, rect.top,
                                rect.right, rect.top,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right, rect.top,
                                rect.right, rect.bottom,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right, rect.bottom,
                                rect.left, rect.bottom,
                                });
                        }
                        if ( (neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left, rect.bottom,
                                rect.left, rect.top,
                                });
                        }
                    }
                }
            }
        }
    }

    if ( !triangleVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0x300080FF); // Tile color: 0xAABBGGRR
        triangleVertices.bind();
        triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
        triangleVertices.drawTriangles();

        if ( !lineVertices.vertices.empty() )
        {
            solidColorShader.solidColor.setColor(0xFFFF32FF); // Rectangle color: 0xAABBGGRR
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
    }
}

void GuiMapGraphics::drawDoodadSelection()
{
    lineVertices.vertices.clear();
    const auto & tileset = scData.terrain.get(guiMap->tileset);
    for ( auto index : guiMap.view.doodads.sel() )
    {
        const auto & selDoodad = guiMap.getDoodad(index);
        if ( auto doodadGroupIndex = tileset.getDoodadGroupIndex(selDoodad.type) )
        {
            const auto & doodad = (Sc::Terrain::DoodadCv5 &)tileset.tileGroups[size_t(*doodadGroupIndex)];
            s32 doodadWidth = 32*s32(doodad.tileWidth);
            s32 doodadHeight = 32*s32(doodad.tileHeight);
            s32 left = (s32(selDoodad.xc) - doodadWidth/2 + 16)/32*32;
            s32 top = (s32(selDoodad.yc) - doodadHeight/2 + 16)/32*32;
            s32 right = left + doodadWidth;
            s32 bottom = top + doodadHeight;

            lineVertices.vertices.insert(lineVertices.vertices.begin(), {
                GLfloat(left), GLfloat(top),
                GLfloat(right), GLfloat(top),
                GLfloat(right), GLfloat(top),
                GLfloat(right), GLfloat(bottom),
                GLfloat(left), GLfloat(bottom),
                GLfloat(right), GLfloat(bottom),
                GLfloat(left), GLfloat(top),
                GLfloat(left), GLfloat(bottom)
                });
        }
    }
    if ( !lineVertices.vertices.empty() )
    {
        auto & solidColorShader = renderDat->shaders->solidColorShader;
        solidColorShader.use();
        solidColorShader.posToNdc.setMat4(gameToNdc);
        solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
        lineVertices.bind();
        lineVertices.bufferData(gl::UsageHint::DynamicDraw);
        lineVertices.drawLines();
    }
}

void GuiMapGraphics::drawPastes()
{
    const auto & images = guiMap.clipboard.animations.images;
    auto layer = guiMap.getLayer();
    auto subLayer = guiMap.getSubLayer();
    auto drawPasteTerrain = [&](point paste) {
        if ( subLayer == TerrainSubLayer::Isom && layer != Layer::CutCopyPaste )
        {
            auto diamond = Chk::IsomDiamond::fromMapCoordinates(paste.x, paste.y);

            s32 diamondCenterX = s32(diamond.x)*64;
            s32 diamondCenterY = s32(diamond.y)*32;

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
            lineVertices.vertices = {
                GLfloat(diamondCenterX-64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY-32),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY-32),
                GLfloat(diamondCenterX+64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX-64), GLfloat(diamondCenterY),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY+32),
                GLfloat(diamondCenterX), GLfloat(diamondCenterY+32),
                GLfloat(diamondCenterX+64), GLfloat(diamondCenterY),
            };
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
        else if ( subLayer == TerrainSubLayer::Rectangular || layer == Layer::CutCopyPaste )
        {
            point center { paste.x+16, paste.y+16 };
            const Sc::Terrain::Tiles & tiles = scData.terrain.get(guiMap->tileset);
            auto & pasteTiles = guiMap.clipboard.getTiles();
            tileVertices.vertices.clear();
            lineVertices.vertices.clear();
            auto xTileTexMask = this->renderDat->tiles->xTileTexMask;
            auto yTileTexShift = this->renderDat->tiles->yTileTexShift;
            for ( auto & tile : pasteTiles )
            {
                u16 tileIndex = tile.value;
                size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                if ( groupIndex < tiles.tileGroups.size() )
                {
                    const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                    u32 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                    auto texX = GLfloat(megaTileIndex & xTileTexMask);
                    auto texY = GLfloat(megaTileIndex >> yTileTexShift);
                    gl::Rect2D<GLfloat> rect {
                        GLfloat((tile.xc + center.x)/32),
                        GLfloat((tile.yc + center.y)/32),
                        GLfloat((tile.xc + 32 + center.x)/32),
                        GLfloat((tile.yc + 32 + center.y)/32)
                    };
                    tileVertices.vertices.insert(tileVertices.vertices.end(), {
                        rect.left , rect.top   , texX    , texY,
                        rect.right, rect.top   , texX+1.f, texY,
                        rect.left , rect.bottom, texX    , texY+1.f,
                        rect.left , rect.bottom, texX    , texY+1.f,
                        rect.right, rect.bottom, texX+1.f, texY+1.f,
                        rect.right, rect.top   , texX+1.f, texY
                    });

                    if ( tile.neighbors != TileNeighbor::None ) // Some edges need to be drawn
                    {
                        if ( (tile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left*32, rect.top*32,
                                rect.right*32, rect.top*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right*32, rect.top*32,
                                rect.right*32, rect.bottom*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.right*32, rect.bottom*32,
                                rect.left*32, rect.bottom*32,
                            });
                        }
                        if ( (tile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                        {
                            lineVertices.vertices.insert(lineVertices.vertices.end(), {
                                rect.left*32, rect.bottom*32,
                                rect.left*32, rect.top*32,
                            });
                        }
                    }
                }
            }
            drawTileVertices(renderDat->tiles->tilesetGrp, windowDimensions.width, windowDimensions.height, tileToNdc);
            if ( !lineVertices.vertices.empty() )
            {
                auto & solidColorShader = renderDat->shaders->solidColorShader;
                solidColorShader.use();
                solidColorShader.posToNdc.setMat4(gameToNdc);
                solidColorShader.solidColor.setColor(0xFFFFFF00); // Line color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
        }
    };
    auto drawPasteFog = [&](point paste) {
        triangleVertices.clear();
        lineVertices.vertices.clear();
        point center { paste.x+16, paste.y+16 };
        u8 currPlayer = guiMap.getCurrPlayer();
        u8 currPlayerMask = u8Bits[currPlayer];
        auto fogTiles = guiMap.clipboard.getFogTiles();
        for ( auto & fogTile : fogTiles )
        {
            gl::Rect2D<GLfloat> rect {
                GLfloat((fogTile.xc + center.x)/32*32),
                GLfloat((fogTile.yc + center.y)/32*32),
                GLfloat((fogTile.xc + 32 + center.x)/32*32),
                GLfloat((fogTile.yc + 32 + center.y)/32*32)
            };
            if ( (fogTile.value & currPlayerMask) != 0 )
            {
                triangleVertices.vertices.insert(triangleVertices.vertices.begin(), {
                    rect.left, rect.top,
                    rect.right, rect.top,
                    rect.left, rect.bottom,
                    rect.left, rect.bottom,
                    rect.right, rect.bottom,
                    rect.right, rect.top
                });
            }

            if ( fogTile.neighbors != TileNeighbor::None ) // Some edges need to be drawn
            {
                if ( (fogTile.neighbors & TileNeighbor::Top) == TileNeighbor::Top )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Right) == TileNeighbor::Right )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.right, rect.top,
                        rect.right, rect.bottom,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Bottom) == TileNeighbor::Bottom )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.right, rect.bottom,
                        rect.left, rect.bottom,
                    });
                }
                if ( (fogTile.neighbors & TileNeighbor::Left) == TileNeighbor::Left )
                {
                    lineVertices.vertices.insert(lineVertices.vertices.end(), {
                        rect.left, rect.bottom,
                        rect.left, rect.top,
                    });
                }
            }
        }

        if ( !triangleVertices.vertices.empty() )
        {
            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0x60000000); // Tile color: 0xAABBGGRR
            triangleVertices.bind();
            triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
            triangleVertices.drawTriangles();

            if ( !lineVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFFFF32FF); // Rectangle color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
        }
    };
    auto drawPasteDoodads = [&](point paste) {
        bool allowIllegalDoodads = guiMap.AllowIllegalDoodadPlacement();
        const auto & doodads = guiMap.clipboard.getDoodads();
        if ( !doodads.empty() )
        {
            const Sc::Terrain::Tiles & tiles = scData.terrain.get(guiMap->tileset);
            point center { paste.x, paste.y };
            tileVertices.vertices.clear();
            triangleVertices.clear();
            triangleVertices2.clear();
            for ( auto & doodad : doodads )
            {
                auto tileWidth = doodad.tileWidth;
                auto tileHeight = doodad.tileHeight;
                bool evenWidth = tileWidth%2 == 0;
                bool evenHeight = tileHeight %2 == 0;
                auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                auto xTileStart = xStart/32;
                auto yTileStart = yStart/32;
                const auto & placability = tiles.doodadPlacibility[doodad.doodadId];
                auto xTileTexMask = this->renderDat->tiles->xTileTexMask;
                auto yTileTexShift = this->renderDat->tiles->yTileTexShift;
                for ( u16 y=0; y<tileHeight; ++y )
                {
                    for ( u16 x=0; x<tileWidth; ++x )
                    {
                        if ( doodad.tileIndex[x][y] != 0 )
                        {
                            u16 tileIndex = doodad.tileIndex[x][y];
                            size_t groupIndex = Sc::Terrain::Tiles::getGroupIndex(tileIndex);
                            if ( groupIndex < tiles.tileGroups.size() )
                            {
                                const Sc::Terrain::TileGroup & tileGroup = tiles.tileGroups[groupIndex];
                                u32 megaTileIndex = tileGroup.megaTileIndex[tiles.getGroupMemberIndex(tileIndex)];
                                auto texX = GLfloat(megaTileIndex & xTileTexMask);
                                auto texY = GLfloat(megaTileIndex >> yTileTexShift);
                                gl::Rect2D<GLfloat> rect {
                                    GLfloat(xTileStart + x),
                                    GLfloat(yTileStart + y),
                                    GLfloat(xTileStart + x + 1),
                                    GLfloat(yTileStart + y + 1)
                                };
                                tileVertices.vertices.insert(tileVertices.vertices.begin(), {
                                    rect.left , rect.top   , texX    , texY,
                                    rect.right, rect.top   , texX+1.f, texY,
                                    rect.left , rect.bottom, texX    , texY+1.f,
                                    rect.left , rect.bottom, texX    , texY+1.f,
                                    rect.right, rect.bottom, texX+1.f, texY+1.f,
                                    rect.right, rect.top   , texX+1.f, texY
                                });
                                if ( placability.tileGroup[y*tileWidth+x] != 0 )
                                {
                                    size_t tileXc = xTileStart + x;
                                    size_t tileYc = yTileStart + y;
                                    if ( tileXc < guiMap->dimensions.tileWidth && tileYc < guiMap->dimensions.tileHeight )
                                    {
                                        u16 existingTileGroup = guiMap.getTile(tileXc, tileYc) / 16;
                                        bool placeable = existingTileGroup == placability.tileGroup[y*tileWidth+x] || allowIllegalDoodads;
                                        auto & verts = placeable ? triangleVertices : triangleVertices2;
                                        verts.vertices.insert(verts.vertices.begin(), {
                                            rect.left*32, rect.top*32,
                                            rect.right*32, rect.top*32,
                                            rect.left*32, rect.bottom*32,
                                            rect.left*32, rect.bottom*32,
                                            rect.right*32, rect.bottom*32,
                                            rect.right*32, rect.top*32
                                        });
                                    }
                                }
                            }
                        }
                    }
                }
            }
            drawTileVertices(renderDat->tiles->tilesetGrp, windowDimensions.width, windowDimensions.height, tileToNdc);
            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            if ( !triangleVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0x30008000); // Doodad tile shading: 0xAABBGGRR
                triangleVertices.bind();
                triangleVertices.bufferData(gl::UsageHint::DynamicDraw);
                triangleVertices.drawTriangles();
            }
            if ( !triangleVertices2.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0x200000FF); // Doodad tile shading: 0xAABBGGRR
                triangleVertices2.bind();
                triangleVertices2.bufferData(gl::UsageHint::DynamicDraw);
                triangleVertices2.drawTriangles();
            }

            auto getDoodadImageId = [&](const auto & doodad) {
                return doodad.isSprite() ? getImageId(Sc::Sprite::Type(doodad.overlayIndex)) : getImageId(Sc::Unit::Type(doodad.overlayIndex));
            };
            auto getDoodadImage = [&](const auto & doodad) -> Animation & {
                return doodad.isSprite() ? getImage(Sc::Sprite::Type(doodad.overlayIndex)) : getImage(Sc::Unit::Type(doodad.overlayIndex));
            };

            prepareImageRendering();
            auto & palette = renderDat->tiles->tilesetGrp.palette;

            if ( loadSettings.skinId == Skin::Id::Classic )
            {
                for ( auto & doodad : doodads )
                {
                    auto tileWidth = doodad.tileWidth;
                    auto tileHeight = doodad.tileHeight;
                    bool evenWidth = tileWidth%2 == 0;
                    bool evenHeight = tileHeight %2 == 0;
                    auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                    auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                    if ( doodad.overlayIndex != 0 )
                        drawClassicImage(*palette, s32(xStart+tileWidth*16), s32(yStart+tileHeight*16), 0, getDoodadImageId(doodad), (Chk::PlayerColor)doodad.owner);
                }
            }
            else
            {
                bool hasCrgb = guiMap->hasSection(Chk::SectionName::CRGB);
                for ( auto & doodad : doodads )
                {
                    auto tileWidth = doodad.tileWidth;
                    auto tileHeight = doodad.tileHeight;
                    bool evenWidth = tileWidth%2 == 0;
                    bool evenHeight = tileHeight %2 == 0;
                    auto xStart = evenWidth ? -16*doodad.tileWidth + (center.x+doodad.tileX*32+16)/32*32 : -16*(doodad.tileWidth-1) + (center.x + doodad.tileX*32)/32*32;
                    auto yStart = evenHeight ? -16*doodad.tileHeight + (center.y+doodad.tileY*32+16)/32*32 : -16*(doodad.tileHeight-1) + (center.y+doodad.tileY*32)/32*32;
                    if ( doodad.overlayIndex != 0 )
                        drawImage(getDoodadImage(doodad), s32(xStart+tileWidth*16), s32(yStart+tileHeight*16), 0, 0, 0xFFFFFFFF, getPlayerColor(doodad.owner, hasCrgb), false);
                }
            }
        }
    };
    auto drawPasteUnits = [&](point paste) {
        if ( paste.x != -1 && paste.y != -1 && (guiMap.clipboard.hasUnits() || guiMap.clipboard.hasQuickUnits()) )
        {
            auto & units = guiMap.clipboard.getUnits();

            lineVertices.vertices.clear();
            lineVertices2.vertices.clear();
            for ( auto & pasteUnit : units )
            {
                if ( pasteUnit.unit.type < Sc::Unit::TotalTypes )
                {
                    const auto & unitDat = scData.units.getUnit(pasteUnit.unit.type);
                    bool isValidPlacement = guiMap.isValidUnitPlacement(pasteUnit.unit.type, paste.x + pasteUnit.xc, paste.y + pasteUnit.yc);
                    bool isBuilding = (unitDat.flags & Sc::Unit::Flags::Building) == Sc::Unit::Flags::Building;
                    auto & vertices = isValidPlacement ? lineVertices : lineVertices2;
                    gl::Rect2D<GLfloat> rect {};
                    if ( isBuilding )
                    {
                        rect = {
                            GLfloat(paste.x + pasteUnit.xc - unitDat.starEditPlacementBoxWidth/2),
                            GLfloat(paste.y + pasteUnit.yc - unitDat.starEditPlacementBoxHeight/2),
                            GLfloat(paste.x + pasteUnit.xc + unitDat.starEditPlacementBoxWidth/2),
                            GLfloat(paste.y + pasteUnit.yc + unitDat.starEditPlacementBoxHeight/2)
                        };
                    }
                    else
                    {
                        rect = {
                            GLfloat(paste.x + pasteUnit.xc - unitDat.unitSizeLeft),
                            GLfloat(paste.y + pasteUnit.yc - unitDat.unitSizeUp),
                            GLfloat(paste.x + pasteUnit.xc + unitDat.unitSizeRight),
                            GLfloat(paste.y + pasteUnit.yc + unitDat.unitSizeDown)
                        };
                    }
                    vertices.vertices.insert(vertices.vertices.end(), {
                        rect.left, rect.top,
                        rect.right, rect.top,
                        rect.right, rect.top,
                        rect.right, rect.bottom,
                        rect.right, rect.bottom,
                        rect.left, rect.bottom,
                        rect.left, rect.bottom,
                        rect.left, rect.top,
                    });
                }
            }

            prepareImageRendering();
            auto & palette = renderDat->tiles->tilesetGrp.palette;

            if ( loadSettings.skinId == Skin::Id::Classic )
            {
                //for ( auto & pasteUnit : units )
                //    drawClassicImage(*palette, paste.x+pasteUnit.xc, paste.y+pasteUnit.yc, 0, getImageId(pasteUnit.unit), (Chk::PlayerColor)pasteUnit.unit.owner);
            }
            else
            {
                //for ( auto & pasteUnit : units )
                //    drawImage(getImage(pasteUnit.unit), paste.x+pasteUnit.xc, paste.y+pasteUnit.yc, 0, 0xFFFFFFFF, getPlayerColor(pasteUnit.unit.owner), false);
            }

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            if ( !lineVertices.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFF00FF00); // Rectangle color: 0xAABBGGRR
                lineVertices.bind();
                lineVertices.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices.drawLines();
            }
            if ( !lineVertices2.vertices.empty() )
            {
                solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
                lineVertices2.bind();
                lineVertices2.bufferData(gl::UsageHint::DynamicDraw);
                lineVertices2.drawLines();
            }
        }
    };
    auto drawPasteSprites = [&](point paste) {
        if ( paste.x != -1 && paste.y != -1 && (guiMap.clipboard.hasSprites() || guiMap.clipboard.hasQuickSprites()) )
        {
            auto & sprites = guiMap.clipboard.getSprites();

            prepareImageRendering();

            auto & palette = renderDat->tiles->tilesetGrp.palette;

            if ( loadSettings.skinId == Skin::Id::Classic )
            {
                //for ( auto & pasteSprite : sprites )
                //{
                //    drawClassicImage(*palette, paste.x+pasteSprite.xc, paste.y+pasteSprite.yc, 0,//pasteSprite.anim.frame,
                //        getImageId(pasteSprite.sprite), (Chk::PlayerColor)pasteSprite.sprite.owner);
                //}
            }
            else
            {
                //for ( auto & pasteSprite : sprites )
                //{
                //    drawImage(getImage(pasteSprite.sprite), paste.x+pasteSprite.xc/*+pasteSprite.anim.xOffset*/, paste.y+pasteSprite.yc/*+pasteSprite.anim.yOffset*/,
                //        images[pasteSprite.testAnim.usedImages[0]]->frame, 0xFFFFFFFF, getPlayerColor(pasteSprite.sprite.owner), false);
                //}
            }
        }
    };

    switch ( layer )
    {
        case Layer::Terrain: drawPasteTerrain(guiMap.selections.endDrag); break;
        case Layer::Doodads: drawPasteDoodads(guiMap.selections.endDrag); break;
        case Layer::Units: drawPasteUnits(guiMap.selections.endDrag); break;
        case Layer::Sprites: drawPasteSprites(guiMap.selections.endDrag); break;
        case Layer::FogEdit:
        {
            const auto brushWidth = guiMap.clipboard.getFogBrush().width;
            const auto brushHeight = guiMap.clipboard.getFogBrush().height;
            s32 hoverTileX = (guiMap.selections.endDrag.x + (brushWidth % 2 == 0 ? 16 : 0))/32;
            s32 hoverTileY = (guiMap.selections.endDrag.y + (brushHeight % 2 == 0 ? 16 : 0))/32;

            const auto startX = 32*(hoverTileX - brushWidth/2);
            const auto startY = 32*(hoverTileY - brushHeight/2);
            const auto endX = startX+32+brushWidth;
            const auto endY = startY+32+brushHeight;
            gl::Rect2D<GLfloat> rect {
                GLfloat(startX), GLfloat(startY),
                GLfloat(endX), GLfloat(endY)
            };

            auto & solidColorShader = renderDat->shaders->solidColorShader;
            solidColorShader.use();
            solidColorShader.posToNdc.setMat4(gameToNdc);
            solidColorShader.solidColor.setColor(0xFF0000FF); // Rectangle color: 0xAABBGGRR
            lineVertices.vertices = {
                rect.left, rect.top,
                rect.right, rect.top,
                rect.right, rect.top,
                rect.right, rect.bottom,
                rect.right, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.bottom,
                rect.left, rect.top,
            };
            lineVertices.bind();
            lineVertices.bufferData(gl::UsageHint::DynamicDraw);
            lineVertices.drawLines();
        }
        break;
        case Layer::CutCopyPaste:
        {
            point paste = guiMap.selections.endDrag;
            bool pastingTerrain = guiMap.getCutCopyPasteTerrain() && guiMap.clipboard.hasTiles();
            bool pastingDoodads = guiMap.getCutCopyPasteDoodads() && guiMap.clipboard.hasDoodads();
            bool pastingFog = guiMap.getCutCopyPasteFog() && guiMap.clipboard.hasFogTiles();
            if ( pastingTerrain || pastingDoodads || pastingFog )
            {
                paste.x = (paste.x+16)/32*32;
                paste.y = (paste.y+16)/32*32;
            }
            if ( pastingTerrain )
                drawPasteTerrain(paste);
            if ( pastingDoodads )
                drawPasteDoodads(paste);
            if ( pastingFog )
                drawPasteFog(paste);
            if ( guiMap.getCutCopyPasteUnits() )
                drawPasteUnits(paste);
            if ( guiMap.getCutCopyPasteSprites() )
                drawPasteSprites(paste);
        }
        break;
    }
}

void GuiMapGraphics::render()
{
    this->frameStart = std::chrono::system_clock::now();
    auto layer = guiMap.getLayer();

    if ( loadSettings.skinId == Skin::Id::Classic )
        drawClassicStars();
    else
        drawStars(0xFFFFFFFF);

    drawTerrain();
    if ( displayElevations || displayBuildability )
        drawTileOverlays();

    drawGrid();
    drawImageSelections();

    bool hasCrgb = guiMap->hasSection(Chk::SectionName::CRGB);
    prepareImageRendering();
    drawActors(hasCrgb);
    drawClipboardActors(hasCrgb);

    switch ( layer ) {
        case Layer::Locations: drawLocations(!guiMap.LockAnywhere(), guiMap.selections.getSelectedLocation()); break;
        case Layer::FogEdit: case Layer::CutCopyPaste: drawFog(guiMap.getCurrPlayer()); break;
    }

    if ( displayTileNums )
        drawTileNums();

    if ( displayIsomNums )
        drawIsomTileNums();

    if ( fpsEnabled )
        drawFps();

    if ( guiMap.selections.hasTiles() && (layer == Layer::Terrain || layer == Layer::CutCopyPaste) )
        drawTileSelection();
    else if ( layer == Layer::Locations )
        drawTemporaryLocations();

    if ( layer == Layer::Doodads || layer == Layer::CutCopyPaste )
        drawDoodadSelection();

    if ( layer == Layer::CutCopyPaste )
        drawFogTileSelection();

    if ( guiMap.clipboard.isPasting() )
        drawPastes();

    if ( layer != Layer::Locations && guiMap.isDragging() && !guiMap.clipboard.isPasting() )
    {
        drawSelectionRectangle({
            GLfloat(guiMap.selections.startDrag.x), GLfloat(guiMap.selections.startDrag.y),
            GLfloat(guiMap.selections.endDrag.x), GLfloat(guiMap.selections.endDrag.y)
        });
    }
}
