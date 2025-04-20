// fix troubles with setState/getState/[]
// add send/recv opt flag O
// extra help info about O and anisotropy

#include <mpi_try.hpp>
#include "CLI.hpp"

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    CLISettings settings;
    if(initGOL(argc, argv, settings))
        return -1;

    int last_rank = world_size - 1;
    int width = settings.width;
    int common_slice_height = settings.height / world_size;
    int height = (rank == last_rank) ? (settings.height - last_rank * common_slice_height) : common_slice_height;
    int iterations_left = settings.iterations_limit;

    Grid grids[] = {Grid(width, height), Grid(width, height)};
    GridRenderer *renderer = settings.renderer;
    int grid_id = 0;

    if(addPatterns(grids[0], settings.initial_state_string, 0, rank * common_slice_height))
        return -1;

    MPI_SYNC(grids[0].render(renderer)); // show initial state

    // boundary cells
    while(iterations_left > 0)
    {
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
                grids[1 - grid_id].setState(i, j, grids[grid_id].getNewState(i, j));
        }
        grid_id = 1 - grid_id; // flip
        MPI_SYNC(grids[grid_id].render(renderer));

        iterations_left--;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
        return 0;
}
