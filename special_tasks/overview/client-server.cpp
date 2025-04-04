#include <client_server.hpp>

void client(int rank)
{
    int msg = 2*rank*rank + rank - 1;
    sendToServer(&msg, sizeof(int));

    int response = 0;
    getResponse(&response, sizeof(int));

    std::cout << "Rank: " << rank << ", msg/response: " <<  msg << "->" << response << std::endl;
}

void server(int durability)
{
    int msg = 0;
    while((durability--) > 0)
    {
        ClientRank client = listen(&msg, sizeof(int));

        msg = 2*msg + 1;
        sendResponse(client, &msg, sizeof(int));
    }
}

int main(int argc, char** argv)
{
    TRY(MPI_Init(&argc, &argv), "Bad MPI initialization");

    int rank;
    TRY(MPI_Comm_rank(MPI_COMM_WORLD, &rank), "Can't get rank of this process");

    int world_size;
    TRY(MPI_Comm_size(MPI_COMM_WORLD, &world_size), "Can't get total count of processes");

    if(rank == 0)
        server(world_size - 1);
    else
        client(rank);

    TRY(MPI_Finalize(), "Bad MPI finalization");
    
    return 0;
}
