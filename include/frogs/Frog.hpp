#ifndef frogs_Frog_hpp
#define frogs_Frog_hpp

#include "actr.hpp"

#include <limits>


namespace frogs {

    /*! The main initiator of actions,
     *  a frog, jumps around.
     */
    class Frog : public actr::ActrBase {
    private:
        int hop_count = 0;
        float x = std::numeric_limits<float>::max(),
              y = std::numeric_limits<float>::max();
        bool healthy = true;

    protected:
        actr::ActrBase* clone();

        // A frog needs a custom parameter processor
        void setup_params(std::vector<std::string> commands);

    public:
        Frog();

        void main_loop();
    };
}

#endif
