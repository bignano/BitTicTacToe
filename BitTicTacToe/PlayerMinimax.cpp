#include "PlayerMinimax.h"
#include <stdlib.h>
#include <stdio.h>
#include <thread>

const I8 MINIMAX_INFINITY = 100;

struct FinalMove
{
	U16 index;
	I8 value;
};

int svCount = 0;
U16 PlayerMinimax::GetMove(Bitboard board)
{
	svCount = 0;
	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();
	Bitboard newBoard;

	FinalMove move;
	move.value = -MINIMAX_INFINITY;
	move.index = nextMoves[0];
	I8 newValue;

	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
	{
		newBoard = board.DoMove(nextMoves[moveIndex]);
		newValue = Mini(newBoard, searchDepth - 1, -MINIMAX_INFINITY, MINIMAX_INFINITY);

		if (newValue > move.value)
		{
			move.index = nextMoves[moveIndex];
			move.value = newValue;
		}
	}

	delete[] nextMoves;
	printf("value: %i  static evaluations: %i\n", move.value, svCount);
	return move.index;
}
//
//I8 PlayerMinimax::Minimax(Bitboard board, I8 depth, I8 alpha, I8 beta)
//{
//	if (depth <= 0 || board.GetWinner() != 0) 
//		return GetStaticValue(board);
//
//	I8 value = -MINIMAX_INFINITY;
//	U16 *nextMoves = board.GetAvailableMoves();
//	I8 moveCount = board.CountBits();
//	Bitboard newBoard;
//	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
//	{
//		newBoard = board.DoMove(nextMoves[moveIndex]);
//		I8 newValue = -Minimax(newBoard, depth - 1, -beta, -alpha);
//		if (newValue > value)
//			value = newValue;
//		if (value > alpha)
//			alpha = value;
//		if (alpha >= beta)
//		{
//			delete[] nextMoves;
//			return alpha;
//		}
//	}
//	delete[] nextMoves;
//	return value;
//}

I8 PlayerMinimax::Mini(Bitboard board, I8 depth, I8 alpha, I8 beta)
{
	if (depth <= 0 || board.GetWinner() != 0)
		return GetStaticValue(board);

	I8 value = MINIMAX_INFINITY;
	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();
	Bitboard newBoard;
	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
	{
		newBoard = board.DoMove(nextMoves[moveIndex]);
		I8 newValue = Max(newBoard, depth - 1, alpha, beta);

		if (newValue < value)
			value = newValue;

		if (value < beta)
			beta = value;

		if (alpha >= beta)
		{
			delete[] nextMoves;
			return alpha;
		}
	}
	delete[] nextMoves;
	return value;
}
I8 PlayerMinimax::Max(Bitboard board, I8 depth, I8 alpha, I8 beta)
{
	if (depth <= 0 || board.GetWinner() != 0)
		return GetStaticValue(board);

	I8 value = -MINIMAX_INFINITY;
	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();
	Bitboard newBoard;
	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
	{
		newBoard = board.DoMove(nextMoves[moveIndex]);
		I8 newValue = Mini(newBoard, depth - 1, alpha, beta);

		if (newValue > value)
			value = newValue;
		
		if (value > alpha)
			alpha = value;
		
		if (alpha >= beta)
		{
			delete[] nextMoves;
			return alpha;
		}
	}
	delete[] nextMoves;
	return value;
}

I8 PlayerMinimax::GetStaticValue(Bitboard board)
{
	svCount++;
	U8 winner = board.GetWinner();
	
	I8 chainScore = 0;
	if (bUseChainScore)
		chainScore = board.ChainScoreForPlayer(playerTag);

	I8 value = -50; 
	if (winner == playerTag)
		value = 50;
	else if (winner == 0)
		value = chainScore;
	else if (winner == 3)
		value = 0;

	return value;	
}