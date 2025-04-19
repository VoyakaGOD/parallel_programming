// sint/ptint configuration
// no include guards!

#include <cmath>

double epsilon = 1e-1;
double from = 1e-4;
double to = 1;

double f(double x)
{
    return std::sin(1/x/x)/x/x;
}
