#include <require.hpp>
#include <pthread.h>
#include <chrono>

struct Task
{
    int from;
    int to;
    double result;
};

void *calculatePart(void *arg)
{
    Task *task = static_cast<Task*>(arg);
    int from = task->from;
    int to = task->to;
    // from [i = 1] to [i = N]
    from++;
    to++;

    double sum = 0;
    for(int i = from; i < to; i++)
        sum += 1.0f / i;
    
    task->result = sum;
    return nullptr;
}

int main(int argc, char** argv)
{
    require(argc >= 2, "You should enter number of threads");
    char *end;
    int threads_count = std::strtol(argv[1], &end, 10);
    require(*end == '\0', "Number of threads should be integer");

    require(argc >= 3, "You should enter N[number of elements in sum]");
    int N = std::strtol(argv[2], &end, 10);
    require(*end == '\0', "N should be integer");

    pthread_t *threads = new pthread_t[threads_count];
    require(threads != nullptr, "Can't allocate memory for threads");
    Task *tasks = new Task[threads_count];
    require(tasks != nullptr, "Can't allocate memory for thread tasks");

    auto start = std::chrono::high_resolution_clock::now();

    int partition_size = N / threads_count;
    int last_rank = threads_count - 1;

    for(int i = 0; i < last_rank; i++)
    {
        tasks[i].from = i * partition_size;
        tasks[i].to = (i + 1) * partition_size;
        pthread_create(&threads[i], nullptr, calculatePart, &tasks[i]);
    }

    tasks[last_rank].from = last_rank * partition_size;
    tasks[last_rank].to = N;
    calculatePart(&tasks[last_rank]);

    for(int i = 0; i < last_rank; i++)
    {
        int code = pthread_join(threads[i], nullptr);
        require(code == 0, "Can't join thread");
    }

    double sum = 0;
    for(int i = 0; i < threads_count; i++)
        sum += tasks[i].result;

    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;

    std::cout << "Total sum(from 1 to 1/" << N << "): " << sum << std::endl;
    std::cout << "Threads: " << threads_count << ", time: " << elapsed.count() << " s" << std::endl;
    
    return 0;
}
