#include "actr/helpers.hpp"
#include "actr/ActrBase.hpp"
#include "actr/exceptions.hpp"

#include <mpi.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>

#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>



namespace actr {

    std::list<ActrBase*> ActrBase::available;

    ActrBase::ActrBase()
    {
        ActrBase::available.push_back(this);
    }

    ActrBase::~ActrBase()
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it) {
            if (it->first == my_rank)
                continue;

            send_str("#! del " + std::to_string(my_rank), it->first);
        }

        delete instance;
    }

    ActrBase* ActrBase::clone_instance(std::string instance_name)
    {
        for (auto it = ActrBase::available.begin();
                  it != ActrBase::available.end(); ++it) {
            if ((*it)->name == instance_name)
                return (*it)->clone();
        }

        throw ExitWithError();
    }

    void ActrBase::function_watch()
    {
        std::string message = get_str().first;
        if (message == "shutdown")
            throw ProgramDeathRequest();
        else {
            if (instance != NULL)
                delete instance;

            instance = clone_instance(message);
        }
    }

    void ActrBase::set_class_usage(std::map<int, std::string> &usage)
    {
        class_usage = usage;
    }

    void ActrBase::execute()
    {
        // If the current instance is not allocated, it should
        // just terminate gracefully
        if (instance != NULL) {
            instance->set_class_usage(class_usage);
            instance->main_loop();

            delete instance;
            instance = NULL;
        }
    }

    void ActrBase::request_allocation(std::string what, int how_many)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
        if (total_ranks - first_free_rank < how_many)
            throw AllocationError("Not enough processes available");

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        // The master propagates the information about
        // which module the actors must load
        if (my_rank == 0) {
            auto requests = new MPI_Request[how_many];
            for (int i = 0; i < how_many; ++i)
                requests[i] = send_str(what, first_free_rank + i);

            if (first_free_rank == 0) function_watch();

            for (int i = 0; i < how_many; ++i) {
                MPI_Status tmp_status;
                MPI_Wait(&requests[i], &tmp_status);
            }

            delete[] requests;
        }

        if (my_rank >= first_free_rank &&
            my_rank < first_free_rank + how_many) function_watch();

        for (int i = first_free_rank;
                 i < first_free_rank + how_many; ++i)
            class_usage.emplace(i, what);

        first_free_rank += how_many;
    }

    void ActrBase::allocate_additional(std::string what, int how_many)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);

        int set_up = 0;
        std::vector<int> added;
        added.reserve(how_many);

        for (int i = 0; i < total_ranks; ++i) {
            if (class_usage.find(i) != class_usage.end())
                continue;

            MPI_Request request;
            MPI_Status tmp_status;

            request = send_str(what, i);
            MPI_Wait(&request, &tmp_status);
            added.push_back(i);

            class_usage.emplace(i, what);
            ++set_up;

            if (set_up == how_many)
                break;
        }

        for (int i = 0; (unsigned)i < added.size(); ++i)
            update_info(added[i]);

    }

    /* Possible system commands
     * #! del rank -> delete the instance at a given rank
     * #! add rank class_id -> add an instance at a  given rank
     *
     * keywords (optional, added at the end of message, delimited
     * by a ';' sign):
     *
     * cont -> there will be a new message coming, wait for it
     */

    void ActrBase::update_info(int rank)
    {
        // Send update commands to a given rank
        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it) {
            std::string message = "#! add " + std::to_string(it->first)
                + " " + it->second;

            if (it != (--class_usage.end()))
                message += ";cont";

            MPI_Request request;
            MPI_Status tmp_status;

            request = send_str(message, rank);
            MPI_Wait(&request, &tmp_status);
        }
    }

    message ActrBase::preprocess_msg(message msg)
    {
        if (msg.first.find("#!") == std::string::npos)
            return msg;

        std::vector<std::string> keyw, comms;
        boost::split_regex(keyw, msg.first, boost::regex(";"));
        boost::split_regex(comms, keyw[0], boost::regex(" "));

        int rank = std::stoi(comms[2]);
        if (comms[1] == "del") {
            if (class_usage.find(rank) != class_usage.end())
                class_usage.erase(rank);
        } else if (comms[1] == "add") {
            if (class_usage.find(rank) != class_usage.end())
                class_usage.erase(rank);

            class_usage.emplace(rank, comms[3]);
        }

        if (keyw.size() > 1 && keyw[1] == "cont")
            return preprocess_msg(get_str(msg.second));

        return std::make_pair("", -1);
    }

    std::map<std::string, int> ActrBase::get_class_counts()
    {
        std::map<std::string, int> counts;
        for (auto it = class_usage.begin(); it != class_usage.end(); ++it) {
            if (counts.find(it->second) == counts.end())
                counts.emplace(it->second, 0);

            counts.at(it->second) += 1;
        }

        return counts;
    }

}
