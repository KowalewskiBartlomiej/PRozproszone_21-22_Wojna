#include "message_handler.h"

void handler::recv_message(){
    
    MPI_Status status;
    int tmp;

    while(true){

        MPI_Recv(&tmp, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG){
            case REQ_DOCK:
                break;
            case ACK_DOCK:
                break;
            case REQ_MECH:
                break;
            case ACK_MECH:
                break;
            case REL_MECH:
                break;
        }

    }

}

void handler::send_message(Msg tag, int rec){

    int tmp;

    
    MPI_Send(&tmp, 1, MPI_INT, rec, tag, MPI_COMM_WORLD);

}
