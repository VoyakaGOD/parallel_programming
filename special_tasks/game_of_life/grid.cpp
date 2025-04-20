#include "grid.hpp"

Grid::Grid(int width, int height, int full_width, int full_height, int x_offset, int y_offset) :
    width(width),
    height(height),
    full_width(std::max(width, full_width)),
    full_height(std::max(height, full_height)),
    x_offset(x_offset),
    y_offset(y_offset)
{
    content.resize(height);
    for(int line = 0; line < height; line++)
        content[line].resize(width);
}

void Grid::setStateToroidal(int x, int y, bool state)
{
    x = (x % full_width + full_width) % full_width;
    y = (y % full_height + full_height) % full_height;

    if(width < full_width)
    {
        x -= x_offset;
        if((x < 0) || (x >= width))
            return;
    }

    if(height < full_height)
    {
        y -= y_offset;
        if((y < 0) || (y >= height))
            return;
    }

    content[y][x] = state;
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
    renderer->render(content, *this);
}

void PipeGridRenderer::render(const std::vector<std::vector<bool>> &content, const Grid &grid)
{
    int center = grid.full_height / 2 - grid.y_offset;
    for(int y = 0; y < grid.height; y++)
    {
        std::cout << ((y == center) ? "@" : "|");
        for(int x = 0; x < grid.width; x++)
            std::cout << (content[y][x] ? "#" : "_");
        std::cout << '\n';
    }
    std::cout.flush();
}

ConsoleGridRenderer::ConsoleGridRenderer(int delay) : delay(delay) {}

void ConsoleGridRenderer::clearScreen() const
{
    std::cout << "\x1B[2J" << "\x1B[H";
}

void ConsoleGridRenderer::render(const std::vector<std::vector<bool>> &content, const Grid &grid)
{
    if(grid.y_offset == 0) // first renderer in chain
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        clearScreen();
    }

    PipeGridRenderer::render(content, grid);
}
