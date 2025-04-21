#include <mpi_try.hpp>
#include <sstream>
#include "CLI.hpp"

void showExtendedHelp(char *name)
{
    showHelp(name);
    std::cout << "    O                          -- enables MPI send/recv optimization" << std::endl;
    std::cout << "WARNING: anisotropy, the grid is cut into horizontal stripes and therefore "
    "the greatest speedup is achieved with a high height and low width of playing field!" << std::endl;
}

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

void exchange(int rank, int world_size, const Grid &grid,
    std::vector<cell_t> *upper_line,
    std::vector<cell_t> *bottom_line)
{
    if(world_size == 1)
        return;

    int width = grid.width;
    int next = (rank + 1) % world_size;
    int prev = (world_size + rank - 1) % world_size;

    if((world_size % 2 == 1) && (rank == (world_size - 1)))
    {
        TRY(MPI_Recv(upper_line->data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
        TRY(MPI_Send(grid.getUpperLine().data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
        TRY(MPI_Send(grid.getBottomLine().data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
        TRY(MPI_Recv(bottom_line->data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
    }
    else if(rank % 2 == 0)
    {
        TRY(MPI_Recv(bottom_line->data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
        TRY(MPI_Send(grid.getBottomLine().data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
        TRY(MPI_Recv(upper_line->data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
        TRY(MPI_Send(grid.getUpperLine().data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
    }
    else
    {
        TRY(MPI_Send(grid.getUpperLine().data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
        TRY(MPI_Recv(upper_line->data(), width, MPI_UINT8_T, prev, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
        TRY(MPI_Send(grid.getBottomLine().data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD), 
            "Can't send boundary line");
        TRY(MPI_Recv(bottom_line->data(), width, MPI_UINT8_T, next, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
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
    if(initGOL(argc, argv, settings, showExtendedHelp))
        return -1;

    int last_rank = world_size - 1;
    int width = settings.width;
    int common_slice_height = settings.height / world_size;
    int height = (rank == last_rank) ? (settings.height - last_rank * common_slice_height) : common_slice_height;
    int iterations_left = settings.iterations_limit;

    std::vector<cell_t> *upper_line;
    std::vector<cell_t> *bottom_line;
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
        exchange(rank, world_size, grids[grid_id], upper_line, bottom_line);

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
