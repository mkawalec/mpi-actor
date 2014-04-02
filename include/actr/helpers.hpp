#ifndef actr_helpers_hpp
#define actr_helpers_hpp

#include <mpi.h>
#include <typeinfo>
#include <vector>


namespace actr {
    const int BUF_SIZE = sizeof(int) * 1000000;
    void *outbuf = malloc(4 + BUF_SIZE),
         *inbuf  = malloc(4 + BUF_SIZE);

    template <typename T>
    void send_msg(std::vector<T> &message, int to_whom);

    template <typename T>
    T get_msg(int from=MPI_ANY_SOURCE);

}

#endif
