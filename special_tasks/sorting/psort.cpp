#include "radix_sort.hpp"
#include <random>

int count_4_bit_groups(uint64_t n)
{
    int count = 0;
    while (n > 0)
    {
        count++;
        n >>= 4;
    }
    return count;
}

// only for positive numbers
bool is_power_of_two(int n)
{
    return (n & (n - 1)) == 0;
}

uint64_t *copy(uint64_t *original, int size)
{
    uint64_t *copy = new uint64_t[size];
    TRY(copy == nullptr, "Can't allocate memory for copying array");
    for(int i = 0; i < size; i++)
        copy[i] = original[i];
    return copy;
}

void merge(uint64_t *array, uint64_t *left, int left_len, uint64_t *right, int right_len)
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

void merge_sort(uint64_t *array, int left, int right)
{
    if(left >= right)
        return;
    
    int mid = left + (right - left) / 2;
    
    merge_sort(array, left, mid);
    merge_sort(array, mid + 1, right);
    
    int left_size = mid - left + 1;
    int right_size = right - mid;
    merge(array + left, array + left, left_size, array + mid + 1, right_size);
}

void radix_sort(uint64_t *array, int size)
{
    int digits_count = 0;
    for(int i = 0; i < size; i++)
        digits_count = std::max(digits_count, count_4_bit_groups(array[i]));

    RadixSorter::sort(array, size, digits_count);
}

void hybrid_sort(uint64_t *array, int size)
{
    const int n0 = 30000;

    if(size > n0)
        radix_sort(array, size);
    else
        merge_sort(array, 0, size - 1);
}

double measure_time(int N, int rank, int world_size, std::string alg)
{
    N += N % world_size;

    uint64_t *array = new uint64_t[N];
    TRY(array == nullptr, "Can't allocate memory for array");

    uint64_t *unordered_array = nullptr;
    if(rank == 0)
    {
        std::random_device device;
        std::mt19937_64 engine(device());
        std::uniform_int_distribution<std::mt19937_64::result_type> random(0, -1ull);

        unordered_array = new uint64_t[N];
        TRY(unordered_array == nullptr, "Can't allocate memory for array");
        for(int i = 0; i < N; i++)
            unordered_array[i] = random(engine);
    }

    int partition_size = N / world_size;
    int from = rank * partition_size;

    double start = MPI_Wtime();

    MPI_Scatter(unordered_array, partition_size, MPI_UINT64_T, array + from, 
        partition_size, MPI_UINT64_T, 0, MPI_COMM_WORLD);

    if(alg != "")
    {
        if(alg == "hybrid")
            hybrid_sort(array + from, partition_size);
        else if(alg == "radix")
            radix_sort(array + from, partition_size);
        else
            throw std::runtime_error("Unknown soring algorithm name");
    }
    else
    {
        merge_sort(array + from, 0, partition_size - 1);
    }

    for(int i = 1; i < world_size; i <<= 1)
    {
        if((rank % i) != 0)
            break;

        if((rank / i) % 2 == 0)
        {
            TRY(MPI_Recv(array + from + i*partition_size, i*partition_size,
                MPI_UINT64_T, rank + i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), "Can't receive msg");
            merge(array, array + from, i*partition_size, array + from + i*partition_size, i*partition_size);
        }
        else
        {
            TRY(MPI_Send(array + from, i*partition_size,
                MPI_UINT64_T, rank - i, 0, MPI_COMM_WORLD), "Can't send msg");
        }
    }
    
    double end = MPI_Wtime();
    if(rank == 0)
        delete[] unordered_array;
    delete[] array;
    return (end - start);
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    TRY(!is_power_of_two(world_size), "Processes count should be power of two");

    TRY(argc < 4, "You should enter array initial size, step size, steps count");
    int N = atoi(argv[1]);
    TRY(N <= 0, "Array size should be positive value");
    int step = atoi(argv[2]);
    int count = atoi(argv[3]);
    TRY(count <= 0, "Steps count should be positive value");

    for(int i = 0; i < count; i++)
    {
        double time = 0;
        for(int j = 0; j < 10; j++)
            time += measure_time(N, rank, world_size, std::string((argc > 4) ? argv[4] : ""));
        if(rank == 0)
            std::cout << (time / 10) << ", ";
        if(step < 0)
            N *= (-step);
        else
            N += step;
    }

    if(rank == 0)
        std::cout << std::endl;
    TRY(MPI_Finalize(), "Bad MPI finalization");
    return 0;
}
