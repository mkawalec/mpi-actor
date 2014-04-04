#ifndef actr_ActrBase_hpp
#define actr_ActrBase_hpp

#include "actr/helpers.hpp"

#include <cstdlib>
#include <list>
#include <string>
#include <map>
#include <utility>


namespace actr {

    class ActrBase {
    private:
        static std::list<ActrBase*> available;
        const int buf_length = 20;

        // Inner MPI related bookkeeping
        int total_ranks, first_free_rank = 0;

        ActrBase* clone_instance(std::string instance_name);
        ActrBase* instance = NULL;

        // Initiate the watch for a role that is designated to the
        // current thread
        void function_watch();

    protected:
        std::string name, description;
        virtual ActrBase* clone() = 0;
        std::map<std::string, int> get_class_counts();
        message preprocess_msg(message msg);

        void update_info(int rank);

        // The following holds information about what
        // classes do processes with different rank hold
        std::map<int, std::string> class_usage;

    public:
        // Registers the instance
        ActrBase();
        virtual ~ActrBase();

        // The main loop, overloaded by
        // child classes to perform some work
        virtual void main_loop() = 0;
        void execute();

        void request_allocation(std::string what, int how_many);
        void set_class_usage(std::map<int, std::string> &usage);

        // Called by just one thread to allocate additional instances
        // of a given class
        void allocate_additional(std::string what, int how_many);
    };
}

#endif
