#include <mpi.h>
#include <iostream>
#include <thread>

#include "structs.h"
#include "message_handler.h"

MPI_Datatype MPI_MESSAGE;

void createType() {
    const int items = 2;
    int bLength[items] = {1, 1};
    MPI_Datatype types[items] = {MPI_INT, MPI_INT};

    MPI_Aint offsets[items];
    offsets[0] = offsetof(message, lamportTime);
    offsets[1] = offsetof(message, mechNumber);

    MPI_Type_create_struct(items, bLength, offsets, types, &MPI_MESSAGE);
    MPI_Type_commit(&MPI_MESSAGE);
}

int main(int argc, char** argv){
    
    int provided;
    int rank, size;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
       perror("Brak wsparcia dla wątków");
       MPI_Finalize();
       exit(0);
    }

    createType();

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    std::thread mess_handle(handler::recv_message);
    Ship ship(rank, size);
}