#ifndef COMM_HANDLER
#define COMM_HANDLER

#include <mpi.h>

#include "ship.h"

namespace handler{

    void recv_message(Ship* ship);

}

#endif