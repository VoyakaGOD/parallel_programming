#include <require.hpp>
#include <pthread.h>
#include <sstream>

int value = 0;
int queue = 0;
pthread_mutex_t mutex;

void *updateValue(void *arg)
{
    int rank = *static_cast<int*>(arg);

    while(true)
    {
        pthread_mutex_lock(&mutex);

        if(queue == rank)
        {
            std::ostringstream output;
            output << "Thread[" << rank << "] " << value;

            queue++;
            value *= 2;
            value += rank;

            output << " -> " << value << "\n";
            std::cout << output.str();
            pthread_mutex_unlock(&mutex);
            break;
        }

        pthread_mutex_unlock(&mutex);
    }
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
        int code = pthread_create(&threads[i], nullptr, updateValue, &ranks[i]);
        require(code == 0, "Can't create thread");
    }

    for(int i = 0; i < threads_count; i++)
    {
        int code = pthread_join(threads[i], nullptr);
        require(code == 0, "Can't join thread");
    }
}
