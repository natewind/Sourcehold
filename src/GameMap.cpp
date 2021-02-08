#include <random>
#include <algorithm>

#include "GameMap.h"
#include "GameManager.h"

#include "Parsers/Gm1File.h"
#include "Parsers/TgxFile.h"

#include "Rendering/TextureAtlas.h"
#include "Rendering/Tileset.h"
#include "Rendering/Renderer.h"
#include "Rendering/Camera.h"

using namespace Sourcehold::Game;

GameMap::GameMap(MapDimension type)
{
    static const int dimensions[] = { 160, 200, 300, 400 };
    dim = dimensions[(int)type];
}

GameMap::GameMap(ghc::filesystem::path path)
{
    // TODO //
    LoadFromDisk(path);
    dim = 160;
}

GameMap::~GameMap()
{
}

void GameMap::LoadFromDisk(ghc::filesystem::path path)
{
    MapFile::LoadFromDisk(path);

    gm1_tile = GetGm1("gm/tile_land8.gm1");

    tileset = gm1_tile->GetTileset();
    tiles.resize(dim * dim);

    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_int_distribution<std::mt19937::result_type> dist(0, 8);

    auto get_tile = [&]() { return tileset->GetTile(dist(rng)); };

    std::generate(begin(tiles), end(tiles), get_tile);
}

void GameMap::Render()
{
    Camera& cam = Camera::instance();

    mult = cam.zoomLevel == ZOOM_NEAR ? 2 : 1;

    for(uint32_t i = 0; i < tiles.size(); i++) {
        SDL_Rect clip = tiles[i];

        int iy = i / dim;
        int ix = i % dim;

        int y = (8 * iy);
        int x = (30 * ix) + (iy % 2 == 0 ? 15 : 0);

        Rendering::Render(
            *tileset,
            int(mult * x - cam.positionX), int(mult * y - cam.positionY),
            30*mult,
            16*mult,
            &clip
        );
    }
}

