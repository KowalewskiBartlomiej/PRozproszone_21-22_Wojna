#include "message_handler.h"

void handler::recv_message(Ship* ship){
    
    MPI_Status status;
    message mess;

    while(true){

        MPI_Recv(&mess, 1, ship->MSG_WAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        ship->timeMutex.lock();
        ship->updateTime(mess.lamportTime);
        ship->timeMutex.unlock();

        switch(status.MPI_TAG){
            case REQ_DOCK:
                if (!ship->kAsk){
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                }
                else{
                    ship->pending.emplace_back(status.MPI_SOURCE);
                }
                break;
            case ACK_DOCK:
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REQ_MECH:
                send_message(Msg::ACK_MECH, status.MPI_SOURCE, ship);
                break;
            case ACK_MECH:
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REL_MECH:
                ship->updateQueue(status.MPI_SOURCE);
                break;
        }

    }

}
