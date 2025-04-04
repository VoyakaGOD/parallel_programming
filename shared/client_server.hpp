#include <mpi_try.hpp>

typedef int ClientRank;

void sendToServer(const void *buffer, int size)
{
    TRY(MPI_Send(buffer, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD), "Can't send data to the server");
}

void getResponse(void *buffer, int size)
{
    TRY(MPI_Recv(buffer, size, MPI_BYTE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE), "Can't receive response");
}

ClientRank listen(void *buffer, int size)
{
    MPI_Status status;
    int data_size;
    TRY(MPI_Probe(MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status), "Can't probe client message");
    TRY(MPI_Get_count(&status, MPI_BYTE, &data_size), "Can't get count of bytes in client message");
    TRY(data_size != size, "Incorrect size of message");

    MPI_Recv(buffer, size, MPI_BYTE, status.MPI_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    return status.MPI_SOURCE;
}

void sendResponse(ClientRank client, void *buffer, int size)
{
    TRY(MPI_Send(buffer, size, MPI_BYTE, client, 0, MPI_COMM_WORLD), "Can't send response from server");
}
