#pragma once
#include "Player.h"
#include <vector>

struct MinimaxMove
{
	U16 index;
	int value;
};

class PlayerMinimax :
	public Player
{
public:

	// Default player
	PlayerMinimax(int tag = PLAYER_X, int depth = 9, bool useChainScore = true, int maxNumberOfThreads=3) :
		Player(tag), 
		m_SearchDepth(CapSearchDepth(depth)),
		 m_bUseChainScore(useChainScore),
		 m_NumberOfThreads(maxNumberOfThreads),
		 svCount(0) {};
	
	~PlayerMinimax() {}

	U16 GetMove(Bitboard board);

private:

	/* Gets a board and moves and retures the best move. 
	Used by GetMove to calculate part of the tree, to suppot multithreading. */
	MinimaxMove MinimaxWorker(Bitboard board, std::vector<U16> nextMoves);

	/* Distributes the number of moves evenly for each thread */
	std::vector<std::vector<int>> UniformDistribution(int moveCount, int numberOfThreads);

	/* Regular minimax functions */
	int Mini(Bitboard board, int depth, int alpha, int beta);
	int Max(Bitboard board, int depth, int alpha, int beta);

	/* The initial search depth*/
	int m_SearchDepth;
	/* Limit the depth to 16 (and above 0) */
	int  CapSearchDepth(int depth) { 
		if (depth > 16) return depth;
		if (depth < 1) return 1; 
		return depth; }

	bool m_bUseChainScore;
	int m_NumberOfThreads;

protected:

	// Default without chainscore (NN's override evaluation)
	PlayerMinimax(int tag = PLAYER_X, int depth = 9, int maxNumberOfThreads = 3) :
		Player(tag),
		m_SearchDepth(CapSearchDepth(depth)),
		m_bUseChainScore(false),
		m_NumberOfThreads(maxNumberOfThreads),
		svCount(0)
	{};

	/* Returns static value for a board */
	virtual int GetStaticValue(Bitboard &board);

	// Count of static evaluations performed in last move
	int svCount;
};

