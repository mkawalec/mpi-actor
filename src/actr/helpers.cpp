#include "actr/helpers.hpp"

#include <typeinfo>
#include <mpi.h>
#include <cstdlib>
#include <string>
#include <vector>
#include <utility>

#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/algorithm/string.hpp>


namespace actr {

    /** Sends a string asynchronously and returns the
     *  request handle, on which MPI_Wait must be called to
     *  resolve the request.
     */
    MPI_Request send_str(std::string message, int to_whom)
    {
        MPI_Request request;
        MPI_Issend((char*)message.c_str(), message.size(),
                  MPI_CHAR, to_whom, 0, MPI_COMM_WORLD, &request);

        return request;
    }


    std::pair<std::string, int> get_str(int from)
    {
        MPI_Status status;
        int msg_size;
        char* inbuf = new char[BUF_SIZE];

        MPI_Recv(inbuf, sizeof(char) * BUF_SIZE, MPI_CHAR, from,
                 0, MPI_COMM_WORLD, &status);

        MPI_Get_count(&status, MPI_CHAR, &msg_size);
        return std::make_pair(std::string(inbuf, msg_size), status.MPI_SOURCE);
    }

    std::vector<std::string> split_and_trim(std::string command,
            std::string delimiter)
    {
        std::vector<std::string> comms;
        boost::split_regex(comms, command, boost::regex(delimiter));

        for (int i = 0; (unsigned)i < comms.size(); ++i)
            boost::trim(comms[i]);

        return comms;
    }


}

