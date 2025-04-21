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

void Grid::setStateToroidal(int x, int y, cell_t state)
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

cell_t Grid::getNewState(int x, int y,
    const std::vector<uint8_t> *upper_line,
    const std::vector<uint8_t> *bottom_line) const
{
    int neighbours = 0;

    if(upper_line && (y == 0))
    {
        neighbours += (*upper_line)[x - 1];
        neighbours += (*upper_line)[x];
        neighbours += (*upper_line)[x + 1];
    }
    else
    {
        neighbours += getStateSafe(x - 1, y - 1);
        neighbours += getStateSafe(x, y - 1);
        neighbours += getStateSafe(x + 1, y - 1);
    }

    neighbours += getStateSafe(x - 1, y);
    neighbours += getStateSafe(x + 1, y);

    if(bottom_line && (y == (height - 1)))
    {
        neighbours += (*bottom_line)[x - 1];
        neighbours += (*bottom_line)[x];
        neighbours += (*bottom_line)[x + 1];
    }
    else
    {
        neighbours += getStateSafe(x - 1, y + 1);
        neighbours += getStateSafe(x, y + 1);
        neighbours += getStateSafe(x + 1, y + 1);
    }

    if(neighbours == 3)
        return true;

    return content[y][x] && (neighbours == 2);
}

cell_t Grid::getStateSafe(int x, int y) const
{
    return content[(height + y) % height][(width + x) % width];
}

void Grid::setState(int x, int y, cell_t state)
{
    content[y][x] = state;
}

void Grid::render(GridRenderer *renderer) const
{
    renderer->render(content, *this);
}

const std::vector<cell_t> &Grid::getUpperLine() const
{
    return content[0];
}

const std::vector<cell_t> &Grid::getBottomLine() const
{
    return content[height - 1];
}

std::ostream *GridRenderer::getOutput()
{
    return output;
}
    
void GridRenderer::setOutput(std::ostream *new_output)
{
    output = new_output;
}

void PipeGridRenderer::render(const std::vector<std::vector<cell_t>> &content, const Grid &grid)
{
    int center = grid.full_height / 2 - grid.y_offset;
    for(int y = 0; y < grid.height; y++)
    {
        *output << ((y == center) ? "@" : "|");
        for(int x = 0; x < grid.width; x++)
            *output << (content[y][x] ? "#" : "_");
        *output << '\n';
    }
    output->flush();
}

ConsoleGridRenderer::ConsoleGridRenderer(int delay) : delay(delay) {}

void ConsoleGridRenderer::clearScreen() const
{
    *output << "\x1B[2J" << "\x1B[H";
}

void ConsoleGridRenderer::render(const std::vector<std::vector<cell_t>> &content, const Grid &grid)
{
    if(grid.y_offset == 0) // first renderer in chain
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay));
        clearScreen();
    }

    PipeGridRenderer::render(content, grid);
}
