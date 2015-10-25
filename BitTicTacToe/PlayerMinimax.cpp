#include "PlayerMinimax.h"
#include <stdio.h>
#include <thread>
#include <future>

const int MINIMAX_INFINITY = 2147483647;

template<class T>
T Max(const T &t1, const T &t2)
{
	return (t1 > t2) ? t1 : t2;
}

template<class T>
T Min(const T &t1, const T &t2)
{
	return (t1 < t2) ? t1 : t2;
}

template<class T>
std::vector<T> subvector(const std::vector<T>& vec, size_t first, size_t last)
{
	std::vector<T> nvec(last - first + 1);
	for (size_t i = 0; i < (last - first); i++)
		nvec[i] = vec[first + i];
	return nvec;
}

U16 PlayerMinimax::GetMove(Bitboard board)
{
	using namespace std;

	// Reset static evaluations count
	svCount = 0;

	// Get all the next moves for the board
	U16 *nextMoves = board.GetAvailableMoves();
	int   moveCount = board.GetClearBitsCount();

	// Convert C-style array to vector
	vector<U16> nextMovesVec(moveCount);
	for (int i = 0; i < moveCount; i++)
		nextMovesVec[i] = nextMoves[i];

	// Find the number of ADDITIONAL threads required (and available).
	int numberOfThreads = m_NumberOfThreads;
	if (moveCount < numberOfThreads)	// Don't use more threads than tasks to performe
		numberOfThreads = moveCount;

	vector<vector<int>> dist = DistributeWork(moveCount, numberOfThreads);

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
												// we have to include the hideden pointer this
			board,
			subvector<U16>(nextMovesVec,					// Construct the subset vector (worker moves)
				dist[threadNumber + 1][0],
				dist[threadNumber + 1][1])
			));
	}

	// Let the main thread do part of the work
	MinimaxMove bestMove = MinimaxWorker(
		board,
		subvector<U16>(nextMovesVec, dist[0][0], dist[0][1]));

	// Get moves and find best
	for (int i = 0; i < numberOfThreads; i++)
	{
		MinimaxMove move = futures[i].get();
		if (move.value > bestMove.value)
			bestMove = move;
	}

	if (m_Verbose)
		printf("\nvalue: %i | static evaluations: %i\n", bestMove.value, svCount);

	delete[] nextMoves;
	return bestMove.index;
}

//U16 PlayerMinimax::GetMove(Bitboard board)
//{
//	svCount = 0;
//	U16 *nextMoves = board.GetAvailableMoves();
//	int moveCount = board.GetClearBitsCount();
//	Bitboard newBoard;
//
//	MinimaxMove move;
//	move.value = -MINIMAX_INFINITY;
//	move.index = nextMoves[0];
//	int newValue;
//
//	for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
//	{
//		newBoard = board.DoMove(nextMoves[moveIndex]);
//		newValue = Alphabeta(newBoard, m_SearchDepth - 1, -MINIMAX_INFINITY, MINIMAX_INFINITY);
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
		int newValue = Alphabeta(newBoard, m_SearchDepth - 1, -MINIMAX_INFINITY, MINIMAX_INFINITY);
		if (newValue > move.value)
		{
			move.index = nextMoves[moveIndex];
			move.value = newValue;
		}

		using namespace std::chrono_literals;
		if (m_PowerSaver)
			std::this_thread::sleep_for(5ms);
	}
	return move;
}

std::vector<std::vector<int>> PlayerMinimax::DistributeWork(int moveCount, int numberOfThreads)
{
	using namespace std;
	vector<vector<int>> dist;
	int baseNumber = moveCount / numberOfThreads;		// Number of move each worker gets, -1 because range is exclusive
	int remainder = moveCount % numberOfThreads;		// The rest, distributed evenly across workers

	// Set the moves to be calculated by main first, 
	// to make sure the main gets the least 
	dist.push_back(vector<int>({0, baseNumber}));

	//printf("(%i,%i)\n", dist[0][0], dist[0][1]);
	//printf("count=%i th=%i \n", moveCount, numberOfThreads);

	// Set moves for additional threads
	for (int i = 1; i < m_NumberOfThreads; ++i)
	{
		// If more extra moves remain, give one the the worker
		int extra = remainder-- > 0 ? 1 : 0;

		// To get the numbers of the moves, we use the last number of the previous thread
		// note that range of vectors is [inclusive, exclusive]
		dist.push_back(vector<int>({dist[i - 1][1], dist[i - 1][1] + baseNumber + extra}));
	}
	return dist;
}

int PlayerMinimax::Alphabeta(Bitboard board, int depth, int alpha, int beta)
{
	// Termination condition
	if (depth <= 0 || board.GetWinner() != GameTag::Result_None)
		return GetStaticValue(board);

	U16 *nextMoves = board.GetAvailableMoves();
	int moveCount = board.GetClearBitsCount();
	Bitboard newBoard;

	int value = -MINIMAX_INFINITY;

	if (board.GetPlayerTag() == m_PlayerTag)	// maximizing player
		for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
		{
			newBoard = board.DoMove(nextMoves[moveIndex]);
			value = Max(value, Alphabeta(newBoard, depth - 1, alpha, beta));
			alpha = Max(alpha, value);

			if (alpha >= beta)
				break;
		}
	else // minimizing player
	{
		value = MINIMAX_INFINITY;
		for (int moveIndex = 0; moveIndex < moveCount; moveIndex++)
		{
			newBoard = board.DoMove(nextMoves[moveIndex]);
			value = Min(value, Alphabeta(newBoard, depth - 1, alpha, beta));
			beta = Min(beta, value);

			if (alpha >= beta)
				break;
		}
	}

	delete[] nextMoves;
	return value;
}

int PlayerMinimax::GetStaticValue(Bitboard &board)
{
	svCount++;	// Count number of static evaluations performed
	GameTag winner = board.GetWinner();

	GameTag otherPlayer =
		(m_PlayerTag == GameTag::Player_X) ? GameTag::Player_O : GameTag::Player_X;

	int value = 0;

	if (winner == m_PlayerTag)
		value = 1000000000 + board.GetClearBitsCount();

	if (winner == otherPlayer)
		value = -1000000000 - board.GetClearBitsCount();

	else if (winner == GameTag::Result_None && m_bUseChainScore)
		value = board.ChainScoreForPlayer(m_PlayerTag);

	return value;
}