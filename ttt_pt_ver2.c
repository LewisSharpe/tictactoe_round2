// Tic Tac Toe - PTheards C version - Version 4 - 7x7 Grid
// Lewis Sharpe
// 25.08.2017 
// compile (seq): gcc -DSEQ -o ttt_pt ttt_pt.c
// compile: gcc -o ttt_pt ttt_pt.c -lpthread
// run: ./ttt_pt

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>
#include "time.h"

// Lewis Sharpe

#define NUM_THREADS     2
/* text colour code declarations */      
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/* enum int const chars */
enum { NOUGHTS, CROSSES, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW };

int loopcount = 49;
/* var definitions */
const int Directions[4] = {1, 7, 8, 14}; // 1 7 8 14  
const int ConvertTo25[49] = { /* positions in 25 array */
        11,12,13,14,15,16,17,
        20,21,22,23,24,25,26,
        29,30,31,32,33,34,35,
        38,39,40,41,42,43,44,
        47,48,49,50,51,52,53,
        56,57,58,59,60,61,62,
        65,66,67,68,69,70,71,
};

const int InMiddle = 41;
const int Corners[4] = { 11, 17, 65, 71 };

/* create thread argument struct for thr_func() */
/* this looks like a generic data structure, not one for this app -- HWL */
typedef struct _thread_data_t {
  int tid;
  double stuff;
} thread_data_t;

/* data structure for arguments to MinMax -- HWL */
typedef struct {
  int *board1;  // the current board of play
  int len;      // number of cells on the board
  int side;     // player making current move
  int ply; 
  int maxPly;
  int positions;
  int *res;     // result from running MinMax on this board; could be an int instead
} minmax_thread_args;

int GetNumForDir (int startSq,
		  const int dir,	
		  const int *board,	
	          const int us) {
	int found = 0; 
	while (board[startSq] != BORDER) { // while start sq not border sq
		if(board[startSq] != us) {
	break;
	}
	found++;
	startSq += dir;
}
	return found;
}

int FindThreeInARow(const int *board,
		    const int ourindex,
		    const int us) {
        int DirIndex = 0;
        int Dir = 0;
        int threeCount = 1;
for(DirIndex - 0; DirIndex <4; ++DirIndex) {
                Dir = Directions[DirIndex];
                threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
                threeCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
                if (threeCount == 3) {
                        break;
                }
                threeCount = 1;
                }
                return threeCount;
}

int FindThreeInARowAllBoard(const int *board,
			    const int us) {
         /* after move made	 */
        int threeFound = 0;
	int index;
	for(index = 0; index < loopcount; ++index) { // for all 9 squares
		if(board[ConvertTo25[index]] == us) { // if player move
			if(FindThreeInARow(board, ConvertTo25[index], us) == 3) {				
				threeFound = 1; // if move results 3 in row,confirm 
				break;
			}
		}
	}	
	return threeFound;
}

int EvalForWin(const int *board,
	       const int us) {
// eval if move is win draw or loss
	if(FindThreeInARowAllBoard(board, us) != 0) // player win?
		return 1; // player win confirmed
	if(FindThreeInARowAllBoard(board, us ^ 1) != 0) // opponent win?
		return -1; // opp win confirmed
	return 0;
}

#ifdef SEQ

// in the sequential version, you probably have just board1, and side as input;
// use a struct in this case as well, so that you can share code with the pthreads version
// ply - how many moves deep into tree
// postitions - no of pos searched
// maxPly - how deep we have went in tree

