#include <mpi_try.hpp>
#include <string>

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    MPI_File file;
    TRY(MPI_File_open(MPI_COMM_WORLD, "./results/files_out.txt", MPI_MODE_CREATE | MPI_MODE_WRONLY, 
        MPI_INFO_NULL, &file), "Can't open file");
    TRY(MPI_File_set_size(file, 0), "Can't clear file");

    std::string line = "By rank " +  std::to_string(rank) + "\n";
    TRY(MPI_File_write_ordered(file, line.data(), line.length(), MPI_CHAR, MPI_STATUS_IGNORE), 
        "Can't write to file");
    
    TRY(MPI_File_close(&file), "Can't close file");

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
