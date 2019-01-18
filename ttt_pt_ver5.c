// Tic Tac Toe - PTheards C version - Version 4 - 7x7 Grid
// Time-stamp: <Fri Dec 14 2018 17:47:09 hwloidl>
// Lewis Sharpe
// 25.08.2017 
// compile (seq): gcc -DSEQ -o ttt_pt ttt_pt.c
// compile: gcc -o ttt_pt ttt_pt.c -lpthread
// run: ./ttt_pt
// compile for debug: gcc -DSEQ  -g -o ttt_pt_ver4 ttt_pt_ver4.c
// debug: gdb ./ttt_pt_ver2

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#ifndef SEQ
#include <pthread.h>
#endif
#include "time.h"
#include <sys/time.h>
#include <assert.h>

// Lewis Sharpe

#define NUM_THREADS     2

#ifdef COLORS
/* text colour code declarations */      
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#else
/* text colour code declarations */      
#define KNRM  ""
#define KRED  ""
#define KGRN  ""
#define KYEL  ""
#define KBLU  ""
#define KMAG  ""
#define KCYN  ""
#define KWHT  ""
#endif

// size in one dim:  7x7 board (proper)
#define BOARD_SIZE 7

#ifdef DEBUG
#define NO_OF_CELLS ((BOARD_SIZE+2)*(BOARD_SIZE+2)+1)
#define LOOP_COUNT  (BOARD_SIZE*BOARD_SIZE)
#define MAGIC_CONST 0xdeadbeef
#else
#define NO_OF_CELLS ((BOARD_SIZE+2)*(BOARD_SIZE+2))
#define LOOP_COUNT  (BOARD_SIZE*BOARD_SIZE)
#endif

/* enum int const chars */
enum { NOUGHTS, CROSSES, BORDER, EMPTY };
enum { HUMANWIN, COMPWIN, DRAW };

int loopcount = LOOP_COUNT;

#ifdef DEBUG
#define LOOKS_LIKE_BOARD(board)  (board[NO_OF_CELLS-1] == MAGIC_CONST)
#else
#define LOOKS_LIKE_BOARD(board)
#endif

#define NO_OF_DIRS 3
#define MAX_SCORE  1000
#define MIN_SCORE  -1000

/* var definitions */
// needs change for generalisation to arbitrary board size -- HWL
// const int Directions[4] = {1, 7, 8, 14}; // 1 7 8 14
const int Directions[NO_OF_DIRS] = {1, 9, 10}; // 1 7 8 14  
const int ConvertTo25[LOOP_COUNT] = { /* positions in 25 array */
  //  1, 2, 3, 4, 5, 6, 7, 8, 9
        11,12,13,14,15,16,17,
        20,21,22,23,24,25,26,
        29,30,31,32,33,34,35,
        38,39,40,41,42,43,44,
        47,48,49,50,51,52,53,
        56,57,58,59,60,61,62,
        65,66,67,68,69,70,71,
  // 73,74,75,76,77,78,79,80,81
};

const int InMiddle = 41;
const int Corners[4] = { 11, 17, 65, 71 };
// int ply = 0;         // how many moves deep into tree
// int positions = 0;   // no of pos searched
// int maxPly = 0;      // how deep we have went in tree

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
  int positions;// number of positions visited in the tree
  int ply;      // current depth in the tree
  int maxPly;   // max depth to traverse to
  int *res;     // result from running MinMax on this board; could be an int instead
} minmax_thread_args;

// -----------------------------------------------------------------------------
// Prototypes:

int number_of_pieces(/* const */ int *board);
void PrintBoard(/* const */ int *board);
void PrintList(int *list, int len);

// -----------------------------------------------------------------------------

