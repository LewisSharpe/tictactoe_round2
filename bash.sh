#!/bin/bash

# This script run the Minimax algorithm and print the resulting output the file 'output.txt'.
#SBATCH --job-name=my_job
#SBATCH --output=output.txt
#
#SBATCH --ntasks=8 
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

./ttt_pt_arvc

