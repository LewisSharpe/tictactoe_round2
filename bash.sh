#!/bin/bash

# This script run the Minimax algorithm and print the resulting output the file 'output.txt'.
#SBATCH --job-name=my_job
#SBATCH --output=output.txt
#
#SBATCH --ntasks=8 
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

<<<<<<< HEAD
<<<<<<< HEAD
./ttt_pt_ver2
=======
./ttt_pt_arvc
>>>>>>> e8e33ef7fd43f9994ba231c86b32421ecb020a00
=======
./ttt_pt_arvc
>>>>>>> e8e33ef7fd43f9994ba231c86b32421ecb020a00

