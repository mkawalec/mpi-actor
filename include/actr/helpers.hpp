#ifndef actr_helpers_hpp
#define actr_helpers_hpp

#include <mpi.h>
#include <typeinfo>
#include <vector>
#include <string>
#include <utility>
#include <list>


namespace actr {
    const int BUF_SIZE = 1000;
    typedef std::pair<std::string, int> message;

    MPI_Request send_str(std::string message, int to_whom);
    message get_str(int from=MPI_ANY_SOURCE, bool force_network=false);

    // Return true if there is an outstanding message
    // waiting from a given source
    bool is_message(int from=MPI_ANY_SOURCE);

    // Returns true if there is a message from a given
    // source laying on the backlog, if not, return false
    bool backlog_has_message(int from);


    template <typename T>
    void unused(T &&) { }

    std::vector<std::string> split_and_trim(std::string command,
            std::string delimiter);

}

// The preprocessor directive to register new instances
#define DECLARE_ACTR_PLUGIN(clsname) auto plugin_ ## clsname = new clsname

#endif
