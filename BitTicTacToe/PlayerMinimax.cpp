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
	I8   moveCount = board.GetClearBitsCount();
	
	// Convert C-style array to vector
	vector<U16> nextMovesVec(moveCount);
	for (int i = 0; i < moveCount; i++)
		nextMovesVec[i] = nextMoves[i];

	// Find the number of ADDITIONAL threads required (and available).
	I8 numberOfThreads = m_NumberOfThreads;
	if (moveCount < numberOfThreads)	// Don't use more threads than tasks to performe
		numberOfThreads = moveCount;
	
	vector<vector<int>> dist = UniformDistribution(moveCount, numberOfThreads);

	--numberOfThreads;	// We are already using one thread

	// Save the futures from async to find the best one later
	vector<future<MinimaxMove>> futures;

	// Start additional threads
	for (int threadNumber = 0; threadNumber < numberOfThreads; ++threadNumber)
	{
		// Start a thread with std::asyc, rather than std::thread
		futures.push_back(async(
			launch::async,						// Start a thread and not lazy-evaluation
			&PlayerMinimax::MinimaxWorker,		// Pass the worker function
			this,								// Since this is a member function, 
												// we have to include the hideden pointer *this
			board,								
			vector<U16>(						// Construct the subset vector
				&nextMovesVec[dist[threadNumber+1][0]], 
				&nextMovesVec[dist[threadNumber+1][1]]) 
			));
	}

	vector<MinimaxMove> bestMoves;
	
	// Let the main thread do part of the work
	bestMoves.push_back(MinimaxWorker(
		board, 
		vector<U16>(
			&nextMovesVec[dist[0][0]], 
			&nextMovesVec[dist[0][1]]
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
	printf("value: %i | static evaluations: %i\n", bestMove.value, svCount);
	return bestMove.index;
}

// WITHOUT MULTI-THREADING
//
//U16 PlayerMinimax::GetMove(Bitboard board)
//{
//	svCount = 0;
//	U16 *nextMoves = board.GetAvailableMoves();
//	I8 moveCount = board.GetClearBitsCount();
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
	
	 //printf("moves got: %i\n", (int)nextMoves.size());
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

std::vector<std::vector<int>> PlayerMinimax::UniformDistribution(int moveCount, int numberOfThreads)
{
	using namespace std;
	vector<vector<int>> dist;
	int baseNumber = moveCount / numberOfThreads;		// Number of move each worker gets
	int remainder = moveCount % numberOfThreads;		// The rest, distributed evenly across workers

	// Set the moves to be calculated by main first, 
	// to make sure the main gets the least 
	dist.push_back(vector<int>({ 0, baseNumber }));

	//printf("(%i,%i)\n", dist[0][0], dist[0][1]);
	//printf("count=%i th=%i \n", moveCount, numberOfThreads);

	// Set moves for additional threads
	for (int i = 1; i < m_NumberOfThreads; ++i)
	{
		// If more moves remain, give one the the worker
		int partOfRemainder = 0;
		if (remainder > 0)
		{
			partOfRemainder = 1;
			--remainder;
		}
		// To get the numbers of the moves, we use the last number of the previous thread
		// note that range of vectors is [inclusive, exclusive]
		dist.push_back(vector<int>({ dist[i - 1][1],	dist[i - 1][1] + baseNumber + partOfRemainder }));
	}
	return dist;
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
//	I8 moveCount = board.GetClearBitsCount();
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

// Fail-hard minimax implemention
I8 PlayerMinimax::Mini(Bitboard board, I8 depth, I8 alpha, I8 beta)
{
	// Termination condition
	if (depth <= 0 || board.GetWinner() != 0)
		return GetStaticValue(board);

	U16 *nextMoves = board.GetAvailableMoves();
	I8 moveCount = board.GetClearBitsCount();
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
	I8 moveCount = board.GetClearBitsCount();
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
	svCount++;	// Count number of static evaluations performed
	U8 winner = board.GetWinner();

	// Assume the winner is the opponent
	I8 value = -50; 

	// Cover all other options
	if (winner == playerTag)
		value = 50;
	else if (winner == RESULT_DRAW)
		value = 0;

	// Avoid calculating chainscore if there's a win
	else if (winner == RESULT_NONE)
	{
		if (m_bUseChainScore)
			value = board.ChainScoreForPlayer(playerTag);
		else value = 0;
	}

	return value;	
}