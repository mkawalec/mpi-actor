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

    bool backlog_has_message(int from)
    {
        if (from == MPI_ANY_SOURCE)
            return !msg_backlog.empty();

        for (auto it = msg_backlog.begin();
                  it != msg_backlog.end(); ++it) {
            if (it->second == from)
                return true;
        }

        return false;
    }

    bool is_message(int from)
    {
        MPI_Status status;
        int msg_awaits;
        MPI_Iprobe(from, 0, MPI_COMM_WORLD, &msg_awaits, &status);

        return (bool)msg_awaits;
    }


    MPI_Request send_str(std::string message, int to_whom)
    {
        // If there is an outstanding message to the
        // current thread, retrieve it first
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        while (is_message()) {
            msg_backlog.push_back(get_str(MPI_ANY_SOURCE, true));
        }

        MPI_Request request;
        MPI_Issend((char*)message.c_str(), message.size(),
                  MPI_CHAR, to_whom, 0, MPI_COMM_WORLD, &request);

        return request;
    }


    message get_str(int from, bool force_network)
    {
        // If there is any message in the backlog,
        // return it instead of looking in the network
        if (!msg_backlog.empty() and !force_network
                and backlog_has_message(from)) {

            if (from == MPI_ANY_SOURCE) {
                message to_return = msg_backlog.front();
                msg_backlog.pop_front();
                return to_return;
            } else {
                for (auto it = msg_backlog.begin();
                          it != msg_backlog.end(); ++it) {
                    if (it->second == from) {
                        message to_return = *it;
                        msg_backlog.erase(it);
                        return to_return;
                    }
                }
            }
        }

        MPI_Status status;
        int msg_size;

        MPI_Recv(inbuf, sizeof(char) * BUF_SIZE, MPI_CHAR, from,
                 0, MPI_COMM_WORLD, &status);

        // Return a message of an appropriate size, don't
        // include the padding in the message
        MPI_Get_count(&status, MPI_CHAR, &msg_size);
        return std::make_pair(std::string(inbuf, msg_size), status.MPI_SOURCE);
    }

    std::vector<std::string> split_and_trim(std::string command,
            std::string delimiter)
    {
        std::vector<std::string> comms;
        boost::split_regex(comms, command, boost::regex(delimiter));

        // Trim each of the parameters of any spaces, if exist
        for (int i = 0; (unsigned)i < comms.size(); ++i)
            boost::trim(comms[i]);

        return comms;
    }


}

