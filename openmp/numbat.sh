#!/bin/bash
<<<<<<< HEAD
# This script run the Minimax algorithm for ten iterations and print the resulting output the file 'numbat.txt'.
=======
>>>>>>> 7904984bbab17d083e5a81611c37068001125fe8
#SBATCH --job-name=my_job
#SBATCH --output=numbat.txt
#
#SBATCH --ntasks=4
#SBATCH --cpus-per-task=4
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

for ((n=0;n<10;n++)); 

do 
echo "NEW ITERATION";  
./ttt_omp;

done;

