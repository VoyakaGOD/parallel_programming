// sint/ptint configuration
// no include guards!

#include <cmath>

double epsilon = 1e-7;
double from = -2.99;
double to = 5;

double f(double x)
{
    return std::sin(1/(x + 3));
}
