sbatch --ntasks=$2 --error=results/$1-%j.err --output=results/$1-%j.out SLURM_script.sh executables/$1 $2
