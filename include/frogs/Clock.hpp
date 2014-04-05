#ifndef frogs_Clock_hpp
#define frogs_Clock_hpp

#include "actr.hpp"

namespace frogs {

    /*! This is a clock class, enforcing years
     *  and simulation finish time
     */
    class Clock : public actr::ActrBase {
    private:
        int year_interval = 200;
        int years_passed = 0;

    protected:
        actr::ActrBase* clone();

    public:
        Clock();

        void main_loop();
    };
}

#endif