int MinMax (minmax_thread_args *arg,
            int *board0,
            int *board1,
            int ply,  
            int positions, 
            int maxPly, 
            const int side) {
#else// in the pthreads version, use a struct as in the pthreads2.c sample code -- HWL
int MinMax (minmax_thread_args *arg,
            int *board0,
            int *board1,
            int ply, 
            int positions, 
            int maxPly,
            const int side) 
{
#endif
// recursive function calling - min max will call again and again through tree - to maximise score
// check if there is a win
// generate tree for all move for side (ply or opp)
// loop moves , make move, min max on move to get score
// assess best score
// end moves return bestscore

// defintions
        int MoveList[49]; // 9 pos sqs on board
        int MoveCount = 0; // count of move
        int bestScore = -2;
        int score = -2; // current score of move
        int bestMove = -1; // best move with score
        int Move; // current move
        int index; // indexing for loop

        ply = 0;
        positions = 0;
        maxPly = 0;

       	/* pthreads defintions */
	pthread_t thr[NUM_THREADS];
	minmax_thread_args thread_args[NUM_THREADS];
	int i, t, rc[i];
        	       
        if(ply > maxPly)  // if current pos depper than max dep
                 maxPly = ply; // max ply set to current pos
        positions++;  // increment positions, as visited new position

        if(ply > 0) {
                score = EvalForWin(board1, side); // is current pos a win
		if(score != 0) { // if draw
                     return score;  /* return score, stop searching, game won */
                }
        }

   // if no win, fill Move List
        // find empty cells
        for(index = 0; index < 49; ++index) {
	  if( board1[ConvertTo25[index]] == EMPTY) {
	    MoveList[MoveCount++] = ConvertTo25[index]; // current pos on loop
	  }
        }

        // loop all moves - put on board
	// MAIN LOOP	
        for(index = 0; index < MoveCount; ++index) {
          /* you probably only want to use the multi-threaded version in the top N levels; so need to pass the level as another argument / elem in the argumnet struct (like ply) and check whether the level is below the constant N  -- HWL */
	  for (t=0; t<NUM_THREADS && index < MoveCount; t++, index++) {
            // these declarations will hide the board0 and board1 arguments to this fct!! -- HWL
	    const int asz = sizeof(thread_data_t); // size of the argument struct
            const int bsz = sizeof(49*sizeof(int)); // size of the board, pointed to

	    // allocate mem for the argument struct
	    minmax_thread_args *new_thread_arg = (minmax_thread_args *)malloc(asz);
            thread_args[t] = *new_thread_arg; // LS 22.10.2018 amended missing pointer 
	    // allocate mem for the board in arg
            new_thread_arg->board1 = (int*)malloc(bsz);
	    new_thread_arg->len = 49; // BAD magic constant
            new_thread_arg->res = (int*)malloc(sizeof(int));
	    // copy board to board0
	    memcpy(new_thread_arg->board1, board1, bsz); // ORDER: dest, source, size
            Move = MoveList[index];
	    // I assume that board0 is the temp board to be used for the thread
            new_thread_arg->board1[Move] = side;
            ply++; // increment ply
            // PARALLEL CALL; BEWARE board shared btw all threads; use board0 instead
        #ifdef SEQ 
            // needs update to use the arg struct in the sequential version as well  
            score = -MinMax(new_thread_arg, *board0, *board1, ply, positions, maxPly, side); /* -MinMax(board0, side^1); */
	#else
            rc[t] = pthread_create(&thr[i], NULL, MinMax,(void *) &thread_args[t]); // HWL
	#endif
	    // undo moves
	    board1[Move] = EMPTY; // else clear board; no longer needed if you use new_thread_arg -- HWL
	    ply--; // decrement ply
	  }

#ifndef SEQ
	  // NUM_THREADs different threads working on sub boards
	  /* block until all threads complete */
	  for (t = 0; t < NUM_THREADS; ++t) {
	    pthread_join(thr[t], NULL);
	    //score = rc[t]; // get restult from thread £t; NO: according to manual, the return code is NOT the value of the fct that is called, rather the status of the thread (0 for ok) -- HWL
            // you probably need to remember the new_thread_arg in an array and res the ->res component of that arg here, to access the result from the thread             -- HWL
	    score = *(thread_args[t].res); // use this to pickup result in pthread version -- HWL // amended from -> to . LS 22.10.18
            if(score > bestScore) { // if score is best score (will be for first move)
	      bestScore = score;
	      bestMove = Move;
	    }
          }
#endif
#ifdef SEQ
	  bestScore = (score>bestScore) ? score : bestScore;
	  // same for bestMove
#endif	
        // tackle  move count is 0 as board is full
        if(MoveCount==0) {
                bestScore = FindThreeInARowAllBoard(board1, side);
#ifdef SEQ
        // if not at top at tree, we return score
	/* why do you return different things depending on the level in the tree?? -- HWL */
	if(ply!=0)
                return bestScore;
        else
                return bestMove;
#else
        *(thread_args[t].res) = bestScore; // amended from -> to . 22.10.18
#endif

#ifdef DEBUG // LS 22.10.18
printf("debug work");
// DO WORK
#endif
    }
   }
  return EXIT_SUCCESS;
}

void InitialiseBoard (int *board) { /* pointer to our board array */  
	int index = 0; /* index for looping */
	for (index = 0; index < 82; ++index) {
		board[index] = BORDER; /* all squares to border square */
	}
	for (index = 0; index < loopcount; ++index) {
		board[ConvertTo25[index]] = EMPTY /* all squares to empty */;
	}
}

void PrintBoard(const int *board) {
	int index = 0;
	char pceChars[] = "OX|-";/* board chars */	
	
	printf("\n\nBoard:\n\n");
	for(index = 0; index < 49; ++index) { /* for the 9 pos on board */
		if(index!=0 && index%7==0) { /* if 3 pos on each line */
			printf("\n\n");
		}
		printf("%4c",pceChars[board[ConvertTo25[index]]]);
	}
	printf("\n");
}

int GetNextBest(const int *board) {
/* if comp didn't find winning move, place priority for move in middle */
/* if middle not available, then */
/* place priority on corners, if corners not available */
/* then make random move */
int ourMove = ConvertTo25[InMiddle]; // set move to middle
	if(board[ourMove] == EMPTY) {
		return ourMove; // if board empty place in middle
	}
	
	int index = 0; // indexing for looping
	ourMove = -1; // next best not found
	
	for(index = 0; index < 4; index++) { // loop for no of coners
		ourMove = ConvertTo25[Corners[index]];
		if(board[ourMove] == EMPTY) {
			break;
		}
		ourMove = -1;
	}	
	return ourMove;
}

int GetWinningMove(int *board, 
		   const int side) {
	int ourMove = -1;
	int winFound = 0;
	int index = 0;
	
	for(index = 0; index < loopcount; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) {
			ourMove = ConvertTo25[index];
			board[ourMove] = side;
			
			if(FindThreeInARow(board, ourMove, side) == 3) {
				winFound = 1;
			}	
			board[ourMove] = EMPTY;
			if(winFound == 1) {
				break;
			}
			ourMove = -1;
		};
	} 
	return ourMove;
}

