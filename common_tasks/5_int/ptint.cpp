#include <require.hpp>
#include "config.hpp"
#include <pthread.h>
#include <iomanip>
#include <cstring>
#include <chrono>

#define MAX_STACK_SIZE 1000
#define LOCAL_STACK_CRITICAL_LINE 10

struct IntegrationData
{
    double A;
    double B;
    double fA;
    double fB;
    double sAB;
};

struct Stack
{
    int ptr;
    IntegrationData data[MAX_STACK_SIZE];
};

#define IS_EMPTY(stack) (stack.ptr == 0)
#define IS_NOT_EMPTY(stack) (stack.ptr > 0)

#define PUSH(stack, nA, nB, nfA, nfB, nsAB) \
    stack.data[stack.ptr].A = nA; \
    stack.data[stack.ptr].B = nB; \
    stack.data[stack.ptr].fA = nfA; \
    stack.data[stack.ptr].fB = nfB; \
    stack.data[stack.ptr].sAB = nsAB; \
    stack.ptr++;

#define POP(stack, nA, nB, nfA, nfB, nsAB) \
    stack.ptr--; \
    nA = stack.data[stack.ptr].A; \
    nB = stack.data[stack.ptr].B; \
    nfA = stack.data[stack.ptr].fA; \
    nfB = stack.data[stack.ptr].fB; \
    nsAB = stack.data[stack.ptr].sAB

pthread_mutex_t global_stack_mutex;
pthread_mutex_t global_sum_mutex;
Stack global_stack;
double global_sum;

pthread_mutex_t task_present_mutex;
int active_handlers_count;

void *handleGlobalStack(void *arg)
{
    Stack local_stack;
    int threads_count = *static_cast<int *>(arg);

    double s = 0;
    double A, B, fA, fB, sAB;

    while(true)
    {
        pthread_mutex_lock(&task_present_mutex);

        pthread_mutex_lock(&global_stack_mutex);
        POP(global_stack, A, B, fA, fB, sAB);
        if(IS_NOT_EMPTY(global_stack))
            pthread_mutex_unlock(&task_present_mutex);
        if(A < B) // common segment
            active_handlers_count++;
        pthread_mutex_unlock(&global_stack_mutex);
        if(A > B) // terminal segment
            break;

        while(true)
        {
            double C = (A + B) / 2;
            double fC = f(C);
            double h = (C - A) / 2;
            double sAC = (fA + fC) * h;
            double sCB = (fC + fB) * h;
            double sACB = sAC + sCB;

            if(std::abs(sACB - sAB) < epsilon * std::abs(sACB))
            {
                s += sACB;
                if(IS_EMPTY(local_stack))
                    break;
                POP(local_stack, A, B, fA, fB, sAB);
            }
            else
            {
                PUSH(local_stack, A, C, fA, fC, sAC);
                A = C;
                fA = fC;
                sAB = sCB;
            }

            // move segments to the global stack
            if(local_stack.ptr == LOCAL_STACK_CRITICAL_LINE)
            {
                if(IS_NOT_EMPTY(global_stack))
                    continue;

                pthread_mutex_lock(&global_stack_mutex);
                local_stack.ptr = 0;
                std::memcpy(global_stack.data + global_stack.ptr,
                    local_stack.data,
                    sizeof(IntegrationData) * LOCAL_STACK_CRITICAL_LINE);
                global_stack.ptr += LOCAL_STACK_CRITICAL_LINE;
                pthread_mutex_unlock(&task_present_mutex);
                pthread_mutex_unlock(&global_stack_mutex);
            }
        }

        pthread_mutex_lock(&global_stack_mutex);
        active_handlers_count--;
        if((active_handlers_count == 0) && IS_EMPTY(global_stack))
        {
            for(int i = 0; i < threads_count; i++)
            {
                PUSH(global_stack, 1, 0, 0, 0, 0); // push terminal segment
            }
            pthread_mutex_unlock(&task_present_mutex);
        }
        pthread_mutex_unlock(&global_stack_mutex);
    }

    pthread_mutex_lock(&global_sum_mutex);
    global_sum += s;
    pthread_mutex_unlock(&global_sum_mutex);
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

    auto start = std::chrono::high_resolution_clock::now();

    PUSH(global_stack, from, to, f(from), f(to), (f(from) + f(to)) * (to - from) / 2);

    for(int i = 1; i < threads_count; i++)
    {
        int code = pthread_create(&threads[i], nullptr, handleGlobalStack, &threads_count);
        require(code == 0, "Can't create thread");
    }
    handleGlobalStack(&threads_count);

    for(int i = 1; i < threads_count; i++)
    {
        int code = pthread_join(threads[i], nullptr);
        require(code == 0, "Can't join thread");
    }

    std::chrono::duration<double> elapsed = std::chrono::high_resolution_clock::now() - start;
    std::cout << std::setprecision(10);
    std::cout << "I = " << global_sum << std::endl;
    std::cout << "Threads: " << threads_count << ", time: " << elapsed.count() << " s" << std::endl;
}
