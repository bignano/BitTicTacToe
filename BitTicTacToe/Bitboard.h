#pragma once

#include <vector>
#include <stdint.h>		// For UINT16_MAX

// Define unsigned integers for board and move storage
typedef uint16_t U16;
typedef uint32_t U32;

enum class GameTag
{
	Result_None,
	Player_X,
	Player_O,
	Result_Draw
};

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
The 10 win patterns encoded as binary numbers and converted to decimal
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
	Bitboard(GameTag firstPlayer=GameTag::Player_X) : 
		xBoard(0),						// Empty board
		oBoard(0),						// Empty board
		m_Player(firstPlayer),			
		m_Winner(GameTag::Result_None),	// No winner, RESULT_NONE
		m_BitCount(16) {};		

	~Bitboard() {} ;

	/* Returns the identifier of the player who is next to make a move. */
	inline GameTag GetPlayerTag() const { return m_Player; }
	/* Returns the identifier of the player who is NOT next to make a move. */
	inline GameTag GetOtherPlayerTag() const;
	/* Returns the player who has won, or indicate a draw or a mid-game state (State_None). */
	inline GameTag GetWinner() const { return m_Winner; }

	/* Returns an array of indicies of all legal moves for the currnt board. */
	U16 *GetAvailableMoves() const;

	/* Returns true if the move is legal, false otherwise. */
	bool CheckMove(U16 move);

	/* 
	Returns a new board after making the specified move.
	Checks for errors, to be used with game managers
	*/
	Bitboard DoMove(U16 move, GameTag player);

	/*
	Returns a new board after making the specified move.
	Does not check for errors, to be used with minimax.
	*/
	Bitboard DoMove(U16 move);

	/* Returns the number of clear bits in the board, i.e. the number of moves available. */
	inline int GetClearBitsCount() const { return m_BitCount; }

	/* Returns an evaluation of the board regarding a player, based on number of chains and longest one. */
	int ChainScoreForPlayer(GameTag player);

	/* Print the board to the console. */
	void Print();
	
	/* Represent the board as 16-component vector, positives for player, negatives for opponent */
	std::vector<double> EncodeBoardDVec(GameTag player);
	 
	U32 EncodeBoardU32(GameTag player);

private:
	
	/* Private constructor, used in move generation. */
	inline Bitboard(U16 xB, U16 oB, GameTag player) :
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
	GameTag m_Player;

	/* Number of clear bits */
	inline int CountClearBits();
	int m_BitCount;

	/* Find the winner, used once during construction. */
	inline GameTag FindWinner();
	/* The identifier of the winner (x, o, draw, none). */
	GameTag m_Winner;

};

