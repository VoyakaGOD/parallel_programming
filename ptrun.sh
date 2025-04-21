#!/bin/sh

# ./ptrun.sh [name] [number of threads] [args...]

name="$1"
nt="$2"
shift 2
sbatch --ntasks=1 --cpus-per-task=$nt --error=results/$name-%j.err --output=results/$name-%j.out pt_SLURM_script.sh executables/$name $@
