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
#include <iostream>


namespace actr {

    // A list is used for message backlog,
    // as operations are O(1) dependent on the length
    // of the backlog
    std::list<message> msg_backlog;
    char* inbuf = new char[BUF_SIZE];

    bool is_message(int from)
    {
        MPI_Status status;
        int msg_awaits;
        MPI_Iprobe(from, 0, MPI_COMM_WORLD, &msg_awaits, &status);

        return (bool) msg_awaits;
    }


    /** Sends a string asynchronously and returns the
     *  request handle, on which MPI_Wait must be called to
     *  resolve the request.
     */
    MPI_Request send_str(std::string message, int to_whom)
    {
        // If there is an outstanding message from this id,
        // retrieve it first

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
            //if (my_rank == 18) std::cout << "I" << std::endl;

        while (is_message()) {
            //if (my_rank == 18) std::cout << "II" << std::endl;
            msg_backlog.push_back(get_str());
        }

            //if (my_rank == 18) std::cout << "III" << std::endl;
        MPI_Request request;
        MPI_Issend((char*)message.c_str(), message.size(),
                  MPI_CHAR, to_whom, 0, MPI_COMM_WORLD, &request);

            //if (my_rank == 18) std::cout << "IV" << std::endl;
        return request;
    }


    std::pair<std::string, int> get_str(int from)
    {
        // If there is any message in the backlog,
        // return it instead of looking in the network
        if (!msg_backlog.empty()) {
            message to_return = msg_backlog.front();
            msg_backlog.pop_front();
            return to_return;
        }

        MPI_Status status;
        int msg_size;

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

