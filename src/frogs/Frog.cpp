#include "frogs/Frog.hpp"
#include "frogs/History.hpp"
#include "actr.hpp"
#include "biol/frog-functions.hpp"

#include <vector>
#include <string>


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
        if (x == -1)
            x = 0.5;
        if (y == -1)
            y = 0.5;

        History<int> infection_hist(500), influx_hist(300);

        // The main loop of a frog actor
        while (true) {
            actr::message msg = preprocess_msg(actr::get_str());
            if (msg.second == -1) continue;

            // First, let's visit a land cell

            // If the message is not a command, it is a reply from a
            // land cell with its details


        }

    }

    void Frog::setup_params(std::vector<std::string> commands)
    {
        if (commands.size() < 4) return;

        x = std::stoi(commands[2]);
        y = std::stoi(commands[3]);
    }


    DECLARE_ACTR_PLUGIN(Frog);
}

