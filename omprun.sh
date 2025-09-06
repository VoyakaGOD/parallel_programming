#!/bin/sh

# ./run.sh [name] [number of threads] [args...]
# --cpus-per-task ??

name="$1"
nt="$2"
shift 2
sbatch --ntasks=1 --error=results/$name-%j.err --output=results/$name-%j.out pt_SLURM_script.sh executables/$name $@
