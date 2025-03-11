#!/bin/sh
# $1 - path to task
# $2 - executable
# $3 - number of processes
#SBATCH -n $3
#SBATCH -o $2-%j.out
#SBATCH -e $2-%j.err
mpirun -np $3 $1/exe/$2
