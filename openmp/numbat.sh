#!/bin/bash
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

