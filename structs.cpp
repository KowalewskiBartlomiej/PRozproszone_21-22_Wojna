#include "structs.h"
#include "main.cpp"

Ship::Ship(){}

Ship::~Ship(){}

Ship::Ship(int rank, int size) {
    //srand(rank + time(NULL));
    this->durability = HP; //rand() % 
    this->status = Status::AT_WAR;
    this->rank = rank;
    this->size = size;
    this->lamportTime = 0;
    this->kAsk = false;
    this->damage = 0;
    this->pending.resize(size, 0);
}

void Ship::updateTime(int received_time) {
    this->lamportTime = max(this->lamportTime, received_time) + 1;
}

int Ship::busyMech() {
    int sum = 0;
    for (int i = 0; i < this->mechQueue.size(); i++) {
        if (this->mechQueue.at(i)[0] != this->rank) {
            sum += this->mechQueue.at(i)[0];
        } else {
            sum += this->mechQueue.at(i)[0];
            break;
        }
    }
    return sum;
}

void Ship::processStatus(){
    switch(this->status) {
        case Status::AT_WAR:
            int interval = floor (MAXTIME / HP);
            int randTime = rand()%(interval * HP - interval) + interval;
            int dmg = floor (randTime / interval);

            sleep(randTime);

            cout << "Otrzymalem " << dmg << " pkt obrazen" << endl;

            this->damage = dmg;

            this->status = Status::SEARCHING_DOCK;
            this->kAsk = true;

            message *msg = (message *) malloc (sizeof(message));

            cout << "Zadam dostepu do doku" << endl;

            for (int i = 0; i < this->size; i++) {
                if (i != this->rank) {
                    handler::send_message(REQ_DOCK, i, this);
                }
            }

            free(msg);

            sleep(WAIT_TIME);

            processStatus();

            break;

        case Status::SEARCHING_DOCK:
            sleep(WAIT_TIME);

            if (this->accepted >= SHIPS - DOCKS) {
                this->status = Status::SEARCHING_MECHANICS;
                this->accepted = 0;
                cout << "Jestem w doku" << endl;

                array<int, 3> arr = {this->rank, this->lamportTime, this->damage};
                this->mechQueue.push_back(arr);

                message *msg = (message *) malloc (sizeof(message));

                cout << "Zadam dostepu do mechanikow" << endl;

                for (int i = 0; i < this->size; i++) {
                    if (i != this->rank){ 
                        handler::send_message(REQ_MECH, i, this);
                    }
                }
            }

            processStatus();

            break;

        case Status::SEARCHING_MECHANICS:
            int mechs = busyMech();
            if (this->accepted == this->size - 1) {
                if (this->damage <= MECHANICS - mechs) {
                    cout << "Dostalem " << mechs << " mechanikow" << endl;
                    
                    this->mechQueue.clear();

                    this->status = Status::FIXING;
                }
            }
            
            processStatus();

            break;

        case Status::FIXING:
            cout << "Jestem w naprawie" << endl;

            sleep(this->damage);

            this->damage = 0;
            this->kAsk = false;
            this->status = Status::AT_WAR;

            for(int i = 0; i < this->size; i++) {
                handler::send_message(REL_MECH, i, this);
                handler::send_message(ACK_DOCK, i, this);
            }

            processStatus();

            break;
    }
}