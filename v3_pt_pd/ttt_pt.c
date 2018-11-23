// Tic Tac Toe - Sequential C version - Version 2 - 7x7 Grid
// Lewis Sharpe
// 25.08.2017 

// compile: gcc -o ttt_pt ttt_pt.c -lpthread
// run: ./ttt_pt

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <pthread.h>

// Lewis Sharpe

#define NUM_THREADS     32

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

int ply = 0; // how many moves deep into tree
int positions = 0; // no of pos searched
int maxPly = 0; // how deep we have went in tree

/* create thread argument struct for thr_func() */
typedef struct _thread_data_t {
  int tid;
  double stuff;
} thread_data_t;

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

int FindThreeInARow(const int *board, const int ourindex, const int us) {

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

int FindThreeInARowAllBoard(const int *board, const int us) {
// after move made	
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

int EvalForWin(const int *board, const int us) {
// eval if move is win draw or loss
	if(FindThreeInARowAllBoard(board, us) != 0) // player win?
		return 1; // player win confirmed
	if(FindThreeInARowAllBoard(board, us ^ 1) != 0) // opponent win?
		return -1; // opp win confirmed
	return 0;
}

int MinMax (int	*board, int side) {      
// recursive function calling -	min max	will call again	and again through tree - to maximise score
// check if there is a win
// generate tree for all move for side (ply or opp)
// loop	moves , make move, min max on move to get score
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

/* pthreads defintions */
pthread_t thr[NUM_THREADS];
  int i, rc;
  // create a thread_data_t argument array
  thread_data_t thr_data[NUM_THREADS];

/* pthread loop 1: executing position identification and reasoning */
  for (i = 0; i < NUM_THREADS; ++i) {

	if(ply > maxPly) // if current pos depper than max dep
 		 maxPly = ply; // max ply set to current pos	
	positions++; // increment positions, as visited new position
	
	if(ply > 0) {
		score = EvalForWin(board, side); // is current pos a win
		if(score != 0) { // if draw					
			return score; // return score, stop searching, game won
		}		
	}
}
	
	// if no win, fill Move List
	for(index = 0; index < 35; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) {
	MoveList[MoveCount++] = ConvertTo25[index]; // current pos on loop
}
	}
	
	// loop all moves - put on board
	for(index = 0; index < MoveCount/16; ++index) {
		Move = MoveList[index];
		board[Move] = side;	

		ply++; // increment ply
		score = -MinMax(board, side^1); // for opposing side
		if(score > bestScore) { // if score is best score (will be for first move)			
			bestScore = score;	
			bestMove = Move;
		}
	// undo moves
		board[Move] = EMPTY; // else clear board
		ply--; // decrement ply
	}
/* pthread loop 1: executing position identification and reasoning */
  for (i = 0; i < NUM_THREADS; ++i) {
	// tackle  move count is 0 as board is full
	if(MoveCount==0) {
		bestScore = FindThreeInARowAllBoard(board, side);	
}
	// if not at top at tree, we return score
	if(ply!=0)
		return bestScore;	
	else 
		return bestMove;	
}
        }

int MinMax2 (int	*board, int side) {      
// recursive function calling -	min max	will call again	and again through tree - to maximise score
// check if there is a win
// generate tree for all move for side (ply or opp)
// loop	moves , make move, min max on move to get score
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

/* pthreads defintions */
pthread_t thr[NUM_THREADS];
  int i, rc;
  // create a thread_data_t argument array
  thread_data_t thr_data[NUM_THREADS];

/* pthread loop 1: executing position identification and reasoning */
  for (i = 0; i < NUM_THREADS; ++i) {

	if(ply > maxPly) // if current pos depper than max dep
 		 maxPly = ply; // max ply set to current pos	
	positions++; // increment positions, as visited new position
	
	if(ply > 0) {
		score = EvalForWin(board, side); // is current pos a win
		if(score != 0) { // if draw					
			return score; // return score, stop searching, game won
		}		
	}
}
	
	// if no win, fill Move List
	for(index = 0; index < 49; ++index) {
		if( board[ConvertTo25[index]] == EMPTY) {
	MoveList[MoveCount++] = ConvertTo25[index]; // current pos on loop
}
	}
	
	// loop all moves - put on board
	for(index = 0; index < MoveCount/16; ++index) {
		Move = MoveList[index];
		board[Move] = side;	

		ply++; // increment ply
		score = -MinMax(board, side^1); // for opposing side
		if(score > bestScore) { // if score is best score (will be for first move)			
			bestScore = score;	
			bestMove = Move;
		}
	// undo moves
		board[Move] = EMPTY; // else clear board
		ply--; // decrement ply
	}
/* pthread loop 1: executing position identification and reasoning */
  for (i = 0; i < NUM_THREADS; ++i) {
	// tackle  move count is 0 as board is full
	if(MoveCount==0) {
		bestScore = FindThreeInARowAllBoard(board, side);	
}
	// if not at top at tree, we return score
	if(ply!=0)
		return bestScore;	
	else 
		return bestMove;	
}
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

int GetComputerMove(int *board, const int side) {
	ply=0;
	positions=0;
	maxPly=0;
	int best = MinMax(board, side);
	printf("Finished searching through positions in tree:%d max depth:%d best move:%d\n",positions,maxPly,best);
	return best;
}

int GetHumanMove(int *board, const int side) {
        ply=0;
	positions=0;
        maxPly=0;
        int best = MinMax2(board, side);
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

void MakeMove (int *board, const int sq, const side) {
	board[sq] = side; /* pos of square equal the side (either x or o) */
}

void RunGame() {
printf("%s TIC TAC TOE \n", KRED);
	int GameOver = 0;
	int Side = NOUGHTS;
	int LastMoveMade = 0;
	int board[82];

	InitialiseBoard(&board[0]);
	PrintBoard(&board[0]);

	while (!GameOver) { // while game is not over
	if (Side==NOUGHTS) {
		LastMoveMade = GetHumanMove (&board[0], Side);
		MakeMove(&board[0], LastMoveMade, Side);
		Side=CROSSES;
printf("%s COMPUTER MOVE \n", KBLU);	
}
	else {
	LastMoveMade = GetComputerMove(&board[0], Side);
	MakeMove(&board[0], LastMoveMade, Side);
	Side=NOUGHTS;
	PrintBoard(&board[0]);
printf("%s PLAYER MOVE \n", KNRM);

	}

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
	GameOver = 1;
	printf("It's a draw! Come on, try harder for the win next time!");
}
	}
	}

int main() {
	srand(time(NULL)); /* seed random no generator - moves on board randomly */
	RunGame();
	return 0;
	}	
