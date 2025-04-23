#!/bin/sh

# ./grun.sh [name] [SLURM/MPI number of processes] [args...]
# special for hgol

name="$1"
np="$2"
shift 2
sbatch --ntasks=$np --ntasks-per-node=1 --error=results/$name-%j.err --output=results/$name-%j.out h_SLURM_script.sh executables/$name $np 8 $@
