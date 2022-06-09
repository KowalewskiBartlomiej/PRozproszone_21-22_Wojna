#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include <array>

using namespace std;

//STAŁE
//////////////////////
#define SHIPS 4
#define DOCKS 2
#define MECHANICS 10
#define MAXTIME 60
#define HP 7
//////////////////////

enum Status {
    AT_WAR,                              //Na wojnie
    SEARCHING_DOCK,                      //Szuka doku
    SEARCHING_MECHANICS,                 //Szuka mechaników
    FIXING                               //W naprawie
};

enum Msg {
    REQ_DOCK,                            //Żądanie doku
    ACK_DOCK,                            //Akceptacja żądania doku
    REQ_MECH,                            //Żądanie mechaników
    ACK_MECH,                            //Akceptacja żądania mechaników
    REL_MECH                             //Zwolnienie mechaników
};

class Ship {
    public:
        Status status;                   //zmienna określająca stan statku
        int damage;                      //zmienna określająca liczbę uszkodzeń (liczbę potrzebnych do naprawy mechaników)
        bool kAsk;                       //zmienna określająca, czy statek ubiega się o dok
        int Lamport_Time;                //zmienna określająca czas przybycia procesu (zegar Lamporta)
        int rank;
        int size;
        vector<array<int, 2>> mechQueue; //kolejka dostępu do mechaników
        vector<int> pending;             //tablica zawierająca okręty, którym należy odpowiedzieć po zwolnieniu doku

        Ship(int rank, int size);
        void updateTime(int received_time);
};

#endif