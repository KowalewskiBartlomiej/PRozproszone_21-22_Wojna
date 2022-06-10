#ifndef COMM_THREAD
#define COMM_THREAD

#include <mpi.h>
#include <structs.h>

namespace handler{

    void recv_message();

    void send_message(Msg tag, int rec);

}

#endif