# Sequential and Parallel implementation of Minimax AI Search in a game of Tictactoe on a 7x7 squared grid
Lewis Sharpe (ls68@hw.ac.uk)
August 2018

## Purpose:
The implementation contains two versions of the same AI playing against each other in a game of tictactoe. The purpose of this parallel implementation of a minimax algorithm was to improve speed up in game decision-making.

## Minimax Algorithm:
The Minimax Algorithm focuses on the procedure of choosing the strategy that least benefits the most advantaged member of a group. 

## Description of implementations: 
### Sequential:
The sequential version of the game is coded in C, and is currently fixed on 7x7 squared grid. The algorithm plays the same AI against the AI we have designed. The  possible playing characters that could apprear on my board are the following:
- 'X'
- 'O'
- '|'
- '-'
### Parallel:
with parallel versions implemented in OpenMP and PThread.

## How to compile files: 

### Sequential:
	      (through gcc) compile: gcc -o ttt ttt.c
			    execute: ./ttt 
              (through bash script) compile and execute: sbatch bash.sh 
              (through bash script that executes an iteration of specified of c$
                                    compile and execute: sbatch numbat.sh
                                    view output generated: nano numbat.txt  				    view output generated: nano output.txt
                

### PThreads: 
	      (through gcc) compile: gcc -o ttt_pt ttt_pt.c -lpthread 	
			    execute: ./ttt_pt
              (through bash script) compile and execute: sbatch bash.sh
				    view output generated: nano output.txt
              (through bash script that executes an iteration of specified of c$
                                    compile and execute: sbatch numbat.sh
                                    view output generated: nano numbat.txt  

### OpenMP: 
              (through gcc) compile: gcc -fopenmp -o ttt_omp ttt_omp.c 
			    execute: ./ttt_omp
              (through bash script) compile and execute: sbatch bash.sh 
				    view output generated: nano output.txt
              (through bash script that executes an iteration of specified of c$
                                    compile and execute: sbatch numbat.sh
                                    view output generated: nano numbat.txt  
