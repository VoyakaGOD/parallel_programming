#ifndef GRID
#define GRID

#include <vector>
#include <iostream>
#include <thread>

class Grid;

class GridRenderer
{
protected:
    std::ostream *output = &std::cout;

public:
    GridRenderer() = default;
    std::ostream *getOutput();
    void setOutput(std::ostream *new_output);
    virtual void render(const std::vector<std::vector<bool>> &content, const Grid &grid) = 0;
};

class Grid
{
private:
    std::vector<std::vector<bool>> content;

public:
    const int width;
    const int height;
    const int full_width;
    const int full_height;
    const int x_offset;
    const int y_offset;

public:
    Grid(int width, int height, int full_width = 0, int full_height = 0, int x_offset = 0, int y_offset = 0);
    void setStateToroidal(int x, int y, bool state);
    bool getNewState(int x, int y) const;
    bool getState(int x, int y) const;
    void setState(int x, int y, bool state);
    void render(GridRenderer *renderer) const;
};

class PipeGridRenderer : public GridRenderer
{
public:
    void render(const std::vector<std::vector<bool>> &content, const Grid &grid);
};

class ConsoleGridRenderer : public PipeGridRenderer
{
private:
    int delay; // ms

private:
    void clearScreen() const;

public:
    ConsoleGridRenderer(int delay);
    void render(const std::vector<std::vector<bool>> &content, const Grid &grid);
};

#endif //GRID
