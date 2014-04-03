#ifndef frogs_Clock_hpp
#define frogs_Clock_hpp

#include "actr.hpp"

namespace frogs {

    class Clock : public actr::ActrBase {
    private:
        int stuff;

    protected:
        actr::ActrBase* clone();

    public:
        void main_loop();
    };
}

#endif
