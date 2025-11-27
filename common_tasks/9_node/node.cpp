#include <require.hpp>
#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

typedef double value_t;

// may expect for double size of A,B,C,F
typedef void (* lsolver_t)
(
    std::vector<value_t> &delta,
    std::vector<value_t> &A,
    std::vector<value_t> &B,
    std::vector<value_t> &C,
    std::vector<value_t> &F
);

struct params_t
{
    lsolver_t solver;
    int max_iterations;
    value_t epsilon;
    value_t mu;
};

void solve_tridiagonal
(
    std::vector<value_t> &delta,
    std::vector<value_t> &A,
    std::vector<value_t> &B,
    std::vector<value_t> &C,
    std::vector<value_t> &F
)
{
    int N = delta.size();
    std::vector<value_t> cp(N);
    std::vector<value_t> dp(N);

    cp[0] = C[0] / B[0];
    dp[0] = -F[0] / B[0];

    for(int i = 1; i < N; i++)
    {
        value_t denom = B[i] - A[i] * cp[i-1];
        cp[i] = (i == N-1) ? 0.0 : (C[i] / denom);
        dp[i] = -(F[i] + A[i] * dp[i-1]) / denom;
    }

    delta[N-1] = dp[N-1];
    for(int i = N-2; i >= 0; i--)
        delta[i] = dp[i] - cp[i] * delta[i+1];
}

void solve_CR_impl
(
    std::vector<value_t> &delta,
    std::vector<value_t>::iterator a,
    std::vector<value_t>::iterator b,
    std::vector<value_t>::iterator c,
    std::vector<value_t>::iterator f,
    int N,
    int shift
)
{
    int base = shift - 1;

    // a[0] & c[max] are ignored
    if (N == 1)
    {
        delta[base] = -f[0] / b[0];
        return;
    }

    int m = N / 2;

    #pragma omp parallel for
    for(int i = 0; i < m; i++)
    {
        int idx = 2 * i + 1;
        value_t alpha = (idx - 1 >= 0) ? a[idx] / b[idx - 1] : 0.0;
        value_t gamma = (idx + 1 < N) ? c[idx] / b[idx + 1] : 0.0;

        a[i + N] = -alpha * a[idx - 1];
        b[i + N] = b[idx] - alpha * c[idx - 1] - gamma * ((idx + 1 < N) ? a[idx + 1] : 0.0);
        c[i + N] = -gamma * ((idx + 1 < N) ? c[idx + 1] : 0.0);
        f[i + N] = f[idx] - alpha * f[idx - 1] - gamma * ((idx + 1 < N) ? f[idx + 1] : 0.0);
    }

    // solve for odd: x_{i-2} & x_{i} & x_{i+2}, ex: x1 & x3 & x5
    solve_CR_impl(delta, a + N, b + N, c + N, f + N, m, 2*shift);

    // solve for even: x_{i-1}, ex: x0 & x2 & x4
    #pragma omp parallel for
    for(int i = 0; i < m; i++)
    {
        int k = 2*i;
        value_t rhs = -f[k];
        if(i > 0)
            rhs -= a[k] * delta[shift*(k-1) + base];
        if(k + 1 < N)
            rhs -= c[k] * delta[shift*(k+1) + base];
        delta[shift*k + base] = rhs / b[k];
    }

    if(N % 2 == 1)
        delta[shift*(N-1) + base] = (-f[N-1] - a[N-1] * delta[shift*(N-2) + base]) / b[N-1];
}

void solve_CR
(
    std::vector<value_t> &delta,
    std::vector<value_t> &A,
    std::vector<value_t> &B,
    std::vector<value_t> &C,
    std::vector<value_t> &F
)
{
    size_t size = 2*delta.size();
    require(A.size() >= size, "A is too short");
    require(B.size() >= size, "B is too short");
    require(C.size() >= size, "C is too short");
    require(F.size() >= size, "F is too short");

    solve_CR_impl(delta, A.begin(), B.begin(), C.begin(), F.begin(), delta.size(), 1);
}

value_t f(value_t y)
{
    return y*y*y - y;
}

value_t F(int m, const std::vector<value_t> &y, value_t mu)
{
    return y[m+1] - 2*y[m] + y[m-1] - mu*(f(y[m+1]) + 10*f(y[m]) + f(y[m-1]));
}

value_t get_norm(const std::vector<value_t> &F_values)
{
    int N = F_values.size();
    value_t sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for(int m = 0; m < N; m++)
        sum += F_values[m] * F_values[m];

    return std::sqrt(sum);
}

int solve_NODE(std::vector<value_t> &grid, const params_t &params)
{
    int N = grid.size();
    std::vector<value_t> delta(N - 2);
    std::vector<value_t> A(2*delta.size());
    std::vector<value_t> B(2*delta.size());
    std::vector<value_t> C(2*delta.size());
    std::vector<value_t> F_values(2*delta.size());

    int i = 0;
    for(;i < params.max_iterations; i++)
    {
        #pragma omp parallel for
        for(int m = 1; m < N - 1; m++)
        {
            A[m - 1] = 1 - params.mu * (3*grid[m-1]*grid[m-1] - 1);
            B[m - 1] = -2 - 10*params.mu * (3*grid[m]*grid[m] - 1);
            C[m - 1] = 1 - params.mu * (3*grid[m+1]*grid[m+1] - 1);
            F_values[m - 1] = F(m, grid, params.mu);
        }

        if(get_norm(F_values) < params.epsilon)
            break;

        params.solver(delta, A, B, C, F_values);

        #pragma omp parallel for
        for(int m = 1; m < N - 1; m++)
            grid[m] += delta[m - 1];
    }

    return i;
}

int main(int argc, char **argv)
{
    params_t params;
    params.solver = solve_tridiagonal;
    params.epsilon = 1e-3;
    params.max_iterations = 1000;

    require(argc >= 3, "Usage: node [a] [h] {seq|CR[threads]|None}");
    double a = std::stod(argv[1]);
    double h = std::stod(argv[2]);
    params.mu = a*h*h / 12;

    if((argc == 4) && !strncmp(argv[3], "CR", 2))
    {
        int threads = std::stoi(argv[3] + 2);
        std::cerr << "solver: cyclic reduction with " << threads << " threads" << std::endl;
        omp_set_num_threads(threads);
        params.solver = solve_CR;
    }
    else
    {
        std::cerr << "solver: sequential" << std::endl;
        omp_set_num_threads(1);
        params.solver = solve_tridiagonal;
    }

    int N = static_cast<int>(20 / h);
    std::vector<value_t> grid(N, std::sqrt(2));
    double start = omp_get_wtime();
    int iterations = solve_NODE(grid, params);
    std::cerr << "iterations: " << iterations << "\n";
    std::cerr << "time: " << (omp_get_wtime() - start) << " s" << std::endl;

    for(auto value : grid)
        std::cout << value << " ";
    return 0;
}
