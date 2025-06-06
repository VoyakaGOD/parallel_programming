#include "CLI.hpp"

int main(int argc, char** argv)
{
    CLISettings settings;
    if(initGOL(argc, argv, settings))
        return -1;

    int width = settings.width;
    int height = settings.height;
    int iterations_left = settings.iterations_limit;

    Grid grids[] = {Grid(width, height), Grid(width, height)};
    GridRenderer *renderer = settings.renderer;
    int grid_id = 0;

    if(addPatterns(grids[0], settings.initial_state_string))
        return -1;

    Statistics statistics(settings.statistics_delay);

    if(renderer)
        grids[0].render(renderer); // show initial state
    statistics.reportAboutNewGeneration(renderer != nullptr);

    while(iterations_left > 0)
    {
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
                grids[1 - grid_id].setState(i, j, grids[grid_id].getNewState(i, j));
        }
        grid_id = 1 - grid_id; // flip
        if(renderer)
            grids[grid_id].render(renderer);
        statistics.reportAboutNewGeneration(renderer != nullptr);

        iterations_left--;
    }

    if((renderer == nullptr) && (settings.iterations_limit % settings.statistics_delay == 0))
        std::cout << std::endl;
}
