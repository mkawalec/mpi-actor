#ifndef frogs_Frog_hpp
#define frogs_Frog_hpp

#include "actr.hpp"

namespace frogs {

    class Frog : public actr::ActrBase {
    private:
        int stuff;

    protected:
        actr::ActrBase* clone();

    public:
        Frog();

        void main_loop();
    };
}

#endif
