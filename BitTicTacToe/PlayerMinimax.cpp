#include "PlayerMinimax.h"
#include <stdio.h>
#include <thread>
#include <future>


const I8 MINIMAX_INFINITY = 100;

int svCount = 0;

U16 PlayerMinimax::GetMove(Bitboard board)
{
	using namespace std;

	// Reset static evaluations count
	svCount = 0;

	// Get all the next moves for the board
	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();

	// Convert C-style array to vector
	vector<U16> nextMovesVec(moveCount);
	for (int i = 0; i < moveCount; i++)
		nextMovesVec[i] = nextMoves[i];

	// Find the number of ADDITIONAL threads required (and available).
	I8 numberOfThreads = m_NumberOfThreads;
	if (moveCount < numberOfThreads)	// Don't use more threads than tasks to performe
		numberOfThreads = moveCount;
	--numberOfThreads;					// We are already using one thread
	
	// Save the futures from async to find the best one
	vector<future<MinimaxMove>> futures;

	// Start additional threads
	for (int threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber)
	{
		// Find the range of the moves for the worker to search
		// firstIndex inclusive, lastIndex exclusive
		int firstIndex = (moveCount / m_NumberOfThreads)*(threadNumber + 1);
		int lastIndex = firstIndex + 4;
		if (threadNumber == numberOfThreads - 1)	// Last thread
			lastIndex = moveCount;

		// Start a thread with std::asyc, rather than std::thread
		futures.push_back(async(
			launch::async,						// Start a thread and not lazy-evaluation
			&PlayerMinimax::MinimaxWorker,		// Pass the worker function
			this,								// Since this is a member function, 
												// we have to include the hideden pointer *this
			board,								
			vector<U16>(						// Construct the subset vector
				&nextMovesVec[firstIndex], 
				&nextMovesVec[lastIndex]) 
			));
	}

	// 
	vector<MinimaxMove> bestMoves;
	
	// Let the main thread do part of the work
	bestMoves.push_back(MinimaxWorker(
		board, 
		vector<U16>(
			&nextMovesVec[0], 
			&nextMovesVec[moveCount/m_NumberOfThreads]
			)));

	// Populate bestMoves with best moves from each thread
	for (int i = 0; i < numberOfThreads; i++)
		bestMoves.push_back(futures[i].get());

	MinimaxMove bestMove = bestMoves[0];

	// Find the overall best move
	for (MinimaxMove currMove : bestMoves)
		if (currMove.value > bestMove.value)
			bestMove = currMove;

	delete[] nextMoves;

	return bestMove.index;
}

// WITHOUT MULTITHREADING
//
//U16 PlayerMinimax::GetMove(Bitboard board)
//{
//	svCount = 0;
//	U16 *nextMoves = board.GetAvailableMoves();
//	I8 moveCount = board.CountBits();
//	Bitboard newBoard;
//
//	MinimaxMove move;
//	move.value = -MINIMAX_INFINITY;
//	move.index = nextMoves[0];
//	I8 newValue;
//
//	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
//	{
//		newBoard = board.DoMove(nextMoves[moveIndex]);
//		newValue = Mini(newBoard, m_SearchDepth - 1, -MINIMAX_INFINITY, MINIMAX_INFINITY);
//
//		if (newValue > move.value)
//		{
//			move.index = nextMoves[moveIndex];
//			move.value = newValue;
//		}
//	}
//
//	delete[] nextMoves;
//	printf("value: %i  static evaluations: %i\n", move.value, svCount);
//	return move.index;
//}

MinimaxMove PlayerMinimax::MinimaxWorker(Bitboard board, std::vector<U16> nextMoves)
{
	// Initialize the first move
	MinimaxMove move;
	move.value = -MINIMAX_INFINITY;
	move.index = nextMoves[0];

	for (int moveIndex = 0; moveIndex < nextMoves.size(); moveIndex++)
	{
		Bitboard newBoard = board.DoMove(nextMoves[moveIndex]);
		I8 newValue = Mini(newBoard, m_SearchDepth - 1, -MINIMAX_INFINITY, MINIMAX_INFINITY);

		if (newValue > move.value)
		{
			move.index = nextMoves[moveIndex];
			move.value = newValue;
		}
	}
	return move;
}

//
// NEGAAMX IMPLEMENTATION, NOT SUITABLE FOR THIS CASE
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

	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();
	Bitboard newBoard;
	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
	{
		newBoard = board.DoMove(nextMoves[moveIndex]);
		I8 score = Max(newBoard, depth - 1, alpha, beta);

		if (score <= alpha) 
		{
			delete[] nextMoves;
			return alpha;
		}

		if (score < beta)
			beta = score;
	}
	delete[] nextMoves;
	return beta;
}
I8 PlayerMinimax::Max(Bitboard board, I8 depth, I8 alpha, I8 beta)
{
	if (depth <= 0 || board.GetWinner() != 0)
		return GetStaticValue(board);

	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.CountBits();
	Bitboard newBoard;
	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
	{
		newBoard = board.DoMove(nextMoves[moveIndex]);
		I8 score = Mini(newBoard, depth - 1, alpha, beta);

		if (score >= beta) 
		{
			delete[] nextMoves;
			return beta;
		}

		if (score > alpha)
			alpha = score;
	}
	delete[] nextMoves;
	return alpha;
}

I8 PlayerMinimax::GetStaticValue(Bitboard board)
{
	svCount++;
	U8 winner = board.GetWinner();
	
	I8 chainScore = 0;
	if (m_bUseChainScore)
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