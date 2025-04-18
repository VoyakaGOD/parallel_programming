#include <mpi_try.hpp>
#include "config.hpp"
#include <fstream>
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

    bool measurement = (argc > 1) && (argv[1] == std::string("-m"));

    Grid u(K + 1);

    double start = MPI_Wtime();

    // initial conditions
    u[0].resize(M + 1);
    for(int m = 0; m <= M; m++)
        u[0][m] = phi(m * h);

    // boundary conditions
    for(int k = 1; k <= K; k++)
    {
        u[k].resize(M + 1);
        u[k][0] = psi(k * tau);
    }

    // first layer by corner scheme
    for(int m = 1; m <= M; m++)
        u[1][m] = solveByCornerScheme(u, 1, m);

    for(int k = 2; k <= K; k++)
    {
        for(int m = 1; m < M; m++)
            u[k][m] = solveByCrossScheme(u, k, m);
        u[k][M] = solveByCornerScheme(u, k, M);
    }

    double solving_time = MPI_Wtime() - start;

    if(!measurement)
    {
        std::ofstream file(OUTPUT_FILE, std::ios::binary);
        file.write(reinterpret_cast<const char *>(&K), sizeof(int));
        file.write(reinterpret_cast<const char *>(&M), sizeof(int));
        file.write(reinterpret_cast<const char *>(&tau), sizeof(double));
        file.write(reinterpret_cast<const char *>(&h), sizeof(double));
        file.write(reinterpret_cast<const char *>(&c), sizeof(double));
        for(int k = 0; k <= K; k++)
        {
            for(int m = 0; m <= M; m++)
                file.write(reinterpret_cast<const char *>(&u[k][m]), sizeof(double));
        }
    }

    double total_time = MPI_Wtime() - start;
    std::cout << "K = " << K << std::endl;
    std::cout << "M = " << M << std::endl;
    std::cout << "c = " << c << std::endl;
    std::cout << "sequential" << std::endl;
    std::cout << "solving time: " << solving_time << " s" << std::endl;
    std::cout << "writing time: " << (total_time - solving_time) << " s" << std::endl;
    std::cout << "total   time: " << total_time << " s" << std::endl;

    TRY(MPI_Finalize(), "Bad MPI finalization");

    return 0;
}
