#include "frogs/Frog.hpp"
#include "frogs/History.hpp"
#include "actr.hpp"
#include "biol/frog-functions.hpp"

#include <vector>
#include <string>
#include <limits>

#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>


namespace frogs {

    Frog::Frog()
    {
        name        = "frog";
        description = "An actor representing a frog";
    }

    actr::ActrBase* Frog::clone()
    {
        return new Frog(*this);
    }

    void Frog::main_loop()
    {
        int my_rank;
        MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

        // Set up the RNG
        long state = -1 - my_rank;
        biol::initialiseRNG(&state);

        // If the starting conditions
        if (x == std::numeric_limits<float>::max())
            x = 0.5;
        if (y == std::numeric_limits<float>::max())
            y = 0.5;

        History<int> infection_hist(500), influx_hist(300);

        // The main loop of a frog actor
        while (true) {
            std::cout << "Entering main loop in " << my_rank << std::endl;
            // If there is an outstanding message, consume it
            actr::message msg("", -1);
            while (actr::is_message() and msg.second == -1) {
                msg = preprocess_msg(actr::get_str());
            }


            //if (my_rank == 18) std::cout << "1" << std::endl;
            // First, let's execute a hop
            int land_num;
            float new_x, new_y;
            biol::frogHop(x, y, &new_x, &new_y, &state);
            land_num = biol::getCellFromPosition(new_x, new_y);

            x = new_x;
            y = new_y;

            //if (my_rank == 18) std::cout << "2" << std::endl;
            MPI_Request request;
            MPI_Status tmp_status;

            if (healthy)
                request = actr::send_str("clean",
                        get_nth_id("land_cell", land_num));
            else
                request = actr::send_str("infected",
                        get_nth_id("land_cell", land_num));

            //if (my_rank == 18) std::cout << "3" << std::endl;
            //std::cout << "before wait at rank " << my_rank << "to " << land_num << std::endl;
            MPI_Wait(&request, &tmp_status);
            //std::cout << "after wait at rank " << my_rank << std::endl;

            while (msg.second == -1) {
                msg = preprocess_msg(actr::get_str());
            }

            //if (my_rank == 18) std::cout << "4" << std::endl;
            // If the message is not a command, it is a reply from a
            // land cell with its details
            std::vector<std::string> split;
            boost::split_regex(split, msg.first, boost::regex(" "));

            influx_hist.insert(std::stoi(split[0]));
            infection_hist.insert(std::stoi(split[1]));


            //if (my_rank == 18) std::cout << "5" << std::endl;
            // Check if birth will happen
            if (hop_count%300 and
                    biol::willGiveBirth(influx_hist.get_avg(), &state))
                allocate_additional("frog " + std::to_string(x) +
                        " " + std::to_string(y), 1);

            // Check if disease will be caught at this hop
            if (biol::willCatchDisease(infection_hist.get_avg(), &state))
                healthy = false;

            // Check if the frog will die
            if (!healthy and hop_count%700 and biol::willDie(&state))
                return;
            //if (my_rank == 18) std::cout << "6" << std::endl;

            hop_count += 1;
        }

    }

    void Frog::setup_params(std::vector<std::string> commands)
    {
        if (commands.size() < 5) return;

        // The command is of a form
        // #! create class_name params

        x = std::stof(commands[3]);
        y = std::stof(commands[4]);
    }


    DECLARE_ACTR_PLUGIN(Frog);
}

