#include "mpi_utils.hpp"
#include "pthread.h"

#define THREADS_COUNT 8

void showExtendedHelp(char *name)
{
    showHelp(name);
    std::cout << "    O                          -- enables MPI send/recv optimization" << std::endl;
    std::cout << "WARNING: it starts always with " << THREADS_COUNT << " threads!" << std::endl;
}

struct TaskSetup
{
    int rank;
    int world_size;
    bool opt;
    int *active_workers_count;
    Grid *grids;
    pthread_mutex_t *barriers;
    pthread_mutex_t *count_mutex;
    std::vector<cell_t> *upper_line;
    std::vector<cell_t> *bottom_line;
    Statistics *statistics;
    GridRenderer *renderer;
};

struct WorkerSetup
{
    int id;
    int iterations_left;
    TaskSetup *shared;
};

void *updateRegion(void *arg)
{
    WorkerSetup task = *static_cast<WorkerSetup *>(arg);
    TaskSetup *shared = task.shared;
    int grid_id = 0;
    int width = shared->grids->width;
    int subslice_height = shared->grids->height / THREADS_COUNT;
    int from = task.id * subslice_height;
    int to = (task.id == (THREADS_COUNT - 1)) ? shared->grids->height : (from + subslice_height);

    while(task.iterations_left > 0)
    {
        pthread_mutex_lock(shared->barriers + task.id);
        for(int j = from; j < to; j++)
        {
            for(int i = 0; i < width; i++)
            {
                bool new_state = shared->grids[grid_id].getNewState(i, j, 
                    shared->upper_line, shared->bottom_line);
                shared->grids[1 - grid_id].setState(i, j, new_state);
            }
        }

        grid_id = 1 - grid_id;
        task.iterations_left--;

        pthread_mutex_lock(shared->count_mutex);
        (*shared->active_workers_count)--;
        if(*shared->active_workers_count == 0)
        {
            if(task.iterations_left > 0)
            {
                exchange(shared->rank, shared->world_size, shared->grids[grid_id], 
                    shared->opt, shared->upper_line, shared->bottom_line);
            }
            render(shared->rank, shared->world_size, shared->grids[grid_id], shared->renderer, *shared->statistics);
            for(int i = 0; i < THREADS_COUNT; i++)
                pthread_mutex_unlock(shared->barriers + i);
            *shared->active_workers_count = THREADS_COUNT;
        }
        pthread_mutex_unlock(shared->count_mutex);
    }

    return nullptr;
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    CLISettings settings;
    if(initGOL(argc, argv, settings, showExtendedHelp))
        return -1;

    int last_rank = world_size - 1;
    int width = settings.width;
    int common_slice_height = settings.height / world_size;
    int height = (rank == last_rank) ? (settings.height - last_rank * common_slice_height) : common_slice_height;

    std::vector<cell_t> *upper_line = nullptr;
    std::vector<cell_t> *bottom_line = nullptr;
    if(world_size > 1)
    {
        upper_line = new std::vector<cell_t>();
        upper_line->resize(width);
        bottom_line = new std::vector<cell_t>();
        bottom_line->resize(width);
    }

    Grid grids[] = {
        Grid(width, height, width, settings.height, 0, rank * common_slice_height),
        Grid(width, height, width, settings.height, 0, rank * common_slice_height)
    };
    GridRenderer *renderer = settings.renderer;
    if((rank > 0) && (renderer != nullptr))
        renderer->setOutput(new std::stringstream());
    int grid_id = 0;

    if(addPatterns(grids[0], settings.initial_state_string))
        return -1;

    Statistics statistics(settings.statistics_delay);

    render(rank, world_size, grids[0], renderer, statistics); // show initial state
    exchange(rank, world_size, grids[grid_id], settings.opt, upper_line, bottom_line);

    TaskSetup task;
    task.rank = rank;
    task.world_size = world_size;
    task.opt = settings.opt;
    int active_workers_count = THREADS_COUNT;
    task.active_workers_count = &active_workers_count;
    task.grids = grids;
    pthread_mutex_t barriers[THREADS_COUNT];
    task.barriers = barriers;
    pthread_mutex_t count_mutex;
    pthread_mutex_init(&count_mutex, nullptr);
    task.count_mutex = &count_mutex;
    task.upper_line = upper_line;
    task.bottom_line = bottom_line;
    task.statistics = &statistics;
    task.renderer = settings.renderer;

    pthread_t threads[THREADS_COUNT] = {};
    WorkerSetup workerTasks[THREADS_COUNT] = {};
    for(int i = 0; i < THREADS_COUNT; i++)
    {
        workerTasks[i].id = i;
        workerTasks[i].iterations_left = settings.iterations_limit;
        workerTasks[i].shared = &task;
        pthread_mutex_init(&barriers[i], nullptr);
    }
    for(int i = 1; i < THREADS_COUNT; i++)
        TRY(pthread_create(threads + i, nullptr, updateRegion, workerTasks + i), "Can't create thread");
    updateRegion(workerTasks);

    for(int i = 1; i < THREADS_COUNT; i++)
        TRY(pthread_join(threads[i], nullptr), "Can't join thread");

    if((rank == 0) && (renderer == nullptr) && (settings.iterations_limit % settings.statistics_delay == 0))
        std::cout << std::endl;

    TRY(MPI_Finalize(), "Bad MPI finalization");
        return 0;
}
