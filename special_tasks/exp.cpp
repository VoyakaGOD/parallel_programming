#include <big_decimal.hpp>
#include <mpi_try.hpp>
#include <chrono>
#include <cmath>

uint32_t getInverseNlogN(uint32_t x)
{
    double low = 1;
    double high = x;

    while (high - low > 0.5)
    {
        double mid = (low + high) / 2;
        double value = mid * std::log10(mid);

        if (value < x)
            low = mid;
        else
            high = mid;
    }

    return static_cast<uint32_t>((low + high) / 2);
}

void sendBigDecimal(const BigDecimal &value, int dest)
{
    const std::vector<uint32_t> &data = value.getData();
    TRY(MPI_Send(data.data(), data.size(), MPI_UINT32_T, dest, 0, MPI_COMM_WORLD), 
        "Can't send BigDecimal to rank " + std::to_string(dest));
}

void recvBigDecimal(BigDecimal &value, int source)
{
    std::vector<uint32_t> &data = value.getData();
    TRY(MPI_Recv(data.data(), data.size(), MPI_UINT32_T, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
        "Can't receive BigDecimal from rank " + std::to_string(source));
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank = 0;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size = 1;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    uint32_t precision = static_cast<uint32_t>(std::strtoul(argv[1], nullptr, 10));
    size_t fbc = (precision + BigDecimal::getBlockLength() - 1) / BigDecimal::getBlockLength();
    fbc += 2; // 2 additional blocks to account for the contribution of division remainders
    size_t N = 2 * getInverseNlogN(precision + 1); // (N/2)lg(N/2) = lg(2) + precision

    size_t partition_size = N / world_size;
    size_t last_rank = world_size - 1;
    size_t from = rank * partition_size;
    size_t to = (rank == last_rank) ? N : ((rank + 1) * partition_size);
    // from [i = 1] to [i = N]
    from++;
    to++;

    BigDecimal e = BigDecimal::getOne(1, fbc, precision);
    BigDecimal sum = BigDecimal(1, fbc, precision);
    BigDecimal item = BigDecimal::getOne(1, fbc, precision);

    auto start = std::chrono::system_clock::now();
    for(uint32_t n = from; n < to; n++)
    {
        item /= n;
        sum += item;
    }

    BigDecimal tail(1, fbc, precision);
    if(rank < (world_size - 1))
        recvBigDecimal(tail, rank + 1);
    sum += item * tail;
    if(rank == 0)
        e += sum;
    else
        sendBigDecimal(sum, rank - 1);
    auto end = std::chrono::system_clock::now();

    if(rank == 0)
        std::cout << "e = " << e << std::endl;
    
    std::chrono::duration<double> time = end - start;
    MPI_SYNC(std::cout << "rank : " << rank << ", time: " << time.count() << " seconds" << std::endl);

    TRY(MPI_Finalize(), "Bad MPI finalization");
}
