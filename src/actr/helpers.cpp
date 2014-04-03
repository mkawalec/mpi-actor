#include "actr/helpers.hpp"

#include <typeinfo>
#include <mpi.h>
#include <cstdlib>
#include <string>
#include <vector>


namespace actr {

    /** Sends a string asynchronously and returns the
     *  request handle, on which MPI_Wait must be called to
     *  resolve the request.
     */
    MPI_Request send_str(std::string &message, int to_whom)
    {
        MPI_Request request;
        MPI_Issend((char*)message.c_str(), message.size() + 1,
                  MPI_CHAR, to_whom, 0, MPI_COMM_WORLD, &request);

        return request;
    }


    std::string get_str(int from)
    {
        MPI_Status status;
        int msg_size;
        char* inbuf = new char[BUF_SIZE];

        MPI_Recv(inbuf, sizeof(char) * BUF_SIZE, MPI_CHAR, from,
                 0, MPI_COMM_WORLD, &status);

        MPI_Get_count(&status, MPI_CHAR, &msg_size);
        return std::string(inbuf, msg_size);
    }
}

