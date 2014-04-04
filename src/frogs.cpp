#include "actr.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    actr::Controller controller;
    int my_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    try {
        controller.request_allocation("clock", 1);
        controller.request_allocation("land_cell", 16);
        controller.request_allocation("frog ill", 4);
        MPI_Barrier(MPI_COMM_WORLD);
        if (my_rank == 0) std::cout << "Ill allocation done" << std::endl;
        controller.request_allocation("frog healthy", 20);
    } catch (const actr::AllocationError& e) {
        if (my_rank == 0)
            std::cerr << e.what() << std::endl;

        MPI_Finalize();
        return 0;
    }

    controller.execute();

    MPI_Finalize();
    return 0;
}
