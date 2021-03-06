#include <csignal>

#include "message_handler.h"

bool commExit;

void signal_handler(int signum){
    commExit = true;   
    MPI_Finalize();   
}

int main(int argc, char** argv){
    
    int provided;
    int rank, size;

    commExit = false;

    signal(SIGINT, signal_handler);

    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    if (provided != MPI_THREAD_MULTIPLE) {
       perror("Brak wsparcia dla wątków");
       MPI_Finalize();
       exit(0);
    }

    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    
    Ship ship(rank, size);
    std::thread mess_handle(handler::recv_message, &ship);
    mess_handle.detach();
    ship.processStatus();
    //mess_handle.join();
    //delete &ship;
    cout << rank << " sfinalizowany!" << endl;
}