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
                ship->timeMutex.lock();
                if (!ship->kAsk){
                    ship->timeMutex.unlock();
                    cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (1) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                }
                else if (ship->status == Status::SEARCHING_DOCK && ship->lamportTime > mess.lamportTime){
                    ship->timeMutex.unlock();
                    cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (2) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                }
                else if (ship->status == Status::SEARCHING_DOCK && ship->lamportTime == mess.lamportTime && ship->rank > status.MPI_SOURCE){
                    ship->timeMutex.unlock();
                    cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (3) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                    }
                else{
                    ship->timeMutex.unlock();
                    cout << ship->rank << " dodal do pending " << status.MPI_SOURCE << " " << ship->lamportTime << endl;
                    ship->pending.emplace_back(status.MPI_SOURCE);
                }
                break;
            case ACK_DOCK:
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REQ_MECH:
            {
                array<int, 3> toAdd = {status.MPI_SOURCE, mess.lamportTime, mess.mechNumber};
                ship->mechQueue.emplace_back(toAdd);
                send_message(Msg::ACK_MECH, status.MPI_SOURCE, ship);
                break;
            }
            case ACK_MECH:
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REL_MECH:
                ship->updateQueue(status.MPI_SOURCE);
                break;
        }

    }

}
