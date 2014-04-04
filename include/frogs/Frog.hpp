#ifndef frogs_Frog_hpp
#define frogs_Frog_hpp

#include "actr.hpp"

namespace frogs {

    class Frog : public actr::ActrBase {
    private:
        int x=-1, y=-1;
        bool healthy = true;

    protected:
        actr::ActrBase* clone();
        void setup_params(std::vector<std::string> commands);

    public:
        Frog();

        void main_loop();
    };
}

#endif
