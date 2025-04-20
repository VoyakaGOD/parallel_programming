#include "grid.hpp"

Grid::Grid(int width, int height) : width(width), height(height)
{
    content.resize(height);
    for(int line = 0; line < height; line++)
        content[line].resize(width);
}

bool Grid::getNewState(int x, int y) const
{
    int neighbours = getState(x - 1, y + 1);
    neighbours += getState(x - 1, y);
    neighbours += getState(x - 1, y - 1);
    neighbours += getState(x, y + 1);
    neighbours += getState(x, y - 1);
    neighbours += getState(x + 1, y + 1);
    neighbours += getState(x + 1, y);
    neighbours += getState(x + 1, y - 1);

    if(neighbours == 3)
        return true;

    return content[y][x] && (neighbours == 2);
}

bool Grid::getState(int x, int y) const
{
    return content[(height + y) % height][(width + x) % width];
}

void Grid::setState(int x, int y, bool state)
{
    content[(height + y) % height][(width + x) % width] = state;
}

void Grid::render(GridRenderer *renderer) const
{
    renderer->render(content, width, height);
}

void PipeGridRenderer::render(const std::vector<std::vector<bool>> &content, int width, int height)
{
    for(int y = 0; y < height; y++)
    {
        std::cout << ((y == height / 2) ? "@" : "|");
        for(int x = 0; x < width; x++)
        {
            std::cout << (content[y][x] ? "#" : "_");
        }
        std::cout << '\n';
    }
    std::cout << "Gen: " << generation;
    std::cout << "\n\n";
    generation++;
}

ConsoleGridRenderer::ConsoleGridRenderer(int delay) : delay(delay) {}

void ConsoleGridRenderer::clearScreen() const
{
    std::cout << "\x1B[2J" << "\x1B[H";
}

void ConsoleGridRenderer::render(const std::vector<std::vector<bool>> &content, int width, int height)
{
    clearScreen();

    for(int y = 0; y < height; y++)
    {
        std::cout << ((y == height / 2) ? "@" : "|");
        for(int x = 0; x < width; x++)
        {
            std::cout << (content[y][x] ? "#" : "_");
        }
        std::cout << "\n";
    }

    std::cout << "Gen: " << generation;
    std::cout.flush();
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
    generation++;
}

BenchmarkGridRenderer::BenchmarkGridRenderer(int delay) : delay(delay) {}

void BenchmarkGridRenderer::render(const std::vector<std::vector<bool>> &content, int width, int height)
{
    if(!is_initialized)
    {
        is_initialized = true;
        initial_time = std::chrono::high_resolution_clock::now();
    }

    if((generation % delay) == 0)
    {
        std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - initial_time;
        std::cout << "Gen: " << generation << ", time: " << elapsed.count() << "s       \r";
        std::cout.flush();
    }

    generation++;
}
