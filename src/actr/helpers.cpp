#include "actr/helpers.hpp"

#include <typeinfo>
#include <mpi.h>
#include <cstdlib>
#include <string>
#include <vector>


namespace actr {

    template <typename T>
    void send_msg(std::vector<T> &message, int to_whom)
    {
        // TODO: Message size (and vector type) checking
        // Determine the vector data type
        std::string prefix;

        switch (typeid(decltype(message)::value_type))
        {
            case typeid(int):
                prefix = "int_";
                break;

            case typeid(double):
                prefix = "doub";
                break;

            case typeid(float):
                prefix = "floa";
                break;

            case typeid(char):
                prefix = "char";
                break;

            case typeid(unsigned int):
                prefix = "uint";
                break;
        }

        // Setting the type prefix
        char *prefix_ptr = reinterpret_cast<char*>(outbuf),
             *prefix_c = prefix.c_str();

        // Set the prefix
        for (int i = 0; i < 4; ++i)
            prefix_ptr[i] = prefix_c[i];

        // Set the message
        auto message_ptr = reinterpret_cast<T*>(prefix_ptr + 4);

        for (int i = 0; i < message.size(); ++i)
            message_ptr[i] = message[i];

        MPI_Ssend(outbuf, 4 + sizeof(T) * message.size(),
                  MPI_BYTE, to_whom, 0, MPI_COMM_WORLD);
    }

    template <typename T>
    T get_msg(int from)
    {
        MPI_Status status;
        int msg_size;

        MPI_Recv(inbuf, sizeof(inbuf), MPI_BYTE, from,
                 0, MPI_COMM_WORLD, &status);

        MPI_Get_count(&status, MPI_BYTE, &msg_size);

        // Unpack and set correct filetypes
        auto prefix_ptr = reinterpret_cast<char*>(inbuf);
        std::string prefix(inbuf, inbuf + 4);

        std::type_info data_type;
        switch (prefix)
        {
            case std::string("int_"):
                data_type = typeid(int);
                break;

            case std::string("doub"):
                data_type = typeid(double);
                break;

            case std::string("floa"):
                data_type = typeid(float);
                break;

            case std::string("char"):
                data_type = typeid(char);
                break;

            case std::string("uint"):
                data_type = typeid(unsigned int);
                break;
        }

        std::vector<decltype(data_type)> output(prefix_ptr + 4,
                prefix_ptr + msg_size);
        return output;
    }
}

