#ifndef SHIP_DEF
#define SHIP_DEF

#include <vector>
#include <array>
#include <cstdlib>
#include <ctime>
#include <mutex>
#include <thread>
#include <iostream>
#include <algorithm>
#include <math.h>
#include <unistd.h>
#include <mpi.h>

#include "structs.h"

class Ship {

    private:
        static bool sortQueue(const array<int, 3> &a, const array<int, 3> &b);

    public:
        Ship();
        ~Ship();
        Ship(int rank, int size);

        Status status;                   //zmienna określająca stan statku
        int damage;                      //zmienna określająca liczbę uszkodzeń (liczbę potrzebnych do naprawy mechaników)
        bool kAsk;                       //zmienna określająca, czy statek ubiega się o dok
        int lamportTime;                 //zmienna określająca czas przybycia procesu (zegar Lamporta)
        int accepted;                    //zmienna określająca liczbę okrętów nieubiegających się o sekcję krytyczną
        int rank;
        int size;
        int durability;
        vector<array<int, 3>> mechQueue; //kolejka dostępu do mechaników
        vector<int> pending;             //tablica zawierająca okręty, którym należy odpowiedzieć po zwolnieniu doku
        mutex timeMutex;
        mutex queueMutex;
        mutex statusMutex;
        mutex pendMutex;
        MPI_Datatype MSG_WAR;

        void updateTime(int received_time);
        int busyMech();
        void processStatus();
        void processAck(int tag, message mess, int rank);
        void updateQueue(int freed);
        MPI_Datatype createType();

};

void send_message(Msg tag, int rec, Ship* ship);

#endif