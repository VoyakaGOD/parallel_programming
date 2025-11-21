#include <require.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <omp.h>
#include <cmath>

typedef double value_t;

struct params_t
{
    value_t border_value;
    value_t epsilon;
    value_t mu;
};

void solve_CR(std::vector<value_t> &delta, std::vector<value_t> &grid)
{

}

value_t f(value_t y)
{
    return y*y*y - y;
}

value_t F(int m, const std::vector<value_t> &y, value_t mu)
{
    return y[m+1] - 2*y[m] + y[m-1] - mu*(f(y[m+1]) + 10*f(y[m]) + f(y[m-1]));
}

value_t get_F_norm(const std::vector<value_t> &grid, value_t mu)
{
    int N = grid.size();
    value_t sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for(int m = 1; m < N - 1; m++)
    {
        value_t val = F(m, grid, mu);
        sum += val * val;
    }

    return std::sqrt(sum);
}

void solve_NODE(std::vector<value_t> &grid, const params_t &params)
{
    std::vector<value_t> delta(grid.size());

    while(get_F_norm(grid, params.mu) > params.epsilon)
        solve_CR(delta, grid);
}

int main(int argc, char **argv)
{
    params_t params;
    params.border_value = std::sqrt(2);
    params.epsilon = 1e-3;

    require(argc == 3, "Usage: node [a] [h]");
    double a = std::stod(argv[1]);
    double h = std::stod(argv[2]);
    params.mu = a*h*h / 12;

    int N = static_cast<int>(20 / h);
    std::vector<value_t> grid(N, params.border_value);
    solve_NODE(grid, params);

    for(auto value : grid)
        std::cout << value << " ";
    return 0;
}
