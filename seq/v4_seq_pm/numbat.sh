#!/bin/bash
#SBATCH --job-name=my_job
#SBATCH --output=output.txt
#
#SBATCH --ntasks=1 --nodes=1
#SBATCH --time=10:00
#SBATCH --mem-per-cpu=6000

for ((n=0;n<10;n++)); 

do 
echo "NEW ITERATION";  
./ttt;

done;

