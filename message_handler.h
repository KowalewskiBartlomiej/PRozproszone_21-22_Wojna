#pragma once

#include <mpi.h>

#include "structs.h"

namespace handler{

    void recv_message(Ship* ship);

    void send_message(Msg tag, int rec, Ship* ship);

}