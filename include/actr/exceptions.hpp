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

    struct ProgramDeathRequest : public Exception {
        using Exception::Exception;
    };
}

#endif
