#include "Bitboard.h"
#include <math.h>		// log2
#include <stdio.h>		// printf
#include <exception>	// std::exception

GameTag Bitboard::FindWinner()
{
	int index = 0;
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
			return GameTag::Player_X;
		if ((oBoard & currPat) == currPat)
			return GameTag::Player_O;
	}

	/*
	(xBoard | oBoard) yields the complete board (both players). 
	If it's equal to UINT16_MAX then it is full (all bits set), 
	and since we didn't return a winner it must be a draw.
	*/
	if ((xBoard | oBoard) == UINT16_MAX)
		return GameTag::Result_Draw;

	// If every check fails it's still mid-game 
	return GameTag::Result_None;		
}

GameTag Bitboard::GetOtherPlayerTag() const
{
	return (m_Player == GameTag::Player_X) ? GameTag::Player_O : GameTag::Player_X;
}

U16 *Bitboard::GetAvailableMoves() const
{
	// Get all set bits on the board, then flip to get all clear bits
	U16 board = ~(xBoard | oBoard);

	// Least significant bit
	U16 ls1b;

	int moveNumber = 0;

	U16 *moves = new U16[m_BitCount]();

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


Bitboard Bitboard::DoMove(U16 move, GameTag player)
{
	// This prevents problems with game logic
	if (player != m_Player)
		throw std::exception("The given player is not next to make a move");
	
	return DoMove(move);
}

Bitboard Bitboard::DoMove(U16 move)
{
	if ((xBoard | oBoard) & move)	// The requested move isn't empty
		throw std::exception("Invalid move");

	if (m_Player == GameTag::Player_X) return Bitboard(xBoard | move, oBoard, GameTag::Player_O);
	else				   			   return Bitboard(xBoard, oBoard | move, GameTag::Player_X);
}

int Bitboard::CountClearBits()
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
int CountBitsU16(U16 x)
{
	int count = 0;
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

int Bitboard::ChainScoreForPlayer(GameTag player)
{
	int chainScore = 0;
	int longestChain = 0;
	int currChain = 0;
	int index = 0;
	U16 currPat;

	U16 board = xBoard;			// Use the player's board to count score for moves
	U16 otherBoard = oBoard;	// Use the other board to exclude moves the other player blocked

	// Set the boards according to the player
	if (player == GameTag::Player_O) { board = oBoard; otherBoard = xBoard; }

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

/* Find the power of ls1b to find its index */
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
	if (m_Winner == GameTag::Result_None)
	{
		if (m_Player == GameTag::Player_X)
			printf("X's turn.\n");
		else
			printf("O's turn.\n");
	}
}

std::vector<double> Bitboard::EncodeBoardDVec(GameTag player)
{
	std::vector<double> encoded(16, 0.0);

	// Save copies of the board to keep it unmodified
	U16 xGrid = xBoard;
	U16 oGrid = oBoard;

	/* 1 for player's square, -1 for opponent's square */
	int xValue = 1, oValue = -1;
	if (player == GameTag::Player_O)
	{
		xValue = -1;
		oValue = 1;
	}
	
	/* Fill the encoded board */
	while (xGrid)
		encoded[15 - GetLS1BPower(xGrid)] = xValue;
	while (oGrid)
		encoded[15 - GetLS1BPower(oGrid)] = oValue;

	return encoded;
}

U32 Bitboard::EncodeBoardU32(GameTag player)
{
	/* This encodes boards in a symmetrical way.
	The board of the given player is put the significant half of the integer. */
	U32 encoded = (U32)xBoard;
	if (player == GameTag::Player_X)
	{
		encoded <<= 16;
		encoded |= (U32)oBoard;
	}
	else
		encoded |= (U32)oBoard << 16;

	return encoded;
}