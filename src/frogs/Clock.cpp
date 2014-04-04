#include "frogs/Clock.hpp"
#include "actr.hpp"

#include <chrono>
#include <thread>


namespace frogs {

    Clock::Clock()
    {
        name        = "clock";
        description = "A chronometer announcing year changes";
    }

    actr::ActrBase* Clock::clone()
    {
        return new Clock(*this);
    }

    void Clock::main_loop()
    {
        // A clock class periodically sends an end-of-year
        // message to all cells it knows of
        std::this_thread::sleep_for(std::chrono::milliseconds(year_interval));

        int land_count = get_class_counts().at("land_cell");
        auto requests = new MPI_Request[land_count];

        int j = 0;
        for (auto it = class_usage.begin();
                  it != class_usage.end(); ++it, ++j) {
            if (it->second == "land_cell")
                requests[j] = actr::send_str("new_year", it->first);
        }

        MPI_Status tmp_status;
        for (int i = 0; i < land_count; ++i)
            MPI_Wait(&requests[i], &tmp_status);

        delete[] requests;
    }

    DECLARE_ACTR_PLUGIN(Clock);

}
