#include "CLI.hpp"

#define REPORT(msg) do { std::cerr << msg << std::endl; return -1; } while(false)

int readInt(const char *str, int &value)
{
    char *end;
    value = std::strtol(str, &end, 10);
    return static_cast<int>(*end);
}

void showHelp(char *name)
{
    std::cout << "GOL usage:\n";
    std::cout << name << " <width> <height> " << "[flags]\n";
    std::cout << "    i<limit>  -- sets the limit of iterations\n";
    std::cout << "    r<name>   -- sets the renderer\n";
    std::cout << "        pipe           -- useful to save all generations to the file\n";
    std::cout << "        cmd<delay>     -- displays generations with the specified delay using ansi sequences\n";
    std::cout << "        measure<delay  -- shows statistics every <delay> generations\n";
    std::cout << "    p<...> -- change initial state with ...\n";
}

int initGOL(int argc, char** argv, CLISettings &settings)
{
    if(argc < 3)
    {
        showHelp(argv[0]);
        return -1;
    }

    if(readInt(argv[1], settings.width))
        REPORT("Width should be integer");

    if(settings.width <= 0)
        REPORT("Width should be positive value");

    if(readInt(argv[2], settings.height))
        REPORT("Height should be integer");

    if(settings.height <= 0)
        REPORT("Height should be positive value");

    settings.iterations_limit = DEFAULT_ITERATIONS_LIMIT;

    for(int i = 3; i < argc; i++)
    {
        const char *flag = argv[i];
        std::string name;

        switch (flag[0])
        {
        case 'i':
            if(readInt(flag + 1, settings.iterations_limit))
                REPORT("Iterations limit should be integer");
            if(settings.iterations_limit <= 0)
                REPORT("Iterations limit should be positive value");
            break;
        case 'r':
            if(settings.renderer != nullptr)
                delete settings.renderer;
            name = flag + 1;
            if(name == "pipe")
            {
                settings.renderer = new PipeGridRenderer();
            }
            else if(name.substr(0, 3) == "cmd")
            {
                int delay;
                if(readInt(flag + 4, delay))
                    REPORT("Delay should be integer");
                if(delay <= 0)
                    REPORT("Delay should be positive value");
                settings.renderer = new ConsoleGridRenderer(delay);
            }
            else if(name.substr(0, 7) == "measure")
            {
                int delay;
                if(readInt(flag + 8, delay))
                    REPORT("Delay should be integer");
                if(delay <= 0)
                    REPORT("Delay should be positive value");
                settings.renderer = new BenchmarkGridRenderer(delay);
            }
            else
            {
                REPORT("Unknown renderer");
            }
            break;
        case 'p':
            settings.initial_state_string = std::string(flag + 1);
            break;
        default:
            REPORT("Unknown flag");
            break;
        }
    }

    if(settings.renderer == nullptr)
        settings.renderer = new BenchmarkGridRenderer(1000);

    return 0;
}