int GetNumForDir (int startSq, const int dir, const int *board, const int us) {
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

// twocount
int FindTwoInARow(const int *board, const int ourindex, const int us) {
        int DirIndex = 0;
        int Dir = 0;
        int twoCount = 1;

        assert(us == NOUGHTS || us == CROSSES);
        for(DirIndex = 0; DirIndex<NO_OF_DIRS; ++DirIndex) {
                Dir = Directions[DirIndex];
                twoCount += GetNumForDir(ourindex + Dir, Dir, board, us);
                twoCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
                if (twoCount == 2) {
                        break;
                }
                twoCount = 1;
        }
        return twoCount;
}

int FindTwoInARowAllBoard(const int *board, const int us) {
         /* after move made      */
        int twoFound = 0;
        int index;
        assert(us == NOUGHTS || us == CROSSES);
        for(index = 0; index < loopcount; ++index) { // for all 9 squares
                if(board[ConvertTo25[index]] == us) { // if player move
                        if(FindTwoInARow(board, ConvertTo25[index], us) == 2) {
                                twoFound = 1; // if move results 2 in row,confirm
                                break;
                        }
                }
        }
        return twoFound;
}

int FindThreeInARow(const int *board, const int ourindex, const int us) {
        int DirIndex = 0;
        int Dir = 0;
        int threeCount = 1;
	
        assert(us == NOUGHTS || us == CROSSES); 
  
        // for(DirIndex - 0; DirIndex <NO_OF_DIRS; ++DirIndex) { // ????????
	for(DirIndex = 0; DirIndex<NO_OF_DIRS; ++DirIndex) {
                Dir = Directions[DirIndex];
		if (!(ourindex + Dir >=1 && ourindex + Dir <= 81)) {
		  PrintBoard(board);
		  fprintf(stderr,"** ERROR: index is %d\n", ourindex + Dir);
		  exit(2);
		}
		assert(ourindex + Dir >=1 && ourindex + Dir <= 81); // index check
                threeCount += GetNumForDir(ourindex + Dir, Dir, board, us);
//		assert(ourindex + Dir * -1 >= 1 && ourindex + Dir * -1 <= 81); // index check
		if (!(ourindex + Dir * -1 >= 1 && ourindex + Dir * -1 <= 81)) {
		  // PrintBoard(board);
		  //fprintf(stderr,"** ERROR: index is %d\n", ourindex + Dir * -1);
		  // exit(2);
		  continue;
		}
		//assert(ourindex + Dir * -1 >= 1 && ourindex + Dir * -1 <= 81); // index check
                threeCount += GetNumForDir(ourindex + Dir * -1, Dir * -1, board, us);
                if (threeCount == 3) {
                        break;
                }
                threeCount = 1;
	}
	return threeCount;
}

int FindThreeInARowAllBoard(const int *board, const int us) {
         /* after move made	 */
        int threeFound = 0;
	int index;
        assert(us == NOUGHTS || us == CROSSES); 
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

int EvalForWin(const int *board, const int us) {
  // eval if move is win draw or loss
  assert(us == NOUGHTS || us == CROSSES); 
  if(FindThreeInARowAllBoard(board, us) != 0) // player win?
    return 1; // player win confirmed
  if(FindThreeInARowAllBoard(board, us ^ 1) != 0) // opponent win?
    return -1; // opp win confirmed
  return 0;
}

int EvalForWin2(const int *board, const int us) {
// eval if move is win draw or loss
  assert(us == NOUGHTS || us == CROSSES);
  if(FindTwoInARowAllBoard(board, us) != 0)
    return 1; // player win confirmed
  if(FindTwoInARowAllBoard(board, us ^ 1) != 0) 
    return -1; // opp win confirmed
  return 0;
}

// static evaluation function; 
int EvalCurrentBoard(const int *board, const int side) {
  int score;
  score = EvalForWin(board, side); // 1 for win, 0 for unknown
  if (score == 1)
    return MAX_SCORE;
  score = EvalForWin2(board, side); // 1 for win, 0 for unknown
  if (score == 1)
    return MAX_SCORE/10;
  return MAX_SCORE/100;
}

#ifdef SEQ
// in the sequential version, you probably have just board1, and side as input;
// use a struct in this case as well, so that you can share code with the pthreads version
int MinMax (minmax_thread_args *arg /* int        *board0, int *board1, int side */) {
#else// in the pthreads version, use a struct as in the pthreads2.c sample code -- HWL
int MinMax (minmax_thread_args *arg) {
#endif
	int *board1 = arg->board1;
	int len = arg->len;
	int side = arg->side;
	int ply = arg->ply;
	int maxPly = arg->maxPly;
	int positions = arg->positions;
	// recursive function calling - min max will call again and again through tree - to maximise score
	// check if there is a win
	// generate tree for all move for side (ply or opp)
	// loop moves , make move, min max on move to get score
	// assess best score
	// end moves return bestscore

	// defintions
        int MoveList[NO_OF_CELLS]; // 9 pos sqs on board
        int MoveCount = 0; // count of move
        int bestScore = MIN_SCORE;
        int score = MIN_SCORE; // current score of move
        int bestMove = -1; // best move with score
        int Move; // current move
        int index; // indexing for loop

	/* pthreads defintions */
	pthread_t thr[NUM_THREADS];
	minmax_thread_args *thread_args[NUM_THREADS];
	int i, t, rc[i];

	// used in loop, to pass arguments to recursive calls
	const int asz = sizeof(minmax_thread_args);  // size of the argument struct
	const int bsz = NO_OF_CELLS*sizeof(int);     // size of the board, pointed to
	minmax_thread_args *new_thread_arg;
	
	/* HERE <============== */
	assert(side == NOUGHTS || side == CROSSES); 
	assert(board1[0] == NOUGHTS || board1[0] == CROSSES || board1[0] ==  BORDER || board1[0] ==  EMPTY);
#ifdef DEBUG
	assert(LOOKS_LIKE_BOARD(board1));
#endif	

#ifdef VERBOSE	
	printf(".. MinMax with ply %d and side %d and board %p", ply, side, board1);
	PrintBoard(board1);
#endif
	
	// can't use GLOBAL vars in the pthreads version
	// if deeper in the tree than mandated by maxPly, then terminated the recursions and return
	/*
        if(ply > maxPly) // if current pos depper than max dep
                 maxPly = ply; // max ply set to current pos
	*/
	// THRESHOLD <=============
        if(ply > maxPly) // if current pos deeper than max dep, finish
	  return EvalCurrentBoard(board1, side);

        positions++; // increment positions, as visited new position

#if 0
	// why is this needed!?? if for immediate win you need to check for MAX_SCORE
        if(ply > 0) { /* remove the global variable -- HWL */
           score = EvalForWin2(board1, side);
            score = EvalForWin(board1, side); // is current pos a win?
		if(score != 0) { // if draw
                     return score;  /* return score, stop searching, game won */
                }
        }
#endif
	
        // if no win, fill Move List
	{ int on = 1;
        // find empty cells
        for(index = 0; index < loopcount; ++index) {
	  if( board1[ConvertTo25[index]] == EMPTY) {
	    MoveList[MoveCount++] = ConvertTo25[index]; // current pos on loop
#if 0
	    if (on) {
	      printf("__ Considering as first move: %d\n", index);
	      on = 0;
	    }
#endif
	  } else {
	    // printf("** NON EMPTY index %d on board\n", index);
	    // PrintBoard(board1);
	  }
        }
	}
	/*
	if (ply==0) {
	  printf("__ Considering moves \n");
	  PrintList(MoveList,MoveCount);
	} 
	*/  

	// loop all moves - put on board
	// MAIN LOOP	
        for(index = 0; index < MoveCount; ++index) {
          /* you probably only want to use the multi-threaded version in the top N levels; so need to pass the level as another argument / elem in the argumnet struct (like ply) and check whether the level is below the constant N  -- HWL */

	  // do this in parallel-ready mode only; say, just for the top 3 levels
	  for (t=0; t<NUM_THREADS && index < MoveCount; t++, index++) {
            // these declarations will hide the board0 and board1 arguments to this fct!! -- HWL
	    // thread_data_t *board0, *board1;

	    // allocate mem for the argument struct
	    new_thread_arg = (minmax_thread_args *)malloc(asz);

            thread_args[t] = new_thread_arg; // LS 22.10.2018 amended missing pointer ; doesn't need the '*', BUT the type for 'thread_args' was wrong

	    // allocate mem for the board in arg
            new_thread_arg->board1 = (int*)malloc(bsz);
	    new_thread_arg->len = NO_OF_CELLS; // BAD magic constant
	    new_thread_arg->side = side^1;
	    new_thread_arg->positions = positions;
	    new_thread_arg->ply = ply+1;
	    new_thread_arg->maxPly = maxPly;
            new_thread_arg->res = (int*)malloc(sizeof(int));
	    // copy board to board0
	    memcpy(new_thread_arg->board1, board1, bsz); // ORDER: dest, source, size
	    Move = MoveList[index];
	    // ASSERTION: Move must be a valid position ie empty cell
	    if (board1[ConvertTo25[Move]] != EMPTY && index >= 1 || index <= 49 ) { 
	      printf("** FAILED ASSERTION: index %d not EMPTY on board\n", Move);
	      PrintBoard(board1);
	      
	      //exit(99);
	    }
	    	    // I assume that board0 is the temp board to be used for the thread
            Move = MoveList[index];
	    // ASSERTION: Move must be a valid position ie empty cell
	    if (board1[ConvertTo25[Move]] != EMPTY) {
	      printf("** FAILED ASSERTION: index %d not EMPTY on board\n", Move);
	      PrintBoard(board1);
	      exit(99);
	    }
	    // I assume that board0 is the temp board to be used for the thread
            new_thread_arg->board1[Move] = side;
            // ply++; // increment ply
            // PARALLEL CALL; BEWARE board shared btw all threads; use board0 instead
#ifdef SEQ 
            // needs update to use the arg struct in the sequential version as well  
            score = -MinMax(new_thread_arg); /* -MinMax(board0, side^1); */
	    // probably need to calculate/update best score here (and remember which move gave this score)
            if(score > bestScore) { // if score is best score (will be for first move)
	      bestScore = score;
	      bestMove = Move;
	    }
	    // bestScore = (score>bestScore) ? score : bestScore;

// cut branch
#else
            rc[t] = pthread_create(&thr[i], NULL, MinMax, (void *) thread_args *new_thread_arg[t]); // HWL
#endif
	    free(new_thread_arg->board1);
	    free(new_thread_arg);
	    // sanity check: is this true in the execution?
	    assert(MIN_SCORE<=score && score<=MAX_SCORE);
	    // if (MIN_SCORE<=score && score<=MAX_SCORE) { } else { exit (1); }

	    // in the thread_arg version yu create a new copy of the board for every move; so you don't have to undo a move
	    // undo moves
	    // board1[Move] = EMPTY; // else clear board; no longer needed if you use new_thread_arg -- HWL
	    // ply--; // decrement ply // why?? this should be the depth of the recursion 
	  } // for ... NUM_THREADS

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
	  // see above
	  // bestScore = (score>bestScore) ? score : bestScore;
	  // same for bestMove
#endif	

	  // ANYTHING to do here?

	} // for ... index

#ifdef SEQ
        // if not at top at tree, we return score
	/* why do you return different things depending on the level in the tree?? -- HWL */
	if (ply!=0) {
	  return bestScore;
	} else {
	  //assert(board1[ConvertTo25[bestMove]] == EMPTY);
	  printf("*****************************\n");
	  printf("** DONE WITH BOARD\n");
	  PrintBoard(board1);
	  printf("** best move %d with score %d side %d\n", bestMove, bestScore, side);
	  return bestMove;
	}
#else
        *(thread_args[t].res) = bestScore; // amended from -> to . 22.10.18
#endif

	assert(0); // never reached

#if 0
	// tackle  move count is 0 as board is full
        if(MoveCount==0) {
                bestScore = FindThreeInARowAllBoard(board1, side);
#ifdef SEQ
        // if not at top at tree, we return score
	/* why do you return different things depending on the level in the tree?? -- HWL */
	if(ply!=0) {
	  return bestScore;
	} else {
	  assert(board1[ConvertTo25[bestMove]] == EMPTY);
	  return bestMove;
	}
#else
        *(thread_args[t].res) = bestScore; // amended from -> to . 22.10.18
#endif
	
#ifdef DEBUG // LS 22.10.18
// DO WORK
// ???	
print(0xdeadbeef);
#endif
    }
   }

#ifdef SEQ
        // if not at top at tree, we return score
	/* why do you return different things depending on the level in the tree?? -- HWL */
	if(ply!=0) {
	  return bestScore;
	} else {
	  assert(board1[ConvertTo25[bestMove]] == EMPTY);
	  return bestMove;
	}
#else
        *(thread_args[t].res) = bestScore; // amended from -> to . 22.10.18
#endif

  assert(0);
  return EXIT_SUCCESS;
<<<<<<< HEAD
}
=======
>>>>>>> ca01f501165a52435844e3975e4b54a030abbb26
#endif
}

void InitialiseBoard (int **board) { /* NB: this is the address of a var holding a pointer to our board array */  
	int index = 0; /* index for looping */
	*board = (int*)malloc(NO_OF_CELLS*sizeof(int)); /* size of the board, including border ; we need to write into the variable, whose address is in board;  */
	printf("\nCreated board @ %p; initialising ...\n\n", *board);
	for (index = 0; index < NO_OF_CELLS; ++index) {
	  (*board)[index] = BORDER;              /* all squares to border square */
	}
	for (index = 0; index < loopcount; ++index) {
	  (*board)[ConvertTo25[index]] = EMPTY   /* all squares to empty */;
	}
#ifdef DEBUG
	(*board)[NO_OF_CELLS-1] = MAGIC_CONST;
#endif
}

int number_of_pieces(/* const */ int *board){
  int index = 0, num = 0;
  for(index = 0; index < loopcount; ++index) { /* for the 9 pos on board */
    if (board[ConvertTo25[index]] == NOUGHTS) {
      num++;
    } else if (board[ConvertTo25[index]] == CROSSES) {
      num++;
    } else if (board[ConvertTo25[index]] == BORDER) {
      /* nothing */
    } else if (board[ConvertTo25[index]] == EMPTY) {
      /* nothing */
    }
  }
  return num;
}
   
void PrintBoard(/* const */ int *board) {
	int index = 0;
	char pceChars[] = "OX|-";/* board chars */	
	printf("\n\nBoard (@ %p):\n\n", board);
	for(index = 0; index < loopcount; ++index) { /* for the 9 pos on board */
#ifdef DEBUG
	  // a board entry must be one of these 4 values; if not, the memory is corrupted
	  assert(board[ConvertTo25[index]] == NOUGHTS ||
		 board[ConvertTo25[index]] == CROSSES ||
		 board[ConvertTo25[index]] == BORDER ||
		 board[ConvertTo25[index]] == EMPTY);
#endif
	  if(index!=0 && index%7==0) { /* if 3 pos on each line */
	    printf("\n\n");
	  }
	  printf("%4c",pceChars[board[ConvertTo25[index]]]);
	}
	printf("\n");
}
void PrintList(int *list, int len) {
  // BROKEN  BROKEN  BROKEN  BROKEN  BROKEN  BROKEN  BROKEN 
	int index = 0;
	char  str1[120], str[120];
	strcpy(str,"");
	for(index = 0; index < len; ++index) { /* for the 9 pos on board */
	  sprintf(str1, "%s, %d ", str, list[index]);
	  strcpy(str, str1);
	}
	printf(str);
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
int GetWinningMove(int *board, const int side) {
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
int GetComputerMove(/* int *board0,*/ int *board1, const int side) {
        // int *arg; // undefined pointer	
	/* WAS global vars; removed now
	ply=0;
	positions=0;
	maxPly=0;
	*/
	// minmax_thread_args *thread_args; 
        int best;
	// where are asz and bsz defined
	int asz = sizeof(minmax_thread_args);
	int bsz = NO_OF_CELLS*sizeof(int);

#ifdef DEBUG
	assert(LOOKS_LIKE_BOARD(board1));
#endif	

	// construct the buffer below -- HWL
	// allocate mem for the argument struct
	minmax_thread_args *thread_arg = (minmax_thread_args *)malloc(asz);
	// allocate mem for the board in arg
	thread_arg->board1 = (int*)malloc(bsz);
	thread_arg->len = NO_OF_CELLS; // BAD magic constant
	thread_arg->positions = 0;
	thread_arg->ply = 0;
	thread_arg->maxPly = 3;
	thread_arg->side = side;
	thread_arg->res = (int*)malloc(sizeof(int));
	// copy board to board1 into structure
	memcpy(thread_arg->board1, board1, bsz); // ORDER: dest, source, size

	printf("__ MinMax for board:\n");
	PrintBoard(thread_arg->board1);

	best = MinMax(thread_arg);       // need to pass a buffer here
	
	// best = MinMax(*arg); // NO: this passes an undefined pointer to MinMax
	printf("++ Finished searching through positions in tree: %d max depth: %d best move: %d \n",
	       thread_arg->positions, thread_arg->maxPly, best); // TODO: fix access to maxPly etc
	assert(thread_arg->board1[ConvertTo25[best]] == EMPTY); // <<============= FAILING ASSERTION
	return best;
}
int GetHumanMove(/* int *board0,*/ int *board1, const int side) {
        // int *arg;        // UNUSED
	/* WAS global vars; removed now
	ply=0;
	positions=0;
	maxPly=0;
	*/
	// minmax_thread_args *thread_args; 
        int best;
	// where are asz and bsz defined
	int asz = sizeof(minmax_thread_args);
	int bsz = NO_OF_CELLS*sizeof(int);

#ifdef DEBUG
	assert(LOOKS_LIKE_BOARD(board1));
#endif	
	// construct the buffer below -- HWL
	// allocate mem for the argument struct
	minmax_thread_args *thread_arg = (minmax_thread_args *)malloc(asz);
	// allocate mem for the board in arg
	thread_arg->board1 = (int*)malloc(bsz);
	thread_arg->len = NO_OF_CELLS; // BAD magic constant
#ifdef DEBUG
	thread_arg->board1[NO_OF_CELLS-1] = MAGIC_CONST;
#endif
	thread_arg->positions = 0;
	thread_arg->ply = 0;
	thread_arg->maxPly = 3;
	thread_arg->side = side;
	thread_arg->res = (int*)malloc(sizeof(int));
	// copy board to board1 into structure
	memcpy(thread_arg->board1, board1, bsz); // ORDER: dest, source, size

	printf("__ MinMax for board:\n");
	PrintBoard(thread_arg->board1);

        best = MinMax(thread_arg); // NO, same as above -- HWL
	assert(thread_arg->board1[ConvertTo25[best]] == EMPTY);

        printf("++ Finished searching through positions in tree: %d max depth; %d best ",
	       thread_arg->maxPly, best);
        return best;
}

int HasEmpty(const int *board) { /* Has board got empty sq */
	int index = 0;
	for (index = 0; index < loopcount; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) return 1;
	}
	return 0;
}

void MakeMove (int *board, const int sq, const int side) {
	board[ConvertTo25[sq]] = side; /* pos of square equal the side (either x or o) */
}

/* thread function */
// void *thr_func(void *arg, int *board0, int *board1) {
void RunGame(){
 	printf("%s TIC TAC TOE \n", KNRM);
 	struct timeval thr_func1, thr_func2;
 	gettimeofday(&thr_func1, NULL);
 	 int i, rc;
	 // thread_data_t *data = (thread_data_t *)arg;
	int GameOver = 0;
	int Side = NOUGHTS;
	int LastMoveMade = 0;
	// int board[82];   // don't use a local array, rather do malloc inside InitialiseBoard
	// int board0[NO_OF_CELLS];   // UNUSED and not needed anyway
	int *board1;
	int num = 0 ; // debugging only

	InitialiseBoard(&board1); // this allocates the board, and returns a valid pointer board1

#ifdef VERBOSE	
	printf("board1 after init (@ %p):\n", board1);
	PrintBoard(board1);
#endif
	
	while (!GameOver) { // while game is not over
	num++;
	if (Side==NOUGHTS) {
	  int m;
	  m = number_of_pieces(board1);
	  // printf("\nPRE BOARD: %d ", num);
	  // PrintBoard(board1);
	  LastMoveMade = GetHumanMove (/*board0,*/ board1, Side);
	  printf("\n__ Suggested Human move: %d ", LastMoveMade);
	  MakeMove(board1, LastMoveMade, Side); // CHECK: board0 here or board1
	  PrintBoard(board1);
	  assert(number_of_pieces(board1)==m+1);
	  Side=CROSSES;
	  //printf("%s COMPUTER MOVE \n", KNRM);		
	} else {
	  int m;
	  struct timeval tv3, tv4;
	  gettimeofday(&tv3, NULL);
	  m = number_of_pieces(board1);
	  // printf("\nPRE BOARD: %d ", num);
	  // PrintBoard(board1);
	  LastMoveMade = GetComputerMove(/*board0,*/ board1, Side);
	  printf("\n__ Suggested Computer move: %d ", LastMoveMade);
	  MakeMove(board1, LastMoveMade, Side);
	  PrintBoard(board1);
	  assert(number_of_pieces(board1)==m+1);
	  Side=NOUGHTS;
#if 1
	  if (num % 10 == 0) {
	    printf("Board after %d moves:", num);
	    PrintBoard(board1); // CHECK board1 or board0
	  }
	  gettimeofday(&tv4, NULL);
	  printf ("Total time = %f seconds\n",
		  (double) (tv4.tv_usec - tv3.tv_usec) / 1000000 +
		  (double) (tv4.tv_sec - tv3.tv_sec));
	  printf("%s PLAYER MOVE \n", KNRM);
#endif
	}
	// printf("handled by thread, thread id: %d\n", data->tid);
	gettimeofday(&thr_func2, NULL);
	printf ("elapsed time performing move: = %f seconds\n",
         (double) (thr_func2.tv_usec - thr_func1.tv_usec) / 1000000 +
         (double) (thr_func2.tv_sec - thr_func1.tv_sec));

 // if two in a row exists Game is over
        if(FindTwoInARow(board1, LastMoveMade, Side ^ 1) == 3) { // ???
                        printf("two found!\n");
                        if(Side==NOUGHTS) {
                                printf("two found\n");
                        } else {
                                printf("two found\n");
                        }
                }
	// if three in a row exists Game is over
	if( FindThreeInARow(board1, LastMoveMade, Side ^ 1) == 3) { // ???
			printf("Game over!\n");
			GameOver = 1;
			if(Side==NOUGHTS) {
				printf("Computer Wins\n");
			} else {
				printf("Human Wins\n");
			}
		}	
	if(!HasEmpty(board1)) { //???
	  printf("Game Over! I know, it's a shame it can't last forever! \n");
	  GameOver= 1;
	  printf("It's a draw! Come on, try harder for the win next time!");
	}
	} // while
	// pthread_exit(NULL);
}

// +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

/* ENTRY point for the prg:
    ./ttt arg1 arg2 arg3 
   args = { "ttt", "arg1",  "arg2",  "arg3" }
   argc = 4 
*/
int main(/* char [] args, int argc */
	 /* int *arg, int *board0, int *board1 */) {
  struct timespec begin, end;
  double elapsed;
  clock_gettime(CLOCK_MONOTONIC, &begin);

  RunGame();
  // thr_func(*arg, *board0, *board1); // LS 22.10.18 amended main func exec

  clock_gettime(CLOCK_MONOTONIC, &end);
  elapsed = end.tv_sec - begin.tv_sec;
  elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
  printf("Elapsed game time = %lf seconds", elapsed);
  return EXIT_SUCCESS;
}



