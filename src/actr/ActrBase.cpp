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

    // A forward declaration of a static property
    std::list<ActrBase*> ActrBase::available;

    // The constructor manages registration of
    // the current instance
    ActrBase::ActrBase()
    {
        name = "base_class";
        description = "an uninitialized base class";

        ActrBase::available.push_back(this);
    }

    // If MPI is still available in the current program
    // (the destruction happens during program execution),
    // notify all other instances that this class
    // is about to be destroyed
    ActrBase::~ActrBase()
    {
        if (instance != NULL)
            delete instance;

        int finalized;
        MPI_Finalized(&finalized);
        if (finalized) return;

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        MPI_Status tmp_status;
        std::vector<MPI_Request> requests;
        requests.reserve(class_usage.size());

        // Send a message to all classes that
        // are capable of receiving it
        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it) {
            if (it->first == my_rank or !is_eventloop_available(it->first))
                continue;

            requests.push_back(send_str(
                        "#! del " + std::to_string(my_rank), it->first));
        }

        // Make sure that the requests complete
        for (int i = 0; (unsigned)i < requests.size(); ++i)
            MPI_Wait(&requests[i], &tmp_status);
    }

    ActrBase* ActrBase::clone_instance(std::string command)
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
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

        // If the requested instance does not exist in the
        // available instances memory area, raise an exception
        throw ExitWithError();
    }

    void ActrBase::function_watch()
    {
        // Wait for a in instance creation
        // message, ignoring all the others
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
            // Instantiate the worker class if it
            // does not exist yet
            if (instance == NULL)
                function_watch();

            // If the current instance didn't get allocated
            // properly, just repeat the process
            if (instance != NULL) {
                instance->set_class_usage(class_usage);
                instance->main_loop();

                // If the main loop terminates, it is
                // a sign that the instance can be discarded
                if (instance != NULL)
                    delete instance;

                instance = NULL;
            }
        }
    }

    void ActrBase::request_allocation(std::string what, int how_many)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);
        if (total_ranks - first_free_rank < how_many)
            throw AllocationError("Not enough processes available");

        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        // The command must me made compatible with the
        // communication protocol
        what = "#! create " + what;

        auto split = split_and_trim(what, " ");
        std::string class_name = split[2];

        // The master propagates the information about
        // which module the actors must load
        if (my_rank == 0) {
            auto requests = new MPI_Request[how_many];
            for (int i = 0; i < how_many; ++i)
                requests[i] = send_str(what, first_free_rank + i);

            // If the master has no function allocated, it
            // should set itself up first
            if (first_free_rank == 0) function_watch();

            for (int i = 0; i < how_many; ++i) {
                MPI_Status tmp_status;
                MPI_Wait(&requests[i], &tmp_status);
            }


            delete[] requests;
        } else if (my_rank >= first_free_rank &&
            my_rank < first_free_rank + how_many) function_watch();

        // Add information about the classes
        // allocated in the previous step to all ranks
        for (int i = first_free_rank;
                 i < first_free_rank + how_many; ++i)
            class_usage.emplace(i, class_name);

        first_free_rank += how_many;
    }

    void ActrBase::allocate_additional(std::string what, int how_many)
    {
        MPI_Comm_size(MPI_COMM_WORLD, &total_ranks);

        // Make the command compatible with the protocol
        what = "#! create " + what;

        auto split = split_and_trim(what, " ");
        std::string class_name = split[2];

        int set_up = 0;
        std::vector<int> added;
        added.reserve(how_many);

        // Look for enough unallocated spaces at which
        // the new class can be allocated, terminate when
        // enough instance had been allocated
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

        // Raise an exception if there is no instance
        // with a given rank
        throw InstanceNotFound();
    }


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
            MPI_Wait(&request, &tmp_status);
        }
    }

    message ActrBase::preprocess_msg(message msg)
    {
        if (msg.first.find("#!") == std::string::npos)
            return msg;

        auto keyw = split_and_trim(msg.first, ";");
        auto comms = split_and_trim(keyw[0], " ");

        // Apply cations of a given command
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

        // If there will be subsequent messages,
        // retrieve them
        if (keyw.size() > 1 && keyw[1] == "cont")
            return preprocess_msg(get_str(msg.second));

        // Otherwise, return the 'this message contained
        // a command' magic value
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

        // If there is not enough instances of a given
        // class, an exception is raised
        throw InstanceNotFound();
    }

}
