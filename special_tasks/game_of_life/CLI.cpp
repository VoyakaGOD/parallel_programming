#include "CLI.hpp"

void show_help(char *name)
{
    std::cout << "GOL usage:\n";
    std::cout << name << " <width> <height> " << "[flags]\n";
    std::cout << "    i<limit>  -- sets the limit of iterations\n";
    std::cout << "    r<name>   -- sets the renderer\n";
    std::cout << "        pipe        -- useful to save all generations to the file\n";
    std::cout << "        cmd<delay>  -- displays generations with the specified delay using ansi sequences\n";
    std::cout << "        measure     -- shows only time of all iterations\n";
    std::cout << "    p<...> -- change initial state with ...\n";
}
