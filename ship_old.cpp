#include "ship.h"

Ship::Ship(){}

Ship::~Ship(){
    MPI_Type_free(&MSG_WAR);
}

Ship::Ship(int rank, int size) {
    srand(rank + time(NULL));
    this->durability = rand()%(HP - 2) + 2;
    this->status = Status::AT_WAR;
    this->rank = rank;
    this->size = size;
    this->lamportTime = 0;
    this->kAsk = false;
    this->damage = 0;
    this->pending.resize(size, 0);
    this->MSG_WAR = this->createType();
    this->accepted = 0;
}

void Ship::updateTime(int received_time) {
    this->lamportTime = max(this->lamportTime, received_time) + 1;
}

int Ship::busyMech() {
    int sum = 0;
    for (size_t i = 0; i < this->mechQueue.size(); i++) {
        if (this->mechQueue.at(i)[0] != this->rank) {
            sum += this->mechQueue.at(i)[2];
        } else {
            break;
        }
    }
    return sum;
}

void Ship::processStatus(){
    if(commExit){
        if(this->rank == 0){
            send_message(Msg::SIG_END, 0, this);
        }
        return;
    }
    switch(this->status) {
        case Status::AT_WAR:
        {
            int interval = floor (MAXTIME / this->durability);
            int randTime = rand()%(interval * (this->durability) - interval) + interval;
            int dmg = floor (randTime / interval);

            sleep(randTime);

            cout << "Otrzymalem " << dmg << " pkt obrazen " << this->rank << " " << lamportTime << endl;

            this->damage = dmg;

            this->timeMutex.lock();
            this->status = Status::SEARCHING_DOCK;
            this->kAsk = true;
            
            this->timeMutex.unlock();

            cout << "Zadam dostepu do doku " << rank << " " << lamportTime << endl;

            for (int i = 0; i < this->size; i++) {
                if (i != this->rank) {
                    send_message(REQ_DOCK, i, this);
                }
            }
            
            sleep(WAIT_TIME);

            processStatus();

            break;
        }
        case Status::SEARCHING_DOCK:
        {
            sleep(WAIT_TIME);

            processStatus();

            break;
        }
        case Status::SEARCHING_MECHANICS:
        {

            sleep(WAIT_TIME);

            if (this->accepted == this->size - 1) {
                int mechs = busyMech();
                if (this->damage <= MECHANICS - mechs) {
                    cout << "Dostepnych: " << MECHANICS - mechs << " mechanikow " << rank << " " << lamportTime << endl;
                    
                    this->mechQueue.clear();
                    this->timeMutex.lock();
                    this->status = Status::FIXING;
                    this->timeMutex.unlock();
                }
            }
            
            processStatus();

            break;
        }
        case Status::FIXING:
        {
            cout << "Jestem w naprawie " << rank << " " << lamportTime << endl;

            sleep(this->damage);

            this->damage = 0;
            this->timeMutex.lock();
            this->kAsk = false;
            this->timeMutex.unlock();
            this->status = Status::AT_WAR;
            this->accepted = 0;

            for(int i = 0; i < this->size; i++) {
                if (i != this->rank){ 
                    send_message(REL_MECH, i, this);
                    cout << rank << " wyslal zwolnienie do " << i << " " << lamportTime << endl;
                    if (count(this->pending.begin(), this->pending.end(), i)) {
                        send_message(ACK_DOCK, i, this);
                        cout << rank << " wyslal ack dock do " << i << " " << lamportTime << endl;
                    } 
                }  
            }

            this->pending.clear();

            processStatus();

            break;
        }
    }
}

void Ship::processAck(int tag, message mess, int rank){
    this->timeMutex.lock();
    if(this->status != Status::SEARCHING_DOCK && tag == Msg::ACK_DOCK){
        this->timeMutex.unlock();
        return;
    }
    this->timeMutex.unlock();
    ++(this->accepted);
    //cout << accepted << " accept inc " << this->rank << " " << lamportTime << endl;
    if(tag == Msg::ACK_MECH && this->accepted == this->size-1){
        sort(this->mechQueue.begin(), this->mechQueue.end(), sortQueue);
        for(unsigned i=0; i<this->mechQueue.size(); ++i){
            cout << this->mechQueue[i].at(0) << " " << this->mechQueue[i].at(1) << " " << this->mechQueue[i].at(2) << " zawartosc kolejki " << this->rank << endl;
        }
        }
        /*for(unsigned i=0; i<this->mechQueue.size(); ++i){
            if(this->mechQueue[i].at(1) > mess.lamportTime){
                this->mechQueue.insert(this->mechQueue.begin()+i, toAdd);
            }
            if(this->mechQueue[i].at(1) == mess.lamportTime){
                if(this->mechQueue[i].at(0) < rank){
                    this->mechQueue.insert(this->mechQueue.begin()+i, toAdd);
                }
                else{
                    if(i+1 == this->mechQueue.size()){
                        this->mechQueue.emplace_back(toAdd);
                    }
                    else{
                        this->mechQueue.insert(this->mechQueue.begin()+i+1, toAdd);
                    }
                }
            }
        }*/
    if (this->status == Status::SEARCHING_DOCK && tag == Msg::ACK_DOCK && this->accepted >= this->size - DOCKS){
        this->status = Status::SEARCHING_MECHANICS;
        this->accepted = 0; 
        cout << "Jestem w doku " << this->rank << " " << lamportTime << endl;

        array<int, 3> arr = {this->rank, this->lamportTime+size-1, this->damage};
        this->mechQueue.push_back(arr);
        cout << "Zadam dostepu do " << arr[2] << " mechanikow " << this->rank << " " << lamportTime << endl;

        for (int i = 0; i < this->size; i++) {
            if (i != this->rank){ 
                send_message(REQ_MECH, i, this);
            }
        }
    }
}

void Ship::updateQueue(int freed){
    for(unsigned i=0; i<this->mechQueue.size(); ++i){
        cout << rank << " " << this->mechQueue[i].at(0) << endl; 
        if(this->mechQueue[i].at(0) == freed){
            cout << rank << " usuwa " << freed << " z kolejki" << endl;
            this->mechQueue.erase(this->mechQueue.begin() + i);
            break;
        }
    }
}

MPI_Datatype Ship::createType() {
    
    MPI_Datatype MPI_MESSAGE_WAR;
    const int items = 2;
    int bLength[items] = {1, 1};
    MPI_Datatype types[items] = {MPI_INT, MPI_INT};

    MPI_Aint offsets[items];
    offsets[0] = offsetof(message, lamportTime);
    offsets[1] = offsetof(message, mechNumber);

    MPI_Type_create_struct(items, bLength, offsets, types, &MPI_MESSAGE_WAR);
    MPI_Type_commit(&MPI_MESSAGE_WAR);

    return MPI_MESSAGE_WAR;
}

bool Ship::sortQueue(const array<int, 3> &a, const array<int, 3> &b){
    if(a.at(1) < b.at(1)) return true;
    if(a.at(1) == b.at(1) && a.at(0) < b.at(0)) return true;

    return false;
}

void send_message(Msg tag, int rec, Ship* ship){

    message mess;

    ship->timeMutex.lock();
    ship->updateTime(0);
    mess.lamportTime = ship->lamportTime;
    mess.mechNumber = ship->damage;
    ship->timeMutex.unlock();

    MPI_Send(&mess, 1, ship->MSG_WAR, rec, tag, MPI_COMM_WORLD);

}