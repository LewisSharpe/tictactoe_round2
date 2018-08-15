#!/bin/bash
# This script run the Minimax algorithm and print the resulting output to the file 'output.txt'.
#SBATCH --job-name=my_job
#SBATCH --output=output.txt
#
#SBATCH --ntasks=1 --nodes=1
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

./ttt
