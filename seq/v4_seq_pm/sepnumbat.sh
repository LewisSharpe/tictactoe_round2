#!/bin/bash
# This script run the Minimax algorithm for ten iterations and print the resulting output to brand new created .out file.
#SBATCH --job-name=my-job
#
#SBATCH --ntasks=1 --nodes=1
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

for ((n=0;n<10;n++))
do
echo "NEW ITERATION";
./ttt
command >> file;
done





