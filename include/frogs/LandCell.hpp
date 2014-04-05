#ifndef frogs_LandCell_hpp
#define frogs_LandCell_hpp

#include "actr.hpp"

#include <fstream>


namespace frogs {

    class LandCell : public actr::ActrBase {
    private:
        int population_influx, infection_level;
        void reset_year();
        std::ofstream *output = NULL;

    protected:
        actr::ActrBase* clone();

    public:
        LandCell();
        ~LandCell();

        void main_loop();
    };

}

#endif
