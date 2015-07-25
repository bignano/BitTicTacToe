#pragma once

#include <vector>
#include <stdint.h>		// For UINT16_MAX

#define RESULT_NONE 0
#define PLAYER_X	1
#define PLAYER_O	2
#define RESULT_DRAW 3

// Define unsigned integer for board storage
typedef unsigned short U16;

/*
BOARD REPRESENTATION

	Index mapping:

	0  1  2  3
	4  5  6  7
	8  9  10 11
	12 13 14 15

	Bit mapping:

	15 14 13 12
	11 10 9  8
	7  6  5  4
	3  2  1  0

NOTES
	
	Bitwise operator usage:
		1. | (OR) 
			get the full board out of xBoard and oBoard
			generate a new board from old board and move
		2. & (AND)
			detecting patterns
			checking move legality
*/

/* 
The 10 win patterns incoded as binary numbers and converted to decimal
0-3 rows  |  4-7 columns  |  8 diagonal \  |  9 diognnal /
*/
static const U16 winPatterns[10] = {
	61440,		/* Rows 1-4 */
	3840,
	240,
	15,
	34952,		/* Columns 1-4 */
	17476,
	8738,
	4369,
	33825,		/* Diagonal \ */
	4680		/* Diagonal / */
};

/* 
A compact representation of tic-tac-toe boards as 16-bit unsigned ints.
Bitboards are immutable to allow minimax search. 
Move generation is done by returning a new board, rather than modifing the currnt one.
The class contains a few standard functions, i.e. GetAvailableMoves, DoMove etc.
a Print function to print the board, and some useful functions for board evaluation.
*/
class Bitboard
{
public:

	/* Default constructor for an empty board */
	Bitboard(int firstPlayer=PLAYER_X) : 
		xBoard(0),				// Empty board
		oBoard(0),				// Empty board
		m_Player(firstPlayer),		// PLAYER_X
		m_Winner(RESULT_NONE),	// No winner, RESULT_NONE
		m_BitCount(16) {};		

	~Bitboard() {} ;

	/* Returns the identifier of the player who is next to make a move. */
	int GetPlayer() const { return m_Player; }
	/* Returns the player who has won, or indicate a draw or a mid-game state (player == None). */
	int GetWinner() const { return m_Winner; }

	/* Returns an array of indicies of all legal moves for the currnt board. */
	U16* GetAvailableMoves();

	/* Returns true if the move is legal, false otherwise. */
	bool CheckMove(U16 move);

	/* 
	Returns a new board after making the specified move.
	Player must give a legal move.
	*/
	Bitboard DoMove(U16 move);

	/* Returns the number of clear bits in the board, i.e. the number of moves available. */
	int GetClearBitsCount() { return m_BitCount; }

	/* Returns an evaluation of the board regarding a player, based on number of chains and longest one. */
	int ChainScoreForPlayer(int player);

	/* Print the board to the console. */
	void Print();
	
	std::vector<double> EncodeBoard();

private:
	
	/* Private constructor, used in move generation. */
	Bitboard(U16 xB, U16 oB, int player) :
		xBoard(xB),
		oBoard(oB),
		m_Player(player),
		m_BitCount(CountClearBits()),
		m_Winner(FindWinner()) {};

	/* The bitboard of the 'X' player. */
	U16 xBoard;
	/* The bitboard of the 'O' player. */
	U16 oBoard;
	
	/* The player who is next to make a move. */
	int m_Player;

	/* Count of empty bits */
	int CountClearBits();
	int m_BitCount;

	/* Find the winner, used once during initialization. */
	int FindWinner();
	/* The identifier of the winner (x, o, draw, none). */
	int m_Winner;
};

