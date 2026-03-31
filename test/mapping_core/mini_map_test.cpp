#include <gtest/gtest.h>
#include "mini_map_test_data.h"
#include "test_assets.h"
#include <mapping_core/mapping_core.h>
#include <mapping_core/render/sc_mini_map.h>
#include <algorithm>
#include <memory>

std::shared_ptr<Scenario> create_scenario(u16 width, u16 height)
{
    auto scenario = std::make_shared<Scenario>();
    auto mapData = std::make_unique<::MapData>();
    mapData->saveType = SaveType::ExpansionScx;
    mapData->tileset = Sc::Terrain::Tileset::Jungle;
    mapData->dimensions = Chk::DIM{.tileWidth = width, .tileHeight = height};
    mapData->tiles.assign(size_t(width)*size_t(height), u16(0));
    u16 numValidNonRemasteredJungleTiles = 25248;
    u16 limit = std::min(u16(width*height), numValidNonRemasteredJungleTiles);
    for ( u16 i=0; i<limit; ++i )
        mapData->tiles[i] = i;

    int right = int(width) * int(Sc::Terrain::PixelsPerTile);
    int bottom = int(height) * int(Sc::Terrain::PixelsPerTile);

    // TODO: Minimap unit draw order needs to be fixed, then the test data can be rebuilt accounting for that & multiple player colors can be used
    mapData->units.assign({
        Chk::Unit {1, u16(128), u16(176), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1},
        Chk::Unit {2, u16(128+96), u16(176-64), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p2
        Chk::Unit {3, u16(128+96), u16(176), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p3
        Chk::Unit {4, u16(128), u16(176-64), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p4

        Chk::Unit {5, u16(right-128), u16(bottom-176), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p5
        Chk::Unit {6, u16(right-128-96), u16(bottom-176-64), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p6
        Chk::Unit {7, u16(right-128-96), u16(bottom-176), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1}, // TODO: p7
        Chk::Unit {8, u16(right-128), u16(bottom-176-64), Sc::Unit::Type::TerranCommandCenter, 0, 0, 0, Sc::Player::Id::Player1} // TODO: p8
    });
    scenario->init_data(*mapData);
    return scenario;
}

TEST(MiniMapTest, MiniMapTest)
{
    Sc::Data scData {};
    bool loaded = TestAssets::LoadScData(scData);

    if ( loaded )
    {
        u16 dims[] {
            64, 64,
            64, 96,
            64, 128,
            64, 192,
            64, 256,
            96, 64,
            96, 96,
            96, 128,
            96, 192,
            96, 256,
            128, 64,
            128, 96,
            128, 128,
            128, 192,
            128, 256,
            192, 64,
            192, 96,
            192, 128,
            192, 192,
            192, 256,
            256, 64,
            256, 96,
            256, 128,
            256, 192,
            256, 256
        };
    
        for ( std::size_t i=0; i<25; ++i )
        {
            u16 width = dims[i*2];
            u16 height = dims[i*2+1];
    
            auto scenario = create_scenario(width, height);
            ClassicMiniMap miniMap(scData, *scenario);

            miniMap.render();
            EXPECT_EQ(128, miniMap.width);
            EXPECT_EQ(128, miniMap.height);
            EXPECT_EQ(128*128, miniMap.pixels.size());

            bool miniMapPixelsEq = (std::memcmp(&miniMapPixels[i][0], &miniMap.pixels[0], 128*128*sizeof(std::uint32_t)) == 0);
            EXPECT_TRUE(miniMapPixelsEq);

            /*std::stringstream ss {};
            for ( std::size_t i=0; i<128*128; ++i )
                ss << miniMap.pixels[i] << ", ";
            stringToFile("C:\\misc\\MiniTestMaps\\" + std::to_string(width) + "_" + std::to_string(height) + ".txt", ss.str());*/
        }
    }
}
