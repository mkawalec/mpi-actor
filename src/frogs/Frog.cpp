#include "frogs/Frog.hpp"
#include "actr.hpp"

namespace frogs {

    Frog::Frog()
    {
        name        = "frog";
        description = "An actor representing a frog";
    }

    actr::ActrBase* Frog::clone()
    {
        return new Frog(*this);
    }

    void Frog::main_loop()
    {
        return;
    }


    DECLARE_ACTR_PLUGIN(Frog);
}

