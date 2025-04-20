#ifndef GOL_CLI
#define GOL_CLI

#include "grid.hpp"
#include <limits>
#include <string>

#define DEFAULT_ITERATIONS_LIMIT std::numeric_limits<int>::max()

struct CLISettings
{
    int width;
    int height;
    int iterations_limit;
    GridRenderer* renderer;
    std::string initial_state_string;
};

int readInt(const char *str, int &value);
void showHelp(char *name);
int initGOL(int argc, char** argv, CLISettings &settings);

#endif //GOL_CLI
