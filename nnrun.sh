#!/bin/sh

# ./nnrun.sh [name] [MPI number of processes] [SLURM number of processes] [args...]

name="$1"
initial_np="$2"
max_np="$3"
shift 3
sbatch --ntasks=$max_np --error=results/$name-%j.err --output=results/$name-%j.out SLURM_script.sh executables/$name $initial_np $@
