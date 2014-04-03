#include "frogs/LandCell.hpp"
#include "actr.hpp"

namespace frogs {

    LandCell::LandCell()
    {
        name = "land_cell";
        description = "A cell being a cell of land";
    }

    actr::ActrBase* LandCell::clone()
    {
        return new LandCell(*this);
    }

    void LandCell::main_loop()
    {
        return;
    }

    DECLARE_ACTR_PLUGIN(LandCell);
}
