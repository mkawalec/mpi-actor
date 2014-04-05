#include "actr.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
    // MPI_Init must be called before any action
    // involing ACTR is performed
    MPI_Init(&argc, &argv);

    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    actr::Controller controller;

    // Allocate the frogs, quit if not
    // enough threads are available
    try {
        controller.request_allocation("clock", 1);
        controller.request_allocation("land_cell", 16);
        controller.request_allocation("frog ill", 4);
        controller.request_allocation("frog healthy", 30);
    } catch (const actr::AllocationError& e) {
        if (my_rank == 0)
            std::cerr << e.what() << std::endl;

        MPI_Finalize();
        return 0;
    }

    try {
        MPI_Barrier(MPI_COMM_WORLD);
        controller.execute();
    } catch (const actr::ProgramDeathRequest& e) {
        if (my_rank == 0)
            std::cerr << "End of simulation reached, terminating" << std::endl;
    }

    MPI_Finalize();
    return 0;
}
