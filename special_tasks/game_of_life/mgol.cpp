#include "mpi_utils.hpp"

void showExtendedHelp(char *name)
{
    showHelp(name);
    std::cout << "    O                          -- enables MPI send/recv optimization" << std::endl;
    std::cout << "WARNING: anisotropy, the grid is cut into horizontal stripes and therefore "
    "the greatest speedup is achieved with a high height and low width of playing field!" << std::endl;
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    CLISettings settings;
    if(initGOL(argc, argv, settings, showExtendedHelp))
        return -1;

    int last_rank = world_size - 1;
    int width = settings.width;
    int common_slice_height = settings.height / world_size;
    int height = (rank == last_rank) ? (settings.height - last_rank * common_slice_height) : common_slice_height;
    int iterations_left = settings.iterations_limit;

    std::vector<cell_t> *upper_line = nullptr;
    std::vector<cell_t> *bottom_line = nullptr;
    if(world_size > 1)
    {
        upper_line = new std::vector<cell_t>();
        upper_line->resize(width);
        bottom_line = new std::vector<cell_t>();
        bottom_line->resize(width);
    }

    Grid grids[] = {
        Grid(width, height, width, settings.height, 0, rank * common_slice_height),
        Grid(width, height, width, settings.height, 0, rank * common_slice_height)
    };
    GridRenderer *renderer = settings.renderer;
    if((rank > 0) && (renderer != nullptr))
        renderer->setOutput(new std::stringstream());
    int grid_id = 0;

    if(addPatterns(grids[0], settings.initial_state_string))
        return -1;

    Statistics statistics(settings.statistics_delay);

    render(rank, world_size, grids[0], renderer, statistics); // show initial state

    while(iterations_left > 0)
    {
        exchange(rank, world_size, grids[grid_id], settings.opt, upper_line, bottom_line);

        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
                grids[1 - grid_id].setState(i, j, grids[grid_id].getNewState(i, j, upper_line, bottom_line));
        }

        grid_id = 1 - grid_id; // flip
        render(rank, world_size, grids[grid_id], renderer, statistics);
        iterations_left--;
    }

    if((rank == 0) && (renderer == nullptr) && (settings.iterations_limit % settings.statistics_delay == 0))
        std::cout << std::endl;

    TRY(MPI_Finalize(), "Bad MPI finalization");
        return 0;
}
