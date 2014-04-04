#ifndef actr_helpers_hpp
#define actr_helpers_hpp

#include <mpi.h>
#include <typeinfo>
#include <vector>
#include <string>
#include <utility>


namespace actr {
    const int BUF_SIZE = 100;
    typedef std::pair<std::string, int> message;

    MPI_Request send_str(std::string message, int to_whom);
    message get_str(int from=MPI_ANY_SOURCE);

}

// The preprocessor directive to register new instances
#define DECLARE_ACTR_PLUGIN(clsname) clsname plugin_ ## clsname

#endif
