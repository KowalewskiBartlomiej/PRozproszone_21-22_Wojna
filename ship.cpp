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
    //this->pending.resize(size, 0);
    this->MSG_WAR = this->createType();
    this->accepted = 0;
}

void Ship::updateTime(int received_time) {
    this->lamportTime = max(this->lamportTime, received_time) + 1;
}

int Ship::busyMech() {
    int sum = 0;
    for (size_t i = 0; i < this->mechQueue.size(); i++) {
        if (this->mechQueue[i][0] != this->rank) {
            sum += this->mechQueue[i][2];
        } else {
            break;
        }
    }
    return sum;
}

void Ship::processStatus(){
    while(true){
    this->statusMutex.lock();
    switch(this->status) {
        case Status::AT_WAR:
        {
            this->statusMutex.unlock();
            int interval = floor (MAXTIME / this->durability);
            int randTime = rand()%(interval * (this->durability) - interval) + interval;
            int dmg = floor (randTime / interval);

            //sleep(randTime);
            //usleep(100000);
            //sleep(10);

            //cout << "Otrzymalem " << dmg << " pkt obrazen " << this->rank << " " << lamportTime << endl;

            this->statusMutex.lock();
            this->damage = dmg;
            this->status = Status::SEARCHING_DOCK;
            this->kAsk = true;
            this->statusMutex.unlock();

            
            this->timeMutex.lock();
            this->lamportReqest = this->lamportTime;
            this->timeMutex.unlock();
            //cout << "Zadam dostepu do doku " << rank << " " << lamportReqest << endl;

            for (int i = 0; i < this->size; i++) {
                if (i != this->rank) {
                    send_message(REQ_DOCK, i, this);
                }
            }
            
            //sleep(WAIT_TIME);

            //processStatus();

            break;
        }
        case Status::SEARCHING_DOCK:
        {
            
            this->statusMutex.unlock();
            //sleep(WAIT_TIME);
            //processStatus();

            break;
        }
        case Status::SEARCHING_MECHANICS:
        {

            
            

            if (this->accepted == this->size - 1) {
                this->statusMutex.unlock();
                this->queueMutex.lock();
                int mechs = busyMech();
                this->queueMutex.unlock();
                if (this->damage <= MECHANICS - mechs) {
                    //cout << "Dostepnych: " << MECHANICS - mechs << " mechanikow " << rank << " " << lamportTime << endl;
                    
                    this->queueMutex.lock();
                    this->updateQueue(rank);
                    this->queueMutex.unlock();
                    this->statusMutex.lock();
                    this->status = Status::FIXING;
                    this->statusMutex.unlock();
                }
            }
            else{
                this->statusMutex.unlock();
            }
            //sleep(WAIT_TIME);
            //processStatus();

            break;
        }
        case Status::FIXING:
        {
            this->statusMutex.unlock();
            //cout << "Jestem w naprawie " << rank << " " << lamportTime << endl;

            //sleep(this->damage);

            
            this->statusMutex.lock();
            this->damage = 0;
            this->kAsk = false;
            this->status = Status::AT_WAR;
            this->accepted = 0;
            this->statusMutex.unlock();
            
            /*this->timeMutex.lock();
            this->lamportReqest = this->lamportTime;
            this->timeMutex.unlock();*/
            this->pendMutex.lock();
            for(int i = 0; i < this->size; i++) {
                if (i != this->rank){ 
                    send_message(REL_MECH, i, this);
                    //cout << rank << " wyslal zwolnienie do " << i << " " << lamportTime << endl;
                    if (count(this->pending.begin(), this->pending.end(), i)) {
                        send_message(ACK_DOCK, i, this);
                        //cout << rank << " wyslal ack dock do " << i << " " << lamportTime << endl;
                    } 
                }  
            }
            this->pending.clear();
            this->pendMutex.unlock();

            //processStatus();

            break;
        }
    }
    }
}

void Ship::processAck(int tag, message mess, int rank){
    this->statusMutex.lock();
    if(this->status != Status::SEARCHING_DOCK && tag == Msg::ACK_DOCK){
        this->statusMutex.unlock();
        return;
    }
    ++(this->accepted);
    //cout << accepted << " accept inc " << this->rank << " " << lamportTime << endl;
    if(tag == Msg::ACK_MECH && this->accepted == this->size-1){
        this->statusMutex.unlock();
        this->queueMutex.lock();
        sort(this->mechQueue.begin(), this->mechQueue.end(), sortQueue);
        /*for(unsigned i=0; i<this->mechQueue.size(); ++i){
            std::cout << this->mechQueue[i][0] << " " << this->mechQueue[i][1] << " " << this->mechQueue[i][2] << " zawartosc kolejki " << this->rank << endl;
        }*/
        this->queueMutex.unlock();
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
    this->statusMutex.unlock();
    this->statusMutex.lock();
    if (this->status == Status::SEARCHING_DOCK && tag == Msg::ACK_DOCK && this->accepted >= this->size - DOCKS){
        this->status = Status::SEARCHING_MECHANICS;
        this->accepted = 0; 
        this->statusMutex.unlock();
        //cout << "Jestem w doku " << this->rank << " " << lamportTime << endl;
        this->timeMutex.lock();
        this->lamportReqest = this->lamportTime;
        this->timeMutex.unlock();
        array<int, 3> arr = {this->rank, this->lamportReqest, this->damage};
        this->queueMutex.lock();
        this->mechQueue.emplace_back(arr);
        this->queueMutex.unlock();
        //cout << "Zadam dostepu do " << arr[2] << " mechanikow " << this->rank << " " << lamportReqest << endl;

        for (int i = 0; i < this->size; i++) {
            if (i != this->rank){ 
                send_message(REQ_MECH, i, this);
            }
        }
    }
    this->statusMutex.unlock();
}

void Ship::updateQueue(int freed){
    for(unsigned i=0; i<this->mechQueue.size(); ++i){
        //cout << rank << " " << this->mechQueue[i][0] << endl; 
        if(this->mechQueue[i][0] == freed){
            //cout << rank << " usuwa " << freed << " z kolejki " << lamportTime << endl;
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
    mess.lamportTime = ship->lamportReqest;
    mess.mechNumber = ship->damage;
    ship->timeMutex.unlock();

    
    MPI_Send(&mess, 1, ship->MSG_WAR, rec, tag, MPI_COMM_WORLD);

}
