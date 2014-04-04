#ifndef frogs_LandCell_hpp
#define frogs_LandCell_hpp

#include "actr.hpp"


namespace frogs {

    class LandCell : public actr::ActrBase {
    private:
        int population_influx, infection_level;
        void reset_year();

    protected:
        actr::ActrBase* clone();

    public:
        LandCell();

        void main_loop();
    };

}

#endif
