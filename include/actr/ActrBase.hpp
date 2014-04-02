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
        const int buf_length = 20;

        ActrBase* clone_instance(std::string instance_name);
        virtual ActrBase* clone() = 0;

    public:
        // Registers the instance
        ActrBase();

        void function_watch();
        virtual void main_loop() = 0;
    };
}

#endif
