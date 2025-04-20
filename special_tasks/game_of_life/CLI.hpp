#ifndef GOL_CLI
#define GOL_CLI

#include "patterns_lib.hpp"
#include <unordered_map>
#include <limits>
#include <string>
#include <chrono>

#define DEFAULT_ITERATIONS_LIMIT std::numeric_limits<int>::max()
#define DEFAULT_STATISTICS_DELAY 1000

class Statistics
{
private:
    decltype(std::chrono::high_resolution_clock::now()) initial_time;
    int generation;
    int delay; // shows statistics every <delay> generations

public:
    Statistics(int delay);
    void reportAboutNewGeneration(bool new_line);
};

struct CLISettings
{
    int width;
    int height;
    int iterations_limit;
    GridRenderer* renderer = nullptr;
    std::string initial_state_string;
    int statistics_delay;
};

int readInt(const char *str, int &value);
void showHelp(char *name);
int initGOL(int argc, char** argv, CLISettings &settings);
int addPatterns(Grid &grid, std::string patterns);

#endif //GOL_CLI
