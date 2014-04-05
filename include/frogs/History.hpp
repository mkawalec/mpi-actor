#ifndef frogs_History_hpp
#define frogs_History_hpp

namespace frogs {

    // History is a simple circular buffer
    // with the ability to compute the average of
    // n of its elements
    template <typename T>
    class History {
    private:
        T* storage;
        int pos, length;

    public:
        History(int max_length);
        virtual ~History();

        float get_avg(int count=-1);
        void insert(T element);
    };
}

#include "frogs/History.cpp"

#endif
