#include <mpi_try.hpp>
#include <iomanip>
#include <chrono>
#include <thread>
#include <cmath>

using namespace std::chrono_literals;

#define BARRIER TRY(MPI_Barrier(MPI_COMM_WORLD), "Can't create barrier")

template <typename SendFuncType>
void measure_send_time(const char *label, SendFuncType send, int buff_size, int rank)
{
    char *buff = new char[buff_size];
    if(rank == 0)
    {
        auto start = std::chrono::system_clock::now();
        TRY(send(buff, buff_size, MPI_CHAR, 1, 0, MPI_COMM_WORLD), "Can't send msg");
        auto end = std::chrono::system_clock::now();
        std::chrono::duration<double> time = end - start;
        std::cout << label << ", time: " << std::round(time.count()) << "s, buffer size: " << buff_size << std::endl;
    }
    else if(rank == 1)
    {
        std::this_thread::sleep_for(1s);
        TRY(MPI_Recv(buff, buff_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE),
            "Can't receive msg");
    }
    delete[] buff;
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    TRY((world_size == 2) ? 0 : -1, "You should create exactly 2 processes");

    measure_send_time("_send", MPI_Send, 1, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 10, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 100, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 1000, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 10000, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 100000, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 1000000, rank); BARRIER;

    if(rank == 0) std::cout << std::endl; BARRIER;
       
    measure_send_time("ssend", MPI_Ssend, 1, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 10, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 100, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 1000, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 10000, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 100000, rank); BARRIER;
    measure_send_time("ssend", MPI_Ssend, 1000000, rank); BARRIER;

    if(rank == 0) std::cout << std::endl; BARRIER;

    measure_send_time("rsend", MPI_Rsend, 1, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 10, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 100, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 1000, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 10000, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 100000, rank); BARRIER;
    measure_send_time("rsend", MPI_Rsend, 1000000, rank); BARRIER;

    if(rank == 0) std::cout << std::endl; BARRIER;

    char *buffer = new char[1200000];
    if(buffer == nullptr)
    {
        std::cerr << "Can't allocate memory for buffer";
        MPI_Abort(MPI_COMM_WORLD, 0);
    }
    TRY(MPI_Buffer_attach(buffer, 1200000), "Can't attach buffer for MPI");
    measure_send_time("bsend", MPI_Bsend, 1, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 10, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 100, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 1000, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 10000, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 100000, rank); BARRIER;
    measure_send_time("bsend", MPI_Bsend, 1000000, rank); BARRIER;

    if(rank == 0) std::cout << std::endl; BARRIER;

    measure_send_time("_send", MPI_Send, 69550, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69551, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69552, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69553, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69554, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69555, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69556, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69557, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69558, rank); BARRIER;
    measure_send_time("_send", MPI_Send, 69559, rank);

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
