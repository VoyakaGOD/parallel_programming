#include "patterns_lib.hpp"

namespace patterns
{
    void place_block(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x, y, true);
        grid.setStateClamped(x, y + 1, true);
        grid.setStateClamped(x + 1, y, true);
        grid.setStateClamped(x + 1, y + 1, true);
    }

    void place_beehive(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x + 1, y, true);
        grid.setStateClamped(x + 2, y, true);
        grid.setStateClamped(x + 3, y + 1, true);
        grid.setStateClamped(x + 1, y + 2, true);
        grid.setStateClamped(x + 2, y + 2, true);
        grid.setStateClamped(x, y + 1, true);
    }

    void place_loaf(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x + 1, y, true);
        grid.setStateClamped(x + 2, y, true);
        grid.setStateClamped(x + 3, y + 1, true);
        grid.setStateClamped(x + 3, y + 2, true);
        grid.setStateClamped(x + 2, y + 3, true);
        grid.setStateClamped(x + 1, y + 2, true);
        grid.setStateClamped(x, y + 1, true);
    }

    void place_blinker(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x + 1, y, true);
        grid.setStateClamped(x + 1, y + 1, true);
        grid.setStateClamped(x + 1, y + 2, true);
    }

    void place_toad(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x, y + 1, true);
        grid.setStateClamped(x, y + 2, true);
        grid.setStateClamped(x + 1, y + 3, true);
        grid.setStateClamped(x + 2, y, true);
        grid.setStateClamped(x + 3, y + 1, true);
        grid.setStateClamped(x + 3, y + 2, true);
    }

    void place_beacon(Grid &grid, int x, int y)
    {
        place_block(grid, x, y);
        place_block(grid, x + 2, y + 2);
    }

    void place_glider(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x, y + 2, true);
        grid.setStateClamped(x + 1, y + 2, true);
        grid.setStateClamped(x + 2, y + 2, true);
        grid.setStateClamped(x + 2, y + 1, true);
        grid.setStateClamped(x + 1, y, true);
    }

    void place_LWSS(Grid &grid, int x, int y)
    {
        grid.setStateClamped(x, y, true);
        grid.setStateClamped(x + 3, y, true);
        grid.setStateClamped(x, y + 2, true);
        grid.setStateClamped(x + 4, y + 1, true);
        grid.setStateClamped(x + 4, y + 2, true);
        grid.setStateClamped(x + 4, y + 3, true);
        grid.setStateClamped(x + 3, y + 3, true);
        grid.setStateClamped(x + 2, y + 3, true);
        grid.setStateClamped(x + 1, y + 3, true);
    }
}
