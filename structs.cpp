#ifndef STRUCTS_CPP
#define STRUCTS_CPP

#include "structs.h"

Ship::Ship(int rank, int size) {
    this->status = Status::AT_WAR;
    this->rank = rank;
    this->size = size;
    this->Lamport_Time = 0;
    this->kAsk = false;
    this->damage = 0;
    this->pending.resize(size, 0);
}

void Ship::updateTime(int received_time) {
    this->Lamport_Time = max(Lamport_Time, received_time) + 1;
}

#endif