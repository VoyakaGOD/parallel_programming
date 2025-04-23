#include "mpi_utils.hpp"

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

void sendLine(int to, const std::vector<cell_t> *line, int &delay, grid_dist_func_t getDistance, 
    const Grid &grid, bool opt)
{
    if(!opt)
    {
        TRY(MPI_Send(line->data(), line->size(), MPI_UINT8_T, to, 0, MPI_COMM_WORLD),
            "Can't send boundary line");
        return;
    }

    delay--;
    if(delay > 0)
        return;

    delay = (grid.*getDistance)();

    TRY(MPI_Send(&delay, 1, MPI_INT, to, 0, MPI_COMM_WORLD), "Can't send BBMsg");
    if(delay == 0)
    {
        TRY(MPI_Send(line->data(), line->size(), MPI_UINT8_T, to, 0, MPI_COMM_WORLD),
            "Can't send boundary line");
    }
}

void recvLine(int from, std::vector<cell_t> *line, int &delay, bool opt)
{
    if(!opt)
    {
        TRY(MPI_Recv(line->data(), line->size(), MPI_UINT8_T, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
        return;
    }

    delay--;
    if(delay > 0)
        return;

    TRY(MPI_Recv(&delay, 1, MPI_INT, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), "Can't send BBMsg");
    if(delay == 0)
    {
        TRY(MPI_Recv(line->data(), line->size(), MPI_UINT8_T, from, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive boundary line");
    }
    else
    {
        std::fill(line->begin(), line->end(), false);
    }
}

void exchange(int rank, int world_size, const Grid &grid, bool opt,
    std::vector<cell_t> *upper_line,
    std::vector<cell_t> *bottom_line)
{
    static int upper_send_delay = 0;
    static int upper_recv_delay = 0;
    static int bottom_send_delay = 0;
    static int bottom_recv_delay = 0;

    if(world_size == 1)
        return;

    int width = grid.width;
    int next = (rank + 1) % world_size;
    int prev = (world_size + rank - 1) % world_size;

    if((world_size % 2 == 1) && (rank == (world_size - 1)))
    {
        recvLine(prev, upper_line, upper_recv_delay, opt);
        sendLine(prev, &grid.getUpperLine(), upper_send_delay, &Grid::getUpperDistance, grid, opt);
        sendLine(next, &grid.getBottomLine(), bottom_send_delay, &Grid::getBottomDistance, grid, opt);
        recvLine(next, bottom_line, bottom_recv_delay, opt);
    }
    else if(rank % 2 == 0)
    {
        recvLine(next, bottom_line, bottom_recv_delay, opt);
        sendLine(next, &grid.getBottomLine(), bottom_send_delay, &Grid::getBottomDistance, grid, opt);
        recvLine(prev, upper_line, upper_recv_delay, opt);
        sendLine(prev, &grid.getUpperLine(), upper_send_delay, &Grid::getUpperDistance, grid, opt);
    }
    else
    {
        sendLine(prev, &grid.getUpperLine(), upper_send_delay, &Grid::getUpperDistance, grid, opt);
        recvLine(prev, upper_line, upper_recv_delay, opt);
        sendLine(next, &grid.getBottomLine(), bottom_send_delay, &Grid::getBottomDistance, grid, opt);
        recvLine(next, bottom_line, bottom_recv_delay, opt);
    }

    if(opt)
    {
        if(upper_recv_delay == 0)
            upper_send_delay = 0;
        if(bottom_recv_delay == 0)
            bottom_send_delay = 0;
        if(upper_send_delay == 0)
            upper_recv_delay = 0;
        if(bottom_send_delay == 0)
            bottom_recv_delay = 0;
    }
}
