#ifndef PATTERNS_LIB
#define PATTERNS_LIB

#include "grid.hpp"

namespace patterns
{
    // Still lifes
    void place_block(Grid &grid, int x, int y);
    void place_beehive(Grid &grid, int x, int y);
    void place_loaf(Grid &grid, int x, int y);

    // Oscillators
    void place_blinker(Grid &grid, int x, int y);
    void place_toad(Grid &grid, int x, int y);
    void place_beacon(Grid &grid, int x, int y);

    // Spaceships
    void place_glider(Grid &grid, int x, int y);
    void place_LWSS(Grid &grid, int x, int y);
}

#endif //PATTERNS_LIB
