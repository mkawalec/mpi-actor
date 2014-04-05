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

        // If the start points were not set at
        // class setup
        if (x == std::numeric_limits<float>::max())
            x = 0.5;
        if (y == std::numeric_limits<float>::max())
            y = 0.5;

        // Set up the histories
        History<int> infection_hist(500), influx_hist(300);
        actr::message msg("", -1);

        // The cell of a last hop
        int cell_id = get_nth_id("land_cell",
                biol::getCellFromPosition(x, y));

        // The main loop of a frog actor
        while (true) {
            // Only jump to a new position, if there is
            // no leftover message from a different process
            // left to compute
            if (msg.second == -1) {
                int land_num;
                float new_x, new_y;
                biol::frogHop(x, y, &new_x, &new_y, &state);
                land_num = biol::getCellFromPosition(new_x, new_y);

                cell_id = get_nth_id("land_cell", land_num);

                x = new_x;
                y = new_y;

                MPI_Request request;
                MPI_Status tmp_status;

                if (healthy)
                    request = actr::send_str("clean", cell_id);
                else
                    request = actr::send_str("infected", cell_id);

                MPI_Wait(&request, &tmp_status);
            }

            // Get the reply from the land cell
            while (msg.second == -1) {
                msg = preprocess_msg(actr::get_str(cell_id));
            }

            auto split = actr::split_and_trim(msg.first, " ");

            influx_hist.insert(std::stoi(split[0]));
            infection_hist.insert(std::stoi(split[1]));

            // Check if birth will happen
            if (hop_count%300 == 0 and
                    biol::willGiveBirth(influx_hist.get_avg(), &state))
                allocate_additional("frog healthy " + std::to_string(x) +
                        " " + std::to_string(y), 1);

            // Check if disease will be caught at this hop
            if (healthy and biol::willCatchDisease(infection_hist.get_avg(),
                        &state))
                healthy = false;

            // Check if the frog will die
            if (!healthy and hop_count%700 == 0 and biol::willDie(&state))
                return;

            hop_count += 1;
            msg = actr::message("", -1);

            // Process any additional commands
            while (actr::is_message() and msg.second == -1) {
                msg = preprocess_msg(actr::get_str());
            }
        }

    }

    void Frog::setup_params(std::vector<std::string> commands)
    {
        // The class instantiate command is of a form
        // #! create class_name params

        if (commands[3] == "ill")
            healthy = false;

        if (commands.size() < 5) return;
        x = std::stof(commands[4]);
        y = std::stof(commands[5]);
    }


    DECLARE_ACTR_PLUGIN(Frog);
}

