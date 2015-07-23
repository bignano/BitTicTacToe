#include "Bitboard.h"
#include <math.h>		// log2
#include <stdio.h>		// printf

U8 Bitboard::FindWinner()
{
	I8 index = 0;
	U16 currPat;

	/* Loop through all 10 possible win patterns (defined in winPatterns) */
	while (index < 10)		
	{
		currPat = winPatterns[index++];

		/* 
		Check if the board of the player contains the current pattern.
		example:
			xBoard   ==  0100 1111 0001 1010
		&	pattern  ==	 0000 1111 0000 0000
			--------------------------------
						 0000 1111 0000 0000 == pattern ==> x wins
		*/

		if ((xBoard & currPat) == currPat)
			return PLAYER_X;
		if ((oBoard & currPat) == currPat)
			return PLAYER_O;
	}

	/*
	(xBoard | oBoard) yields the complete board (both players). 
	If it's equal to UINT16_MAX then it is full (all bits set), 
	and since we didn't return a winner it must be a draw.
	*/
	if ((xBoard | oBoard) == UINT16_MAX)
		return RESULT_DRAW;

	// If every check fails it's still mid-game 
	return RESULT_NONE;		
}


U16* Bitboard::GetAvailableMoves()
{
	// Get all the set bits on the board, then flip to get all empty bits
	U16 board = ~(xBoard | oBoard);

	// Least significant bit
	U16 ls1b;

	U8 moveNumber = 0;

	U16 *moves = new U16[GetClearBitsCount()]();

	if (moves != NULL)
		while (board)
		{
			ls1b = board & -board;			// Isolate the least-significant bit
			moves[moveNumber++] = ls1b;		// Add move to array
			board ^= ls1b;					// Clear ls1b from the board
		}
	return moves;
}

bool Bitboard::CheckMove(U16 move)
{ 
	// Check if one of the player has made this move already
	return !((xBoard | oBoard) & move);
}


Bitboard Bitboard::DoMove(U16 move)
{
	if (m_Player == PLAYER_X) return Bitboard(xBoard | move, oBoard, PLAYER_O);
	else				   	  return Bitboard(xBoard, oBoard | move, PLAYER_X);
}


I8 Bitboard::CountClearBits()
{
	int count = 0;
	/* Same method as CountBitsU16 */
	U16 ls1b;
	U16 x = ~(xBoard | oBoard);		// Get the full board and invert bits
	while (x)
	{
		ls1b = x & -x;
		++count;
		x ^= ls1b;
	}
	return count;
}

/* Counts the number of set bits in a given number */
I8 CountBitsU16(U16 x)
{
	I8 count = 0;
	// Least significant bit
	U16 ls1b;
	
	// while x>0
	while (x)
	{
		ls1b = x & -x;	// Isolate ls1b
		++count;		
		x ^= ls1b;		// Renove ls1b from board
	}
	return count;
}

I8 Bitboard::ChainScoreForPlayer(U8 player)
{
	I8 chainScore = 0;
	I8 longestChain = 0;
	I8 currChain = 0;
	I8 index = 0;
	U16 currPat;

	U16 board = xBoard;			// Use the player's board to count score for moves
	U16 otherBoard = oBoard;	// Use the other board to exclude moves the other player blocked

	// Set the boards according to the player
	if (player == PLAYER_O) { board = oBoard; otherBoard = xBoard; }

	// Loop through all win patterns
	while (index < 10)
	{
		currPat = winPatterns[index++];
		if (!(currPat & otherBoard))						// Completely ignore blocked moves
		{
			currChain = CountBitsU16((currPat & board));	// Get the number of squares of the chain we have
			chainScore += currChain*currChain;				// Add the score of the chain
			if (currChain>longestChain)						// Update longest chain
				longestChain = currChain;	
		}
	}
	return chainScore + longestChain;	// The score is the sum of all partial chains squared and the longest one
}

/* Find the power of the ls1b to find its index */
inline int GetLS1BPower(U16 &x)
{
	U16 ls1b = x & -x;
	x ^= ls1b;
	return (int)log2((double)ls1b);
}

void Bitboard::Print()
{
	// The base 4x4 grid
	char grid[16] = {
		'-', '-', '-', '-',
		'-', '-', '-', '-',
		'-', '-', '-', '-',
		'-', '-', '-', '-', };

	// Save copies of the board to keep it unmodified
	U16 xGrid = xBoard;
	U16 oGrid = oBoard;

	/* Fill the grid with symbols according to the board */
	while (xGrid)
		grid[15 - GetLS1BPower(xGrid)] = 'X';
	while (oGrid)
		grid[15-GetLS1BPower(oGrid)] = 'O';

	/* Print the grid nicely */
	printf("\nBoard:\n\t   1   2   3   4\n\n");
	for (int row = 0; row < 4; ++row)
	{
		printf("\t%i  %c   %c   %c   %c\n\n",
			row		+		1,
			grid[row * 4	],
			grid[row * 4 + 1],
			grid[row * 4 + 2],
			grid[row * 4 + 3]);
	}

	/* Print which player is taking the next turn */
	if (m_Winner == RESULT_NONE)
	{
		if (m_Player == PLAYER_X)
			printf("X's turn.\n");
		else
			printf("O's turn.\n");
	}
}