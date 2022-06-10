#include "structs.h"

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
    this->lamportTime = max(lamportTime, received_time) + 1;
}

void Ship::processStatus(){
    cout << this->rank << endl;
}
