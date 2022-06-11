#ifndef STRUCT_H
#define STRUCT_H

using namespace std;

//STAŁE
//////////////////////
#define DOCKS 3
#define MECHANICS 24
#define MAXTIME 20
#define HP 16
#define WAIT_TIME 1
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

typedef struct {
    int lamportTime;                     //czas lamporta           
    int mechNumber;                      //liczba potrzebnych mechaników
} message;



#endif