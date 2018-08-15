#!/bin/bash
# This script run the Minimax algorithm for ten iterations and print the resulting output to the file 'numbat.txt'.
#SBATCH --job-name=my_job
#SBATCH --output=numbat.txt
#
#SBATCH --ntasks=2
#SBATCH --cpus-per-task=2
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

for ((n=0;n<10;n++)); 

do 
echo "NEW ITERATION";  
./ttt;
command >> file

done;

