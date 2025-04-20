#ifndef GRID
#define GRID

#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

class GridRenderer
{
protected:
    int generation;

public:
    virtual void render(const std::vector<std::vector<bool>> &content, int width, int height) = 0;
};

class Grid
{
private:
    std::vector<std::vector<bool>> content;
    const int width;
    const int height;

public:
    Grid(int width, int height);
    void setStateClamped(int x, int y, bool state);
    bool getNewState(int x, int y) const;
    bool getState(int x, int y) const;
    void setState(int x, int y, bool state);
    void render(GridRenderer *renderer) const;
};

class PipeGridRenderer : public GridRenderer
{
public:
    void render(const std::vector<std::vector<bool>> &content, int width, int height);
};

class ConsoleGridRenderer : public GridRenderer
{
private:
    int delay; // ms

private:
    void clearScreen() const;

public:
    ConsoleGridRenderer(int delay);
    void render(const std::vector<std::vector<bool>> &content, int width, int height);
};

class BenchmarkGridRenderer : public GridRenderer
{
private:
    decltype(std::chrono::high_resolution_clock::now()) initial_time;
    bool is_initialized;
    int delay; // shows statistics every <delay> generations

public:
    BenchmarkGridRenderer(int delay);
    void render(const std::vector<std::vector<bool>> &content, int width, int height);
};

#endif //GRID
