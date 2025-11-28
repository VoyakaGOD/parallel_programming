# General

This is a repository with solutions to parallel programming problems using the MPI and OpenMP with C++.

Also here is work with pthreads and scripts for SLURM.

# Make

`make` creates a folder `executables` in the root of the project.

# SLURM

You can use {...}run.sh scripts to work with SLURM queue system:

`./run.sh [task_name] [number of processes] [args...]` for MPI

`./omprun.sh [task_name] [number of threads] [args...]` for OpenMP

# Other folders:

- shared - folder with reusable code base

- {common/special}_tasks - folders with solutions
