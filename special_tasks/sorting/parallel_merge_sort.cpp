#include <mpi_try.hpp>
#include <random>

// only for positive numbers
bool is_power_of_two(int n)
{
    return (n & (n - 1)) == 0;
}

int *copy(int *original, int size)
{
    int *copy = new int[size];
    TRY(copy == nullptr, "Can't allocate memory for copying array");
    for(int i = 0; i < size; i++)
        copy[i] = original[i];
    return copy;
}

void merge(int *array, int *left, int left_len, int *right, int right_len)
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

void sort(int *array, int left, int right)
{
    if(left >= right)
        return;
    
    int mid = left + (right - left) / 2;
    
    sort(array, left, mid);
    sort(array, mid + 1, right);
    
    int left_size = mid - left + 1;
    int right_size = right - mid;
    merge(array + left, array + left, left_size, array + mid + 1, right_size);
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");
    TRY(!is_power_of_two(world_size), "Processes count should be power of two");

    TRY(argc < 2, "You should enter array size");
    int N = atoi(argv[1]);
    TRY(N <= 0, "Array size should be positive value");
    N += N % world_size;

    int *array = new int[N];
    TRY(array == nullptr, "Can't allocate memory for array");

    int *unordered_array = nullptr;
    if(rank == 0)
    {
        std::random_device device;
        std::mt19937 engine(device());
        std::uniform_int_distribution<std::mt19937::result_type> random(0, 7*N);

        unordered_array = new int[N];
        TRY(unordered_array == nullptr, "Can't allocate memory for array");
        for(int i = 0; i < N; i++)
            unordered_array[i] = random(engine);
    }

    int partition_size = N / world_size;
    int from = rank * partition_size;

    double start = MPI_Wtime();

    MPI_Scatter(unordered_array, partition_size, MPI_INT, array + from, partition_size, MPI_INT, 0, MPI_COMM_WORLD);
    if(rank == 0)
        delete[] unordered_array;
    
    sort(array + from, 0, partition_size - 1);
    for(int i = 1; i < world_size; i <<= 1)
    {
        if((rank % i) != 0)
            break;

        if((rank / i) % 2 == 0)
        {
            TRY(MPI_Recv(array + from + i*partition_size, i*partition_size,
                MPI_INT, rank + i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), "Can't receive msg");
            merge(array, array + from, i*partition_size, array + from + i*partition_size, i*partition_size);
        }
        else
        {
            TRY(MPI_Send(array + from, i*partition_size,
                MPI_INT, rank - i, 0, MPI_COMM_WORLD), "Can't receive msg");
        }
    }

    if(rank == 0)
    {
        double end = MPI_Wtime();

        std::cout << array[0];
        for(int i = 1; i < std::min(N, 1000); i++)
            std::cout << ", " << array[i];
        std::cout << std::endl;

        std::cout << "time: " << (end - start) << " s";
    }

    delete[] array;
    TRY(MPI_Finalize(), "Bad MPI finalization");
    return 0;
}
