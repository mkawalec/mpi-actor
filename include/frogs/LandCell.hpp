#ifndef frogs_LandCell_hpp
#define frogs_LandCell_hpp

#include "actr.hpp"

#include <fstream>


namespace frogs {

    /*! A land cell replies to messages from
     *  Frogs and consumes messages from a Clock
     */
    class LandCell : public actr::ActrBase {
    private:
        int population_influx, infection_level;

        // Action applied when year end is detected
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
