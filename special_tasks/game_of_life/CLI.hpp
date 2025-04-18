#ifndef GOL_CLI
#define GOL_CLI

#include "grid.hpp"

void initGOL(int argc, char** argv, int &iterations_limit, GridRenderer* &renderer, Grid &initial_state);
void show_help(char *name);

#endif //GOL_CLI
