#!/bin/bash
# This script run the Minimax algorithm for ten iterations and print the resulting output to brand new created .out file.
#SBATCH --job-name=my-job
#
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=2
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

for ((n=0;n<10;n++))
do
echo "NEW ITERATION";
<<<<<<< HEAD
<<<<<<< HEAD
./ttt_pt_ver2
=======
./ttt_pt_arvc
>>>>>>> e8e33ef7fd43f9994ba231c86b32421ecb020a00
=======
./ttt_pt_arvc
>>>>>>> e8e33ef7fd43f9994ba231c86b32421ecb020a00
command >> file;
done





