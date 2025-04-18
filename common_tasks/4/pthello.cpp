#include <pthread.h>
#include <iostream>
#include <sstream>

void require(bool condition, const char *message)
{
    if(condition)
        return;

    std::cerr << message << std::endl;
    exit(0);
}

void *sayHello(void *arg)
{
    pthread_t id = pthread_self();
    int rank = *(int*)arg;

    std::ostringstream output;
    output << "Hello world!, pthread_id: " << id << ", rank: " << rank << "\n";
    std::cout << output.str();

    return nullptr;
}

int main(int argc, char** argv)
{
    require(argc >= 2, "You should enter number of threads");
    char *end;
    int threads_count = std::strtol(argv[1], &end, 10);
    require(*end == '\0', "Number of threads should be integer");
    
    pthread_t *threads = new pthread_t[threads_count];
    require(threads != nullptr, "Can't allocate memory for threads");
    int *ranks = new int[threads_count];
    require(ranks != nullptr, "Can't allocate memory for thread ranks");

    for(int i = 0; i < threads_count; i++)
    {
        ranks[i] = i;
        int code = pthread_create(&threads[i], nullptr, sayHello, &ranks[i]);
        require(code == 0, "Can't create thread");
    }

    for(int i = 0; i < threads_count; i++)
    {
        int code = pthread_join(threads[i], nullptr);
        require(code == 0, "Can't join thread");
    }
}
