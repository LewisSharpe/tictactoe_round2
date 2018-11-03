#!/bin/bash
<<<<<<< HEAD
# This script run the Minimax algorithm and print the resulting output the file 'output.txt'.
=======
>>>>>>> 8bf6052e25bc27896d7e473a1260c348f17451d1
#SBATCH --job-name=my_job
#SBATCH --output=output.txt
#
#SBATCH --ntasks=8 
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

./ttt_pt

