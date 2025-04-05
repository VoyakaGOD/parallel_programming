#include <mpi_try.hpp>
#include <vector>
#include <cmath>

#define SEP " "

// configuration ///////////////////////////////////////

const double T = 100; // 0 <= t <= T
const double X = 100; // 0 <= x <= X
const double tau = 0.025;
const double h = 0.1;
const double c = 1.35; // should be positive

double phi(double x)
{
    return std::sin(x/5);
}

double psi(double t)
{
    return 2*std::sin(t);
}

double f(double t, double x)
{
    if((x < 40) || (x > 60))
        return 0;
    return 5*std::sin(x);
}

// configuration ///////////////////////////////////////

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
    Grid u(K + 1);

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
    for(int m = 0; m <= M; m++)
        u[1][m] = solveByCornerScheme(u, 1, m);

    for(int k = 2; k <= K; k++)
    {
        for(int m = 1; m < M; m++)
            u[k][m] = solveByCrossScheme(u, k, m);
        u[k][M] = solveByCornerScheme(u, k, M);
    }

    std::cout << K << SEP << M << SEP << tau << SEP << h << SEP << c << std::endl;
    for(int k = 0; k <= K; k++)
    {
        std::cout << u[k][0];
        for(int m = 1; m <= M; m++)
            std::cout << " " << u[k][m];
        std::cout << std::endl;
    }
    
    return 0;
}
