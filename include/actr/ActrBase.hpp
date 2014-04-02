#ifndef actr_ActrBase_hpp
#define actr_ActrBase_hpp

#include <cstdlib>
#include <list>
#include <string>


namespace actr {

    class ActrBase {
    private:
        static std::list<ActrBase*> available;
        std::string name, description;


    public:
        // Registers the instance
        ActrBase();

        void enter_watch();
        virtual void main_loop() = 0;
    };
}

#endif
