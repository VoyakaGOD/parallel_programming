#ifndef GRID
#define GRID

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

class GridRenderer
{
public:
    virtual void render(const std::vector<std::vector<bool>> &content, int width, int height) const = 0;
};

class Grid
{
private:
    std::vector<std::vector<bool>> content;
    const int width;
    const int height;

public:
    Grid(int width, int height);
    bool getNewState(int x, int y) const;
    bool getState(int x, int y) const;
    void setState(int x, int y, bool state);
    void render(const GridRenderer &renderer) const;
};

class PipeGridRenderer : public GridRenderer
{
public:
    void render(const std::vector<std::vector<bool>> &content, int width, int height) const;
};

class ConsoleGridRenderer : public GridRenderer
{
private:
    int delay; // ms

private:
    void hideCursor() const;
    void showCursor() const;
    void clearScreen() const;

public:
    ConsoleGridRenderer(int delay);
    void render(const std::vector<std::vector<bool>> &content, int width, int height) const;
};

#endif //GRID
