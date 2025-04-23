#ifndef MPI_GOL_UTILS
#define MPI_GOL_UTILS

#include <mpi_try.hpp>
#include <sstream>
#include "CLI.hpp"

void render(int rank, int world_size, const Grid &grid, GridRenderer *renderer, Statistics &statistics);

void exchange(int rank, int world_size, const Grid &grid, bool opt,
    std::vector<cell_t> *upper_line,
    std::vector<cell_t> *bottom_line);

#endif //MPI_GOL_UTILS
