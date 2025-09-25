#include <require.hpp>
#include <iostream>
#include <cstring>
#include <random>
#include <limits>
#include <omp.h>

typedef uint64_t value_t;
typedef std::mt19937_64 engine_t;
typedef std::uniform_int_distribution<engine_t::result_type> distribution_t;

#define inf std::numeric_limits<double>::infinity()

value_t *copy(value_t *original, int size)
{
    value_t *copy = new value_t[size];
    require(copy, "Can't allocate memory for copying array");
    memcpy(copy, original, sizeof(value_t) * size);
    return copy;
}

void merge(value_t *array, value_t *left, int left_len, value_t *right, int right_len)
{
    left = copy(left, left_len);
    right = copy(right, right_len);

    int i = 0, j = 0, k = 0;
    while ((i < left_len) && (j < right_len))
    {
        if (left[i] < right[j])
        {
            array[k] = left[i];
            i++;
        }
        else
        {
            array[k] = right[j];
            j++;
        }
        k++;
    }

    while (i < left_len)
    {
        array[k] = left[i];
        i++;
        k++;
    }

    while (j < right_len)
    {
        array[k] = right[j];
        j++;
        k++;
    }

    delete[] left;
    delete[] right;
}

void merge_sort(value_t *array, int left, int right, int threshold)
{
    if(left >= right)
        return;
    
    int mid = left + (right - left) / 2;

    #pragma omp task shared(array) firstprivate(left, mid) if(right - left > threshold)
    merge_sort(array, left, mid, threshold);

    #pragma omp task shared(array) firstprivate(left, mid) if(right - left > threshold)
    merge_sort(array, mid + 1, right, threshold);

    int left_size = mid - left + 1;
    int right_size = right - mid;

    #pragma omp taskwait
    merge(array + left, array + left, left_size, array + mid + 1, right_size);
}

void insertion_sort(value_t *array, int left, int right)
{
    for(int i = left + 1; i <= right; i++)
    {
        value_t pivot = array[i];
        int j = i - 1;
        while((j >= left) && (array[j] > pivot))
        {
            array[j+1] = array[j];
            j--;
        }
        array[j+1] = pivot;
    }
}

void hybrid_sort(value_t *array, int left, int right, int threshold)
{
    if((right - left) < threshold)
    {
        insertion_sort(array, left, right);
        return;
    }
    
    int mid = left + (right - left) / 2;

    #pragma omp task shared(array) firstprivate(left, mid) 
    hybrid_sort(array, left, mid, threshold);

    #pragma omp task shared(array) firstprivate(left, mid)
    hybrid_sort(array, mid + 1, right, threshold);

    int left_size = mid - left + 1;
    int right_size = right - mid;

    #pragma omp taskwait
    merge(array + left, array + left, left_size, array + mid + 1, right_size);
}

value_t *init_array(int N)
{
    value_t *array = nullptr;
    std::random_device device;
    engine_t engine(device());
    distribution_t random(0, -1ull);

    array = new value_t[N];
    require(array, "Can't allocate memory for array");
    for(int i = 0; i < N; i++)
        array[i] = random(engine);

    return array;
}

template <void (*Sort)(value_t *, int, int, int)>
double measure_sort_time(int N, int threshold, int num_threads)
{
    value_t *array = init_array(N);
    double start_time = omp_get_wtime();

    #pragma omp parallel num_threads(num_threads)
    #pragma omp single
    Sort(array, 0, N - 1, threshold);

    double end_time = omp_get_wtime();
    std::cout << "sorting time(N = " << N <<", threshold = " << threshold;
    std::cout << ", threads = " << num_threads << "): ";
    std::cout << (end_time - start_time) << " s." << std::endl;

    delete[] array;
    return end_time - start_time;
}

bool is_sorted(value_t *array, int size)
{
    for(int i = 1; i < size; i++)
        if(array[i-1] > array[i])
            return false;
    return true;
}

int main(int argc, char** argv)
{
    require(argc > 1, "You should enter array size");
    int N = atoi(argv[1]);
    require(N > 0, "Array size should be positive value");

    if((argc > 2) && !strcmp(argv[2], "check"))
    {
        value_t *array = init_array(N);
        std::cout << "Merge: ";
        std::cout << (is_sorted(array, N) ? "sorted" : "not sorted") << " ---> ";
        merge_sort(array, 0, N-1, 10);
        std::cout << (is_sorted(array, N) ? "sorted" : "not sorted") << std::endl;
        delete[] array;

        array = init_array(N);
        std::cout << "Hybrid: ";
        std::cout << (is_sorted(array, N) ? "sorted" : "not sorted") << " ---> ";
        hybrid_sort(array, 0, N-1, 10);
        std::cout << (is_sorted(array, N) ? "sorted" : "not sorted") << std::endl;
        delete[] array;

        return 0;
    }

    for(int i = 1; i <= 15; i++)
        measure_sort_time<hybrid_sort>(N, pow(2,i), 8);
}
