#include "patterns_lib.hpp"

int main(int argc, char** argv)
{
    int width = 100;
    int height = 25;
    int iterations_left = 1000;

    Grid grids[] = {Grid(width, height), Grid(width, height)};
    ConsoleGridRenderer renderer(200);
    int grid_id = 0;

    patterns::place_block(grids[0], 5, 1);
    patterns::place_beehive(grids[0], 25, 1);
    patterns::place_loaf(grids[0], 45, 1);

    patterns::place_blinker(grids[0], 5, 7);
    patterns::place_toad(grids[0], 25, 7);
    patterns::place_beacon(grids[0], 45, 7);

    patterns::place_LWSS(grids[0], 10, 15);

    grids[0].render(renderer); // show initial state

    while(true)
    {
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
                grids[1 - grid_id].setState(i, j, grids[grid_id].getNewState(i, j));
        }
        grid_id = 1 - grid_id; // flip
        grids[grid_id].render(renderer);

        if(iterations_left > 0)
        {
            iterations_left--;
            if(iterations_left == 0)
                break;
        }
    }
}
