#include <mpi.h>
#include <iostream>
#include <thread>

#include "structs.h"
#include "message_handler.h"

int main(int argc, char** argv){
    
    int provided;
    int rank, size;

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
       perror("Brak wsparcia dla wątków");
       MPI_Finalize();
       exit(0);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    Ship ship(rank, size);
    ship.processStatus();
    //std::thread mess_handle(handler::recv_message, &ship);
    MPI_Finalize();
}