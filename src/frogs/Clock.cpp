#include "frogs/Clock.hpp"
#include "actr.hpp"

namespace frogs {

    Clock::Clock()
    {
        name        = "clock";
        description = "A chronometer announcing year changes";
    }

    actr::ActrBase* Clock::clone()
    {
        return new Clock(*this);
    }

    void Clock::main_loop()
    {
        return;
    }

    DECLARE_ACTR_PLUGIN(Clock);

}
