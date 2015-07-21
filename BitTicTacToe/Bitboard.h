#pragma once

#define UINT16_MAX  0xffffui16

#define RESULT_NONE 0
#define PLAYER_X	1
#define PLAYER_O	2
#define RESULT_DRAW 3

// Define standard ints
typedef signed   char	I8;
typedef unsigned char	U8;
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
	Bitboard() : 
		xBoard(0),			// Empty board
		oBoard(0),			// Empty board
		m_player(1),		// PLAYER_X
		m_winner(RESULT_NONE) {};		// No winner, RESULT_NONE

	~Bitboard() {} ;

	/* Returns the identifier of the player who is next to make a move. */
	U8 GetPlayer() const { return m_player; }
	/* Returns the player who has won, or indicate a draw or a mid-game state (player == None). */
	U8 GetWinner() const { return m_winner; }

	/* Returns an array of indicies of all legal moves for the currnt board. */
	U16* GetAvailableMoves();

	/* Returns true if the move is legal, false otherwise. */
	bool CheckMove(U16 move);

	/* 
	Returns a new board after making the specified move.
	Player must give a legal move.
	*/
	Bitboard DoMove(U16 move);

	/* Returns the number of set bits in the board, i.e. the number of moves taken. */
	I8 CountBits();

	/* Returns an evaluation of the board regarding a player, based on number of chains and longest one. */
	I8 ChainScoreForPlayer(U8 player);

	/* Print the board to the console. */
	void Print();
	
private:
	
	/* Private constructor, used in move generation. */
	Bitboard(U16 xB, U16 oB, U8 player) :
		xBoard(xB),
		oBoard(oB),
		m_player(player),
		m_winner(FindWinner()) {};

	/* The bitboard of the 'X' player. */
	U16 xBoard;
	/* The bitboard of the 'O' player. */
	U16 oBoard;
	
	/* The player who is next to make a move. */
	U8 m_player;

	/* Find the winner, used once during initialization. */
	U8 FindWinner();
	/* The identifier of the winner (x, o, draw, none). */
	U8 m_winner;

};

