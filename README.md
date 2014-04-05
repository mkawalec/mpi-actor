mpi-actor
=========

Actor model in written in C++11 and with the help of MPI

In order to compile please make sure you have the latest version of GCC or a fairly new version of Clang available. The solution uses CMake to generate makefiles. In order to build it, please do the following:

    cd build
    cmake ..
    make -j3


## Important notes

The frogs, an example application using the ACTR framework requires to be ran on at least 51 threads using

    mpirun -n 51 ./frogs

but preferably, if the simulation is to have any signs of sensibility more threads are required. In order to fulfil the requirements from the assignment, 117 threads are required.

Additionally, when using the ACTR framework remember to set *has_eventloop* to false if the class you have just created as a plug-in is not capable of processing an arbitrary amount of ACTR commands.


