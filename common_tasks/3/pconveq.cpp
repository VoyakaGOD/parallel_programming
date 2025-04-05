#include <mpi_try.hpp>
#include "config.hpp"
#include <vector>

typedef std::vector<std::vector<double>> Grid; //u[t][x]

double solveByCornerScheme(const Grid &u, int k, int m)
{
    k--;
    return u[k][m] + tau*(f(k*tau, m*h) - c * (u[k][m] - u[k][m-1])/h);
}

double solveByCrossScheme(const Grid &u, int k, int m)
{
    k--;
    return u[k-1][m] + 2*tau*(f(k*tau, m*h) - c * (u[k][m+1] - u[k][m-1])/(2*h));
}

const int K = static_cast<int>(T / tau);
const int M = static_cast<int>(X / h);

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    int buffer_size = sizeof(double) * 4 + MPI_BSEND_OVERHEAD;
    TRY(MPI_Buffer_attach(new char[buffer_size], buffer_size), "Can't attach buffer for Bsend");

    Grid u(K + 1);
    int partition_size = M / world_size;
    int last = world_size - 1;
    int min_m = (rank == 0) ? 1 : (rank * partition_size);
    int max_m = (rank == last) ? (M + 1) : ((rank + 1) * partition_size);

    // initial conditions
    u[0].resize(M + 1);
    for(int m = min_m - 1; m < max_m; m++)
        u[0][m] = phi(m * h);

    for(int k = 1; k <= K; k++)
    {
        u[k].resize(M + 1);

        // boundary conditions on left side
        if(rank == 0)
            u[k][0] = psi(k * tau);
    }

    // first layer by corner scheme
    for(int m = min_m; m < max_m; m++)
        u[1][m] = solveByCornerScheme(u, 1, m);

    if(rank > 0)
    {
        TRY(MPI_Bsend(&u[1][min_m-1], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD),
            "Can't send information to left process");
    }
    if(rank < last)
    {
        TRY(MPI_Bsend(&u[1][max_m], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD),
            "Can't send information to right process");
    }

    MPI_Request left_request;
    MPI_Request right_request;
    for(int k = 2; true; k++)
    {
        if(rank > 0)
        {
            TRY(MPI_Irecv(&u[k-1][min_m-1], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD, &left_request),
                "Can't receive information from left process");
        }
        if(rank < last)
        {
            TRY(MPI_Irecv(&u[k-1][max_m], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD, &right_request),
                "Can't receive information from right process");
        }

        for(int m = min_m + 1; m < max_m - 1; m++)
            u[k][m] = solveByCrossScheme(u, k, m);

        if(rank > 0)
            TRY(MPI_Wait(&left_request, MPI_STATUS_IGNORE), "Can't receive information from left process");
        if(rank < last)
            TRY(MPI_Wait(&right_request, MPI_STATUS_IGNORE), "Can't receive information from right process");

        u[k][min_m] = solveByCrossScheme(u, k, min_m);
        if(rank == last)
            u[k][M] = solveByCornerScheme(u, k, M);
        else
            u[k][max_m-1] = solveByCrossScheme(u, k, max_m-1);

        if(k == K)
            break;

        if(rank > 0)
        {
            TRY(MPI_Bsend(&u[k-1][min_m-1], 1, MPI_DOUBLE, rank-1, 0, MPI_COMM_WORLD),
                "Can't send information to left process");
        }
        if(rank < last)
        {
            TRY(MPI_Bsend(&u[k-1][max_m], 1, MPI_DOUBLE, rank+1, 0, MPI_COMM_WORLD),
                "Can't send information to right process");
        }
    }

    MPI_File file;
    TRY(MPI_File_open(MPI_COMM_WORLD, OUTPUT_FILE, MPI_MODE_CREATE | MPI_MODE_WRONLY, 
        MPI_INFO_NULL, &file), "Can't open file");
    TRY(MPI_File_set_size(file, 0), "Can't clear file");

    if(rank == 0)
    {
        TRY(MPI_File_write(file, &K, 1, MPI_INT, MPI_STATUS_IGNORE), "Can't write to the output file");
        TRY(MPI_File_write(file, &M, 1, MPI_INT, MPI_STATUS_IGNORE), "Can't write to the output file");
        TRY(MPI_File_write(file, &tau, 1, MPI_DOUBLE, MPI_STATUS_IGNORE), "Can't write to the output file");
        TRY(MPI_File_write(file, &h, 1, MPI_DOUBLE, MPI_STATUS_IGNORE), "Can't write to the output file");
        TRY(MPI_File_write(file, &c, 1, MPI_DOUBLE, MPI_STATUS_IGNORE), "Can't write to the output file");
        min_m = 0;
    }

    int header_size = 2*sizeof(int) + 3*sizeof(double);
    for(int k = 0; k <= K; k++)
    {
        int offset = header_size + (k*(M+1) + min_m) * sizeof(double);
        TRY(MPI_File_write_at(file, offset, u[k].data() + min_m, max_m - min_m, MPI_DOUBLE, MPI_STATUS_IGNORE),
            "Can't save solution to the output file");
    }

    TRY(MPI_Finalize(), "Bad MPI finalization");

    return 0;
}
