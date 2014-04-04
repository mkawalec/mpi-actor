#include "frogs/Clock.hpp"
#include "actr.hpp"

#include <chrono>
#include <thread>
#include <iostream>

namespace frogs {

    Clock::Clock()
    {
        name          = "clock";
        description   = "A chronometer announcing year changes";
        has_eventloop = false;
    }

    actr::ActrBase* Clock::clone()
    {
        return new Clock(*this);
    }

    void Clock::main_loop()
    {
        while (true) {
            // A clock class periodically sends an end-of-year
            // message to all cells it knows of
            std::this_thread::sleep_for(std::chrono::milliseconds(year_interval));

            int land_count = get_class_counts().at("land_cell");
            auto requests = new MPI_Request[land_count];


            int j = 0;
            for (auto it = class_usage.begin();
                      it != class_usage.end(); ++it) {
                if (it->second == "land_cell")
                    requests[j++] = actr::send_str("new_year", it->first);
            }

            MPI_Status tmp_status;
            for (int i = 0; i < land_count; ++i)
                MPI_Wait(&requests[i], &tmp_status);

            delete[] requests;

            years_passed += 1;
            std::cout << "3" << std::endl;

            // If the simulation is supposed to end
            // send a death command to all the processes
            if (years_passed == 100) {
                int my_rank, process_count;
                MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
                MPI_Comm_size(MPI_COMM_WORLD, &process_count);

                auto requests = new MPI_Request[process_count - 1];

                j = 0;
                for (auto i = 0; i < process_count; ++i) {
                    if (i != my_rank)
                        requests[j++] = actr::send_str("#! die", i);
                }

                for (int i = 0; i < process_count - 1; ++i) {
                    MPI_Status tmp_status;
                    MPI_Wait(&requests[i], &tmp_status);
                }

                delete[] requests;
                throw actr::ProgramDeathRequest();
            }

        }
    }

    DECLARE_ACTR_PLUGIN(Clock);

}
