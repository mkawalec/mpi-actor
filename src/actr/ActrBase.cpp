#include "actr/helpers.hpp"
#include "actr/ActrBase.hpp"
#include "actr/exceptions.hpp"

#include <mpi.h>
#include <iostream>
#include <string>

namespace actr {

    ActrBase::ActrBase()
    {
        ActrBase::available.push_back(this);
    }

    ActrBase* ActrBase::clone_instance(std::string)
    {
        for (auto it = ActrBase::available.begin();
                  it != ActrBase::available.end(); ++it) {
            continue;
        }
    }


    void ActrBase::function_watch()
    {
        std::string message = get_str(0);
        if (message == "shutdown")
            throw ProgramDeathRequest();
        else {
            auto instance = clone_instance(message);
            instance->main_loop();
        }
    }
}
