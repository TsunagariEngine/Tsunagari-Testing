/*************************************
** Tsunagari Tile Engine            **
** grove-house.cpp                  **
** Copyright 2016-2021 Paul Merrill **
*************************************/

// **********
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// **********

#include "areas/grove-house.h"

#include "core/area.h"
#include "core/log.h"
#include "core/tile.h"
#include "util/assert.h"

GroveHouse::GroveHouse() noexcept {
    scripts[StringView("open_door")] = (TileScript)&GroveHouse::onOpenDoor;
    scripts[StringView("sound_armor")] = (TileScript)&GroveHouse::armorSound;
    scripts[StringView("sound_book")] = (TileScript)&GroveHouse::bookSound;
    scripts[StringView("sound_ouch")] = (TileScript)&GroveHouse::ouchSound;
}

void
GroveHouse::onOpenDoor(Entity&) noexcept {
    if (openedDoor) {
        logErr("grove_house", "onOpenDoor called again");
        return;
    }
    openedDoor = true;

    // torch which activated this trigger should make "ouch" now
    ivec3 torch = area->grid.virt2phys(vicoord{6, 0, 0.0});
    area->grid.scripts[TileGrid::SCRIPT_TYPE_USE][torch] =
        (TileScript)&GroveHouse::ouchSound;

    // closed exit on north wall, object layer
    auto door = area->grid.virt2phys(vicoord{4, 0, 0.0});
    area->grid.exits[EXIT_NORMAL][door] =
        Exit{"areas/secret_room.json", 4, 5, 0.0};
    area->grid.flags[door] &= ~TILE_NOWALK;

    auto tileSet = area->getTileSet("areas/tiles/indoors.bmp");
    assert_(tileSet);

    // closed exit on north wall, graphics layer
    // change to open exit
    vicoord doorGraphic = {4, 0, -0.2};
    area->grid.setTileType(doorGraphic, tileSet->at(2, 9));

    area->requestRedraw();

    playSoundEffect("sounds/door.oga");
}

void
GroveHouse::armorSound() noexcept {
    playSoundEffect("sounds/metal_clank.oga");
}

void
GroveHouse::bookSound() noexcept {
    playSoundEffect("sounds/book.oga");
}

void
GroveHouse::ouchSound() noexcept {
    playSoundEffect("sounds/ouch.oga");
}
