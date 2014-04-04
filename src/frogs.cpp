#include "actr.hpp"

#include <iostream>


int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);

    actr::Controller controller;

    try {
        controller.request_allocation("clock", 1);
        controller.request_allocation("land_cell", 16);
        controller.request_allocation("frog", 1);
    } catch (const actr::AllocationError& e) {
        std::cerr << e.what() << std::endl;
    }

    controller.execute();

    MPI_Finalize();
    return 0;
}
