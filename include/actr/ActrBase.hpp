#ifndef actr_ActrBase_hpp
#define actr_ActrBase_hpp

#include "actr/helpers.hpp"

#include <cstdlib>
#include <list>
#include <string>
#include <map>
#include <utility>


namespace actr {

    /*! This is a main class which must be inherited from by
     *  any instances willing to use the ACTR framework.
     *  The virtual functions needs to be overloaded in child
     *  classes to make usage reasonable.
     */
    class ActrBase {
    private:
        // A list containing all available classes that can be
        // loaded in runtime by this thread.
        static std::list<ActrBase*> available;

        // MPI related bookkeeping
        int total_ranks, first_free_rank = 0;

        // Returns a pointer to a new instance described
        // by the spec *command*. The returned instance
        // should be properly pre-initialized
        ActrBase* clone_instance(std::string command);

        // Pointer to an instance that can be ran on its own
        ActrBase* instance = NULL;

        // Initiate the watch for a role that is designated to the
        // current thread
        void function_watch();

        // Return true if a class at a given *rank* has
        // an event loop capable of retrieving
        // messages
        bool is_eventloop_available(int rank);

    protected:
        // The name and description of a current instance
        std::string name, description;

        // Overloaded by child classes returns a pointer
        // to a new instance of the child class
        virtual ActrBase* clone() = 0;

        // Returns the amounts of instances of different
        // classes, is essentially an inverse of class usage
        std::map<std::string, int> get_class_counts();

        // If a message contains a framework command, apply
        // the command and return a message("", -1), otherwise
        // return the message itself
        message preprocess_msg(message msg);

        // If a new instance was created at *rank*, the following
        // notifies all currently existing instances of that
        // fact as well as gives the newly created instance
        // a current information about other instances
        void update_info(int rank);

        // The following holds information about which
        // classes are held at which rank
        std::map<int, std::string> class_usage;

        // If the class accepts more parameters at its
        // setup stage, overload the following in order
        // to apply these parameters
        virtual void setup_params(std::vector<std::string> commands)
        {
            unused(commands);
        }

        // Returns a rank of an n-th instance of a given class
        int get_nth_id(std::string classname, int which);

        // The main loop, overloaded by
        // child classes to perform some work
        virtual void main_loop() = 0;

        // Only notify the class of changes if it has
        // a chance of processing the message
        bool has_eventloop = true;

        // Used to set information about class usage
        // to another instance of class_usage
        void set_class_usage(std::map<int, std::string> &usage);

    public:
        ActrBase();
        virtual ~ActrBase();

        // Called in order to start the main loop
        void execute();

        // Called to request an allocation of *how_many*
        // instances of a class *what*
        void request_allocation(std::string what, int how_many);

        // Called (by ONE THREAD only) to allocate more instances
        // of a given class. If the class is not available,
        // a standard ExitWithError() exception is thrown at a
        // target rank
        void allocate_additional(std::string what, int how_many);
    };
}

#endif
