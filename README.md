# Sequential and Parallel implementation of Minimax AI Search in a game of Tictactoe on a 7x7 squared grid
August 2018

#Purpose:
The implementation contains two versions of the same AI playing against each other
in a game of tictactoe. The purpose of this parallel implementation of a minimax
algorithm was to improve speed up in game decision-making. 

#Description of implementations: 
The sequential version of the game is coded in C, with parallel versions implemented
in OpenMP and PThreads, and all measured for performance on the same platform.

#How to compile files:
Sequential:
(through gcc) compile: gcc -o ttt ttt.c //
	      execute: ./ttt //
(through bash script) compile and execute: sbatch bash.sh //
		      view output generated: nano output.txt 

PThreads:
(through gcc) compile: gcc -o ttt_pt ttt_pt.c -lpthread
	      execute: ./ttt_pt
(through bash script) compile and execute: sbatch bash.sh
                      view output generated: nano output.txt

OpenMP:
(through gcc) compile: gcc -fopenmp -o ttt_omp ttt_omp.c
	      execute: ./ttt_omp
(through bash script) compile and execute: sbatch bash.sh
                      view output generated: nano output.txt


