mpi-actr
=========

Actor model in written in C++11 and with the help of MPI

In order to compile please make sure you have at least GCC 4.8.2 or a fairly new version of Clang available. The solution uses CMake to generate makefiles. In order to build it, please do the following:

    cd build
    cmake ..
    make -j3

The successfully executed build operation creates three files:

* *libactr.so* containing compiled ACTR library. The header files are available at include/actr/. Note that it is only needed to include include/actr.hpp in order to include the whole framework
* *libbiol.so* containing the functions supplied by the biologists. Headers are available at include/biol/
* *frogs*, which is an executable example application written with the ACTR framework.


## Important notes

The frogs, an example application using the ACTR framework requires to be ran on at least 51 threads using

    mpirun -n 51 ./frogs

but preferably, if the simulation is to have any signs of sensibility more threads are required. In order to fulfil the requirements from the assignment, 117 threads are required.

Additionally, when using the ACTR framework remember to set *has_eventloop* to false if the class you have just created as a plug-in is not capable of processing an arbitrary amount of ACTR commands.


