#ifndef actr_Controller_hpp
#define actr_Controller_hpp

#include "actr/ActrBase.hpp"


namespace actr {

    /*! This should be created in a target
     *  program in order to book instances and
     *  execute everything
     */
    class Controller : public ActrBase {
    protected:
        ActrBase* clone() { return new Controller(*this);}

    public:
        void main_loop() { }
    };
}

#endif
