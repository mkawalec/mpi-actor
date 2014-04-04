#ifndef actr_Controller_hpp
#define actr_Controller_hpp

#include "actr/ActrBase.hpp"


namespace actr {

    class Controller : public ActrBase {
    protected:
        ActrBase* clone() { return new Controller(*this);}

    public:
        void main_loop() { }
    };
}

#endif
