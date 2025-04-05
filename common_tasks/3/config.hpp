// conveq/pconveq configuration
// no include guards!

#include <cmath>

#define OUTPUT_FILE "./results/conveq.bin"

const double T = 100; // 0 <= t <= T
const double X = 100; // 0 <= x <= X
const double tau = 0.007;
const double h = 0.05;
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
    int s = static_cast<int>(x) % 100;
    if((s < 40) || (s > 60))
        return 0;
    return 5*std::sin(x);
}
