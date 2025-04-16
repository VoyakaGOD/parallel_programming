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

void Grid::render(const GridRenderer &renderer) const
{
    renderer.render(content, width, height);
}

ConsoleGridRenderer::ConsoleGridRenderer()
{

}

void ConsoleGridRenderer::render(const std::vector<std::vector<bool>> &content, int width, int height) const
{
    for(int y = 0; y < height; y++)
    {
        std::cout << ((y == height / 2) ? "@" : "|");
        for(int x = 0; x < width; x++)
        {
            std::cout << (content[y][x] ? "#" : "_");
        }
        std::cout << std::endl;
    }
    std::cout << "\n\n";
}
