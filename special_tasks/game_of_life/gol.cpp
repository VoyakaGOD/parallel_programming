#include "grid.hpp"

int main(int argc, char** argv)
{
    int width = 100;
    int height = 25;
    int iterations_left = 1000;

    Grid grids[] = {Grid(width, height), Grid(width, height)};
    ConsoleGridRenderer renderer;
    int grid_id = 0;

    // grids[0].setState(15, 15, true);
    // grids[0].setState(15, 14, true);
    // grids[0].setState(15, 13, true);
    // grids[0].setState(14, 14, true);
    // grids[0].setState(16, 15, true);

    grids[0].setState(15, 15, true);
    grids[0].setState(16, 15, true);
    grids[0].setState(17, 15, true);
    grids[0].setState(17, 14, true);
    grids[0].setState(16, 13, true);

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
