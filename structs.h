#ifndef STRUCT_H
#define STRUCT_H

using namespace std;

//STAŁE
//////////////////////
#define SHIPS 2
#define DOCKS 1
#define MECHANICS 10
#define MAXTIME 10
#define HP 7
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