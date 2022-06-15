#include "message_handler.h"

void handler::recv_message(Ship* ship){
    

    MPI_Status status;
    message mess;

    while(true){

        MPI_Recv(&mess, 1, ship->MSG_WAR, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG){
            case REQ_DOCK:
                //cout << ship->rank << " dostal req_dock " << ship->lamportTime << " od " << status.MPI_SOURCE << endl;
                //ship->statusMutex.lock();
                if (!ship->kAsk){
                    //ship->statusMutex.unlock();
                    //cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (1) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                }
                else if (ship->status == Status::SEARCHING_DOCK && ship->lamportReqest > mess.lamportTime){
                    //ship->statusMutex.unlock();
                    //cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (2) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                }
                else if (ship->status == Status::SEARCHING_DOCK && ship->lamportReqest == mess.lamportTime && ship->rank > status.MPI_SOURCE){
                    //ship->statusMutex.unlock();
                    //cout << ship->rank << " wyslal po req ack dock do " << status.MPI_SOURCE << " (3) " << ship->lamportTime << endl;
                    send_message(Msg::ACK_DOCK, status.MPI_SOURCE, ship);
                    }
                else{
                    //ship->statusMutex.unlock();
                    //cout << ship->rank << " dodal do pending " << status.MPI_SOURCE << " " << ship->lamportTime << endl;
                    ship->pendMutex.lock();
                    ship->pending.emplace_back(status.MPI_SOURCE);
                    ship->pendMutex.unlock();
                }
                break;
            case ACK_DOCK:
                //cout << ship->rank << " dostal ack_dock " << ship->lamportTime << " od " << status.MPI_SOURCE << endl;
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REQ_MECH:
            {
                //cout << ship->rank << " dostal req_mech " << ship->lamportTime << " od " << status.MPI_SOURCE << endl;
                array<int, 3> toAdd = {status.MPI_SOURCE, mess.lamportTime, mess.mechNumber};
                ship->queueMutex.lock();
                ship->mechQueue.emplace_back(toAdd);
                ship->queueMutex.unlock();
                send_message(Msg::ACK_MECH, status.MPI_SOURCE, ship);
                break;
            }
            case ACK_MECH:
                //cout << ship->rank << " dostal ack_mech " << ship->lamportTime << " od " << status.MPI_SOURCE << endl;
                ship->processAck(status.MPI_TAG, mess, status.MPI_SOURCE);
                break;
            case REL_MECH:
                //cout << ship->rank << " dostal rel_mech " << ship->lamportTime << " od " << status.MPI_SOURCE << endl;
                ship->queueMutex.lock();
                ship->updateQueue(status.MPI_SOURCE);
                ship->queueMutex.unlock();
                break;
        }

    ship->timeMutex.lock();
    ship->updateTime(mess.lamportTime);
    ship->timeMutex.unlock();
    }

}
