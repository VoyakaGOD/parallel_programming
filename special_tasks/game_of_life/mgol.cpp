// todo: fix troubles with setState/getState/[]
// todo: add send/recv opt flag O
// todo: extra help info about O and anisotropy

#include <mpi_try.hpp>
#include <sstream>
#include "CLI.hpp"

void render(int rank, int world_size, const Grid &grid, GridRenderer *renderer, Statistics &statistics)
{
    if(renderer == nullptr)
    {
        if(rank == 0)
            statistics.reportAboutNewGeneration(false);
        return;
    }

    // slow solution, but it doesn't use in measurements    
    grid.render(renderer);
    if(rank == 0)
    {
        char *part = nullptr;
        for(int i = 1; i < world_size; i++)
        {
            MPI_Status status;
            int data_size;
            TRY(MPI_Probe(i, 0, MPI_COMM_WORLD, &status), "Can't probe part");
            TRY(MPI_Get_count(&status, MPI_BYTE, &data_size), "Can't get count of bytes in part");
            part = new char[data_size];
            TRY(part == nullptr, "Can't allocate memory for part");
            MPI_Recv(part, data_size, MPI_CHAR, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::cout << part;

            delete[] part;
        }
        statistics.reportAboutNewGeneration(true);
    }
    else
    {
        std::stringstream *output = dynamic_cast<std::stringstream *>(renderer->getOutput());
        std::string part = output->str();
        TRY(MPI_Send(part.c_str(), part.length() + 1, MPI_CHAR, 0, 0, MPI_COMM_WORLD), "Can't send part");
        output->str("");
    }
}

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

    // todo: boundary cells
    while(iterations_left > 0)
    {
        for(int j = 0; j < height; j++)
        {
            for(int i = 0; i < width; i++)
                grids[1 - grid_id].setState(i, j, grids[grid_id].getNewState(i, j));
        }
        grid_id = 1 - grid_id; // flip
        render(rank, world_size, grids[grid_id], renderer, statistics);

        iterations_left--;
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");
        return 0;
}
