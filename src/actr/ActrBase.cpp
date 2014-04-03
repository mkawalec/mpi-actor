#include "actr/helpers.hpp"
#include "actr/ActrBase.hpp"
#include "actr/exceptions.hpp"

#include <mpi.h>
#include <iostream>
#include <string>
#include <map>

namespace actr {

    std::list<ActrBase*> ActrBase::available;

    ActrBase::ActrBase()
    {
        ActrBase::available.push_back(this);
    }

    ActrBase::~ActrBase()
    {
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
        std::string message = get_str(0);
        if (message == "shutdown")
            throw ProgramDeathRequest();
        else {
            instance = clone_instance(message);
        }
    }

    void ActrBase::execute()
    {
        // If the current instance is not allocated, it should
        // just terminate gracefully
        if (instance == NULL)
            throw ProgramDeathRequest();

        instance->main_loop();
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
