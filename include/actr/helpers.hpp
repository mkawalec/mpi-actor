#ifndef actr_helpers_hpp
#define actr_helpers_hpp

#include <mpi.h>
#include <typeinfo>
#include <vector>
#include <string>
#include <utility>
#include <list>


namespace actr {
    // The maximum length of a message in bytes
    const int BUF_SIZE = 1000;
    typedef std::pair<std::string, int> message;

    // Sends a *message* to a process with rank *to_whom*
    MPI_Request send_str(std::string message, int to_whom);

    // Returns a message from rank *from*. It is a blocking
    // procedure that will block until a message is received.
    //
    // If *force_network* is set to true, a message will be
    // pulled from the network and not from the backlog
    message get_str(int from=MPI_ANY_SOURCE, bool force_network=false);

    // Return true if there is an outstanding message
    // waiting from a given source on the network
    bool is_message(int from=MPI_ANY_SOURCE);

    // Returns true if there is a message from a given
    // source present in the backlog, if not, return false
    bool backlog_has_message(int from);


    // Used to mark a parameter as unused
    template <typename T>
    void unused(T &&) { }

    // Return a vector of commands from a *command* string
    std::vector<std::string> split_and_trim(std::string command,
            std::string delimiter);

}

// The preprocessor directive to register new
// plug-in instances. Causes a tiny memory leak, which
// is negligible on the scale of a program
#define DECLARE_ACTR_PLUGIN(clsname) auto plugin_ ## clsname = new clsname

#endif
