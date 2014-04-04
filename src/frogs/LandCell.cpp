#include "frogs/LandCell.hpp"
#include "actr.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <string>
#include <utility>
#include <mpi.h>


namespace frogs {

    LandCell::LandCell()
    {
        name        = "land_cell";
        description = "A cell being a cell of land";
    }

    actr::ActrBase* LandCell::clone()
    {
        return new LandCell(*this);
    }

    void LandCell::main_loop()
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        while (true) {
            std::cout << "About to get a msg at " << my_rank << std::endl;

            actr::message msg = preprocess_msg(actr::get_str());
            if (msg.second == -1) continue;
            std::cout << "Received at " << my_rank << std::endl;

            MPI_Request request;
            MPI_Status status;

            if (msg.first == "infected") {
                infection_level += 1;
                population_influx += 1;
                request = actr::send_str(std::to_string(population_influx) + " "
                        + std::to_string(infection_level), msg.second);
                MPI_Wait(&request, &status);

            } else if (msg.first == "clean") {
                population_influx += 1;
                request = actr::send_str(std::to_string(population_influx) + " "
                        + std::to_string(infection_level), msg.second);
                MPI_Wait(&request, &status);
            } else if (msg.first == "new_year") {
                reset_year();
            } else if (msg.first == "terminate") {
                throw actr::ProgramDeathRequest();
            }

        }

    }

    void LandCell::reset_year()
    {
        population_influx = 0;
        infection_level   = 0;
    }

    DECLARE_ACTR_PLUGIN(LandCell);
}
