sbatch --ntasks=$2 --error=results/$1-%j.err --output=results/$1-%j.out mpi_run.sh common_tasks/1/exe/$1 $2
