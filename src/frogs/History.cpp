#include <limits>


namespace frogs {

    template <typename T>
    History<T>::History(int max_length)
    {
        storage = new T[max_length];

        length = max_length;
        pos = 0;
    }

    template <typename T>
    History<T>::~History()
    {
        delete storage;
    }

    template <typename T>
    void History<T>::insert(T element)
    {
        storage[pos%length] = element;

        // We don't want an overflow if too
        // many insert operations were made
        if (pos == std::numeric_limits<int>::max())
            pos = length + pos%length;
        else
            pos += 1;
    }

    template <typename T>
    float History<T>::get_avg(int count)
    {
        if (count == -1)
            count = length;

        // If there is not enough elements
        // or an average of more elements then length
        // is requested, we cannot be of any help
        if (pos < count or count > length) return 0;

        double avg = 0;
        for (int i = pos - 1; i > pos - count - 1; --i)
            avg += storage[pos%length];

        return avg / count;
    }
}





