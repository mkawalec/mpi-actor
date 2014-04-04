#include "actr/helpers.hpp"
#include "actr/ActrBase.hpp"
#include "actr/exceptions.hpp"

#include <mpi.h>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <cstdlib>


#include <iostream>

namespace actr {

    std::list<ActrBase*> ActrBase::available;

    ActrBase::ActrBase()
    {
        ActrBase::available.push_back(this);
    }

    ActrBase::~ActrBase()
    {
        if (instance != NULL)
            delete instance;

        int finalized;
        MPI_Finalized(&finalized);
        if (finalized) return;

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it) {
            if (it->first == my_rank)
                continue;

            send_str("#! del " + std::to_string(my_rank), it->first);
        }
    }

    ActrBase* ActrBase::clone_instance(std::string command)
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        srand(my_rank);
        auto keyw = split_and_trim(command, " ");

        for (auto it = ActrBase::available.begin();
                  it != ActrBase::available.end(); ++it) {
            if ((*it)->name == keyw[2]) {
                if (instance != NULL)
                    delete instance;

                auto instance = (*it)->clone();
                instance->setup_params(keyw);

                return instance;
            }
        }

        throw ExitWithError();
    }

    void ActrBase::function_watch()
    {
        while (instance == NULL) {
            preprocess_msg(get_str());
        }
    }

    void ActrBase::set_class_usage(std::map<int, std::string> &usage)
    {
        class_usage = usage;
    }

    void ActrBase::execute()
    {
        while (true) {
            if (instance == NULL)
                function_watch();

            // If the current instance is not allocated, it should
            // just terminate gracefully
            if (instance != NULL) {
                instance->set_class_usage(class_usage);
                instance->main_loop();
                std::cout << "OUT OF MAIN LOOP!!!!!!!" << std::endl;

                if (instance != NULL)
                    delete instance;

                instance = NULL;
            }
        }
    }

    void ActrBase::request_allocation(std::string what, int how_many)
    {
        //std::cout << "request_allocation called" << std::endl;
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
        if (total_ranks - first_free_rank < how_many)
            throw AllocationError("Not enough processes available");

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        what = "#! create " + what;

        auto split = split_and_trim(what, " ");
        std::string class_name = split[2];

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
        } else if (my_rank >= first_free_rank &&
            my_rank < first_free_rank + how_many) function_watch();

        for (int i = first_free_rank;
                 i < first_free_rank + how_many; ++i)
            class_usage.emplace(i, class_name);

        first_free_rank += how_many;
    }

    void ActrBase::allocate_additional(std::string what, int how_many)
    {
        std::cout << "Allocating additional " << what << std::endl;
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
        what = "#! create " + what;

        auto split = split_and_trim(what, " ");
        std::string class_name = split[2];

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

            class_usage.emplace(i, class_name);
            ++set_up;

            if (set_up == how_many)
                break;
        }

        for (int i = 0; (unsigned)i < added.size(); ++i)
            update_info(added[i]);

    }

    bool ActrBase::is_eventloop_available(int rank)
    {
        std::string class_name = class_usage.at(rank);
        for (auto it = ActrBase::available.begin();
                  it != ActrBase::available.end(); ++it) {
            if ((*it)->name == class_name)
                return (*it)->has_eventloop;
        }

        throw InstanceNotFound();
    }


    /* Possible system commands
     * #! del rank -> delete the instance at a given rank
     * #! add rank class_id -> add an instance at a  given rank
     * #! die -> terminates an instance
     * #! create class_name params
     *
     * keywords (optional, added at the end of message, delimited
     * by a ';' sign):
     *
     * cont -> there will be a new message coming, wait for it
     */

    void ActrBase::update_info(int rank)
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        // Send update commands to the new class
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
            //std::cout << "Sent info on " << it->first << " as: " << message << std::endl;
        }

        // Notify the old instances about the new class
        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it) {
            if (it->first == my_rank or !is_eventloop_available(it->first) or
                    it->first == rank)
                continue;

            MPI_Request request;
            MPI_Status tmp_status;

            request = send_str("#! add " + std::to_string(rank)
                    + " " + class_usage.at(rank), it->first);
            int num = rand();
            //std::cout << "Wait " << num << " sent from " << my_rank << " to " << it->first << std::endl;
            MPI_Wait(&request, &tmp_status);
            //std::cout << "Completed " << num << std::endl;
        }
    }

    message ActrBase::preprocess_msg(message msg)
    {
        if (msg.first.find("#!") == std::string::npos)
            return msg;

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
        //std::cout << "command detected in: " << msg.first << ":: at rank " << my_rank << " from " << std::to_string(msg.second) << std::endl;
        auto keyw = split_and_trim(msg.first, ";");
        auto comms = split_and_trim(keyw[0], " ");

        if (comms[1] == "del") {
            int rank = std::stoi(comms[2]);
            if (class_usage.find(rank) != class_usage.end())
                class_usage.erase(rank);
        } else if (comms[1] == "add") {
            int rank = std::stoi(comms[2]);
            if (class_usage.find(rank) != class_usage.end())
                class_usage.erase(rank);

            class_usage.emplace(rank, comms[3]);
        } else if (comms[1] == "die") {
            throw ProgramDeathRequest();
        } else if (comms[1] == "create") {
            instance = clone_instance(msg.first);
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

    int ActrBase::get_nth_id(std::string classname, int which)
    {
        int found = 0;
        for (auto it = class_usage.begin(); it != class_usage.end(); ++it) {
            if (it->second == classname) {
                if (found == which)
                    return it->first;

                found += 1;
            }
        }

        throw InstanceNotFound();
    }

}
