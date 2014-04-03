#ifndef frogs_LandCell_hpp
#define frogs_LandCell_hpp

#include "actr.hpp"


namespace frogs {

    class LandCell : public actr::ActrBase {
    private:
        int stuff;

    protected:
        actr::ActrBase* clone();

    public:
        LandCell();

        void main_loop();
    };

}

#endif
