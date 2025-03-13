#!/bin/sh
name="$1"
np="$2"
shift 2
sbatch --ntasks=$np --error=results/$name-%j.err --output=results/$name-%j.out SLURM_script.sh executables/$name $np $@