int GetComputerMove (int *board0,
                     int *board1,
                     const int side,
                     int ply, // computer
                     int positions, 
                     int maxPly) {
	int *arg;	
	ply=0;
	positions=0;
	maxPly=0;
	int best = MinMax(*arg, *board0, *board1, ply, positions, maxPly, side);
	printf("Finished searching through positions in tree:%d max depth:%d best move:%d\n",positions,maxPly,best);
	return best;
}

int GetHumanMove(int *board0, 
		 int *board1,
		 const int side,	
		 int ply, // human
            	 int positions, 
                 int maxPly) {
	int *arg;        
	ply=0;
	positions=0;
        maxPly=0;
        int best = MinMax(*arg, *board0, *board1, ply, positions, maxPly, side);
        printf("Finished searching through positions in tree:%d max depth:%d best");
        return best;
}

int HasEmpty(const int *board) { /* Has board got empty sq */
	int index = 0;
	for (index = 0; index < loopcount; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) return 1;
	}
	return 0;
}

void MakeMove (int *board,
	       const int sq, 	
               const side) {
	board[sq] = side; /* pos of square equal the side (either x or o) */
}

/* thread function */
void *RunGame() {
//(minmax_thread_args *arg,
//	       int *board0,
  //             int *board1,
    //           int ply,
      //         int positions,
        //       int maxPly,
	  //     const int side) {
	printf("%s TIC TAC TOE \n", KNRM);
	struct timeval thr_func1, thr_func2;
	gettimeofday(&thr_func1, NULL);
 	int i, rc;
 	thread_data_t *data = (thread_data_t *)arg;
	int GameOver = 0;
	int Side = NOUGHTS;
	int LastMoveMade = 0;
	int board[82];
	InitialiseBoard(&board1);
	PrintBoard(&board1);
		while (!GameOver) { // while game is not over
			if (Side==NOUGHTS) {
	struct timeval tv3, tv4;
	gettimeofday(&tv3, NULL);
	LastMoveMade = GetHumanMove (&board0, &board1, Side, ply, positions, maxPly);
	MakeMove(&board1, LastMoveMade, Side);
	Side=CROSSES;
       gettimeofday(&tv4, NULL);
	printf ("Total time = %f seconds\n",
         (double) (tv4.tv_usec - tv3.tv_usec) / 1000000 +
         (double) (tv4.tv_sec - tv3.tv_sec));
	printf("%s COMPUTER MOVE \n", KNRM);		
			}
				else {
	LastMoveMade = GetComputerMove(&board0, &board1, Side, ply, positions, maxPly);
	MakeMove(&board1, LastMoveMade, Side);
	Side=NOUGHTS;
	PrintBoard(&board1);
	printf("%s PLAYER MOVE \n", KNRM);
	}
	printf("handled by thread, thread id: %d\n", data->tid);
	gettimeofday(&thr_func2, NULL);
	printf ("elapsed time performing move: = %f seconds\n",
         (double) (thr_func2.tv_usec - thr_func1.tv_usec) / 1000000 +
         (double) (thr_func2.tv_sec - thr_func1.tv_sec));
// if three in a row exists Game is over
		if( FindThreeInARow(board, LastMoveMade, Side ^ 1) == 3) {
			printf("Game over!\n");
			GameOver = 1;
			if(Side==NOUGHTS) {
				printf("Computer Wins\n");
			} else {
				printf("Human Wins\n");
			}
		}	
		if(!HasEmpty(board)) {
			printf("Game Over! I know, it's a shame it can't last forever! \n");
			GameOver= 1;
			printf("It's a draw! Come on, try harder for the win next time!");
					}
				}
			pthread_exit(NULL);
		}

int main(string *arg, int argc ) {
struct timespec begin, end;
double elapsed;
clock_gettime(CLOCK_MONOTONIC, &begin);

RunGame(&arg, &board0, &board1, ply, positions, maxPly, side); // LS 22.10.18 amended main func exec

clock_gettime(CLOCK_MONOTONIC, &end);
elapsed = end.tv_sec - begin.tv_sec;
elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
printf("Elapsed game time = %lf seconds", elapsed);
return EXIT_SUCCESS;
}


