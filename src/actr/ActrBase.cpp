#include "actr/ActrBase.hpp"

#include <mpi.h>

namespace actr {

    ActrBase::ActrBase()
    {
        ActrBase::available.push_back(this);
    }

    void ActrBase::function_watch()
    {
        // There is a limit on the class
        // name of 20 characters
        auto recv_buffer = new char[buf_length];
        MPI_Recv(recv_buffer, buf_length,
