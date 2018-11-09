#!/bin/bash
<<<<<<< HEAD
# This script run the Minimax algorithm and print the resulting output to the file 'output.txt' .
=======
>>>>>>> 7904984bbab17d083e5a81611c37068001125fe8
#
#SBATCH --job-name=test_omp
#SBATCH --output=ttt_omp.txt
#
#SBATCH --ntasks=8
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=100

export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK
./ttt_omp
