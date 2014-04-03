#ifndef actr_exceptions_hpp
#define actr_exceptions_hpp

#include <string>

namespace actr {

    struct Exception {
    protected:
        std::string message;

    public:
        Exception() { message = "";}
        Exception(std::string msg) { message = msg; }
        std::string what() const { return message;}
    };

    // Thrown when the program intends to terminate
    struct ProgramDeathRequest : public Exception {
        using Exception::Exception;
    };

    // Thrown when the program wants to terminate
    // unconditionally, no recovery should be possible
    struct ExitWithError : public Exception {
        using Exception::Exception;
    };

    // If an error during role allocation phase was
    // encountered, this is used. Handle accordingly.
    struct AllocationError : public Exception {
        using Exception::Exception;
    };
}

#endif
